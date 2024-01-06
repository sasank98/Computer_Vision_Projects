#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>


int main() {

	std::string imgpath = "./doggie.jpg";

	cv::Mat image = cv::imread(imgpath, cv::IMREAD_COLOR);
	resize(image, image, { 500,500 }, 0, 0, cv::INTER_NEAREST);
	int a = 1;
	while (a == 1) {
		imshow("doggie", image);
		char key = cv::waitKey(10);
		if (key == 'q') {
			break;
		}
	}

	return 0;
}

