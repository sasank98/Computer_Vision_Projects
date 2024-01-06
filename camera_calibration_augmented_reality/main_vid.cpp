
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;



int main(int argc, char* argv[]) {
    cv::VideoCapture* capdev;

    // open the video device
    capdev = new cv::VideoCapture(0);
    if (!capdev->isOpened()) {
        printf("Unable to open video device\n");
        return(-1);
    }

    // get some properties of the image
    cv::Size refS((int)capdev->get(cv::CAP_PROP_FRAME_WIDTH),
        (int)capdev->get(cv::CAP_PROP_FRAME_HEIGHT));
    printf("Expected size: %d %d\n", refS.width, refS.height);

    cv::namedWindow("Video", WINDOW_NORMAL); // identifies a window
    cv::namedWindow("altered", WINDOW_NORMAL);
    cv::resizeWindow("altered", cv::Size(400, 400));
    cv::Mat frame;

    std::string source = "chessimgs/";
    
    //counter for image name during saving 
    int count = 0;

    //Initializing corner list and point list only once 
    std::vector<std::vector<cv::Point2f> > corner_list;
    std::vector<std::vector<cv::Vec3f> > point_list;

    //Prompt user whether to save image or not
    std::cout << "Save image or not?: 1 or 0" << endl;
    int save_image;
    std::cin >> save_image;

    //Counter for number of images where chessboard corners are extracted
    int patternfound_num = 0;

    //Prompt user whether to Augment object on image or not
    cout << "Augment Yes or No?: 1 or 0" << endl;
    int aug_check;
    cin >> aug_check;
    
    std::string aug;
    if (aug_check == 1) {
        cout << "Choose Augmentation: 'axes', 'cube', 'pillars':" << endl;
        cin >> aug;
    }

    for (;;) {
        *capdev >> frame; // get a new frame from the camera, treat as a stream
        if (frame.empty()) {
            printf("frame is empty\n");
            break;
        }
        cv::imshow("Video", frame);

        // see if there is a waiting keystroke
        char key = cv::waitKey(10);

        if (key == 'q') {
            break;
        }

        if (key == 's') {

            Mat gray;
            bool patternfound;
            Size patternsize(9, 6); // width, height
            vector<Point2f> corners;
            vector<Vec3f> point_set;

            Mat recent_img;
            vector<Point2f> recent_corners;

            for (int i = 0; i < patternsize.height; i++)
            {
                for (int j = 0; j < patternsize.width; j++)
                {
                    point_set.push_back(Vec3f(float(j), float(i), 0.0));
                }
            }

            cvtColor(frame, gray, COLOR_BGR2GRAY);

            patternfound = findChessboardCorners(gray, patternsize, corners,
                CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE
                + CALIB_CB_FAST_CHECK);

            if (patternfound) {

                patternfound_num++;

                //saving image
                if (save_image == 1) {
                    string save_path = source + to_string(count) + ".jpg";
                    cv::imwrite(save_path, frame);
                    count++;
                    cv::imshow("altered", frame);
                }

                recent_img = frame.clone();
                recent_corners = corners;

                //extracting image corners and 3D world points and saving in respective list
                cornerSubPix(gray, corners, Size(11, 11), Size(-1, -1),
                    TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 30, 0.1));
                drawChessboardCorners(frame, patternsize, Mat(corners), patternfound);
                corner_list.push_back(corners);
                point_list.push_back(point_set);

                cv::imshow("altered", frame);
            }

            if (patternfound_num > 4) {
                std::cout << "Run Calibration on images: 1 or 0:" << endl;
                int run_calib;
                std::cin >> run_calib;

                if (run_calib == 1) {

                    int flag = CALIB_FIX_ASPECT_RATIO;
                    Mat dist_coeffs;
                    std::vector<cv::Mat> R, T;
                    Mat camera_matrix = Mat::eye(3, 3, CV_64F);
                    camera_matrix.at<double>(0, 2) = recent_img.cols / static_cast<double>(2);
                    camera_matrix.at<double>(1, 2) = recent_img.rows / static_cast<double>(2);

                    std::cout << "Before (Camera Matrix)" << endl << camera_matrix << endl;;
                    cout << "Before (Distortion Coefficients)" << endl << dist_coeffs << endl;
                    calibrateCamera(point_list, corner_list, recent_img.size(), camera_matrix, dist_coeffs, R, T, flag);
                    std::cout << "After (Camera Matrix)" << endl << camera_matrix << endl;
                    cout << "After (Distortion Coefficients)" << endl << dist_coeffs << endl;

                    Mat undistorted;
                    undistort(recent_img, undistorted, camera_matrix, dist_coeffs);

                    cv::namedWindow("original", WINDOW_NORMAL);
                    cv::resizeWindow("original", cv::Size(400, 400));

                    cv::imshow("original", recent_img);

                    cv::imshow("altered", undistorted);

                    // Compute the reprojection error
                    double totalError = 0;
                    for (size_t i = 0; i < point_list.size(); i++) {
                        std::vector<cv::Point2f> projectedPoints;
                        cv::projectPoints(point_list[i], R[i], T[i], camera_matrix, dist_coeffs, projectedPoints);

                        double error = cv::norm(projectedPoints, corner_list[i], cv::NORM_L2);
                        totalError += error;
                    }

                    double meanError = totalError / point_list.size();
                    std::cout << "Reprojection error: " << meanError << std::endl;

                    std::cout << "Save intrinsic parameters to file?: 1 or 0" << endl;
                    int save_params;
                    std::cin >> save_params;

                    if (save_params == 1) {
                        cv::FileStorage fs("intrinsics.yml", cv::FileStorage::WRITE);
                        fs << "cameraMatrix" << camera_matrix;
                        fs << "distCoeffs" << dist_coeffs;
                        fs.release();
                    }
                }
            }
        }

        if (key == 't') {

            cv::Mat cameraMatrix;
            cv::Mat distCoeffs;
            cv::FileStorage fs("intrinsics.yml", cv::FileStorage::READ);
            fs["cameraMatrix"] >> cameraMatrix;
            fs["distCoeffs"] >> distCoeffs;
            fs.release();

            Mat gray;
            bool patternfound;
            Size patternsize(9, 6); // width, height
            vector<Point2f> corners;
            vector<Vec3f> point_set;

            for (int i = 0; i < patternsize.height; i++)
            {
                for (int j = 0; j < patternsize.width; j++)
                {
                    point_set.push_back(Vec3f(float(j), float(i), 0.0));
                }
            }

            cvtColor(frame, gray, COLOR_BGR2GRAY);

            patternfound = findChessboardCorners(gray, patternsize, corners,
                CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE
                + CALIB_CB_FAST_CHECK);

            if (patternfound) {
                Mat orig_img = frame.clone();
                //extracting image corners and 3D world points and saving in respective list
                cornerSubPix(gray, corners, Size(11, 11), Size(-1, -1),
                    TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 30, 0.1));
                drawChessboardCorners(frame, patternsize, Mat(corners), patternfound);
                cv::imshow("altered", frame);

                cv::Mat R, T;

                bool spnp = solvePnP(point_set, corners, cameraMatrix, distCoeffs, R, T);

                std::cout << "Rotation matrix:" << endl << R << endl;
                std::cout << "Translation matrix:" << endl << T << endl;
                std::cout << endl;                

                std::vector<cv::Point3f> augment;
                
                if (aug == "axes") {                    
                    augment.push_back(cv::Point3f(0, 0, 0));
                    augment.push_back(cv::Point3f(5, 0, 0));
                    augment.push_back(cv::Point3f(0, 5, 0));
                    augment.push_back(cv::Point3f(0, 0, -5));

                    vector<Point2f> imagepoints;
                    cv::projectPoints(augment, R, T, cameraMatrix, distCoeffs, imagepoints);

                    for (int i = 1; i < imagepoints.size(); i++) {

                        cv::Point origin;
                        origin.x = imagepoints[0].x;
                        origin.y = imagepoints[0].y;

                        cv::Point pt;
                        pt.x = imagepoints[i].x;
                        pt.y = imagepoints[i].y;

                        cv::line(orig_img, origin, pt, cv::Scalar(0, 255, 0), 2);
                    }
                }

                if (aug == "cube") {
                    
                    augment.push_back(cv::Point3f(0, 0, 0)); // top left front 0
                    augment.push_back(cv::Point3f(3, 0, 0)); // top right front 1
                    augment.push_back(cv::Point3f(3, 3, 0)); // bottom right front 2
                    augment.push_back(cv::Point3f(0, 3, 0)); // bottom left front 3
                    augment.push_back(cv::Point3f(0, 0, -3)); // top left back 4
                    augment.push_back(cv::Point3f(3, 0, -3)); // top right back 5
                    augment.push_back(cv::Point3f(3, 3, -3)); // bottom right back 6
                    augment.push_back(cv::Point3f(0, 3, -3)); // bottom left back 7

                    vector<Point2f> imagepoints;
                    cv::projectPoints(augment, R, T, cameraMatrix, distCoeffs, imagepoints);

                    for (int i = 0; i < augment.size(); i++) {

                        if (i < 4) {
                            if (i != 3) {
                                cv::line(orig_img, imagepoints[i], imagepoints[i + 1], cv::Scalar(0, 0, 255), 2);
                            }
                            if (i == 3) {
                                cv::line(orig_img, imagepoints[i], imagepoints[0], cv::Scalar(0, 0, 255), 2);
                            }
                        }

                        if (i >= 4) {
                            if (i != 7) {
                                cv::line(orig_img, imagepoints[i], imagepoints[i + 1], cv::Scalar(0, 0, 255), 2);
                            }
                            if (i == 7) {
                                cv::line(orig_img, imagepoints[i], imagepoints[4], cv::Scalar(0, 0, 255), 2);

                            }
                        }

                        if (i < 4) {
                            cv::line(orig_img, imagepoints[i], imagepoints[i+4], cv::Scalar(0, 0, 255), 2);
                        }
                    }
                }

                if (aug == "pillars") {

                    int height = 0;                    
                    int dist = 9;                    
                    int topheight = -5;                    
                    int shape_dist = 12;                    

                    //left
                    augment.push_back(cv::Point3f(-1, 0, height)); // 0 (15)
                    augment.push_back(cv::Point3f(-1, 1, height)); // 1
                    augment.push_back(cv::Point3f(-2, 2, height)); // 2
                    augment.push_back(cv::Point3f(-3, 3, height)); // 3
                    augment.push_back(cv::Point3f(-4, 3, height)); // 4
                    augment.push_back(cv::Point3f(-5, 3, height)); // 5
                    augment.push_back(cv::Point3f(-6, 2, height)); // 6
                    augment.push_back(cv::Point3f(-7, 1, height)); // 7

                    augment.push_back(cv::Point3f(-7, 0, height)); // 8
                    augment.push_back(cv::Point3f(-7, -1, height)); // 9
                    augment.push_back(cv::Point3f(-6, -2, height)); // 10
                    augment.push_back(cv::Point3f(-5, -3, height)); // 11
                    augment.push_back(cv::Point3f(-4, -3, height)); // 12
                    augment.push_back(cv::Point3f(-3, -3, height)); // 13
                    augment.push_back(cv::Point3f(-2, -2, height)); // 14
                    augment.push_back(cv::Point3f(-1, -1, height)); // 15

                    //top left
                    for (int i = 0; i < 16; i++) {
                        cv::Point3f pt = augment[i];
                        augment.push_back(cv::Point3f(pt.x, pt.y, pt.z + topheight));
                    }

                    //right
                    for (int i = 0; i < 16; i++) {
                        cv::Point3f pt = augment[i];
                        augment.push_back(cv::Point3f(pt.x + shape_dist, pt.y, pt.z));
                    }  


                    //top left
                    for (int i = 0; i < 16; i++) {
                        cv::Point3f pt = augment[i];
                        augment.push_back(cv::Point3f(pt.x + shape_dist, pt.y, pt.z + topheight));
                    }           


                    vector<Point2f> imagepoints;
                    cv::projectPoints(augment, R, T, cameraMatrix, distCoeffs, imagepoints);


                    //Altering the target

                    std::vector<cv::Point3f> rectang;
                    rectang.push_back(cv::Point3f(-1, -1, 0));//1
                    rectang.push_back(cv::Point3f(9, -1, 0));//2
                    rectang.push_back(cv::Point3f(9, 7, 0));//4
                    rectang.push_back(cv::Point3f(-1, 7, 0));//3
                    

                    vector<Point2f> rectangpts;
                    cv::projectPoints(rectang, R, T, cameraMatrix, distCoeffs, rectangpts);

                    Point corner1(rectangpts[0].x, rectangpts[0].y);
                    Point corner2(rectangpts[1].x, rectangpts[1].y);
                    Point corner3(rectangpts[2].x, rectangpts[2].y);
                    Point corner4(rectangpts[3].x, rectangpts[3].y);

                    vector<Point> polygon_vertices;
                    polygon_vertices.push_back(corner1);
                    polygon_vertices.push_back(corner2);
                    polygon_vertices.push_back(corner3);
                    polygon_vertices.push_back(corner4);

                    cv::Mat mask(orig_img.rows, orig_img.cols, CV_8UC1, cv::Scalar(0));

                    std::vector<std::vector<cv::Point>> polygons = { polygon_vertices};
                    cv::fillPoly(mask, polygons, cv::Scalar(255));
                    
                    orig_img.setTo(cv::Scalar(255, 0, 0), mask);                   


                    //left pillar formed
                    int count0 = 16;
                    for (int i = 0; i < 16; i++) {
                        
                        cv::line(orig_img, imagepoints[i], imagepoints[count0], cv::Scalar(0, 0, 255), 2);

                        if(i < 15){
                            cv::line(orig_img, imagepoints[i], imagepoints[static_cast<std::vector<cv::Point2f, std::allocator<cv::Point2f>>::size_type>(i) + 1], cv::Scalar(0, 0, 255), 2);                        }

                        if (i == 15) {
                            cv::line(orig_img, imagepoints[i], imagepoints[0], cv::Scalar(0, 0, 255), 2);
                        }

                        if (count0 < 31) {
                            cv::line(orig_img, imagepoints[count0], imagepoints[static_cast<std::vector<cv::Point2f, std::allocator<cv::Point2f>>::size_type>(count0) + 1], cv::Scalar(0, 0, 255), 2);
                        }

                        if (count0 == 31) {
                            cv::line(orig_img, imagepoints[count0], imagepoints[16], cv::Scalar(0, 0, 255), 2);
                        }

                        count0++;
                    }


                    //right pillar formed
                    int count1 = 48;
                    for (int i = 32; i < 48; i++) {

                        cv::line(orig_img, imagepoints[i], imagepoints[count1], cv::Scalar(0, 0, 255), 2);

                        if (i < 47) {
                            cv::line(orig_img, imagepoints[i], imagepoints[static_cast<std::vector<cv::Point2f, std::allocator<cv::Point2f>>::size_type>(i) + 1], cv::Scalar(0, 0, 255), 2);
                        }

                        if (i == 47) {
                            cv::line(orig_img, imagepoints[i], imagepoints[32], cv::Scalar(0, 0, 255), 2);
                        }

                        if (count1 < 63) {
                            cv::line(orig_img, imagepoints[count1], imagepoints[static_cast<std::vector<cv::Point2f, std::allocator<cv::Point2f>>::size_type>(count1) + 1], cv::Scalar(0, 0, 255), 2);
                        }

                        if (count1 == 63) {
                            cv::line(orig_img, imagepoints[count1], imagepoints[48], cv::Scalar(0, 0, 255), 2);
                        }

                        count1++;
                    }
                }

                cv::namedWindow("3D", WINDOW_NORMAL);
                cv::resizeWindow("3D", cv::Size(400, 400));

                cv::imshow("3D", orig_img);


            }
            else {
                cout << "Chessboard not present in frame" << endl;
            }            
        }

        if (key == 'h') {
            cv::Mat corners;
            cv::Mat gray;

            cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
            cv::cornerHarris(gray, corners, 2, 3, 0.04);

            cv::Mat normalized_corners;
            cv::normalize(corners, normalized_corners, 0, 255, cv::NORM_MINMAX, CV_8UC1);
            cv::Mat thresholded_corners;
            cv::threshold(normalized_corners, thresholded_corners, 150, 255, cv::THRESH_BINARY);

            cv::imshow("altered", thresholded_corners);
        }
    }
    return 0;
}

   