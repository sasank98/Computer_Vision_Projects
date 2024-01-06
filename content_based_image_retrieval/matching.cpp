
#include<iostream>
#include<string>
#include<opencv2/opencv.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/videoio.hpp>
#include<opencv2/imgproc.hpp>
#include"matching.h"

/*
Input - Takes image of type cv::Mat as input
Output - Gives 1D feature vector of type float which is essentially a flattened 9x9x3 kernel
*/

int baseline(cv::Mat& src, std::vector<float>& feat_vect) {

	int middle_pix_row = src.rows/2 -5;
	int middle_pix_col = src.cols/2 -5;

	for (int i = middle_pix_row; i < middle_pix_row + 9; i++) {
		cv::Vec3b* srcptr = src.ptr<cv::Vec3b>(i);
		for (int j = middle_pix_col; j < middle_pix_col + 9; j++) {
			feat_vect.push_back(srcptr[j][0]);
			feat_vect.push_back(srcptr[j][1]);
			feat_vect.push_back(srcptr[j][2]);
		}
	}
	return 0;
}

/*
Input - Takes two 1D feature vectors of type float
Output - Gives the distance of type float following sum of squared distances metric between the two vectors
*/
float sum_of_sq_dist(std::vector<float>& feat_vect1, std::vector<float>& feat_vect2) {
		float dist = 0;

		for (int i = 0 ; i < feat_vect1.size() ; i++) {
			dist += std::pow((feat_vect1[i] - feat_vect2[i]), 2);
		}
		return dist;
}

/*
Input - Takes image of type cv::Mat and histogram size of type int as inputs
Output - Gives 1D feature vector of type float which is essentially a flattened 3D histogram of size 8x8x8
*/

int hist_matching(cv::Mat& src, std::vector<float>& hist_vect, int& hist_size) {
	
	const int divisor = 256 / hist_size; 

	int dims[3] = { hist_size,hist_size,hist_size }; //8

	cv::Mat hist = cv::Mat::zeros(3, dims, CV_32S);

	for (int i = 0; i < src.rows; i++) {
		cv::Vec3b* srcptr = src.ptr<cv::Vec3b>(i);
		for (int j = 0; j < src.cols; j++) {
			int r = srcptr[j][2] / divisor;
			int g = srcptr[j][1] / divisor;
			int b = srcptr[j][0] / divisor;
			hist.at<int>(r, g, b)++;
		}
	}

	//flatten
	for (int i = 0; i < hist_size; i++) {
		for (int j = 0; j < hist_size; j++) {
			for (int k = 0; k < hist_size; k++) {
				hist_vect.push_back(hist.at<int>(i, j, k));
			}
		}
	}
	int norm_coeff = 0;
	//norm_coeff calculation
	for (int i = 0; i < hist_vect.size(); i++) {
		norm_coeff += hist_vect[i];
	}
	//normalising
	for (int i = 0; i < hist_vect.size(); i++) {
		hist_vect[i] /= norm_coeff;
	}

	return 0;
}

/*
Input - Takes two 1D feature vector of type float as inputs
Output - Gives a distance value of type float following histogram intersection metric
*/

float intersection_hist_matching(std::vector<float>&vect_target, std::vector<float>&vect_db_img ) {
	float dist = 0;
	
	for (int i = 0; i < vect_target.size(); i++) {
		dist += vect_target[i] < vect_db_img[i] ? vect_target[i] : vect_db_img[i];
	}
	
	return 1-dist;
}

/*
Input - Takes image of type cv::Mat and histogram size of type int as inputs
Output - Gives 1D feature vector of type float which is essentially a flattened and appended version of 
		 two 3D histogram of size 8x8x8 (one for upper part of image and one for lower part) 
*/

int multi_hist_matching(cv::Mat& src, std::vector<float>& multi_hist_vect, int& hist_size) {

	std::vector<float> hist_vect_upper; 
	std::vector<float> hist_vect_lower;
	
	const int divisor = 256 / hist_size;
	int dims[3] = { hist_size,hist_size,hist_size }; //8

	int r = src.rows % 2 == 0 ? src.rows / 2 : (src.rows + 1) / 2;

	cv::Mat histupper = cv::Mat::zeros(3, dims, CV_32S);
	cv::Mat histlower = cv::Mat::zeros(3, dims, CV_32S);

	for (int i = 0; i < r; i++) {
		cv::Vec3b* srcptr = src.ptr<cv::Vec3b>(i);
		for (int j = 0; j < src.cols; j++) {
			int r = srcptr[j][2] / divisor;
			int g = srcptr[j][1] / divisor;
			int b = srcptr[j][0] / divisor;
			histupper.at<int>(r, g, b)++;
		}
	}

	for (int i = r; i < src.rows; i++) {
		cv::Vec3b* srcptr = src.ptr<cv::Vec3b>(i);
		for (int j = 0; j < src.cols; j++) {
			int r = srcptr[j][2] / divisor;
			int g = srcptr[j][1] / divisor;
			int b = srcptr[j][0] / divisor;
			histlower.at<int>(r, g, b)++;
		}
	}

	//flatten
	for (int i = 0; i < hist_size; i++) {
		for (int j = 0; j < hist_size; j++) {
			for (int k = 0; k < hist_size; k++) {
				hist_vect_upper.push_back(histupper.at<int>(i, j, k));
				hist_vect_lower.push_back(histlower.at<int>(i, j, k));
			}
		}
	}

	int norm_coeff_upper = 0;
	int norm_coeff_lower = 0;

	//norm_coeff calculation
	for (int i = 0; i < hist_vect_upper.size(); i++) {
		norm_coeff_upper += hist_vect_upper[i];
		norm_coeff_lower += hist_vect_lower[i];
	}

	//normalising
	for (int i = 0; i < hist_vect_upper.size(); i++) {
		hist_vect_upper[i] /= norm_coeff_upper;
		hist_vect_lower[i] /= norm_coeff_lower;
	}

	hist_vect_upper.insert(hist_vect_upper.end(), hist_vect_lower.begin(), hist_vect_lower.end());
	multi_hist_vect = hist_vect_upper;

	return 0;
}

/*
Input - Takes sobel magnitude image of type cv::Mat and a 1D color vector acquired 
		using hist_mathcing function as input
Output - Gives 1D feature vector of type float which is essentially a flattened and appended version of
		 a 2D histogram of texture and the color vector 
*/

int texture_and_color_vect(cv::Mat& sobel_mag, std::vector<float>& color_vect) {
	
	cv::Mat texture_hist = cv::Mat::zeros(1, 256, CV_32S);
	
	std::vector<float> texture_vect;


	for (int i = 0; i<sobel_mag.rows; i++) {
		cv::Vec3b* sobelptr = sobel_mag.ptr<cv::Vec3b>(i);
		for (int j = 0; j < sobel_mag.cols; j++) {
			
			int r = sobelptr[j][2];
			int g = sobelptr[j][1];
			int b = sobelptr[j][0];

			int val = std::max(r, g);
			val = std::max(b, val);

			texture_hist.at<int>(0,val)++;
		}
	}

	for (int i = 0; i < texture_hist.rows; i++) {
		for (int j = 0; j < texture_hist.cols; j++) {
			texture_vect.push_back(texture_hist.at<int>(i, j));
		}
	}

	int norm_coeff = 0;

	for (int i = 0; i < texture_vect.size(); i++) {
		norm_coeff += texture_vect[i];
	}

	for (int i = 0; i < texture_vect.size(); i++) {
		texture_vect[i] /= norm_coeff;
	}

	color_vect.insert(color_vect.end(), texture_vect.begin(), texture_vect.end());

	return 0;
}















