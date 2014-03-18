#ifndef FILESHELPER_HPP_
#define FILESHELPER_HPP_

#include <string>
#include <cstring>
#include <vector>
#include <map>

#include <iostream>
#include <fstream>
#include <sstream>

#include <algorithm>
#include <dirent.h>

class FilesHelper {
public:
	static std::string toLowerCase( std::string in )
	{
		std::transform( in.begin(), in.end(), in.begin(), ::tolower );

		return in;
	}

	static void getFilesInDirectory( const std::string& dirName, std::vector< std::string >& fileNames,
			const std::vector< std::string >& validExtensions)
	{
		if(FilesHelper::debug) std::cout << "Opening directory " << dirName << "..." << std::endl;

		struct dirent* ep;
		size_t extensionLocation;

		DIR* dp = opendir( dirName.c_str() );
		if( dp != NULL )
		{
			while( (ep = readdir(dp)) )
			{
				if( ep->d_type & DT_DIR )
				{
					continue;
				}

				extensionLocation = std::string( ep->d_name ).find_last_of( "." );
				std::string tempExt = toLowerCase( std::string( ep->d_name ).substr( extensionLocation + 1 ) );

				if( find(validExtensions.begin(), validExtensions.end(), tempExt) != validExtensions.end() )
				{
					if(FilesHelper::debug) std::cout << "Found matching data file '" << ep->d_name << "'." << std::endl;

					fileNames.push_back( (std::string) dirName + "/" + ep->d_name );
				} else {
					if(FilesHelper::debug)
						std::cout << "Found file does not match required file type, skipping: '"
						<< ep->d_name << "'!" << std::endl;
				}
			}
			closedir( dp );
		}
		else
		{
			if(FilesHelper::debug) std::cout << "Error opening directory " << dirName << "!" << std::endl;
		}
		return;
	}

	static std::string getFileName( std::string fullPath)
	{
		int ini = fullPath.find_last_of( "/" );
		std::string result = fullPath.substr( ini+1 );

		return result;
	}

	static void parseKeyValue( std::string file, std::map< std::string, std::vector< std::string > >& kValues, char delimiter )
	{
		std::ifstream infile;
		std::vector< std::string > tokens;

		infile.open( file.c_str() );
		if( infile.is_open() )
		{
			while( infile )
			{
				std::string line;
				getline( infile, line );
				std::istringstream iss( line );

				tokens = streamSplit( iss, delimiter );

				if( tokens.size() > 1 )
				{
					std::string key( tokens.at(0) );
					tokens.erase(tokens.begin());

					std::map< std::string, std::vector< std::string > >::iterator it;
					it = kValues.find(key);

					if( it == kValues.end() )
					{
						kValues.insert( std::pair< std::string, std::vector< std::string > >( key, tokens ) );
					}
					else
					{
						it->second.reserve(it->second.size()+tokens.size());
						it->second.insert(it->second.end(), tokens.begin(),tokens.end());
					}
				}
			}
		}

		infile.close();
	}

	static void filterChars( std::string& str, char* chars )
	{
		for( unsigned int i = 0; i < strlen(chars); ++i )
		{
			str.erase( std::remove(str.begin(), str.end(), chars[i]), str.end() );
		}
	}

	static std::vector< std::string > streamSplit( std::istringstream& instream, char delimiter )
	{
		std::string token;
		std::vector< std::string > result;

		while( getline(instream, token, delimiter) )
		{
			result.push_back( token );
		}

		return result;
	}

	static bool debug;

	virtual ~FilesHelper();
private:
	FilesHelper();
};

#endif /* FILESHELPER_HPP_ */
