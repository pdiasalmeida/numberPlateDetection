#include "auxiliar/FilesHelper.hpp"
#include "detection/EdgeBasedDetector.hpp"
#include "detection/ShapeBasedDetector.hpp"
#include "recognition/OCRClassifier.hpp"

#include <opencv2/highgui/highgui.hpp>

bool FilesHelper::debug = false;

int main( int argc, char** argv )
{
	EdgeBasedDetector detector = EdgeBasedDetector();
	OCRClassifier classifier = OCRClassifier();

	detector.testDir("./tests");
	classifier.testDir("./results");

	cv::waitKey(0);
}
