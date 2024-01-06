/*
Input - Takes color image of type cv::Mat with CV_8UC3
Output - Gives greyscale image of type cv::Mat with CV_8UC3
*/
int greyscale(cv::Mat& src, cv::Mat& dst);

/*
Input - Takes greyscale image of type cv::Mat with CV_8UC3
Output - Gives gaussian blur of kernel 5X5 greyscale image of type cv::Mat with CV_16SC3
*/
int blur5x5(cv::Mat& src, cv::Mat& dst);

/*
Input - Takes image of type cv::Mat on which sobelX is to be applied in cv::Mat format
Output - Gives sobelX image of type cv::Mat with CV_8USC3
*/
int sobelX3x3(cv::Mat& src, cv::Mat& dst);

/*
Input - Takes image of type cv::Mat on which sobelY is to be applied in cv::Mat format
Output - Gives sobelY image of type cv::Mat with CV_8USC3
*/
int sobelY3x3(cv::Mat& src, cv::Mat& dst);

/*
Input - Takes SobelX and SobelY of type cv::Mat with CV_16SC3
Output - Gives sobel magnitude image of type cv::Mat with CV_8UC3
*/
int magnitude(cv::Mat& sx, cv::Mat& sy, cv::Mat& dst);

/*
Input - Takes SobelX and SobelY of type cv::Mat with CV_16SC3
Output - Gives sobel magnitude image of type cv::Mat with CV_8UC3
*/
int blurQuantize(cv::Mat& src, cv::Mat& dst, int levels);

/*
Input - Takes SobelX and SobelY of type cv::Mat with CV_16SC3
Output - Gives sobel magnitude image of type cv::Mat with CV_8UC3
*/
int cartoon(cv::Mat& src, cv::Mat& dst, int levels, int magThreshold);

/*
Input - Takes grey image of type cv::Mat with CV_8UC3
Output - Gives max minus grey image of type cv::Mat with CV_8UC3
*/
int Negative(cv::Mat& src, cv::Mat& dst);

/*
Input - Takes grey image of type cv::Mat with CV_8UC3
Output - Gives a dilated image of type cv::Mat with CV_8UC3
*/
int dilation(cv::Mat& src, cv::Mat& dst);

/*
Input - Takes grey image of type cv::Mat with CV_8UC3
Output - Gives a eroded image of type cv::Mat with CV_8UC3
*/
int erosion(cv::Mat& src, cv::Mat& dst);