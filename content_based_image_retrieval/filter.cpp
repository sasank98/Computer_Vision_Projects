
#include<iostream>
#include<string>
#include<opencv2/opencv.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/videoio.hpp>
#include<opencv2/imgproc.hpp>
#include"filter.h"

int sobelX3x3(cv::Mat& src, cv::Mat& dst) {
    int row = src.rows;
    int col = src.cols;
    dst = cv::Mat::zeros(src.size(), CV_16SC3);
    cv::Mat temp = cv::Mat::zeros(src.size(), CV_16SC3);
    for (int i = 0;i < row;i++) {
        cv::Vec3b* sptr = src.ptr<cv::Vec3b>(i);
        cv::Vec3s* dptr = temp.ptr<cv::Vec3s>(i);

        for (int j = 1;j < col - 1;j++) {
            dptr[j][0] = -1 * sptr[j - 1][0] + 1 * sptr[j + 1][0];
            dptr[j][1] = -1 * sptr[j - 1][1] + 1 * sptr[j + 1][1];
            dptr[j][2] = -1 * sptr[j - 1][2] + 1 * sptr[j + 1][2];
        }
        dptr[0] = dptr[1];
        dptr[col - 1] = dptr[col - 2];
    }

    for (int i = 1;i < row - 1;i++) {
        cv::Vec3s* s1ptr = temp.ptr<cv::Vec3s>(i - 1);
        cv::Vec3s* s2ptr = temp.ptr<cv::Vec3s>(i);
        cv::Vec3s* s3ptr = temp.ptr<cv::Vec3s>(i + 1);
        cv::Vec3s* d1ptr = dst.ptr<cv::Vec3s>(i);

        for (int j = 0;j < col;j++) {
            d1ptr[j][0] = (1 * s1ptr[j][0] + 2 * s2ptr[j][0] + 1 * s3ptr[j][0]) / 4;
            d1ptr[j][1] = (1 * s1ptr[j][1] + 2 * s2ptr[j][1] + 1 * s3ptr[j][1]) / 4;
            d1ptr[j][2] = (1 * s1ptr[j][2] + 2 * s2ptr[j][2] + 1 * s3ptr[j][2]) / 4;
        }
    }
    cv::Vec3s* s1ptr = temp.ptr<cv::Vec3s>(1);
    cv::Vec3s* slastptr = temp.ptr<cv::Vec3s>(row - 2);

    cv::Vec3s* d1ptr = dst.ptr<cv::Vec3s>(0);
    cv::Vec3s* dlastptr = dst.ptr<cv::Vec3s>(row - 1);

    for (int j = 0; j < col; j++) {
        d1ptr[j] = s1ptr[j];
        dlastptr[j] = slastptr[j];
    }
    return 0;
}

/*
Input - Takes image of type cv::Mat on which sobelY is to be applied in cv::Mat format
Output - Gives sobelY image of type cv::Mat with CV_16SC3
*/
int sobelY3x3(cv::Mat& src, cv::Mat& dst) {
    int row = src.rows;
    int col = src.cols;
    dst = cv::Mat::zeros(src.size(), CV_16SC3);
    cv::Mat temp = cv::Mat::zeros(src.size(), CV_16SC3);
    for (int i = 0;i < row;i++) {
        cv::Vec3b* sptr = src.ptr<cv::Vec3b>(i);
        cv::Vec3s* dptr = temp.ptr<cv::Vec3s>(i);

        for (int j = 1;j < col - 1;j++) {
            dptr[j][0] = (1 * sptr[j - 1][0] + 2 * sptr[j][0] + 1 * sptr[j][0]) / 4;
            dptr[j][1] = (1 * sptr[j - 1][1] + 2 * sptr[j][1] + 1 * sptr[j][1]) / 4;
            dptr[j][2] = (1 * sptr[j - 1][2] + 2 * sptr[j][2] + 1 * sptr[j][2]) / 4;
        }
        dptr[0] = dptr[1];
        dptr[col - 1] = dptr[col - 2];
    }

    for (int i = 1;i < row - 1;i++) {
        cv::Vec3s* s1ptr = temp.ptr<cv::Vec3s>(i - 1);
        cv::Vec3s* s2ptr = temp.ptr<cv::Vec3s>(i);
        cv::Vec3s* s3ptr = temp.ptr<cv::Vec3s>(i + 1);
        cv::Vec3s* d1ptr = dst.ptr<cv::Vec3s>(i);

        for (int j = 0;j < col;j++) {
            d1ptr[j][0] = -1 * s1ptr[j][0] + 1 * s3ptr[j][0];
            d1ptr[j][1] = -1 * s1ptr[j][1] + 1 * s3ptr[j][1];
            d1ptr[j][2] = -1 * s1ptr[j][2] + 1 * s3ptr[j][2];
        }
    }

    cv::Vec3s* s1ptr = temp.ptr<cv::Vec3s>(1);
    cv::Vec3s* slastptr = temp.ptr<cv::Vec3s>(row - 2);

    cv::Vec3s* d1ptr = dst.ptr<cv::Vec3s>(0);
    cv::Vec3s* dlastptr = dst.ptr<cv::Vec3s>(row - 1);

    for (int j = 0; j < col; j++) {
        d1ptr[j] = s1ptr[j];
        dlastptr[j] = slastptr[j];
    }

    return 0;
}

/*
Input - Takes SobelX and SobelY of type cv::Mat with CV_16SC3
Output - Gives sobel magnitude image of type cv::Mat with CV_8UC3
*/

int magnitude(cv::Mat& sx, cv::Mat& sy, cv::Mat& dst) {
    dst = cv::Mat::zeros(sx.size(), CV_8UC3);
    for (int i = 0;i < sx.rows;i++) {
        cv::Vec3s* sxptr = sx.ptr<cv::Vec3s>(i);
        cv::Vec3s* syptr = sy.ptr<cv::Vec3s>(i);
        cv::Vec3b* dp = dst.ptr<cv::Vec3b>(i);
        for (int j = 0; j < sx.cols;j++) {
            dp[j][0] = pow((sxptr[j][0] * sxptr[j][0] + syptr[j][0] * syptr[j][0]), 0.5);
            dp[j][1] = pow((sxptr[j][1] * sxptr[j][1] + syptr[j][1] * syptr[j][1]), 0.5);
            dp[j][2] = pow((sxptr[j][2] * sxptr[j][2] + syptr[j][2] * syptr[j][2]), 0.5);
        }
    }
    return 0;
}

