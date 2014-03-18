#include "../auxiliar/FilesHelper.hpp"

std::map< std::string, std::vector< std::string > > _plates;
std::map< std::string, std::vector< std::string > > _potentialPlates;

int main( int argc, char** argv )
{
	char chars[] = "()- .'_[]~iÍ";
	int n = 0;
	int f = 0;

	FilesHelper::parseKeyValue("../tests/benchmark.txt", _plates, ',' );
	FilesHelper::parseKeyValue("../results/results.txt", _potentialPlates, ',' );

	n = _plates.size();

	std::map< std::string, std::vector< std::string > >::iterator itBench = _plates.begin();
	std::map< std::string, std::vector< std::string > >::iterator itRes = _potentialPlates.begin();
	for( ; itRes != _potentialPlates.end(); itRes++ )
	{
		std::string answer( (_plates.find(itRes->first))->second.at(0) );
		FilesHelper::filterChars( answer, chars );

		std::cout << "Searching for plate in file '" << itRes->first << "', with plate: " << answer << std::endl;

		bool found = false;
		std::vector< std::string >::iterator itPPlates = itRes->second.begin();
		for( ; !found && itPPlates != itRes->second.end(); itPPlates++ )
		{
			std::string pAnswer(*itPPlates);
			FilesHelper::filterChars( pAnswer, chars );
			std::cout << "\tTesting potential answer '" << pAnswer << "' ... ";
			if( answer.compare(pAnswer) == 0 )
			{
				std::cout << "success." << std::endl;
				found = true;
				f++;
			}
			else std::cout << "failed." << std::endl;
		}
	}
	std::cout << std::endl << "Finished test. Found " << f << " of " << n <<
			" plates, with a success rate of " << ((float)f/n)*100 << "%." << std::endl;
}
