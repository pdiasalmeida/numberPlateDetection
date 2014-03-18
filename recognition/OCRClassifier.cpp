#include "OCRClassifier.hpp"
#include "../auxiliar/FilesHelper.hpp"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <tesseract/baseapi.h>

#include <iostream>
#include <fstream>
#include <sstream>

OCRClassifier::OCRClassifier()
{
	_ratio = 1.5;
	_rerror = 0.5;
	_maxShapeArea = 800;
	_minShapeArea = 40;

	_debug = false;
}

void OCRClassifier::setImage( cv::string imagePath )
{
	_characters.clear();

	cv::Mat image = cv::imread(imagePath);
	_cImgName = imagePath;

	_cImg = image.clone();
	if( image.channels() == 3 )
		cv::cvtColor( _cImg, _gray, CV_BGR2GRAY );
	else
		_gray = _cImg.clone();

	_toOCR = _gray.clone();
}

void OCRClassifier::filter()
{
	blur( _gray, _gray, cv::Size(3,3) );
	threshold( _gray, _thresh, 0, 255, CV_THRESH_OTSU+CV_THRESH_BINARY_INV );

	_toOCR = _thresh.clone();
}

void OCRClassifier::mopOpen()
{
	cv::Mat element = getStructuringElement( cv::MORPH_RECT, cv::Size( 2, 2 ) );
	morphologyEx( _thresh, _mop, cv::MORPH_ERODE, element );
	morphologyEx( _mop, _mop, cv::MORPH_DILATE, element );

	_toOCR = _mop.clone();
}

void OCRClassifier::mopClose()
{
	cv::Mat element = getStructuringElement( cv::MORPH_RECT, cv::Size( 2, 2 ) );
	morphologyEx( _thresh, _mop, cv::MORPH_DILATE, element );
	morphologyEx( _mop, _mop, cv::MORPH_ERODE, element );

	_toOCR = _mop.clone();
}

std::string OCRClassifier::classify()
{
	tesseract::TessBaseAPI tess;
	tess.Init( NULL, "eng", tesseract::OEM_DEFAULT);
	//tess.SetVariable("tessedit_char_blacklist", "-,£.;[]");
	tess.SetPageSegMode( tesseract::PSM_SINGLE_BLOCK );

	tess.SetImage( (uchar*) _toOCR.data, _toOCR.cols, _toOCR.rows, 1, _toOCR.cols );
	char* outc = tess.GetUTF8Text();

	std::string out(outc);

	std::stringstream resultsString;
	char chars[] = ",\n";

	if( out.length() != 0 )
	{
		std::string testFileName = FilesHelper::getFileName(_cImgName);
		FilesHelper::filterChars(out,chars);

		resultsString << testFileName.substr(0, testFileName.find_first_of("_")) << "," << out.substr(0, out.size());
		resultsString << std::endl;
		std::cout << "Letters for image '" + _cImgName << "': " << out << std::endl;
	}
	else
		std::cout << "Could not recognise letters in '" + _cImgName << "'.\n" << std::endl;

	return resultsString.str();
}

void OCRClassifier::findCharacters()
{
	std::vector< std::vector< cv::Point > > contours;
	cv::Mat imgCon = _thresh.clone();
	findContours( imgCon, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE );

	std::vector< std::vector< cv::Point > >::iterator it = contours.begin();
	for( ; it != contours.end(); it++ )
	{
		std::vector< cv::Point> approx;
		approxPolyDP( *it, approx, 3, true );

		cv::Rect roi;
		getBBox( approx, roi );
		if( validateShape(roi) && cv::countNonZero(cv::Mat(_thresh, roi)) > 55 )
		{
			cv::Rect expRoi;
			expRoi.x = (roi.x-5 > 0)?roi.x-5:0;expRoi.width = (expRoi.x+roi.width+10 <= _cImg.cols)?roi.width+10:_cImg.cols-expRoi.x;
			expRoi.y = (roi.y-5 > 0)?roi.y-5:0;expRoi.height = (expRoi.y+roi.height+10 < _cImg.rows)?roi.height+10:_cImg.rows-expRoi.y;
			cv::rectangle( _out, expRoi.tl(), expRoi.br(), cv::Scalar(0, 255, 0, 255) );

			cv::Mat sample( _cImg, expRoi );

			_characters.push_back(sample);
		}
	}
}

void OCRClassifier::getBBox( std::vector< cv::Point > points, cv::Rect& roi )
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

bool OCRClassifier::validateShape( cv::Rect& roi )
{
	bool result = false;

	double ratio = 0;
	double area = 0;

	ratio = (double)roi.height / roi.width;
	area = roi.height * roi.width;

	// shapes found have to be approximately rectangular
	result = ( ratio >= _ratio-_ratio*_rerror && ratio <= _ratio+4*_ratio*_rerror );
	if(_debug) std::cout << "Ratio test result: " << result << ". With ratio value: " << ratio << std::endl;

	// shape area has to be between a certain threshold
	result = result && ( area < _maxShapeArea  );
	if(_debug) std::cout << "Max area test result: " << result << ". With area value: " << area << std::endl;

	result = result && ( area > _minShapeArea  );
	if(_debug) std::cout << "Min area test result: " << result << ". With area value: " << area << std::endl;

	return result;
}

void OCRClassifier::setDebug(bool value)
{
	_debug = value;
}

void OCRClassifier::testDir( std::string path )
{
	_files.clear();

	std::vector< std::string > extensions;
	extensions.push_back("jpg");
	extensions.push_back("png");

	FilesHelper::getFilesInDirectory( path, _files, extensions );

	std::ofstream resultsfile;
	resultsfile.open(std::string(path+"/results.txt").c_str());

	std::stringstream out;

	std::vector< std::string >::iterator it = _files.begin();
	for( ; it != _files.end(); it++ )
	{
		_cImgName = (*it);
		setImage(_cImgName);
		filter();
		mopClose();

		out << classify();
	}

	resultsfile << out.str();

	resultsfile.close();
}

OCRClassifier::~OCRClassifier(){}
