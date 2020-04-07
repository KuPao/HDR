#include <iostream>
#include <vector>
#include <string>

#include "load_images.h"
#include "process.h"
#include "tone_mapping.h"

int main(int argc, char* argv[]) {
	std::string image_list = argv[1]; //"E:/hdr/3";// 
	image_list += "/";
	auto image_data = load_images(image_list);
	
	auto hdr_image = process(image_data);

	cv::imwrite(image_list + "hdr.hdr", hdr_image);

	auto tonemap_image = tone_mapping(hdr_image);

	cv::imwrite(image_list + "tone.jpg", tonemap_image);

	return 0;
}
