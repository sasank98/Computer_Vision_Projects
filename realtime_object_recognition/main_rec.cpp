
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include "functions.h"
#include "csv_util.h"


int main(int argc, char* argv[]) {
    cv::VideoCapture* capdev;

    capdev = new cv::VideoCapture(1);
    if (!capdev->isOpened()) {
        printf("Unable to open video device\n");
        return(-1);
    }

    cv::Size refS((int)capdev->get(cv::CAP_PROP_FRAME_WIDTH),
        (int)capdev->get(cv::CAP_PROP_FRAME_HEIGHT));
    printf("Expected size: %d %d\n", refS.width, refS.height);

    cv::namedWindow("Video", cv::WINDOW_NORMAL);
    cv::resizeWindow("Video", cv::Size(400, 400));
    cv::Mat frame;


    int reset_file = 0;

    //Window size for each opencv window
    int winsize = 400;
    for (;;) {
        *capdev >> frame; 
        if (frame.empty()) {
            printf("frame is empty\n");
            break;
        }
        cv::imshow("Video", frame);
        cv::Mat image;
        cv::resize(frame, image, cv::Size(600, 600));

        char key = cv::waitKey(10);

        if (key == 'q') {
            break;
        }

        if (key == 'x') {
            
            //Threshold (Task 1)
            cv::Mat thresholded;
            threshold(image, thresholded);
            cv::namedWindow("thresh", cv::WINDOW_NORMAL);
            cv::resizeWindow("thresh", cv::Size(winsize, winsize));
            cv::imshow("thresh", thresholded);

            cv::Mat clean;
            cleanup(thresholded, clean);
            cv::namedWindow("clean", cv::WINDOW_NORMAL);
            cv::resizeWindow("clean", cv::Size(winsize, winsize));
            cv::imshow("clean", clean);

            //Segment (Task 3)
            cv::Mat segmented_image, lb, stats, centroids;
            int n_labels;
            std::vector<int> biggest_label;
            segment(clean, segmented_image, lb, stats, centroids, n_labels, biggest_label);
            cv::namedWindow("seg", cv::WINDOW_NORMAL);
            cv::resizeWindow("seg", cv::Size(winsize, winsize));
            cv::imshow("seg", segmented_image);

            //Draw bounding box and calculate moment
            std::vector<double> featvect;
            drawbbox_calcmoment(thresholded, image, featvect,centroids);
            cv::namedWindow("bbox", cv::WINDOW_NORMAL);
            cv::resizeWindow("bbox", cv::Size(winsize, winsize));
            cv::imshow("bbox", image);
        }

        if (key == 't') {

            //Threshold (Task 1)
            cv::Mat thresholded;
            threshold(image, thresholded);
            cv::namedWindow("thresh", cv::WINDOW_NORMAL);
            cv::resizeWindow("thresh", cv::Size(winsize, winsize));
            cv::imshow("thresh", thresholded);

            //Image Cleanup (Task 2)
            cv::Mat clean;
            cleanup(thresholded, clean);
            cv::namedWindow("clean", cv::WINDOW_NORMAL);
            cv::resizeWindow("clean", cv::Size(winsize, winsize));
            cv::imshow("clean", clean);

            //Segment (Task 3)
            cv::Mat segmented_image, lb, stats, centroids;
            int n_labels;
            std::vector<int> biggest_label;
            segment(clean, segmented_image, lb, stats, centroids, n_labels, biggest_label);
            cv::namedWindow("seg", cv::WINDOW_NORMAL);
            cv::resizeWindow("seg", cv::Size(winsize, winsize));
            cv::imshow("seg", segmented_image);

            //Draw bounding box and calculate moment
            std::vector<double> featvect;
            drawbbox_calcmoment(thresholded, image, featvect, centroids);
            cv::namedWindow("bbox", cv::WINDOW_NORMAL);
            cv::resizeWindow("bbox", cv::Size(winsize, winsize));
            cv::imshow("bbox", image);

            std::cout << "Enter Object Name: " << std::endl;
            std::string obj_name;
            std::cin >> obj_name;

            char* object_name = new char[obj_name.length() + 1];
            std::strcpy(object_name, obj_name.c_str());

            char database[256] = "database.csv";
            append_image_data_csv(database, object_name, featvect, reset_file);
        }

        if (key == 'i') {

            cv::Mat thresholded;
            threshold(image, thresholded);

            //Image Cleanup (Task 2)
            cv::Mat clean;
            cleanup(thresholded, clean);

            //Segment (Task 3)
            cv::Mat segmented_image, lb, stats, centroids;
            int n_labels;
            std::vector<int> biggest_label;
            segment(clean, segmented_image, lb, stats, centroids, n_labels, biggest_label);


            std::vector<double> featvect;
            drawbbox_calcmoment(thresholded, image, featvect,centroids);

            //Inference
            std::cout << "Nearest Neighbour or K Nearest Neighbor classification?: 1 or 0 " << std::endl;
            int classify;
            std::cin >> classify;

            if (classify == 1) {

                char db_name[256] = "database.csv";
                std::vector<char*> obj_names;
                std::vector<std::vector<float>> obj_feat;
                read_image_data_csv(db_name, obj_names, obj_feat);

                std::vector<double> dist_bw_featvects;


                for (int i = 0; i < obj_names.size(); i++) {

                    std::vector<double> row_vect;
                    for (int j = 0; j < obj_feat[i].size(); j++) {
                        row_vect.push_back(obj_feat[i][j]);
                    }

                    double final_val;
                    euclideanDistance(row_vect, featvect, final_val);
                    dist_bw_featvects.push_back(final_val);
                }

                std::vector< std::pair <double, char*> > combined_vect;

                for (int i = 0; i < obj_names.size(); i++) {
                    combined_vect.push_back(std::make_pair(dist_bw_featvects[i], obj_names[i]));
                }

                std::sort(combined_vect.begin(), combined_vect.end());

                std::string most_similar_obj = combined_vect[0].second;


                int fontFace = cv::FONT_HERSHEY_SIMPLEX;
                double fontScale = 1;
                int thickness = 2;
                int baseline = 0;
                cv::Size textSize = cv::getTextSize(most_similar_obj, fontFace, fontScale, thickness, &baseline);

                // Calculate the position to write the text
                cv::Point textOrg((image.cols - textSize.width) / 2, (image.rows + textSize.height) / 2);

                putText(image, most_similar_obj, textOrg, fontFace, fontScale, cv::Scalar(0, 0, 255), thickness);
                cv::namedWindow("bbox", cv::WINDOW_NORMAL);
                cv::resizeWindow("bbox", cv::Size(winsize, winsize));
                cv::imshow("bbox", image);
            }

            if (classify == 0) {


                char db_name[256] = "database.csv";
                std::vector<char*> obj_names;
                std::vector<std::vector<float>> obj_feat;
                read_image_data_csv(db_name, obj_names, obj_feat);

                std::vector<double> dist_bw_featvects;
                
                //For K nearest neighbor I have only considered the first 4 out of the 7 Humoments
                featvect.erase(featvect.begin() + 4, featvect.begin() + 7);

                for (int i = 0; i < obj_names.size(); i++) {

                    std::vector<double> row_vect;
                    int ctr = 0;
                    
                    for (int j = 0; j < obj_feat[i].size(); j++) {

                        row_vect.push_back(obj_feat[i][j]);                    
                    }
                    row_vect.erase(row_vect.begin() + 4, row_vect.begin() + 7);

                    double final_val;
                    euclideanDistance(row_vect, featvect, final_val);
                    dist_bw_featvects.push_back(final_val);
                }

                std::vector< std::pair <double, char*> > combined_vect;

                for (int i = 0; i < obj_names.size(); i++) {
                    combined_vect.push_back(std::make_pair(dist_bw_featvects[i], obj_names[i]));
                }

                std::vector<double> kdistances;
                std::vector<char*> kdist_objnames;


                for (int i = 0; i < obj_names.size(); i=i+6) {
                    std::vector<double> temp;
                    for (int j = 0; j < 6; j++) {
                        temp.push_back(combined_vect[i + j].first);
                    }
                    std::sort(temp.begin(), temp.end());
                    double final_val = temp[0] + temp[1] + temp[2];
                    kdistances.push_back(final_val);
                    kdist_objnames.push_back(combined_vect[i].second);
                    


                }
                std::vector< std::pair <double, char*> > final_combined_vect;

                for (int i = 0; i < kdistances.size(); i++) {
                    final_combined_vect.push_back(std::make_pair(kdistances[i], kdist_objnames[i]));
                }

                std::sort(final_combined_vect.begin(), final_combined_vect.end());

                std::cout << std::endl;

                for (int i = 0; i < final_combined_vect.size(); i++) {
                    std::cout << final_combined_vect[i].first<<" ";
                    std::cout << final_combined_vect[i].second<<std::endl;
                }      


                std::string most_similar_obj = final_combined_vect[0].second;

                int fontFace = cv::FONT_HERSHEY_SIMPLEX;
                double fontScale = 1;
                int thickness = 2;
                int baseline = 0;
                cv::Size textSize = cv::getTextSize(most_similar_obj, fontFace, fontScale, thickness, &baseline);

                // Calculate the position to write the text
                cv::Point textOrg((image.cols - textSize.width) / 2, (image.rows + textSize.height) / 2);

                putText(image, most_similar_obj, textOrg, fontFace, fontScale, cv::Scalar(0, 0, 255), thickness);
                cv::namedWindow("bbox", cv::WINDOW_NORMAL);
                cv::resizeWindow("bbox", cv::Size(winsize, winsize));
                cv::imshow("bbox", image);
            }
        }

     }

        return 0;
}


