#ifndef DETECTOR_HPP_
#define DETECTOR_HPP_

#include <opencv2/core/core.hpp>

class EdgeBasedDetector
{
public:
	EdgeBasedDetector();

	void detect();

	void filter();
	void findInterestAreas();
	void drawInterestAreas();
	void saveInterestAreas();

	void testDir( std::string path );

	void setImage( std::string imagePath );
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

	std::string _cImgName;

	std::vector< cv::RotatedRect > _interestAreas;
	std::vector< cv::Mat > _output;
	std::vector< std::string > _files;

	bool _showSteps;
};

#endif /* DETECTOR_HPP_ */

