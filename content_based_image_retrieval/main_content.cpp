
#include <cstring>
#include <cstdlib>
#include<iostream>
#include <vector>
#include<string>
#include<opencv2/opencv.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/videoio.hpp>
#include<opencv2/imgproc.hpp>
#include"matching.h"
#include"filter.h"
#include"csv_util.h"
#include <dirent.h>


int main(int argc, char* argv[]) {


    std::cout << "Enter 1 to start 0 to exit: " << std::endl;
    int cont;
    std::cin >> cont;

    while (cont == 1) {

        std::cout << "Save CSV? : 1 or 0" << std::endl;
        int csv_save;
        std::cin >> csv_save;

        if (csv_save == 1) {
            std::cout << "Save CSV type (baseline, hist, multihist, texcolor)" << std::endl;
            std::string save_type;
            std::cin >> save_type;

            char dirname[256];
            char buffer[256];
            FILE* fp;
            DIR* dirp;
            struct dirent* dp;
            int i;

            // check for sufficient arguments
            if (argc < 2) {
                printf("usage: %s <directory path>\n", argv[0]);
                exit(-1);
            }

            // get the directory path
            strcpy_s(dirname, argv[1]);

            printf("Processing directory %s\n", dirname);

            // open the directory
            dirp = opendir(dirname);
            if (dirp == NULL) {
                printf("Cannot open directory %s\n", dirname);
                exit(-1);
            }
            int count = 0;
            // loop over all the files in the image file listing
            while ((dp = readdir(dirp)) != NULL) {
                // check if the file is an image
                if (strstr(dp->d_name, ".jpg") ||
                    strstr(dp->d_name, ".png") ||
                    strstr(dp->d_name, ".ppm") ||
                    strstr(dp->d_name, ".tif")) {
                    strcpy_s(buffer, dirname);
                    strcat_s(buffer, dp->d_name);

                    //reset file or not
                    int reset_file = 0;
                    if (count == 0) {
                        int reset_file = 1;
                        count += 1;
                    }
                    else {
                        int reset_file = 0;
                    }

                    //filename same for all
                    char image_filename[256];
                    strcpy_s(image_filename, dp->d_name);

                    //image read same for all
                    cv::Mat image_initial = cv::imread(buffer, cv::IMREAD_COLOR);
                    cv::Mat image;
                    cv::resize(image_initial, image, cv::Size(600, 600));

                    //baseline csv
                    if (save_type == "baseline") {
                        char baseline_csv[256] = "baseline_custom.csv";
                        std::vector<float> feat_vect;
                        baseline(image, feat_vect);
                        append_image_data_csv(baseline_csv, image_filename, feat_vect, reset_file);
                    }

                    //hist_matching 
                    if (save_type == "hist") {
                        char hist_matching_csv[256] = "hist_custom.csv";
                        std::vector<float> hist_vect;
                        int hist_size = 8;
                        hist_matching(image, hist_vect, hist_size);
                        append_image_data_csv(hist_matching_csv, image_filename, hist_vect, reset_file);
                    }

                    //multi_hist
                    if (save_type == "multihist") {

                        char multi_hist_matching_csv[256] = "multi_hist_custom.csv";
                        std::vector<float> multi_hist_vect;
                        int hist_size = 8;
                        multi_hist_matching(image, multi_hist_vect, hist_size);
                        append_image_data_csv(multi_hist_matching_csv, image_filename, multi_hist_vect, reset_file);

                    }

                    //texture and color
                    if (save_type == "texcolor") {
                        char texcolor_csv[256] = "tex_color_custom.csv";

                        cv::Mat xsobel_img;
                        sobelX3x3(image, xsobel_img);

                        cv::Mat ysobel_img;
                        sobelY3x3(image, ysobel_img);

                        cv::Mat magnitude_img;
                        magnitude(xsobel_img, ysobel_img, magnitude_img);

                        std::vector<float> color_vect;
                        int hist_size = 8;
                        hist_matching(image, color_vect, hist_size);

                        texture_and_color_vect(magnitude_img, color_vect);

                        append_image_data_csv(texcolor_csv, image_filename, color_vect, reset_file);

                    }
                }
            }
        }

        std::cout << "Run compare? : 1 or 0" << std::endl;
        int run;
        std::cin >> run;


        if (run == 1) {

            std::cout << "Compare type (baseline, hist, multihist, texcolor)" << std::endl;
            std::string compare_type;
            std::cin >> compare_type;

            //image name same for all
            std::cout << "Input image name" << std::endl;
            std::string img_name;
            std::cin >> img_name;
            img_name = img_name + ".jpg";

            //Target image location
            char target[256];
            strcpy(target, argv[1]);
            strcat(target, img_name.c_str());

            //input number of images to output
            std::cout << "Input number of images to output" << std::endl;
            int N;
            std::cin >> N;

            //baseline compare
            if (compare_type == "baseline") {

                char baseline_csv[256] = "baseline_custom.csv";
                std::vector<char*> filenames;
                std::vector<std::vector<float>> data;
                int echo_file = 0;
                read_image_data_csv(baseline_csv, filenames, data, echo_file);

                cv::Mat target_img = cv::imread(target, cv::IMREAD_COLOR);
                std::vector<float> target_feat_vect;
                baseline(target_img, target_feat_vect);

                std::vector<float> dist_vector;

                for (int i = 0; i < filenames.size(); i++) {
                    std::vector<float> row_vect;
                    for (int j = 0; j < data[i].size(); j++) {
                        row_vect.push_back(data[i][j]);
                    }
                    dist_vector.push_back(sum_of_sq_dist(target_feat_vect, row_vect));
                }

                std::vector< std::pair <float, char*> > combined_vect;

                for (int i = 0; i < filenames.size(); i++) {
                    combined_vect.push_back(std::make_pair(dist_vector[i], filenames[i]));
                }


                std::sort(combined_vect.begin(), combined_vect.end());

                char src[256];
                strcpy(src, argv[1]);
                std::string source = std::string(src);

                for (int i = 0; i < N; i++) {

                    cv::namedWindow(combined_vect[i].second, cv::WINDOW_NORMAL);
                    cv::resizeWindow(combined_vect[i].second, cv::Size(250, 250));

                    std::string imgpath = source + combined_vect[i].second;
                    cv::Mat image = cv::imread(imgpath, cv::IMREAD_COLOR);
                    cv::imshow(combined_vect[i].second, image);
                    cv::waitKey(10);

                }

                for (int j = 0; j < N; j++) {
                    std::cout << combined_vect[j].second << std::endl;
                }
            }

            //hist compare
            if (compare_type == "hist") {

                char hist_csv[256] = "hist_custom.csv";
                std::vector<char*> filenames;
                std::vector<std::vector<float>> data;
                int echo_file = 0;
                read_image_data_csv(hist_csv, filenames, data, echo_file);

                cv::Mat target_img = cv::imread(target, cv::IMREAD_COLOR);
                std::vector<float> target_feat_vect;
                int hist_size = 8;
                hist_matching(target_img, target_feat_vect, hist_size);

                std::vector<float> dist_vector;

                for (int i = 0; i < filenames.size(); i++) {
                    std::vector<float> row_vect = {};
                    for (int j = 0; j < data[i].size(); j++) {
                        row_vect.push_back(data[i][j]);
                    }
                    dist_vector.push_back(intersection_hist_matching(target_feat_vect, row_vect));
                }

                std::vector< std::pair <float, char*> > combined_vect;

                for (int i = 0; i < filenames.size(); i++) {
                    combined_vect.push_back(std::make_pair(dist_vector[i], filenames[i]));
                }

                std::sort(combined_vect.begin(), combined_vect.end());

                char src[256];
                strcpy(src, argv[1]);
                std::string source = std::string(src);

                for (int i = 0; i < N; i++) {

                    cv::namedWindow(combined_vect[i].second, cv::WINDOW_NORMAL);
                    cv::resizeWindow(combined_vect[i].second, cv::Size(250, 250));

                    std::string imgpath = source + combined_vect[i].second;
                    cv::Mat image = cv::imread(imgpath, cv::IMREAD_COLOR);
                    cv::imshow(combined_vect[i].second, image);
                    cv::waitKey(10);
                }

                for (int j = 0; j < N; j++) {
                    std::cout << combined_vect[j].second << std::endl;
                }


            }

            //multi hist compare
            if (compare_type == "multihist") {

                char hist_csv[256] = "multi_hist_custom.csv";
                std::vector<char*> filenames;
                std::vector<std::vector<float>> data;
                int echo_file = 0;
                read_image_data_csv(hist_csv, filenames, data, echo_file);

                cv::Mat target_img = cv::imread(target, cv::IMREAD_COLOR);
                std::vector<float> target_vect;
                int hist_size = 8;
                multi_hist_matching(target_img, target_vect, hist_size);

                std::vector<float> dist_vector;

                for (int i = 0; i < filenames.size(); i++) {
                    std::vector<float> row_vect = {};
                    for (int j = 0; j < data[i].size(); j++) {
                        row_vect.push_back(data[i][j]);
                    }
                    dist_vector.push_back(intersection_hist_matching(target_vect, row_vect));
                }

                std::vector< std::pair <float, char*> > combined_vect;

                for (int i = 0; i < filenames.size(); i++) {
                    combined_vect.push_back(std::make_pair(dist_vector[i], filenames[i]));
                }

                std::sort(combined_vect.begin(), combined_vect.end());

                char src[256];
                strcpy(src, argv[1]);
                std::string source = std::string(src);

                for (int i = 0; i < N; i++) {

                    cv::namedWindow(combined_vect[i].second, cv::WINDOW_NORMAL);
                    cv::resizeWindow(combined_vect[i].second, cv::Size(250, 250));

                    std::string imgpath = source + combined_vect[i].second;
                    cv::Mat image = cv::imread(imgpath, cv::IMREAD_COLOR);
                    cv::imshow(combined_vect[i].second, image);
                    cv::waitKey(10);
                }

                for (int j = 0; j < N; j++) {
                    std::cout << combined_vect[j].second << std::endl;
                }

            }

            //texture and color compare
            if (compare_type == "texcolor") {

                char hist_csv[256] = "tex_color_custom.csv";
                std::vector<char*> filenames;
                std::vector<std::vector<float>> data;
                int echo_file = 0;
                read_image_data_csv(hist_csv, filenames, data, echo_file);

                cv::Mat target_img = cv::imread(target, cv::IMREAD_COLOR);

                cv::Mat xsobel_img;
                sobelX3x3(target_img, xsobel_img);

                cv::Mat ysobel_img;
                sobelY3x3(target_img, ysobel_img);

                cv::Mat magnitude_img;
                magnitude(xsobel_img, ysobel_img, magnitude_img);

                std::vector<float> target_vect;
                int hist_size = 8;
                hist_matching(target_img, target_vect, hist_size);

                texture_and_color_vect(magnitude_img, target_vect);

                std::vector<float> dist_vector;

                for (int i = 0; i < filenames.size(); i++) {
                    std::vector<float> row_vect = {};
                    for (int j = 0; j < data[i].size(); j++) {
                        row_vect.push_back(data[i][j]);
                    }
                    dist_vector.push_back(intersection_hist_matching(target_vect, row_vect));
                }

                std::vector< std::pair <float, char*> > combined_vect;

                for (int i = 0; i < filenames.size(); i++) {
                    combined_vect.push_back(std::make_pair(dist_vector[i], filenames[i]));
                }

                std::sort(combined_vect.begin(), combined_vect.end());

                char src[256];
                strcpy(src, argv[1]);
                std::string source = std::string(src);

                for (int i = 0; i < N; i++) {

                    std::string num = std::to_string(i);
                    cv::namedWindow(combined_vect[i].second, cv::WINDOW_NORMAL);
                    cv::resizeWindow(combined_vect[i].second, cv::Size(250, 250));

                    std::string imgpath = source + combined_vect[i].second;
                    cv::Mat image = cv::imread(imgpath, cv::IMREAD_COLOR);
                    cv::imshow(combined_vect[i].second, image);
                    cv::waitKey(10);
                }
                for (int j = 0; j < N; j++) {
                    std::cout << combined_vect[j].second << std::endl;
                }

            }

        }

        std::cout << "Continue? : 1 or 0 " << std::endl;
        std::cin >> cont;
        cv::destroyAllWindows();
    }
    printf("Terminating\n");
    return(0);
}
