/*	SpriteView.cpp
	Robert Kirk DeLisle
	28 August 2006

	Purpose:	Define an interface for the SpriteView and expose basic operations for a view visualization.
				This version will be for a general 2D view but starts to define the basic interface
				if I should want to make a more general/extensible set of classes.

	Modification History:

*/

#include "SpriteView.h"
#include "Utilities\\BitmapLoader.h"

void SpriteView::SetBitmap(string Filename, const long TileCount)
{
	/*	Purpose:	Load the bitmap into the internal DC

		Parameters:	Filename - name of the bitmap to load in
					TileCount - actually the frame count, or how many animation frames are in the bit
						assumed that the frames/tiles are arranged linearlly in the bitmap

		Return:		none

		Exceptions:

	*/

	BitmapLoader BM;

	//set the tilecount member
	m_TileCount = TileCount;

	//get the bitmap opened
	BM.OpenBitmap(Filename);

	//get some basic specs we need for DC creation
	m_Width = BM.GetBitmapWidth()/m_TileCount;
	m_Height = BM.GetBitmapHeight();

	//create a DC, load in a bitmap
	HDC screen = GetDC(NULL);
	m_SpriteDC = CreateCompatibleDC(screen);
	m_SpriteBitmap = CreateCompatibleBitmap(screen, m_Width*m_TileCount, m_Height);

	//and a mask
	m_MaskDC = CreateCompatibleDC(screen);
	m_Mask = CreateBitmapMask(RGB(0,0,0)); //black is background color
	m_OldMask = (HBITMAP)SelectObject(m_MaskDC, m_Mask);

	//select the bitmap into the DC
	m_OldSpriteBitmap = (HBITMAP)SelectObject(m_SpriteDC, m_SpriteBitmap);

	//paint the bitmap into the DC and get some specs
	BM.CopyBitmapToDC(&m_SpriteDC);

	//need to also create an erase buffer
	m_EraseDC = CreateCompatibleDC(screen);
	m_EraseBitmap = CreateCompatibleBitmap(screen, m_Width, m_Height);
	m_OldEraseBitmap = (HBITMAP)SelectObject(m_EraseDC, m_EraseBitmap);

    ReleaseDC(NULL, screen);

	return;
}

SpriteView::~SpriteView()
{
	/*	Purpose:	Free all the references.

		Parameters:	none

		Return:		none

		Exceptions:

	*/

	SelectObject(m_SpriteDC, m_OldSpriteBitmap);
	SelectObject(m_EraseDC, m_OldEraseBitmap);
	SelectObject(m_MaskDC, m_OldMask);

	DeleteObject(m_SpriteBitmap);
	DeleteObject(m_EraseBitmap);
	DeleteObject(m_Mask);

	DeleteDC(m_SpriteDC);
	DeleteDC(m_EraseDC);
	DeleteDC(m_MaskDC);

	return;
}

void SpriteView::Erase()
{
	/*	Purpose:	Erase the sprite by copying the contents of the erase buffer to the TargetDC in the current
					sprite location.  Need to check to make sure something is in the erase buffer.

		Parameters:	none

		Return:		none

		Exceptions:

	*/

	//check whether there is something in the erase buffer
	if ( !m_EraseActive)
		return;

	//get the current X and Y of the sprite
	long PreviousX, PreviousY;
	m_Model->GetPreviousPosition(PreviousX, PreviousY);

	//erase the sprite
	BitBlt(  *m_TargetDC,
			 PreviousX, PreviousY,
			 m_Width, m_Height,
			 m_EraseDC,
			 0,0,
			 SRCCOPY);

	return;
}

void SpriteView::StoreEraseBuffer()
{
	/*	Purpose:	Grab what we're drawing over from the targetDC

		Parameters:	none

		Return:		none

		Exceptions:

	*/

	//get the current X and Y of the sprite
	long CurrentX, CurrentY;
	m_Model->GetPosition(CurrentX, CurrentY);

	//get the section from the TargetDC into the EraseBuffer
	BitBlt(	m_EraseDC,
			0,0,
			m_Width, m_Height,
			*m_TargetDC,
			CurrentX, CurrentY,
			SRCCOPY);

	m_EraseActive = true;

	return;
}

void SpriteView::Draw()
{
	/*	Purpose:	Draw to the TargetDC.  First, copy the location from the TargetDC into the
					Erase buffer.

		Parameters:	none

		Return:		none

		Exceptions:

	*/

	//get the current X and Y of the sprite
	long CurrentX, CurrentY;
	m_Model->GetPosition(CurrentX, CurrentY);

	//get the current animation frame
	long state;
	m_Model->GetState(state);

	long frame;
	frame = m_mapAnimationSequence[state].at(m_FrameCounter);

	//now draw the frame to the TargetDC
	BitBlt(	*m_TargetDC,
			CurrentX, CurrentY,
			m_Width, m_Height,
			m_SpriteDC,
			(frame*m_Width),0,
			SRCCOPY);

//    BitBlt( *m_TargetDC,
//            CurrentX, CurrentY,
//            m_Width, m_Height,
//            m_MaskDC,
//            (frame*m_Width), 0,
//            SRCAND);
//
//    BitBlt( *m_TargetDC,
//            CurrentX, CurrentY,
//            m_Width, m_Height,
//            m_SpriteDC,
//            (frame*m_Width), 0,
//            SRCPAINT);

	//move to the next animation frame
	m_FrameCounter++;

	if ( m_FrameCounter == m_mapAnimationSequence[state].size() )
		m_FrameCounter=0;	//reset to the beginning if we hit the end

	return;
}

HBITMAP SpriteView::CreateBitmapMask(COLORREF crTransparent)
{
    HDC hdcMem, hdcMem2;
    HBITMAP hbmMask;
    BITMAP bm;
    HBITMAP tempBM1, tempBM2;

    // Create monochrome (1 bit) mask bitmap.

    GetObject(m_SpriteBitmap, sizeof(BITMAP), &bm);
    hbmMask = CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);

    // Get some HDCs that are compatible with the display driver

    hdcMem = CreateCompatibleDC(0);
    hdcMem2 = CreateCompatibleDC(0);

    tempBM1 = (HBITMAP)SelectObject(hdcMem, m_SpriteBitmap);
    tempBM2 = (HBITMAP)SelectObject(hdcMem2, hbmMask);

    // Set the background colour of the colour image to the colour
    // you want to be transparent.
    SetBkColor(hdcMem, crTransparent);

    // Copy the bits from the colour image to the B+W mask... everything
    // with the background colour ends up white while everythig else ends up
    // black...Just what we wanted.

    BitBlt(hdcMem2, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);

    // Take our new mask and use it to turn the transparent colour in our
    // original colour image to black so the transparency effect will
    // work right.
    BitBlt(hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem2, 0, 0, SRCINVERT);

    // Clean up.

    SelectObject(hdcMem, tempBM1);
    SelectObject(hdcMem2, tempBM2);

    DeleteDC(hdcMem);
    DeleteDC(hdcMem2);

    return hbmMask;
}

