#include "alignment.h"

void MTBA(std::vector<Image>& images, int n)
{
	cv::Mat sample = images[0].image.clone();
	Brightness_Preserving(sample);
	std::vector<int> move_x(n, 0);
	std::vector<int> move_y(n, 0);

	/*calculate offset for remaining image*/
	for (int i = 1; i < n; i++) {
		cv::Mat bp_image = images[i].image.clone();
		Brightness_Preserving(bp_image);
		/*start pyramid*/
		for (int j = 5; j >= 0; j--) {
			cv::Mat origin_clone = sample.clone();
			cv::Mat current_clone = bp_image.clone();

			cv::Mat sampimg = sample.clone();
			cv::Mat otherimg = bp_image.clone();

			cv::Mat samp = origin_clone.clone();
			cv::Mat other = bp_image.clone();
			cv::resize(samp, samp, cv::Size((int)(sample.cols / pow(2, j + 1)), (int)(sample.rows / pow(2, j + 1))));
			cv::resize(samp, samp, cv::Size((int)(sample.cols / pow(2, j)), (int)(sample.rows / pow(2, j))));
			cv::resize(other, other, cv::Size((int)(sample.cols / pow(2, j + 1)), (int)(sample.rows / pow(2, j + 1))));
			cv::resize(other, other, cv::Size((int)(sample.cols / pow(2, j)), (int)(sample.rows / pow(2, j))));

			for (int a = 0; a < samp.rows; a++) {
				for (int b = 0; b < samp.cols; b++) {
					if (samp.at<uchar>(a, b) != sampimg.at<uchar>(a, b)) {
						origin_clone.at<uchar>(a, b) = 0;
					}
					if (other.at<uchar>(a, b) != otherimg.at<uchar>(a, b)) {
						current_clone.at<uchar>(a, b) = 0;
					}
					if (sampimg.at<uchar>(a, b) != otherimg.at<uchar>(a, b)) {
						current_clone.at<uchar>(a, b) = 0;
					}

				}
			}

			double total[9] = { 0 };

			for (int a = 0; a < origin_clone.rows; a++) {
				for (int b = 0; b < origin_clone.cols; b++) {
					int count = 0;
					for (int y = -1; y <= 1; y++) {
						for (int x = -1; x <= 1; x++) {
							if (b + move_x[i] + x > 0 && b + move_x[i] + x < current_clone.cols && a + move_y[i] + y > 0 && a + move_y[i] + y < current_clone.rows) {
								if ((int)origin_clone.at<uchar>(a, b) != (int)current_clone.at<uchar>(a + move_y[i] + y, b + move_x[i] + x))
									total[count]++;
							}
							count++;
						}
					}
				}
			}

			int b = 0;

			for (int a = 1; a < 9; a++)
				if (total[b] > total[a])
					b = a;

			/*stay middle*/
			if (total[b] * 1.5 >= total[4])
				b = 4;

			switch (b) {
			case 0:
				move_y[i] = (move_y[i] - 1) * 2;
				move_x[i] = (move_x[i] - 1) * 2;
				break;
			case 1:
				move_y[i] = (move_y[i] - 1) * 2;
				break;
			case 2:
				move_y[i] = (move_y[i] - 1) * 2;
				move_x[i] = (move_x[i] + 1) * 2;
				break;
			case 3:
				move_x[i] = (move_x[i] - 1) * 2;
				break;
			case 4:
				break;
			case 5:
				move_x[i] = (move_x[i] + 1) * 2;
				break;
			case 6:
				move_y[i] = (move_y[i] + 1) * 2;
				move_x[i] = (move_x[i] - 1) * 2;
				break;
			case 7:
				move_y[i] = (move_y[i] + 1) * 2;
				break;
			case 8:
				move_y[i] = (move_y[i] + 1) * 2;
				move_x[i] = (move_x[i] + 1) * 2;
				break;
			default:
				break;
			}
		}
		/*end of pyramid*/
	}
	/*end of calculation*/

	int min_x = *std::min_element(move_x.begin(), move_x.end());
	int max_x = *std::max_element(move_x.begin(), move_x.end());
	int min_y = *std::min_element(move_y.begin(), move_y.end());
	int max_y = *std::max_element(move_y.begin(), move_y.end());

	int b, i, j, k;
#pragma parallel for private(b, i, j, k)
	for (b = 0; b < n; b++) {
		cv::Mat&& dest = cv::Mat::zeros(sample.rows + abs(min_y) + abs(max_y), sample.cols + abs(min_x) + abs(max_x), CV_8UC3);
		for (i = 0; i < images[b].image.rows; i++) {
			for (j = 0; j < images[b].image.cols; j++) {
				for (k = 0; k < 3; k++) {
					dest.at<cv::Vec3b>(i + abs(min_y) + move_y[b], j + abs(min_x) + move_x[b])[k] = images[b].image.at<cv::Vec3b>(i, j)[k];
				}
			}
		}
		images[b].image = dest.clone();
	}
}

void Brightness_Preserving(cv::Mat& image)
{
	int color_count[256] = { 0 };
	cv::Mat gray;
	if (image.type() == CV_8UC3)
		cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

	else
		gray = image.clone();

	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < image.cols; j++) {
			color_count[gray.at<uchar>(i, j)]++;
		}
	}

	int count = 0;
	double threshold = 0;
	// find median
	for (int i = 0; i < image.rows; i++) {
		count += color_count[i];

		if (count >= (gray.cols * gray.rows) / 2) {
			threshold = i;
			break;
		}
	}

	cv::Mat&& dest = cv::Mat::zeros(gray.rows, gray.cols, CV_8UC1);

	for (int i = 0; i < image.rows; i++)
	{
		for (int j = 0; j < image.cols; j++)
		{
			gray.at<uchar>(i, j) > threshold ? dest.at<uchar>(i, j) = 255 : dest.at<uchar>(i, j) = 0;
		}
	}

	image = dest.clone();
	gray.release();
}
