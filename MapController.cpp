/*	MapController.cpp
	Robert Kirk DeLisle
	25 August 2006

	Purpose:	Serves as an interface between MapModel, MapView and other classes

	Modification History:

*/

#include "MapController.h"


MapController::MapController()
{
	/*	Purpose:	Consturct the Map MVC triad

		Parameters:	hInst	-	handle to the application instance, used for some bitmap operations

		Return:		none

		Exceptions:

	*/

	//set up the MVC connections
	m_View.Connect(&m_Model);

	return;
}

 void MapController::LoadMap(const string MapFile)
{
	/*	Purpose:	Specifies a map file to be loaded into the model

		Parameters:	MapFile	-	Location of a CSV file containing a map.

		Return:		none

		Exceptions:	caught and thrown from m_Model->LoadMap()

	*/

	//tell the model to load a map file
	try
	{
		m_Model.LoadMap(MapFile);
	}
	catch(ExceptionRKD &ex)
	{
		throw ex;
	}

	return;
}

long MapController::GetPixelPosition(const long X, const long Y) const
{
    /*	Purpose:	get the value of the tile at pixel postion (X,Y)
                    requires conversion between pixels and tiles

		Parameters:	X,Y	-	Pixel positions to check

		Return:		none

		Exceptions:

	*/

	//validate pixels
//	if ( (X<0 || Y<0) || ( X>=m_Model.Width() || Y <=m_Model.Height() ) )
        //out of bounds

    long tileX, tileY;

    tileX = (X/m_View.GetTileWidth());
    tileY = (Y/m_View.GetTileHeight());

    return m_Model.GetPosition(tileX, tileY);
}

bool MapController::CheckMapByPixels(const long MinX, const long MaxX, const long MinY, const long MaxY, long &ret) const
{
    /*	Purpose:	check the content of the map within the given rectangle.

		Parameters:	MinX, MaxX, MinY, MaxY - defines the rectangle to check in pixels

		Return:		true if the rectangle contains only 0s
                    false if the rectangle contains only 1s

		Exceptions:

	*/

	long x, y;  //counter indices
	bool bSuccess = true;  //optimistic outlook
    ret = 0;

	//convert pixels to tiles
	long MinTileX = MinX/m_View.GetTileWidth();
	long MaxTileX = MaxX/m_View.GetTileWidth();
	long MinTileY = MinY/m_View.GetTileHeight();
	long MaxTileY = MaxY/m_View.GetTileHeight();


    if (MaxTileX >= m_Model.Width())
        MaxTileX = m_Model.Width()-1;

    if (MaxTileY >= m_Model.Height())
        MaxTileY = m_Model.Height()-1;

	for (x=MinTileX; x<=MaxTileX; x++)
	{
	    for (y=MinTileY; y<=MaxTileY; y++)
	    {
	        if ( m_Model.GetPosition(x,y) != 0 )
	        {
	            //found something - set the flag
                bSuccess = false;
                ret = m_Model.GetPosition(x,y);
	        }
	    }
	}

	return bSuccess;
}

long MapController::ModifyTileByPixel(const long MinX, const long MaxX, const long MinY, const long MaxY)
{
    /*	Purpose:	Change the value of the tile(s) occuring within X,Y pixels

		Parameters:	X,Y - pixel

		Return:		none

		Exceptions:

	*/

	long x, y;  //counter indices
    long mods=0;

	//convert pixels to tiles
	long MinTileX = MinX/m_View.GetTileWidth();
	long MaxTileX = MaxX/m_View.GetTileWidth();
	long MinTileY = MinY/m_View.GetTileHeight();
	long MaxTileY = MaxY/m_View.GetTileHeight();

    if (MaxTileX >= m_Model.Width())
        MaxTileX = m_Model.Width()-1;

    if (MaxTileY >= m_Model.Height())
        MaxTileY = m_Model.Height()-1;

	for (x=MinTileX; x<=MaxTileX; x++)
	{
	    for (y=MinTileY; y<=MaxTileY; y++)
	    {
	        if ( m_Model.CheckPosition(x,y) )
                mods++;

	        m_Model.ModifyTile(x,y,0);
	        m_View.EraseTile(x,y);
	    }
	}

	return mods;
}





