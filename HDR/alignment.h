#ifndef ALIGN
#define ALIGN

#include <vector>
#include <opencv2/opencv.hpp>

#include "ImageData.h"

void MTBA(std::vector<Image>& images, int n);
void Brightness_Preserving(cv::Mat& src, cv::Mat& bp_image, cv::Mat& mask_img, int mask);
cv::Mat TranslateImage(cv::Mat& img, cv::Mat& dst, int offsetx, int offsety);
int BitmapTotal(cv::Mat& img);
void GetExpShift(cv::Mat& img1, cv::Mat& img2, int shift_bits, int shift_ret[2]);

#endif