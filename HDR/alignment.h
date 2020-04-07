#ifndef ALIGN
#define ALIGN

#include <vector>
#include <opencv2/opencv.hpp>

#include "ImageData.h"

void MTBA(std::vector<Image>& images, int n);
void Brightness_Preserving(cv::Mat& image);

#endif