

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include "functions.h"
#include "csv_util.h"

//Window size for all OpenCV windows when the program runs
int winsize = 200;


/*
The function first blurs the image then, converts the blurred image to grayscale.
Finally the all the pixels in the grayscale image are set to 255 if pixel value is less than threshold
else 0.

parameters: the input image 
			empty cv::Mat to save the final output
 
 */

int threshold(cv::Mat& frame, cv::Mat& thresholded) {

	cv::Mat temp = cv::Mat::zeros(frame.size(), CV_8UC3);
	cv::GaussianBlur(frame, temp, cv::Size(9, 9), 0, 0);
	thresholded = cv::Mat::zeros(frame.size(), CV_8UC1);
	int thrs = 50;		
	cv::cvtColor(temp, thresholded, cv::COLOR_BGR2GRAY);

	for (int i = 0; i < frame.rows; i++) {
		uchar* dptr = thresholded.ptr<uchar>(i);
		for (int j = 0; j < frame.cols; j++) {

			if (dptr[j] < thrs) {
				dptr[j] = 255;
			}
			else {
				dptr[j] = 0;
			}
		}
	}

	return 0;
}


/*
The function takes input a thresholded image and performs morphological filters in the followingorder:
first dilation and then erosion. The output is the cleaned image

parameters: the input thresholded image
			empty cv::Mat to save the final output

 */

int cleanup(cv::Mat& thresholded, cv::Mat& clean) {

	int morph_size = 5;
	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * morph_size + 1, 2 * morph_size + 1),
		cv::Point(-1, -1));
	cv::morphologyEx(thresholded, clean, cv::MORPH_CLOSE, kernel, cv::Point(-1, -1), 3);

	return 0;
}


/*
The function takes input a clean image after morphological operation and performs segmentation on the image.
Each object contour is given a different color. 

parameters: the input image after morphological operations
			empty cv::Mat to save the final output
			empty cv::Mat lb to save the corresponding labels of each segmented object is saved
			empty cv::Mat stats to save information about all the contours is saved
			empty cv::Mat centroid to save centroids of all the segmented regions are saved
			empty int n_labels to save the number of labeled contours
			empty std::vector<int> biggest_label to save a vector where the 0th element is the biggest contour's label
 */

int segment(cv::Mat& clean, cv::Mat& segmented_image, cv::Mat& lb, cv::Mat& stats, cv::Mat& centroids, int& n_labels, std::vector<int>& biggest_label) {

	n_labels = connectedComponentsWithStats(clean, lb, stats, centroids);
	
	cv::Mat areas = cv::Mat::zeros(1, n_labels - 1, CV_32S);
    cv::Mat sortedIdx;

    for (int i = 1; i < n_labels; i++) {
        int area = stats.at<int>(i, cv::CC_STAT_AREA);
        areas.at<int>(i - 1) = area;
    }
    if (areas.cols > 0) {
        sortIdx(areas, sortedIdx, cv::SORT_EVERY_ROW + cv::SORT_DESCENDING);
    }

	std::vector<cv::Vec3b> colors(n_labels);
	colors[0] = cv::Vec3b(0, 0, 0); 

    int N = 2; 
    N = (N < sortedIdx.cols) ? N : sortedIdx.cols;
	int thresh_area = 200;

    for (int i = 0; i < N; i++) {
        int label = sortedIdx.at<int>(i) + 1;
        if (stats.at<int>(label, cv::CC_STAT_AREA) > thresh_area) {
            colors[label] = cv::Vec3b(rand() % 256, rand() % 256, rand() % 256);
			biggest_label.push_back(label);
        }
    }

	segmented_image = cv::Mat::zeros(clean.size(), CV_8UC3);
	for (int i = 0; i < clean.rows; i++)
	{
		for (int j = 0; j < clean.cols; j++)
		{
			int label = lb.at<int>(i, j);
			segmented_image.at<cv::Vec3b>(i, j) = colors[label];
		}
	}

	return 0;
}


/*
The function takes input a cv::Mat image, centroid x and centroid y of the biggest contour in the image
and find the axis of least central moment.

parameters: the cv::Mat input image on which the axis of least central moment is drawn
			double x is the x centroid of the biggest contour in the image
			double y is the y centroid of the biggest contour in the image
			double alpha is the angle of the least central moment axis with horizontal
			cv::Scalar color is for giving arrow line color for drawing purposes

 */
void drawcentaxis(cv::Mat &image, double x, double y, double alpha, cv::Scalar color) {
    double length = 100.0;
    double edge1 = length * sin(alpha);
    double edge2 = sqrt(length * length - edge1 * edge1);
    double xPrime = x + edge2, yPrime = y + edge1;

    arrowedLine(image, cv::Point(x, y), cv::Point(xPrime, yPrime), color, 3);

}


/*
The function takes input a cv::Mat thresholdedimage and centroids of the biggest contour in the image
and draws a rotated bounding box over the image, calculates humoment of the biggest contour in the image
and saves the feature vector with humoment, perentage area filled in bounding box and ratio of height and
width of the bounding box. 

parameters: cv::Mat thresholded image to calculate bounding box and contours
			cv::Mat image to draw bounding box around the biggest contour in the image
			empty std::vector<double> featvect to save the feature vector of the object
			cv::Mat centroids to get the centroids of the biggest contour and use it in the feature vector calculation

 */
int drawbbox_calcmoment(cv::Mat& thresholded, cv::Mat& image, std::vector<double>& featvect, cv::Mat& centroids) {

	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(thresholded, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	std::vector<cv::RotatedRect> minRect(contours.size());

	int area = 0;
	int biggest_area_idx = 0;

	for (size_t i = 0; i < contours.size(); i++) {
		double newArea = cv::contourArea(contours.at(i));
		if (newArea > area) {
			area = newArea;
			biggest_area_idx = i;
		}
		minRect[i] = minAreaRect(contours[i]);
	}

	double minDistance = DBL_MAX;
	cv::Point2f pt1, pt2;
	
	cv::Point2f vertices[4];
	minRect[biggest_area_idx].points(vertices);
	for (int j = 0; j < 4; j++) {
		cv::Point2f p1 = vertices[j];
		cv::Point2f p2 = vertices[(j + 1) % 4];
		double distance = norm(p1 - p2);

		if (distance < minDistance) {
			minDistance = distance;
			pt1 = p1;
			pt2 = p2;
		}
	}

	cv::Scalar color = cv::Scalar(0, 255, 0);

	minRect[biggest_area_idx].points(vertices);
	for (int j = 0; j < 4; j++) {
		cv::line(image, vertices[j], vertices[(j + 1) % 4], color, 3);
	}

	cv::Moments moments = cv::moments(contours[biggest_area_idx], true);

	double centroidX = centroids.at<double>(1, 0);
    double centroidY = centroids.at<double>(1, 1);
    double alpha = 1.0 / 2.0 * atan2(2 * moments.mu11, moments.mu20 - moments.mu02);
	cv::Scalar line_color = cv::Scalar(0, 0, 255);

	drawcentaxis(image, centroidX, centroidY, alpha, line_color);

	double hu[7];
	HuMoments(moments, hu);
	
	cv::Rect bbox = minRect[biggest_area_idx].boundingRect();
	float precent_filled = cv::contourArea(contours.at(biggest_area_idx))/(bbox.height * bbox.width);
	float hwratio = float(bbox.height) / bbox.width;

	for (int i = 0; i < 7; i++) {
		featvect.push_back(hu[i]);
	}	
	featvect.push_back(precent_filled);
	featvect.push_back(hwratio);
	

	return 0;
}



/*
The function takes input two feature vectors and calculates the distance between them

parameters: std::vector<double> features1 is the first feature vector 
			std::vector<double> features2 is the second feature vector 
			empty double& final_val saves the distance between the two feature vectors
 */


int euclideanDistance(std::vector<double> features1, std::vector<double> features2, double& final_val) {
    double sum1 = 0;
	double sum2 = 0;
	double sumDifference = 0;
    for (int i = 0; i < features1.size(); i++) {
        sumDifference += (features1[i] - features2[i]) * (features1[i] - features2[i]);
        sum1 += features1[i] * features1[i];
        sum2 += features2[i] * features2[i];
    }
	final_val = sqrt(sumDifference) / (sqrt(sum1) + sqrt(sum2));
    return 0;
}