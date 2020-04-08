#include "alignment.h"

void MTBA(std::vector<Image>& images, int n)
{
	int shift_amt[2];
	for (int i = 0; i < n - 1; i++) {
		GetExpShift(images[i].image, images[i + 1].image, 4, shift_amt);
		TranslateImage(images[i + 1].image, images[i + 1].image, shift_amt[0], shift_amt[1]);
	}
}

void Brightness_Preserving(cv::Mat& src, cv::Mat& bp_image, cv::Mat& mask_img, int mask)
{
	int color_count[256] = { 0 };
	bp_image = src.clone();
	cv::Mat gray;
	if (bp_image.type() == CV_8UC3)
		cv::cvtColor(bp_image, gray, cv::COLOR_BGR2GRAY);

	else
		gray = bp_image.clone();

	for (int i = 0; i < bp_image.rows; i++) {
		for (int j = 0; j < bp_image.cols; j++) {
			color_count[gray.at<uchar>(i, j)]++;
		}
	}

	int count = 0;
	double threshold = 0;
	// find median
	for (int i = 0; i < bp_image.rows; i++) {
		count += color_count[i];

		if (count >= (gray.cols * gray.rows) / 2) {
			threshold = i;
			break;
		}
	}

	cv::Mat&& dest = cv::Mat::zeros(gray.rows, gray.cols, CV_8UC1);
	mask_img = cv::Mat::zeros(gray.rows, gray.cols, CV_8UC1);

	for (int i = 0; i < bp_image.rows; i++)
	{
		for (int j = 0; j < bp_image.cols; j++)
		{
			gray.at<uchar>(i, j) > threshold ? dest.at<uchar>(i, j) = 255 : dest.at<uchar>(i, j) = 0;
			gray.at<uchar>(i, j) >= threshold + mask &&
				gray.at<uchar>(i, j) <= threshold - mask ? mask_img.at<uchar>(i, j) = 255 : mask_img.at<uchar>(i, j) = 0;
		}
	}

	bp_image = dest.clone();
	gray.release();
}

cv::Mat TranslateImage(cv::Mat& img, cv::Mat& dst, int offsetx, int offsety) {
	cv::Mat trans_mat = (cv::Mat_<double>(2, 3) << 1, 0, offsetx, 0, 1, offsety);
	cv::warpAffine(img, dst, trans_mat, img.size());
	return dst;
}

/*Compute the sum of all white pixel in the bitmap.*/
int BitmapTotal(cv::Mat& img) {
	int sum = 0;
	for (int i = 0; i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			sum += img.at<uchar>(i, j);
		}
	}
	return sum / 255;
}

void GetExpShift(cv::Mat& img1, cv::Mat& img2, int shift_bits, int shift_ret[2]) {
	int min_err;
	int cur_shift[2];
	cv::Mat tb1, tb2;
	cv::Mat eb1, eb2;
	int i, j;
	if (shift_bits > 0) {
		cv::Mat sml_img1, sml_img2;
		cv::resize(img1, sml_img1, cv::Size((int)img1.cols / 2, (int)img1.rows / 2));
		cv::resize(img2, sml_img2, cv::Size((int)img2.cols / 2, (int)img2.rows / 2));
		GetExpShift(sml_img1, sml_img2, shift_bits - 1, cur_shift);
		cur_shift[0] *= 2;
		cur_shift[1] *= 2;
	}
	else {
		cur_shift[0] = 0;
		cur_shift[1] = 0;
	}

	Brightness_Preserving(img1, tb1, eb1, 4);
	Brightness_Preserving(img2, tb2, eb2, 4);
	min_err = img1.rows * img1.cols;

	for (i = -1; i <= 1; i++) {
		for (j = -1; j <= 1; j++) {
			int xs = cur_shift[0] + i;
			int ys = cur_shift[1] + j;
			cv::Mat shifted_tb2;
			cv::Mat shifted_eb2;
			cv::Mat diff_b;
			int err;
			//printf("xs = %d, ys = %d\n",xs,ys);
			(void)TranslateImage(tb2, shifted_tb2, xs, ys);
			(void)TranslateImage(eb2, shifted_eb2, xs, ys);
			cv::bitwise_xor(tb1, shifted_tb2, diff_b);
			cv::bitwise_and(diff_b, eb1, diff_b);
			cv::bitwise_and(diff_b, shifted_eb2, diff_b);
			err = BitmapTotal(diff_b);
			if (i == 0 && j == 0)
				err -= 1;
			if (err < min_err) {
				shift_ret[0] = xs;
				shift_ret[1] = ys;
				min_err = err;
			}
		}
	}
}