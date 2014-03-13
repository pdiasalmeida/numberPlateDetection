#include "Plate.hpp"

Plate::Plate() {}

Plate::Plate( cv::Mat img, cv::Rect pos )
{
	_plateImg = img;
	_position = pos;
}

std::string Plate::str()
{
	std::string result = "";
	//Order numbers
	std::vector< int > orderIndex;
	std::vector< int > xpositions;
	for( unsigned int i = 0; i < _charsPos.size(); i++ )
	{
		orderIndex.push_back(i);
		xpositions.push_back(_charsPos[i].x);
	}

	float min = xpositions[0];
	int minIdx = 0;
	for( unsigned int i = 0; i < xpositions.size(); i++ )
	{
		min = xpositions[i];
		minIdx = i;
		for( unsigned int j = i; j < xpositions.size(); j++ )
		{
			if( xpositions[j] < min )
			{
				min = xpositions[j];
				minIdx = j;
			}
		}
		int aux_i = orderIndex[i];
		int aux_min = orderIndex[minIdx];
		orderIndex[i] = aux_min;
		orderIndex[minIdx] = aux_i;

		float aux_xi = xpositions[i];
		float aux_xmin = xpositions[minIdx];
		xpositions[i] = aux_xmin;
		xpositions[minIdx] = aux_xi;
	}

	for( unsigned int i = 0; i < orderIndex.size(); i++ )
	{
		result = result+_chars[orderIndex[i]];
	}

	return result;
}

Plate::~Plate(){}

