/*	MapController.h
	Robert Kirk DeLisle
	23 August 2006

	Purpose:	Serves as an interface between MapModel, MapView and other classes

	Modification History:

*/

#if !defined(MapController_RKD_23Aug06)
#define MapController_RKD_23Aug06

#include <windows.h>

#include "MapModel.h"
#include "MapView.h"

class MapController
{
	private:

		//MVC links
		MapModel m_Model;
		MapView m_View;

	public:

		MapController();
		~MapController() {};

		void LoadMap(const string MapFile);
			//tell the model to load a map file

		void SetBitmap(string FileName, const long TileCount)
			{  m_View.SetBitmap(FileName, TileCount);  }
			//tell the view where to find its bitmap

		void SetTargetDC(HDC *TargetDC)
			{  m_View.SetTargetDC(TargetDC);  }
			//tell the view where all drawing occurs

		void Draw()
			{  m_View.Draw();  }
			//tell the view to draw on the target DC

		long GetPixelWidth()
			{  return m_Model.Width() * m_View.GetTileWidth(); };

		long GetPixelHeight()
			{  return m_Model.Height() * m_View.GetTileHeight();  };

        long GetWidth()
            {  return m_Model.Width();  };

        long GetHeight()
            {  return m_Model.Height();  };

        long GetPosition(const long X, const long Y) const
            {  return m_Model.GetPosition(X,Y);  };

        void GetTileDimensions(long X, long Y)
            {  X = m_View.GetTileWidth();  Y = m_View.GetTileHeight();  };

        long GetPixelPosition(const long X, const long Y) const;
            //get the value of the tile at pixel postion (X,Y) - requires conversion between pixels and tiles

        bool CheckMapByPixels(const long MinX, const long MaxX, const long MinY, const long MaxY, long &ret) const;
            //check the content of the map within the given rectangle.
            //if it contains anything other than 0s, return false

        long ModifyTileByPixel(const long MinX, const long MaxX, const long MinY, const long MaxY);
            //change the value of the tiles within the pixel ranges

        void ModifyTile(const long X, const long Y, const long Value)
        {
            m_Model.ModifyTile(X, Y, Value);
        }

        void Reset()
        {
            m_Model.Reset();
        }

        long CountTilesOfType(RECT Bounds, const long Type) const
        {
            //get a count of the number of tiles of Type inside the rectangle defined by top, bottom, left, right
            return m_Model.CountTilesOfType(Bounds, Type);
        }


};

#endif
