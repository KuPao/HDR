#ifndef LOAD_IMAGES
#define LOAD_IMAGES

#include <string>
#include <vector>
#include <tuple>

#include <opencv2/opencv.hpp>
#include "ImageData.h"

std::tuple<std::vector<Image>, int, int> load_images(std::string image_list);
#endif
