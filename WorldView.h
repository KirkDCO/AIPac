/*	WorldView.h
	Robert Kirk DeLisle
	19 August 2006

	Purpose:  Defines a View on the World Model that is Win32 platform specific and
				expects graphical objects to be resources (bitmaps are not DIBs, but rather
				resources within the project).

	Modificaton History:

*/

#if !defined(WorldView_RKD_19Aug06)
#define WorldView_RKD_19Aug06

#include <windows.h>

#include "WorldModel.h"

class WorldView
{
	private:

		WorldModel *m_Model;
			//MVC connection

		//need a backbuffer and bitmap
		HDC m_hBackBuffer;
		HBITMAP m_hBitmap;
		HBITMAP m_hOldBitmap;

		//dimensions of the world based on the Maze
		long m_PixelHeight;
		long m_PixelWidth;

	public:

		WorldView() {};
		~WorldView();

		void Connect(WorldModel *Model)
			{  m_Model = Model;  }
			//connect to the model

		void Draw(HDC TargetDC) const;
			//draw the world to the screen (assumed to be the passed DC)

        void Clear();
            //cleans off the backbuffer for clean drawing after reset

		HDC *GetBackBuffer()
			{  return &m_hBackBuffer;  }
			//provide access to the BackBuffer so other views can draw onto it

		void SetViewDimensions(const long Height, const long Width);
			//set the dimensions and create the backbuffer
};

#endif
