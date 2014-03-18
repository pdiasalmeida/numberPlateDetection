#include "EdgeBasedDetector.hpp"
#include "../auxiliar/FilesHelper.hpp"

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <sstream>

EdgeBasedDetector::EdgeBasedDetector()
{
	_showSteps = false;
}

void EdgeBasedDetector::detect()
{
	filter();
	findInterestAreas();
}

void EdgeBasedDetector::filter()
{
	sobel();
	thresh();
}

void EdgeBasedDetector::drawInterestAreas()
{
	for( std::vector< cv::RotatedRect >::iterator it = _interestAreas.begin(); it != _interestAreas.end(); it++ )
	{
		cv::Point2f rect_points[4];
		(*it).points( rect_points );

		for( int j = 0; j < 4; j++ )
		{
			line( _cImage, rect_points[j], rect_points[(j+1)%4], cv::Scalar(255, 0, 0, 255), 1, 8 );
		}
	}
}

void EdgeBasedDetector::saveInterestAreas()
{
	std::vector< cv::Mat >::iterator it = _output.begin();
	int index = 1;
	for( ; it != _output.end(); it++ )
	{
		std::stringstream ss;
		ss << index;
		cv::imwrite( "./results/"+_cImgName+"_plate_"+ss.str()+".jpg", *it );
		index++;
	}
}

// area of the code most dependent of plate size in the image;
// try different methods to localize possible plates besides vertical edges
// or different method to relate approximate edges
void EdgeBasedDetector::findInterestAreas()
{
	// connect all regions that have a high number of vertical edges
	// size of the structural element varies with image size
	cv::Mat element = getStructuringElement( cv::MORPH_RECT, cv::Size(17, 3) );
	morphologyEx( _threshOut, _closeOut, CV_MOP_CLOSE, element );

	if( _showSteps )
	{
		cv::namedWindow( "close", CV_WINDOW_NORMAL );
		cv::imshow( "close", _closeOut );
	}

	//Find contours of possibles plates
	std::vector< std::vector< cv::Point> > contours;
	findContours( _closeOut, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE );

	//Start to iterate to each contour found
	std::vector< std::vector< cv::Point > >::iterator itc = contours.begin();

	//Remove patch that has no inside limits of aspect ratio and area.
	while( itc != contours.end() )
	{
		//Create bounding rect of object
		cv::RotatedRect mr = minAreaRect( cv::Mat(*itc) );
		if( !verifySizes(mr) )
		{
			itc = contours.erase(itc);
		}
		else
		{
			++itc;
			_interestAreas.push_back(mr);
		}
	}

	floodFl();
}

void EdgeBasedDetector::floodFl()
{
	_cImage.copyTo(_result);

	for( unsigned int i = 0; i < _interestAreas.size(); i++ )
	{
		//For better rect cropping for each posible box, make floodfill algorithm because the plate has white background
		//And then we can retrieve more clearly the contour box
		circle( _result, _interestAreas[i].center, 3, cv::Scalar(0,255,0), -1 );

		//get the min size between width and height
		float minSize = (_interestAreas[i].size.width < _interestAreas[i].size.height) ?
				_interestAreas[i].size.width :
				_interestAreas[i].size.height;
		minSize = minSize - minSize*0.5;

		//initialize rand and get 5 points around center for floodfill algorithm
		srand ( time(NULL) );
		//Initialize floodfill parameters and variables
		cv::Mat mask;
		mask.create( _cImage.rows + 2, _cImage.cols + 2, CV_8UC1 );
		mask = cv::Scalar::all(0);

		int loDiff = 30;
		int upDiff = 30;
		int connectivity = 4;
		int newMaskVal = 255;
		int NumSeeds = 10;
		cv::Rect ccomp;
		int flags = connectivity + (newMaskVal << 8 ) + CV_FLOODFILL_FIXED_RANGE + CV_FLOODFILL_MASK_ONLY;

		for( int j = 0; j < NumSeeds; j++ )
		{
			cv::Point seed;
			seed.x = _interestAreas[i].center.x + rand()%(int)minSize-(minSize/2);
			seed.y = _interestAreas[i].center.y + rand()%(int)minSize-(minSize/2);
			circle( _result, seed, 1, cv::Scalar(0,255,255), -1 );
			floodFill( _cImage, mask, seed, cv::Scalar(255,0,0), &ccomp, cv::Scalar(loDiff, loDiff, loDiff),
					cv::Scalar(upDiff, upDiff, upDiff), flags );
		}

		//Check new floodfill mask match for a correct patch.
		//Get all points detected for get Minimal rotated Rect
		std::vector< cv::Point > pointsInterest;
		cv::Mat_< uchar >::iterator itMask = mask.begin< uchar >();
		cv::Mat_< uchar >::iterator end = mask.end< uchar >();
		for( ; itMask != end; ++itMask )
		{
			if( *itMask == 255 ) pointsInterest.push_back( itMask.pos() );
		}

		cv::RotatedRect minRect = (pointsInterest.size()>0)?minAreaRect(pointsInterest):cv::RotatedRect();

		if( verifySizes(minRect) )
		{
			// rotated rectangle drawing
			cv::Point2f rect_points[4]; minRect.points( rect_points );
			for( int j = 0; j < 4; j++ )
			{
				line( _result, rect_points[j], rect_points[(j+1)%4], cv::Scalar(0,0,255), 1, 8 );
			}

			//Get rotation matrix
			float r = (float)minRect.size.width / (float)minRect.size.height;
			float angle = minRect.angle;
			if( r < 1 ) angle = 90+angle;

			cv::Mat rotmat = getRotationMatrix2D( minRect.center, angle, 1 );

			//Create and rotate image
			cv::Mat img_rotated;
			warpAffine( _cImage, img_rotated, rotmat, _cImage.size(), CV_INTER_CUBIC );

			//Crop image
			cv::Size rect_size = minRect.size;
			if( r < 1 ) std::swap( rect_size.width, rect_size.height );
			cv::Mat img_crop;
			getRectSubPix( img_rotated, rect_size, minRect.center, img_crop );

			cv::Mat resultResized;
			resultResized.create( 33,144, CV_8UC3 );
			resize( img_crop, resultResized, resultResized.size(), 0, 0, cv::INTER_CUBIC );
			//Equalize croped image
			cv::Mat grayResult;
			cvtColor( resultResized, grayResult, CV_BGR2GRAY );
			blur( grayResult, grayResult, cv::Size(3,3) );
			grayResult = histEq(grayResult);

			_output.push_back( grayResult );

			if( _showSteps )
			{
				std::stringstream ss;
				ss << i;
				cv::namedWindow( "plate"+ss.str(), CV_WINDOW_NORMAL );
				cv::imshow( "plate"+ss.str(), grayResult );
			}
		}
	}
	if( _showSteps )
	{
		cv::namedWindow( "result", CV_WINDOW_NORMAL );
		cv::imshow( "result", _result );
	}
}

cv::Mat EdgeBasedDetector::histEq( cv::Mat in )
{
	cv::Mat out( in.size(), in.type() );

	if( in.channels() == 3 )
	{
		cv::Mat hsv;
		std::vector< cv::Mat > hsvSplit;

		cvtColor( in, hsv, CV_BGR2HSV );
		split( hsv, hsvSplit );

		equalizeHist( hsvSplit[2], hsvSplit[2] );

		merge( hsvSplit, hsv );
		cvtColor( hsv, out, CV_HSV2BGR );
	}
	else if( in.channels() == 1 )
	{
		equalizeHist( in, out );
	}

	return out;
}

void EdgeBasedDetector::sobel()
{
	//Find vertical lines. Car plates have high density of vertical lines
	Sobel( _grayImage, _sobelOut, CV_8U, 1, 0, 3, 1, 0 );
	if( _showSteps )
	{
		cv::namedWindow( "Sobel", CV_WINDOW_NORMAL );
		cv::imshow( "Sobel", _sobelOut );
	}
}

void EdgeBasedDetector::thresh()
{
	threshold( _sobelOut, _threshOut, 0, 255, CV_THRESH_OTSU+CV_THRESH_BINARY );
	if( _showSteps )
	{
		cv::namedWindow( "thresh", CV_WINDOW_NORMAL );
		cv::imshow( "thresh", _threshOut );
	}
}

bool EdgeBasedDetector::verifySizes( cv::RotatedRect candidate )
{
	bool result = true;
	float error = 0.4;

	//Spain car plate size: 52x11 aspect 4,7272
	const float aspect = 4.7272;

	//Set a min and max area. All other patches are discarded
	int min = 15*aspect*15; // minimum area
	int max = 125*aspect*125; // maximum area

	//Get only patches that match to a respect ratio.
	float rmin = aspect - aspect*error;
	float rmax = aspect + aspect*error;
	int area = candidate.size.height * candidate.size.width;

	float r = (float)candidate.size.width / (float)candidate.size.height;
	if( r < 1 )
		r = (float)candidate.size.height / (float)candidate.size.width;

	if( (area < min || area > max) || (r < rmin || r > rmax) )
	{
		result = false;
	}

	return result;
}

void EdgeBasedDetector::setImage( std::string imagePath )
{
	cv::Mat image = cv::imread(imagePath);

	assert( (image.channels() == 3) );

	_interestAreas.clear();
	_output.clear();

	_cImage = image.clone();

	cv::cvtColor( _cImage, _grayImage, CV_BGR2GRAY );
	cv::blur( _grayImage, _grayImage, cv::Size(5,5) );
}

cv::Mat EdgeBasedDetector::getImage()
{
	return _cImage;
}

void EdgeBasedDetector::setShowSteps( bool value )
{
	_showSteps = value;
}

void EdgeBasedDetector::testDir( std::string path )
{
	_files.clear();
	std::vector< std::string > extensions;
	extensions.push_back("jpg");
	extensions.push_back("png");

	FilesHelper::getFilesInDirectory( path, _files, extensions );

	std::vector< std::string >::iterator it = _files.begin();
	for( ; it != _files.end(); it++ )
	{
		_cImgName = FilesHelper::getFileName(*it);
		setImage(*it);

		detect();
		saveInterestAreas();
	}
}

EdgeBasedDetector::~EdgeBasedDetector() {}
