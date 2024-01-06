#include<iostream>
#include<string>
#include<opencv2/opencv.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/videoio.hpp>
#include<opencv2/imgproc.hpp>
#include"filter.h"

using std::max;

int greyscale(cv::Mat& src, cv::Mat& dst) {
    dst = cv::Mat::zeros(src.rows, src.cols, CV_8UC1);
    for (int i = 0; i < src.rows;i++) {
        cv::Vec3b* srcptr = src.ptr<cv::Vec3b>(i);
        uchar* dstptr = dst.ptr<uchar>(i);
        for (int j = 0; j < src.cols;j++) {
            dstptr[j] = (srcptr[j][0] + srcptr[j][1] + srcptr[j][2])/3;
        }
    }
    return 0;
}   


int blur5x5(cv::Mat& src, cv::Mat& dst) {
    int row = src.rows;
    int col = src.cols;
    dst = cv::Mat::zeros(row, col, CV_8UC3);
    cv::Mat temp = cv::Mat::zeros(row, col, CV_8UC3);
    for (int i = 0;i < row;i++) {
        cv::Vec3b* sptr = src.ptr<cv::Vec3b>(i);
        cv::Vec3b* dptr = temp.ptr<cv::Vec3b>(i);

        for (int j = 2;j < col - 2;j++) {

            for (int c = 0;c < 3;c++) {
                dptr[j][c] = (1 * sptr[j - 2][c] + 2 * sptr[j - 1][c] + 4 * sptr[j][c] + 2 * sptr[j + 1][c] + 1 * sptr[j + 2][c]) / 10;
            }
        }
        dptr[0] = dptr[2];
        dptr[1] = dptr[3];
        dptr[col - 1] = dptr[col - 3];
        dptr[col - 2] = dptr[col - 4];
    }

    for (int i = 2;i <= row - 2;i++) {
        cv::Vec3b* s1ptr = temp.ptr<cv::Vec3b>(i - 2);
        cv::Vec3b* s2ptr = temp.ptr<cv::Vec3b>(i - 1);
        cv::Vec3b* s3ptr = temp.ptr<cv::Vec3b>(i);
        cv::Vec3b* s4ptr = temp.ptr<cv::Vec3b>(i + 1);
        cv::Vec3b* s5ptr = temp.ptr<cv::Vec3b>(i + 2);
        cv::Vec3b* d1ptr = dst.ptr<cv::Vec3b>(i);

        for (int j = 0;j < col;j++) {
            for (int c = 0;c < 3;c++) {
                d1ptr[j][c] = (1 * s1ptr[j][c] + 2 * s2ptr[j][c] + 4 * s3ptr[j][c] + 2 * s4ptr[j][c] + 1 * s5ptr[j][c]) / 10;
            }
        }
    }
    cv::Vec3b* s2ptr = temp.ptr<cv::Vec3b>(2);
    cv::Vec3b* s3ptr = temp.ptr<cv::Vec3b>(3);

    cv::Vec3b* d1ptr = dst.ptr<cv::Vec3b>(0);
    cv::Vec3b* d2ptr = dst.ptr<cv::Vec3b>(1);

    for (int j = 0; j < col; j++) {
        d1ptr[j] = s2ptr[j];
        d2ptr[j] = s3ptr[j];

    }
    return 0;

}

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

    for (int i = 1;i < row-1;i++) {
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

int blurQuantize(cv::Mat& src, cv::Mat& dst, int levels) {
    int b = 255 / levels;
    cv::Mat blurred;
    blur5x5(src, blurred);
    dst = cv::Mat::zeros(src.size(), CV_8UC3);
    for (int i = 0;i < src.rows;i++) {
        cv::Vec3b* sptr = blurred.ptr<cv::Vec3b>(i);
        cv::Vec3b* dptr = dst.ptr<cv::Vec3b>(i);
        for (int j = 0;j < src.cols;j++) {
            dptr[j][0] = ((sptr[j][0]) / b) * b;
            dptr[j][1] = ((sptr[j][1]) / b) * b;
            dptr[j][2] = ((sptr[j][2]) / b) * b;
        }
    }
    return 0;
}

int cartoon(cv::Mat& src, cv::Mat& dst, int levels, int magThreshold) {
    cv::Mat sobel_x, sobel_y, gradient_mag, blur_quantize;
    sobelX3x3(src, sobel_x);
    sobelY3x3(src, sobel_y);
    magnitude(sobel_x, sobel_y, gradient_mag);
    blurQuantize(src, blur_quantize, levels);
    dst = cv::Mat::zeros(src.size(), CV_8UC3);
    for (int i = 0; i < src.rows;i++) {
        cv::Vec3b* gradmag_ptr = gradient_mag.ptr<cv::Vec3b>(i);
        cv::Vec3b* blurquant_ptr = blur_quantize.ptr<cv::Vec3b>(i);
        cv::Vec3b* dptr = dst.ptr<cv::Vec3b>(i);
        for (int j = 0; j < src.cols;j++) {
            for (int c = 0;c < 3;c++) {
                if (gradmag_ptr[j][c] > magThreshold) {
                    dptr[j][c] = 0;
                }
                else {
                    dptr[j][c] = blurquant_ptr[j][c];
                }
            }
        }
    }
    return 0;
}

int Negative(cv::Mat& src, cv::Mat& dst) {
    dst = cv::Mat::zeros(src.size(), CV_8UC3);
    for (int i = 0; i < src.rows;i++) {
        cv::Vec3b* sptr = src.ptr<cv::Vec3b>(i);
        cv::Vec3b* dptr = dst.ptr<cv::Vec3b>(i);
        for (int j = 0;j < src.cols;j++) {
            for (int c = 0;c < 3;c++) {
                dptr[j][c] = 255 - sptr[j][c];
            }
        }
    }
    return 0;
}

int dilation(cv::Mat& src, cv::Mat& dst) {
    dst = cv::Mat::zeros(src.size(), CV_8UC3);
    int temp[5][5]; 
    for (int i = 2; i < src.rows - 2;i++) {
        cv::Vec3b* dptr = dst.ptr<cv::Vec3b>(i);

        for (int j = 2; j < src.cols - 2; j++) {
            int prev_val = 0; 

            for (int k = 0; k < 5; k++) {

                cv::Vec3b* sptr = src.ptr<cv::Vec3b>(i + k - 2);

                for (int l = 0; l < 5; l++) {
                    
                    temp[k][l] = (sptr[j + l - 2][0] + sptr[j + l - 2][1] + sptr[j + l - 2][2])/3;
        
                    if (temp[k][l] > prev_val) {
                        dptr[j] = sptr[j + l - 2];
                        prev_val = temp[k][l];
                        }
                    }
                }
            }
    }
    return 0;
}

int erosion(cv::Mat& src, cv::Mat& dst) {
    dst = cv::Mat::zeros(src.size(), CV_8UC3);
    int temp[5][5]; 
    for (int i = 2; i < src.rows - 2;i++) {
        cv::Vec3b* dptr = dst.ptr<cv::Vec3b>(i);

        for (int j = 2; j < src.cols - 2; j++) {
            int prev_val = 255; 

            for (int k = 0; k < 5; k++) {

                cv::Vec3b* sptr = src.ptr<cv::Vec3b>(i + k - 2);

                for (int l = 0; l < 5; l++) {
                    
                    temp[k][l] = (sptr[j + l - 2][0] + sptr[j + l - 2][1] + sptr[j + l - 2][2])/3;
        
                    if (temp[k][l] < prev_val) {
                        dptr[j] = sptr[j + l - 2];
                        prev_val = temp[k][l];
                        }
                    }
                }
            }
    }
    return 0;
}