#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include "filter.h"

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

    cv::namedWindow("Video", cv::WINDOW_NORMAL); // identifies a window
    cv::resizeWindow("Video",cv::Size (640,640));
    cv::namedWindow("Altered_vid", cv::WINDOW_NORMAL);
    cv::resizeWindow("Altered_vid",cv::Size (640,640));
    cv::Mat frame, bwframe;

    std::string name = "me.jpg";
    std::string source = "C:/Users/91991/OneDrive/Desktop/CS5330/Images_save/";
    std::string save_path = source + name;

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
            cv::imwrite(save_path, frame);
        }

        if (key == 'g') {
            cv::cvtColor(frame, bwframe, cv::COLOR_BGR2GRAY);
            cv::imshow("Altered_vid", bwframe);
        }

        if (key == 'h') {
            cv::Mat grey_img;
            greyscale(frame, grey_img);
            cv::imshow("Altered_vid", grey_img);
        }

        if (key == 'b') {
            cv::Mat blur_img;
            blur5x5(frame, blur_img);
            cv::imshow("Altered_vid", blur_img);
        }

        if (key == 'x') {
            cv::Mat xsobel_img;
            sobelX3x3(frame, xsobel_img);
            cv::convertScaleAbs(xsobel_img,xsobel_img);
            cv::imshow("Altered_vid", xsobel_img);
        }

        if (key == 'y') {
            cv::Mat ysobel_img;
            sobelY3x3(frame, ysobel_img);
            cv::convertScaleAbs(ysobel_img,ysobel_img);
            cv::imshow("Altered_vid", ysobel_img);
        }

        if (key == 'm') {
            cv::Mat xsobel_img;
            sobelX3x3(frame, xsobel_img);

            cv::Mat ysobel_img;
            sobelY3x3(frame, ysobel_img);

            cv::Mat magnitude_img;
            magnitude(xsobel_img, ysobel_img, magnitude_img);
            cv::imshow("Altered_vid", magnitude_img);
        }

        if (key == 'i') {
            int levels = 15;
            cv::Mat blurQuantize_img;
            blurQuantize(frame, blurQuantize_img, levels);
            cv::imshow("Altered_vid", blurQuantize_img);
        }

        if (key == 'c') {
            int levels = 15;
            int magthreshold = 15;
            cv::Mat cartoon_img;
            cartoon(frame, cartoon_img, levels, magthreshold);
            cv::imshow("Altered_vid", cartoon_img);
        }

        if (key == 'n') {
            cv::Mat negative_img;
            Negative(frame, negative_img);
            cv::imshow("Altered_vid", negative_img);
        }

        if (key == '1') {
            std::cout<<"hi";
            cv::Mat dilated_img;
            dilation(frame, dilated_img);
            cv::imshow("Altered_vid", dilated_img);


        }

        if (key == '2') {
            cv::Mat eroded_img;
            erosion(frame, eroded_img);
            cv::imshow("Altered_vid", eroded_img);


        }
    }

    delete capdev;
    return(0);
}

