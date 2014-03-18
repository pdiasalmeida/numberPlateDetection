#ifndef OCRCLASSIFIER_HPP_
#define OCRCLASSIFIER_HPP_

#include <opencv2/core/core.hpp>

class OCRClassifier {
public:
	OCRClassifier();

	void getBBox( std::vector< cv::Point > points, cv::Rect& roi );
	bool validateShape( cv::Rect& roi );

	void setImage( cv::string imagePath );
	void filter();
	void mopOpen();
	void mopClose();

	void findCharacters();
	std::string classify();

	void testDir( std::string path );

	void setDebug(bool value);

	virtual ~OCRClassifier();

protected:
	cv::Mat _cImg;
	cv::Mat _gray;
	cv::Mat _thresh;
	cv::Mat _mop;
	cv::Mat _toOCR;
	cv::Mat _out;

	std::string _cImgName;

	std::vector< cv::Mat > _characters;
	std::vector< std::string > _files;

	double _ratio;
	double _rerror;
	int _maxShapeArea;
	int _minShapeArea;

	bool _debug;
};

#endif /* OCRCLASSIFIER_HPP_ */
