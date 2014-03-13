#include "EdgeBasedDetector.hpp"
#include "ShapeBasedDetector.hpp"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

int main( int argc, char** argv )
{
	ShapeBasedDetector sH = ShapeBasedDetector();

//	cv::Mat image = cv::imread( "./tests/test5.jpg" );
//	sH.setShowSteps(true);
//
//	sH.detect(image);
//	sH.drawInterestAreas();
//	sH.saveInterestAreas();

	sH.testDir("./tests");

//	cv::namedWindow("Original", CV_WINDOW_NORMAL);
//	cv::imshow("Original", sH.getImage());

	cv::waitKey(0);
}
