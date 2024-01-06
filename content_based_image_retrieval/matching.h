

//Task 1------------------------------------------------------------

/*
Input - Takes image of type cv::Mat as input
Output - Gives 1D feature vector of type float which is essentially a flattened 9x9x3 kernel
*/
int baseline(cv::Mat& src, std::vector<float>& feat_vect);

/*
Input - Takes two 1D feature vectors of type float
Output - Gives a distance value of type float following sum of squared distances metric between the two vectors
*/
float sum_of_sq_dist(std::vector<float>& feat_vect1, std::vector<float>& feat_vect2);

//Task 2------------------------------------------------------------

/*
Input - Takes image of type cv::Mat and histogram size of type int as inputs
Output - Gives 1D feature vector of type float which is essentially a flattened 3D histogram of size 8x8x8
*/
int hist_matching(cv::Mat& src, std::vector<float>& hist_vect, int& hist_size);

/*
Input - Takes two 1D feature vector of type float as inputs
Output - Gives a distance value of type float following histogram intersection metric
*/
float intersection_hist_matching(std::vector<float>& vect_target, std::vector<float>& vect_db_img);

//Task 3------------------------------------------------------------

/*
Input - Takes image of type cv::Mat and histogram size of type int as inputs
Output - Gives 1D feature vector of type float which is essentially a flattened and appended version of
		 two 3D histogram of size 8x8x8 (one for upper part of image and one for lower part)
*/

int multi_hist_matching(cv::Mat& src, std::vector<float>& multi_hist_vect, int& hist_size);

//Task 4------------------------------------------------------------

/*
Input - Takes sobel magnitude image of type cv::Mat and a 1D color vector acquired
		using hist_mathcing function as input
Output - Gives 1D feature vector of type float which is essentially a flattened and appended version of
		 a 2D histogram of texture and the color vector
*/

int texture_and_color_vect(cv::Mat& sobel_mag, std::vector<float>& color_vect);