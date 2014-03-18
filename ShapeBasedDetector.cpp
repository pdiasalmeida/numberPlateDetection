#include "ShapeBasedDetector.hpp"

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <sstream>

#include <dirent.h>

ShapeBasedDetector::ShapeBasedDetector()
{
	_showSteps = false;

	_threshLower = 120;
	_threshUpper = 255;
	_threshMethod = cv::THRESH_BINARY;

	_contMode = cv::RETR_TREE;
	_contMethod = cv::CHAIN_APPROX_SIMPLE;

	_approxEpsilon = 3;
	_approxClose = true;

	_maxShapeArea = 200000;
	_minShapeArea = 400;

	_ratio = 4.5;
	_rerror = 0.3;
}

void ShapeBasedDetector::detect( cv::Mat image )
{
	setImage(image);
	filter();
	findInterestAreas();
}

void ShapeBasedDetector::filter()
{
	threshold( _grayImage, _threshOut, _threshLower, _threshUpper, _threshMethod );
	if( _showSteps )
	{
		cv::namedWindow( "thresh", CV_WINDOW_NORMAL );
		cv::imshow( "thresh", _threshOut );
	}
}

void ShapeBasedDetector::findInterestAreas()
{
	std::vector< std::vector< cv::Point> > contours;

	findContours( _threshOut, contours, _contMode, _contMethod );
	if( _showSteps )
	{
		cv::Mat contAll = _cImage.clone();
		cv::drawContours( contAll, contours, -1, cv::Scalar(255,0,0,255) );
		cv::namedWindow( "All Contours", CV_WINDOW_NORMAL );
		cv::imshow( "All Contours", contAll );
	}

	for( std::vector< std::vector< cv::Point> >::iterator it = contours.begin(); it != contours.end(); it++ )
	{
		std::vector< cv::Point> approx;
		approxPolyDP( *it, approx, _approxEpsilon, _approxClose );

		if( approx.size() >= 4 && approx.size() <= 10 )
		{
			cv::Rect roi;

			getDims( approx, roi );
			if( _showSteps )
			{
				std::cout << "Found shape in: (" << roi.x << ", " << roi.y << "). With dimensions: " <<
						roi.width << "x" << roi.height << std::endl;
			}

			if( validateShape( roi.width, roi.height ) )
			{
				_interestAreas.push_back( roi );
			}
		}
	}
}

void ShapeBasedDetector::drawInterestAreas()
{
	std::vector< cv::Rect >::iterator it = _interestAreas.begin();
	for( ; it != _interestAreas.end(); it++)
	{
		cv::rectangle( _out, (*it).tl(), (*it).br(), cv::Scalar(255,0,0,255) );
	}
}

void ShapeBasedDetector::saveInterestAreas()
{
	std::vector< cv::Rect >::iterator it = _interestAreas.begin();
	int index = 1;
	for( ; it != _interestAreas.end(); it++ )
	{
		std::stringstream ss;
		ss << index;
		cv::imwrite( "./results/"+_cImgName+"_plate_"+ss.str()+".jpg", cv::Mat( _cImage, *it ) );
		index++;
	}
}

bool ShapeBasedDetector::validateShape( double width, double height )
{
	bool result = false;

	double ratio = 0;
	double area = 0;

	ratio = width / height;
	area = height * width;

	// shapes found have to be approximately rectangular
	result = ( ratio >= _ratio-_ratio*_rerror && ratio <= _ratio+_ratio*_rerror );
	if( _showSteps )
	{
		std::cout << "Ratio test result: " << result << ". With ratio value: " << ratio << std::endl;
	}

	// shape area has to be between a certain threshold
	result = result && ( area < _maxShapeArea  );
	if( _showSteps )
	{
		std::cout << "Max area test result: " << result << ". With area value: " << area << std::endl;
	}

	result = result && ( area > _minShapeArea  );
	if( _showSteps )
	{
		std::cout << "Min area test result: " << result << ". With area value: " << area << std::endl;
	}

	return result;
}

void ShapeBasedDetector::getDims( std::vector< cv::Point > points, cv::Rect& roi )
{
	double minX, minY; minX = minY = 999999;
	double maxX, maxY; maxX = maxY = 0;

	for( std::vector< cv::Point >::iterator it = points.begin(); it != points.end(); it++ )
	{
		if( (*it).x < minX ) minX = (*it).x;
		if( (*it).x > maxX) maxX = (*it).x;
		if( (*it).y < minY ) minY = (*it).y;
		if( (*it).y > maxY) maxY = (*it).y;
	}

	roi.x = minX;
	roi.y = minY;

	roi.width = maxX - minX;
	roi.height = maxY - minY;
}

void ShapeBasedDetector::setImage( cv::Mat image )
{
	assert( (image.channels() == 3) );

	_interestAreas.clear();

	_cImage = image.clone();
	_out = _cImage.clone();

	cv::cvtColor( _cImage, _grayImage, CV_BGR2GRAY );
	cv::blur( _grayImage, _grayImage, cv::Size(5,5) );
}

cv::Mat ShapeBasedDetector::getImage()
{
	return _out;
}

void ShapeBasedDetector::setShowSteps( bool value )
{
	_showSteps = value;
}

void ShapeBasedDetector::testDir( std::string path )
{
	getFileList(path);

	std::set< std::string >::iterator it = _files.begin();
	for( ; it != _files.end(); it++ )
	{
		_cImgName = (*it);
		cv::Mat img = cv::imread(path+"/"+_cImgName);
		setImage(img);

		detect(img);
		saveInterestAreas();
	}
}

void ShapeBasedDetector::getFileList( std::string path )
{
	DIR *dir;
	struct dirent *ent;
	if( ( dir = opendir( path.c_str() ) ) != NULL )
	{
		while( ( ent = readdir( dir ) ) != NULL)
		{
			if( ent->d_type == DT_REG )
			{
				_files.insert( std::string( ent->d_name ) );
			}
		}
		closedir (dir);
	}
	else
	{
	  std::cout << "Unable to access directory " << path << std::endl;
	}
}

ShapeBasedDetector::~ShapeBasedDetector() {}

