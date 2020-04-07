#ifndef IMAGE_DATA
#define IMAGE_DATA
#include <opencv2/opencv.hpp>

class Image {
public:
	cv::Mat image;
	double expose_time;

	Image(cv::Mat i_image, double i_expose_time) :image(i_image), expose_time(i_expose_time)
	{}
};

#endif