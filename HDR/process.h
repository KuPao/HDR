#ifndef PROCESS
#define PROCESS

#include <string>
#include <vector>
#include <tuple>

#include <opencv2/opencv.hpp>

#include "ImageData.h"

cv::Mat process(std::tuple<std::vector<Image>, int, int> image_data);
cv::Mat cv_solve(std::vector<Image> images, int pixels, int rows, int cols, int channel, std::vector<int> random_index);
cv::Mat construct(std::vector<Image> images, std::vector<cv::Mat> g, int rows, int cols);
#endif
