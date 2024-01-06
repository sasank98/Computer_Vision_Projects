
/*
Input - Takes image of type cv::Mat on which sobelX is to be applied in cv::Mat format
Output - Gives sobelX image of type cv::Mat with CV_16SC3
*/
int sobelX3x3(cv::Mat& src, cv::Mat& dst);

/*
Input - Takes image of type cv::Mat on which sobelY is to be applied in cv::Mat format
Output - Gives sobelY image of type cv::Mat with CV_16SC3
*/
int sobelY3x3(cv::Mat& src, cv::Mat& dst);

/*
Input - Takes SobelX and SobelY of type cv::Mat with CV_16SC3
Output - Gives sobel magnitude image of type cv::Mat with CV_8UC3
*/
int magnitude(cv::Mat& sx, cv::Mat& sy, cv::Mat& dst);
