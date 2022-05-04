/*	MapView.h
	Robert Kirk DeLisle
	26 August 2006

	Purpose:	Provides visualization for the MapModel for Win32 GDI using resourced graphics.

	Modification History:

*/

#if !defined(MapView_RKD_22Aug06)
#define MapView_RKD_22Aug06

#include <windows.h>

#include "Utilities\\ExceptionRKD.h"
#include "MapModel.h"

class MapView
{
	private:

		//MVC link
		MapModel *m_Model;

		HDC *m_TargetDC;
			//DC where all drawing occurs

		HDC m_hDC;
			//DC buffer to hold the bitmap and for BitBlt transfers

		HBITMAP m_hBitmap;
			//bitmap for the DC to draw onto

		HBITMAP m_hOldBitmap;
			//old bitmap to capture what was in the DC previously

		BITMAPFILEHEADER *m_pBitmapHeader;
			//bitmap containing the graphic

		long m_TileHeight;
		long m_TileWidth;
			//dimensions of the tiles themselves

		long m_TileCount;
			//number of tiles in the bitmap

	public:

		MapView() {};
		~MapView();

		void Connect(MapModel *Model)
			{  m_Model = Model;  }
			//connect the model to the view to pull data

		void SetTargetDC(HDC *TargetDC)
			{  m_TargetDC = TargetDC;  }
			//set the target of all drawing operations

		void Draw();
			//code for drawing to the target DC

        void EraseTile(long X, long Y);
            //draw a black rectangle at the specified tile location

		void SetBitmap(string FielName, const long TileCount);
			//load the associated bitmap in the passed filename

		//provide access to the individual tile height/width
		long GetTileWidth() const
			{  return m_TileWidth;  };

		long GetTileHeight() const
			{  return m_TileHeight;  };
};

#endif
