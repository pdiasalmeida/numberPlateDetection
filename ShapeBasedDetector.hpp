#ifndef SHAPEBASEDDETECTOR_HPP_
#define SHAPEBASEDDETECTOR_HPP_

#include "Plate.hpp"

#include <set>

class ShapeBasedDetector {
public:
	ShapeBasedDetector();

	void detect( cv::Mat image );

	void filter();
	void findInterestAreas();
	void drawInterestAreas();
	void saveInterestAreas();

	void testDir( std::string path );

	void setImage( cv::Mat image );
	cv::Mat getImage();

	void setShowSteps( bool value );
	void getShowSteps( bool value );

	void getFileList( std::string path );

	virtual ~ShapeBasedDetector();
private:
	bool validateShape( double width, double height );
	void getDims( std::vector< cv::Point > points, cv::Rect& roi );
protected:
	cv::Mat _cImage;
	cv::Mat _out;
	cv::Mat _grayImage;
	cv::Mat _threshOut;
	cv::Mat _countOut;
	cv::Mat _result;

	std::string _cImgpath;

	std::vector< cv::Rect > _interestAreas;
	std::vector< Plate > _output;
	std::set< std::string > _files;

	bool _showSteps;

	int _threshLower;
	int _threshUpper;
	int _threshMethod;

	int _contMode;
	int _contMethod;

	double _approxEpsilon;
	bool _approxClose;

	double _maxShapeArea;
	double _minShapeArea;
	double _ratio;
	double _rerror;
};

#endif /* SHAPEBASEDDETECTOR_HPP_ */
