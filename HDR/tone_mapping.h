#ifndef TONE_MAPPING
#define TONE_MAPPING

#include <opencv2/opencv.hpp>

cv::Mat tone_mapping(cv::Mat hdr_image);
void normalize(cv::Mat& image, double gamma_value);
#endif
