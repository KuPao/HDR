#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <vector>
#include <tuple>

#include <opencv2/opencv.hpp>

#include "load_images.h"
#include "alignment.h"

std::tuple<std::vector<Image>, int, int> load_images(std::string image_dir) {
	std::vector<Image> images;

	std::ifstream infile(image_dir + "image_list.txt");
	if (infile.fail()) {
		std::cerr << "fail to read file" << std::endl;
	}

	std::string line;
	int n;
	while (std::getline(infile, line)) {
		std::istringstream iss(line);
		std::string filename;
		double t;
		if (!(iss >> filename >> t)) { break; }

		cv::Mat image;
		image = cv::imread(image_dir + filename, CV_LOAD_IMAGE_COLOR);
		if (!image.data) {
			std::cout << "Could not open or find the image" << std::endl;
			break;
		}

		n = image.rows * image.cols;
		images.push_back(Image(image, 1 / t));
	}

	std::sort(begin(images), end(images), [](auto const& t1, auto const& t2) {
		return t1.expose_time > t2.expose_time;
		});

	MTBA(images, images.size());
	n = images[0].image.rows * images[0].image.cols;

	return std::make_tuple(images, n, images.size());
}
