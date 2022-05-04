/*	MapModel.cpp
	Robert Kirk DeLisle
	13 August 2006

	Purpose:	Holds a 2D map and exposes various methods for pathfinding, navigation, etc.

	Modification History:

*/

#include "MapModel.h"
#include "Utilities\\ExceptionRKD.h"

#include <fstream>
using namespace std;

MapModel::MapModel(const string MapFile)
{
	/*	Purpose:	create a MapModel from a specific file

		Parameters:	MapFile -	the file from which the map will be created
								it is assumed to be a CSV file with long values
								at each tile position.  these values will be returned
								by the GetPosition().  An "empty" tile is assumed
								to be value 0, and this is checked by CheckPosition()

		Return:		none

		Exceptions:	none - handled by other functions and thrown
	*/

	//delegate file opening
	try
	{
		LoadMap(MapFile);
	}
	catch(ExceptionRKD &ex)
	{
		//if there's a problem, throw to the caller
		throw ex;
	}
}

bool MapModel::CheckPosition(const long X, const long Y) const
{
	/*	Purpose:	Help to identify obstacles and empty space
					"empty space" assumed to be zeros in the map

		Parameters:	X,Y - the tile coordinates to check
							upper left assumed to be (0,0)
							bottom right assumed to be (m_Width-1, m_Height-1)

		Return:		false if tile at (X,Y) is empty
					true if tile at (X,Y) has something other than a 0

		Exceptions:	none - handled by other functions and thrown
	*/

	bool ret;	//return value holder

	//check the value at the specified tile location
	try
	{
		if ( GetPosition(X,Y) != 0 )
			ret=true;
		else
			ret=false;
	}
	catch(ExceptionRKD &ex)
	{
		throw ex;
	}

	return ret;
}

long MapModel::GetPosition(const long X, const long Y) const
{
	/*	Purpose:	return the value in tile position (X,Y) from the m_Map vector

		Parameters:	X,Y - the tile coordinates to check
							upper left assumed to be (0,0)
							bottom right assumed to be (m_Width-1, m_Height-1)

		Return:		the value from the original file at position (X,Y)

		Exceptions:	out of bounds for m_Map
	*/

	//do bounds checking
	if ( ( X >= m_Width) || ( Y >= m_Height) || ( X < 0 ) || ( Y < 0 ) )
	{
	    return -1;
		//throw ExceptionRKD(101,"MapModel::GetPosition","Tile coordinates out of bounds.");
	}

	return m_Map.at( X+(Y*m_Width) );  //convert X,Y to linear access
}

long MapModel::CountTilesOfType(RECT Bounds, const long Type) const
{
	/*	Purpose:	get a count of the number of tiles of Type inside the rectangle defined by top, bottom, left, right

		Parameters:	Top, Bottom, Left, Right -	define a rectangle in tiles.  All boundaries are inclusive, so
												minimum is assumed 0, and maximum is assumed Height-1, Width-1
					Type	-	Value to search for in the map

		Return:		count of the number of times Type occurs in the map within the defined rectangle

		Exceptions:	out of bounds for m_Map
	*/

	long count=0;	//accumulator and return

	//verify a proper rectangle
	if ( (Bounds.top > Bounds.bottom) )
	    Bounds.top = Bounds.bottom;
    if ( (Bounds.left > Bounds.right) )
        Bounds.left = Bounds.right;
//	{
//
//		throw ExceptionRKD(104,"MapModel::CountTilesOfType","Improper rectangle definition.");
//	}

	//do bounds checking
//	if ( (Bounds.top < 0) || (Bounds.bottom < 0) || (Bounds.left < 0) || (Bounds.right < 0) )
//	{
//		throw ExceptionRKD(105,"MapModel::CountTilesOfType","Rectangle coordinates out of bounds.");
//	}
    if ( Bounds.top < 0 )
        Bounds.top = 0;
    if ( Bounds.bottom < 0 )
        Bounds.bottom = 0;
    if ( Bounds.left < 0 )
        Bounds.left = 0;
    if ( Bounds.right < 0 )
        Bounds.right = 0;

//	if ( (Bounds.top >= m_Height) || (Bounds.bottom >= m_Height) || (Bounds.left >= m_Width) || (Bounds.right >= m_Width) )
//    {
//		throw ExceptionRKD(105,"MapModel::CountTilesOfType","Rectangle coordinates out of bounds.");
//	}

    if ( Bounds.top >= m_Height )
        Bounds.top = m_Height-1;
    if ( Bounds.bottom >= m_Height )
        Bounds.bottom = m_Height-1;
    if ( Bounds.left >= m_Width )
        Bounds.left = m_Width-1;
    if ( Bounds.right >= m_Width )
        Bounds.right = m_Width-1;

	//step through the rectangle and tally up the tiles
	for (int y=Bounds.top; y<=Bounds.bottom; y++)
	{
		for (int x=Bounds.left; x<=Bounds.right; x++)
		{
			if ( m_Map.at(x+(y*m_Width)) == Type )
				count++;
		}
	}

	return count;
}

void MapModel::LoadMap(const string MapFile)
{
	/*	Purpose:	load a map from the specified file

		Parameters:	MapFile	-	string containing a correct filename

		Return:		none

		Exceptions:	error opening map file
					error parsing map file

	*/

	fstream file;
	string line;	//holds a line from the map file
	long idx;		//loop counter
	string::size_type pos1;	//used for string parsing
	string::size_type pos2;

	//reinitialize the height and width
	m_Height=0;
	m_Width=0;

	//clear out the old map
	m_Map.clear();

	//try to open the file
	file.open( MapFile.c_str() );
	if (!file)
	{
		m_Initialized=false;
		throw ExceptionRKD(102,"MapModel::LoadMap","Error opening map file.");
	}

	try
	{
		//get the first line and set the width
		getline(file,line);
		for ( idx=0; idx<line.length(); idx++)
		{
			if ( line.at(idx) == ',' )
				m_Width++;
		}
		m_Width++; //one more than the number of delimiters

		file.seekg(ios::beg); //reset to the first line of the file

		//step through the file
		while ( !file.eof() )
		{
			//get the next line
			getline(file,line);

			m_Height++;  //increment the height/line count

			//parse the current line into the vector
			pos1=0;
			pos2 = line.find_first_of(',',pos1);
			while (pos2 != string::npos)
			{
				m_Map.push_back( atol( (line.substr(pos1, pos2-pos1)).c_str() ));
				pos1=pos2+1;
				pos2 = line.find_first_of(',',pos1);
			}
			m_Map.push_back( atol( (line.substr(pos1, pos2-pos1)).c_str() ));
		}
	}
	catch(...)
	{
		throw ExceptionRKD(103,"MapModel::LoadMap","Error parsing map file.");
	}

	m_BackupMap = m_Map; //make a backup copy

	m_Initialized=true;

	return;
}

void MapModel::ModifyTile(const long X, const long Y, const long TileValue)
{
	/*	Purpose:	change the value at (X,Y) in the map to TileValue

		Parameters:	X,Y	-	coordinates of tile to change
					Tile -	value to store at (X,Y)

		Return:		none

		Exceptions:	none
	*/

	m_Map.at( X+(Y*m_Width) ) = TileValue;
}



MapModel::~MapModel()
{
	/*
		Purpose:	clear out the m_Map vector to reclaim in references

		Parameters:	none

		Return:		none

		Exceptions:	none
	*/

	m_Map.clear();
	m_BackupMap.clear();
}
