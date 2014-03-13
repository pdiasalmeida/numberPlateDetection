#ifndef PLATE_HPP_
#define PLATE_HPP_

#include <opencv2/core/core.hpp>

class Plate {
public:
	Plate();
	Plate( cv::Mat img, cv::Rect pos );

	std::string str();

	virtual ~Plate();
protected:
	cv::Rect _position;
	cv::Mat _plateImg;
	std::vector< char > _chars;
	std::vector< cv::Rect > _charsPos;
};

#endif /* PLATE_HPP_ */
