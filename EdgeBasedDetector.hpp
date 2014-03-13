#ifndef DETECTOR_HPP_
#define DETECTOR_HPP_

#include "Plate.hpp"

class EdgeBasedDetector
{
public:
	EdgeBasedDetector();

	void detect( cv::Mat image );
	void filter();
	void findInterestAreas();
	void drawInterestAreas();

	void setImage( cv::Mat image );
	cv::Mat getImage();

	void setShowSteps( bool value );
	void getShowSteps( bool value );

	virtual ~EdgeBasedDetector();

private:
	void sobel();
	void thresh();

	void floodFl();

	cv::Mat histEq( cv::Mat in );

	bool verifySizes( cv::RotatedRect candidate );

protected:
	cv::Mat _cImage;
	cv::Mat _grayImage;
	cv::Mat _sobelOut;
	cv::Mat _threshOut;
	cv::Mat _closeOut;
	cv::Mat _result;

	std::vector< cv::RotatedRect > _interestAreas;
	std::vector< Plate > _output;

	bool _showSteps;
};

#endif /* DETECTOR_HPP_ */

