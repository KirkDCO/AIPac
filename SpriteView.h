/*	SpriteView.h
	Robert Kirk DeLisle
	1 August 2006

	Purpose:	Define an interface for the SpriteView and expose basic operations for a view visualization.
				This version will be for a general 2D view but starts to define the basic interface
				if I should want to make a more general/extensible set of classes.

	Modification History:

*/

#if !defined(SpriteView_RKD_01Aug06)
#define SpriteView_RKD_01Aug06

#include <windows.h>
#include <vector>
#include <map>
#include <string>
using namespace std;

#include "SpriteModel.h"

class SpriteView
{
	private:

		//MVC links
		SpriteModel *m_Model;

		map<long, vector<long> > m_mapAnimationSequence;
			//holds the order of the frames that make up the animation for the sprite
			//m_frameCounter holds the current index of the animation sequence.

		long m_FrameCounter;
			//index into m_animationSequence vector

		long m_TileCount;
			//how many tiles (frames) are available in the bitmap?

		HDC *m_TargetDC;
			//target of all drawing functions

		HDC m_SpriteDC;
			//DC to contain the internal bitmap

		HDC m_EraseDC;

		HDC m_MaskDC;

		HBITMAP m_EraseBitmap;
			//used to hold a picture of the map before the sprite was drawn

		HBITMAP m_OldEraseBitmap;
			//used to free resources later

		HBITMAP m_SpriteBitmap;
			//holds the graphic animation frames of the sprite

		HBITMAP m_OldSpriteBitmap;
			//holds handle to the old bitmap for resource freeing

        HBITMAP m_Mask;
            //holds handle to the mask

        HBITMAP m_OldMask;


		long m_Width;
			//width of the graphic cell (tile) in pixels

		long m_Height;
			//height of the graphic cell (tile) in pixels

		bool m_EraseActive;
			//if true, there is something in the erase buffer

        HBITMAP CreateBitmapMask(COLORREF crTransparent);

	public:

		SpriteView() {};
		~SpriteView();

		void Connect(SpriteModel *Model)
			{  m_Model = Model;  };
			//set the MVC links

		void SetTargetDC(HDC *TargetDC)
			{  m_TargetDC = TargetDC;  };
			//set the target for all drawing functions

		void SetBitmap(string FileName, const long Tilecount);
			//set the bitmap to the internal DC

		void Erase();
			//erase the sprite from the TargetDC

		void Draw();
			//draw the sprite to the TargetDC

        void StoreEraseBuffer();
            //grab a copy of where we're drawing for later erase

		void SetAnimationSequence(long State, vector<long> &Seq)
			{  m_mapAnimationSequence[State] = Seq;  m_FrameCounter = 0;  };
			//associate an animation sequence with a state, and set the frame counter to the first in the seq.

         long GetWidth() const
            {  return m_Width;  }

        long GetHeight() const
            {  return m_Height;  }

        void ResetFrameCounter()
            {  m_FrameCounter = 0;  };

        void ClearBackbuffer()
            { BitBlt(m_EraseDC,0,0,m_Width,m_Height,NULL,NULL,NULL,BLACKNESS); }
};

#endif
