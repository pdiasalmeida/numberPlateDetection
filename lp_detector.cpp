#include "EdgeBasedDetector.hpp"
#include "ShapeBasedDetector.hpp"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

int main( int argc, char** argv )
{
	ShapeBasedDetector sH = ShapeBasedDetector();

	sH.testDir("./tests");

	cv::waitKey(0);
}
