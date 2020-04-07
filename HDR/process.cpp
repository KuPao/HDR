#include <string>
#include <vector>
#include <algorithm>
#include <tuple>
#include <cmath>
#include <iostream>
#include <fstream>
using namespace std;

#include <opencv2/opencv.hpp>

#include "process.h"

const int lambda = 10;
const int n = 100;

bool IsFiniteNumber(double x)
{
	return (x <= DBL_MAX && x >= -DBL_MAX);
}

cv::Mat process(std::tuple<std::vector<Image>, int, int> image_data) {
	auto images = std::get<0>(image_data);
	auto pixels = std::get<1>(image_data);
	auto p = std::get<2>(image_data);

	int rows = n * p + 255;
	int cols = 256 + n;

	std::srand(std::time(nullptr));
	std::vector<int> random_index;
	for (int i = 0; i != n; i++)
		random_index.push_back(std::rand() % pixels);

	std::vector<cv::Mat> g;
	for (int channel = 0; channel != 3; ++channel)
		g.push_back(cv_solve(images, pixels, rows, cols, channel, random_index));

	rows = images[0].image.rows;
	cols = images[0].image.cols;
	return construct(images, g, rows, cols);
}

int weight(int z) {
	if (z < 128)
		return z + 1;
	else
		return 256 - z;
}
cv::Mat cv_solve(std::vector<Image> images, int pixels, int rows, int cols, int channel, std::vector<int> random_index) {
	cv::Mat A = cv::Mat::zeros(rows, cols, CV_64F);
	cv::Mat b = cv::Mat::zeros(rows, 1, CV_64F);

	int it = 0;
	for (auto& element : images) {
		cv::Mat image = element.image;
		double t = element.expose_time;

		for (int i = 0; i != n; ++i) {
			int index = random_index[i];
			int z = image.data[image.channels() * index + channel];
			A.at<double>(it, z) = weight(z);
			A.at<double>(it, 256 + i) = -weight(z);

			b.at<double>(it, 0) = weight(z) * std::log(t);

			++it;
		}
	}

	A.at<double>(it++, 128) = 1;

	for (int i = 1; i != 255; ++i) {
		A.at<double>(it, i - 1) = lambda * weight(i);
		A.at<double>(it, i) = -2 * lambda * weight(i);
		A.at<double>(it, i + 1) = lambda * weight(i);
		++it;
	}

	cv::Mat x(A.rows, 1, CV_64F);
	// Ax=b
	cv::solve(A, b, x, cv::DECOMP_QR);

	return x;
}

cv::Mat construct(std::vector<Image> images, std::vector<cv::Mat> g, int rows, int cols) {
	cv::Mat hdr_image(rows, cols, CV_32FC3);

	for (int i = 0; i != rows; ++i) {
		for (int j = 0; j != cols; ++j) {
			double rad_sum[3] = { 0.0, 0.0, 0.0 };
			int weight_sum = 0;

			for (auto& element : images) {
				cv::Mat image = element.image;
				double t = element.expose_time;

				double weight_mean = 0.0;
				for (int channel = 0; channel != 3; ++channel) {
					int z = image.at<cv::Vec3b>(i, j)[channel];
					weight_mean += weight(z);
				}
				weight_mean /= 3;

				for (int channel = 0; channel != 3; ++channel) {
					int z = image.at<cv::Vec3b>(i, j)[channel];
					double x = g[channel].at<double>(z, 0);
					rad_sum[channel] += weight_mean * (x - std::log(t));
				}
				weight_sum += weight_mean;
			}

			for (int channel = 0; channel != 3; ++channel)
				hdr_image.at<cv::Vec3f>(i, j)[channel] = std::exp(rad_sum[channel] / weight_sum);
		}
	}

	return hdr_image;
}
