/*	MapView.cpp
	Robert Kirk DeLisle
	23 August 2006

	Purpose:	Provides visualization for the MapModel for Win32 GDI using resourced graphics.

	Modification History:

*/

#include "MapView.h"
#include "Utilities\\ExceptionRKD.h"
#include "Utilities\\BitmapLoader.h"

void MapView::Draw()
{
	/*	Purpose:	Draw the map to the target DC
					Assumes the the Target DC is compatible with the internal DC.  The internal DC
					for this class is compatible with the screen.

		Parameters:	Target	-	pointer to DC onto which drawing will be done

		Return:		none

		Exceptions:

	*/

	long x, y;  //loop indices
	long TileID; //which tile to draw?

	//parse through the map and blit the appropriate picture to the targetDC
	for (x=0; x<m_Model->Width(); x++)
	{
		for (y=0; y<m_Model->Height(); y++)
		{
			TileID = m_Model->GetPosition(x,y);
			if (TileID != 0)
			{
				BitBlt( *m_TargetDC,
						x*m_TileWidth, y*m_TileHeight,
						m_TileWidth, m_TileHeight,
						m_hDC,
						(TileID-1)*m_TileWidth, 0,
						SRCCOPY);
			}
		}
	}

	return;
}

void MapView::EraseTile(long X, long Y)
{
    /*	Purpose:	Erase a tile from the map's visualization

		Parameters:	X,Y - tile coordinates to erase

		Return:		none

		Exceptions:

	*/

    //make a black brush and pen
    HBRUSH Brush = CreateSolidBrush(RGB(0,0,0));
    HPEN Pen = CreatePen(PS_SOLID, 1, RGB(0,0,0));

    Brush = HBRUSH (SelectObject(*m_TargetDC, Brush));
    Pen = HPEN (SelectObject(*m_TargetDC, Pen));

    Rectangle(*m_TargetDC, X*m_TileHeight, Y*m_TileWidth, (X+1)*m_TileHeight, (Y+1)*m_TileWidth);

    Brush = HBRUSH (SelectObject(*m_TargetDC, Brush));
    Pen = HPEN (SelectObject(*m_TargetDC, Pen));

    DeleteObject(Brush);
    DeleteObject(Pen);

    return;
}

void MapView::SetBitmap(string Filename, const long TileCount)
{
	/*	Purpose:	load the bitmap stored as Filename

		Parameters:	Filename -	filename of bitmap to load
					TileCount - The number of tiles in the bitmap.  They are assumed to be arranged
								linearly and sequentially

		Return:		none

		Exceptions:

	*/

	BitmapLoader BM;

	//set the tilecount member
	m_TileCount = TileCount;

	//get the bitmap opened
	BM.OpenBitmap(Filename);

	//get some basic specs we need for DC creation
	m_TileWidth = BM.GetBitmapWidth()/m_TileCount;
	m_TileHeight = BM.GetBitmapHeight();

	//create a DC, load in a bitmap
	HDC screen = GetDC(NULL);
	m_hDC = CreateCompatibleDC(screen);
	m_hBitmap = CreateCompatibleBitmap(screen, m_TileWidth*m_TileCount, m_TileHeight);
	m_hOldBitmap = (HBITMAP)SelectObject(m_hDC, m_hBitmap);
	ReleaseDC(NULL, screen);

	//paint the bitmap into the DC and get some specs
	BM.CopyBitmapToDC(&m_hDC);

	return;
}

MapView::~MapView()
{
	/*	Purpose:	Free references

		Parameters:	none

		Return:		none

		Exceptions:

	*/

	//put the old bitmap back in, and kill the new one
	SelectObject(m_hDC, m_hOldBitmap);
	DeleteObject(m_hBitmap);

	//get rid of the dc
	DeleteDC(m_hDC);

	return;
}
