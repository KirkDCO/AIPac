/*	WorldView.cpp
	Robert Kirk DeLisle
	19 August 2006

	Purpose:  Defines a View on the World Model that is Win32 platform specific and
				expects graphical objects to be resources (bitmaps are not DIBs, but rather
				resources within the project).

	Modificaton History:

*/
#include <string>

#include "WorldView.h"
void WorldView::Clear()
{
    BitBlt( m_hBackBuffer,
				0,0,
				m_PixelWidth, m_PixelHeight,
				m_hBackBuffer,
				0,0,
				BLACKNESS);
}


void WorldView::Draw(HDC TargetDC) const
{
	/*	Purpose:	Draw the backbuffer to the target

		Parameters: TargetDC - where the drawing will occur

		Return:		none

		Exceptions:

	*/

    //convert the score to a string
    string score;
    char cscore[10];
    score = itoa(m_Model->GetScore(),cscore,10);

	//write out the score
	SetTextColor(m_hBackBuffer, RGB(255,0,0));
	SetBkColor(m_hBackBuffer, RGB(0,0,0));
	TextOut(m_hBackBuffer, 0, 0, score.c_str(), score.length());

    //draw the backbuffer to the target
	BitBlt( TargetDC,
				0,0,
				m_PixelWidth, m_PixelHeight,
				m_hBackBuffer,
				0,0,
				SRCCOPY);

	return;
}

void WorldView::SetViewDimensions(const long Height, const long Width)
{
	/*	Purpose:	Set the dimensions of the view and create the backbuffer

		Parameters:	Height and Width in pixels

		Return:		none

		Exceptions:

	*/

	//store the dimensions
	m_PixelHeight = Height;
	m_PixelWidth = Width;

	//create the DC compatible with the screen and get a bitmap inside for drawing
	HDC screen = GetDC(NULL);
	m_hBackBuffer = CreateCompatibleDC(screen);
	m_hBitmap = CreateCompatibleBitmap(screen, m_PixelWidth, m_PixelHeight);
	ReleaseDC(NULL, screen);

	m_hOldBitmap = (HBITMAP) SelectObject(m_hBackBuffer, m_hBitmap);

	return;
}

WorldView::~WorldView()
{
	/*	Purpose:	Free the GDI references

		Parameters:	none

		Return:		none

		Exceptions:

	*/

	//put the old bitmap back in, delete the new
	SelectObject(m_hBackBuffer, m_hOldBitmap);
	DeleteObject(m_hBitmap);

	//get rid of the backbuffer
	DeleteDC(m_hBackBuffer);

	return;
}
