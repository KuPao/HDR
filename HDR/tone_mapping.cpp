#include <algorithm>
#include <numeric>
#include <limits>
#include <cmath>

#include <opencv2/opencv.hpp>

#include "tone_mapping.h"

const double gamma_correction = 1.5;
double intensity = 0.0;
const double light_adaption = 0.0;
const double color_adaption = 0.0;

cv::Mat tone_mapping(cv::Mat hdr_image) {
  auto rows = hdr_image.rows;
  auto cols = hdr_image.cols;

  normalize(hdr_image, 1.0);

  std::vector<double> lum(rows * cols);
  std::vector<double> log_lum(rows * cols);
  double channel_mean[3] = { 0.0, 0.0, 0.0 };
  for (int i = 0; i != rows; ++i) {
    for (int j = 0; j != cols; ++j) {
      lum[i * cols + j] = 0.114 * hdr_image.at<cv::Vec3f>(i, j)[0] + 0.587 * hdr_image.at<cv::Vec3f>(i, j)[1] + 0.299 * hdr_image.at<cv::Vec3f>(i, j)[2];
      log_lum[i * cols + j] = std::log(0.00001 + lum[i * cols + j]);
      for (int channel = 0; channel != 3; ++channel) {
        channel_mean[channel] += hdr_image.at<cv::Vec3f>(i, j)[channel];
      }
    }
  }
  for (int channel = 0; channel != 3; ++channel)
    channel_mean[channel] /= lum.size();
  auto log_lum_minmax = std::minmax_element(log_lum.begin(), log_lum.end());
  double log_lum_min = log_lum[log_lum_minmax.first - log_lum.begin()];
  double log_lum_max = log_lum[log_lum_minmax.second - log_lum.begin()];
  double log_lum_mean = std::accumulate(log_lum.begin(), log_lum.end(), 0.0) / log_lum.size();
  double lum_mean = std::exp(log_lum_mean);

  intensity = std::exp(-intensity);
  double contrast = 0.3 + 0.7 * std::pow((log_lum_max - log_lum_mean) / (log_lum_max - log_lum_min), 1.4);

  for (int i = 0; i != rows; ++i) {
    for (int j = 0; j != cols; ++j) {
      for (int channel = 0; channel != 3; ++channel) {
        double i_l = color_adaption * hdr_image.at<cv::Vec3f>(i, j)[channel] + (1 - color_adaption) * lum[i * cols + j];
        double i_g = color_adaption * channel_mean[channel] + (1 - color_adaption) * lum_mean;
        double i_a = light_adaption * i_l + (1 - light_adaption) * i_g;
        hdr_image.at<cv::Vec3f>(i, j)[channel] /= (hdr_image.at<cv::Vec3f>(i, j)[channel] + std::pow(intensity * i_a, contrast));
      }
    }
  }

  normalize(hdr_image, gamma_correction);

  return hdr_image * 255;
}

void normalize(cv::Mat& image, double gamma_value) {
  auto rows = image.rows;
  auto cols = image.cols;

  double min_v = std::numeric_limits<double>::infinity();
  double max_v = -std::numeric_limits<double>::infinity();
  for (int i = 0; i != rows; ++i) {
    for (int j = 0; j != cols; ++j) {
      for (int channel = 0; channel != 3; ++channel) {
        min_v = std::min(min_v, static_cast<double>(image.at<cv::Vec3f>(i, j)[channel]));
        max_v = std::max(max_v, static_cast<double>(image.at<cv::Vec3f>(i, j)[channel]));
      }
    }
  }

  for (int i = 0; i != rows; ++i) {
    for (int j = 0; j != cols; ++j) {
      for (int channel = 0; channel != 3; ++channel) {
        if (max_v - min_v > DBL_EPSILON)
          image.at<cv::Vec3f>(i, j)[channel] = (image.at<cv::Vec3f>(i, j)[channel] - min_v) / (max_v - min_v);
        image.at<cv::Vec3f>(i, j)[channel] = std::pow(image.at<cv::Vec3f>(i, j)[channel], 1 / gamma_value);
      }
    }
  }
}
