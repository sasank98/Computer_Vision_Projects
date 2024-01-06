/*
The function first blurs the image then, converts the blurred image to grayscale.
Finally the all the pixels in the grayscale image are set to 255 if pixel value is less than threshold
else 0.

parameters: the input image
			empty cv::Mat to save the final output

 */
int threshold(cv::Mat& src, cv::Mat& dst);




/*
The function takes input a thresholded image and performs morphological filters in the followingorder:
first dilation and then erosion. The output is the cleaned image

parameters: the input thresholded image
			empty cv::Mat to save the final output

 */
int cleanup(cv::Mat& src, cv::Mat& dst);




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
int segment(cv::Mat& clean, cv::Mat& segmented_image, cv::Mat& lb, cv::Mat& stats, cv::Mat& centroids, int& n_labels, std::vector<int>& biggest_label);




/*
The function takes input a cv::Mat image, centroid x and centroid y of the biggest contour in the image
and find the axis of least central moment.

parameters: the cv::Mat input image on which the axis of least central moment is drawn
			double x is the x centroid of the biggest contour in the image
			double y is the y centroid of the biggest contour in the image
			double alpha is the angle of the least central moment axis with horizontal
			cv::Scalar color is for giving arrow line color for drawing purposes

 */
void drawcentaxis(cv::Mat& image, double x, double y, double alpha, cv::Scalar color);




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
int drawbbox_calcmoment(cv::Mat& thresholded, cv::Mat& image, std::vector<double>& featvect, cv::Mat& centroids);





/*
The function takes input two feature vectors and calculates the distance between them

parameters: std::vector<double> features1 is the first feature vector
			std::vector<double> features2 is the second feature vector
			empty double& final_val saves the distance between the two feature vectors
 */
int euclideanDistance(std::vector<double> features1, std::vector<double> features2, double& final_val);