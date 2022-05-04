/*	MapModel.h
	Robert Kirk DeLisle
	1 August 2006

	Purpose:	Holds a 2D map and exposes various methods for pathfinding, navigation, etc.

	Modification History:

*/

#if !defined(MapModel_RKD_01August2006)
#define MapModel_RKD_01August2006

#include <windows.h>
#include <vector>
#include <string>
using namespace std;

class MapModel
{
	private:

		vector<long> m_Map;
			//holds the map content

		vector<long> m_BackupMap;
			//holds a backup copy of the map just in case we make modifications and need to reset

		bool m_Initialized;
			//set to true once the map has been loaded correctly and is ready to go

		long m_Height;
			//number of tiles in the height

		long m_Width;
			//number of tiles in the width

	public:

		MapModel()
			{ m_Initialized = false;  m_Height=0;  m_Width=0; };
			//default constructor - not ready for use yet

		MapModel(const string MapFile);
			//create a MapModel from a specific file

		bool CheckPosition(const long X, const long Y) const;
			//Return true if the tile at (X,Y) is empty (=0), false if something is there
			//identify obstacles and empty space

		long GetPosition(const long X, const long Y) const;
			//return the value in tile position (X,Y) from the m_Map vector

		long CountTilesOfType(RECT Bounds, const long Type) const;
			//get a count of the number of tiles of Type inside the rectangle defined by top, bottom, left, right

		bool IsReady() const
			{ return m_Initialized; }
			//check to see if the map has been loaded successfully

		void LoadMap(const string MapFile);
			//load a map from the specified file

		long Height() const
			{ return m_Height; }
			//map height in tiles

		long Width() const
			{ return m_Width; }
			//map width in tiles

		void ModifyTile(const long X, const long Y, const long TileValue);
			//change the value at (X,Y) in the map to TileValue

		void Reset()
			{ m_Map.clear(); m_Map = m_BackupMap; }
			//reset the contents of the map to the original

		~MapModel();
			//clear out the m_Map vector

};

#endif
