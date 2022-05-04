/*	SpriteController.h
	Robert Kirk DeLisle
	22 August 2006

	Purpose:	Controller to serve as an interface between SpriteModel, SpriteView, and other classes

	Modification History:

*/

#if !defined(SpriteController_RKD_22Aug06)
#define SpriteController_RKD_22Aug06

#include <string>
using namespace std;

#include "SpriteModel.h"
#include "SpriteView.h"
#include "IBrain.h"

class SpriteController
{
	private:

		//MVC links
		SpriteModel m_Model;
		SpriteView m_View;

		//AI (or otherwise) link to directional control
		IBrain *m_pBrain;

	public:

		SpriteController();
		~SpriteController() {};

		void SetTargetDC(HDC *TargetDC)
			{  m_View.SetTargetDC(TargetDC);  };
			//set the target DC so we don't have to keep passing it around

		void Erase()
			{  m_View.Erase();  };
			//tell the view to erase the sprite from the targetDC

		void Draw()
			{  m_View.Draw();  };
			//tell the view to draw the sprite to the targetDC

        void StoreEraseBuffer()
            {  m_View.StoreEraseBuffer();  };

		void SetPosition(long &X, long &Y)
			{  m_Model.SetPosition(X, Y);  };
			//set the X and Y position, and return the old position in the parameters

		void SetPosition(long X, long Y)
			{  long XPos=X, YPos=Y;  m_Model.SetPosition(XPos, YPos);  };
			//overloaded option in case the caller passes hard constants

        void GetPosition(long &X, long &Y)
            {  m_Model.GetPosition(X,Y);  };

		void SetMaxMinBounds(const long MinX, const long MaxX, const long MinY, const long MaxY)
			{  m_Model.SetMaxMinBounds(MinX, MaxX, MinY, MaxY);  };
			//set the maximum and minimum X and Y positions for bounds checking

		void SetBoundaryWrap(const bool Wrap)
			{  m_Model.SetBoundaryWrap(Wrap);  };
			//set the ability for boundaries to auto wrap

		void SetSpeed(long &XSpeed, long &YSpeed)
			{  m_Model.SetSpeed(XSpeed, YSpeed);  };
			//set the speed variables and return the old X and Y speed in the parameters

        void GetSpeed(long &XSpeed, long &YSpeed)
            {  m_Model.GetSpeed(XSpeed, YSpeed);  };

        void GetPreviousSpeed(long &XSpeed, long &YSpeed)
            { m_Model.GetPreviousSpeed(XSpeed, YSpeed);  };

		void SetState(long &newState)
			{  m_Model.SetState(newState);
                m_View.ResetFrameCounter();
            };
			//set the state and return the old state

        void GetState(long &state)
            {  return m_Model.GetState(state);  };

		void SetBitmap(string Filename, const long TileCount)
			{  m_View.SetBitmap(Filename, TileCount);  };
			//set the bitmap for the view

		void SetAnimationSequence(long State, vector<long> &Seq)
			{  m_View.SetAnimationSequence(State, Seq);  };
			//Associates a particular animation sequence with a state

        void SetSpriteBrain(IBrain *Brain)
            {   m_pBrain = Brain;  };
            //set the AI, keyboard, etc.

        void UpdateSpeed(bool loggit);
            //update the head on the sprite based on the Brain

        void Move( bool loggit )
            {  m_Model.Move( loggit );  };
            //move the sprite

        void RevertState()
            {  m_Model.RevertState();  };
        void RevertSpeed()
            { m_Model.RevertSpeed(); };
            //revert to the "previous" speed/

        long GetWidth() const
            {  return m_View.GetWidth();  };

        long GetHeight() const
            {  return m_View.GetHeight();  };

        void ClearBackbuffer()
            {  m_View.ClearBackbuffer();  };
};

#endif
