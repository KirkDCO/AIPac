/*	SpriteModel.h
	Robert Kirk DeLisle
	30 July 2006

	Purpose:  Data model for a sprite.

	Modificaton History:

*/

#if !defined(SpriteModel_RKD_30July2006)
#define	SpriteModel_RKD_30July2006

#include "Enumerations.h"

class SpriteModel
{
	private:

		long m_CurrentX;
		long m_CurrentY;
			//current location of the upper left corner in pixels

        long m_PreviousX;
        long m_PreviousY;
            //used to keep track of previous locations

		long m_MinX;
		long m_MaxX;
		long m_MinY;
		long m_MaxY;
			//bounds

		bool m_BoundaryWrap;
			//determines if boundaries wrap around or if they are hard bounds

		long m_SpeedX;
			//the sprite's speed

		long m_SpeedY;
			//the sprite's speed

		long m_PreviousSpeedX;
			//previous X speed for boundary checking

		long m_PreviousSpeedY;
			//previous Y speed for boundary checking

        long m_PreviousState;
		long m_State;
			//holds a value corresponding to the state of the sprite
			//it is up to the user to interpret this value and make necessary changes based upon it

	public:

		SpriteModel()
			{ m_CurrentX=0; m_CurrentY=0; m_SpeedX=0; m_SpeedY=0;
				m_MinX=0; m_MaxX=0; m_MinY=0; m_MaxY=0;
				m_BoundaryWrap=false; m_SpeedX=0; m_SpeedY=0;
				m_PreviousSpeedX=0; m_PreviousSpeedY=0; m_State=0;
				m_PreviousX=0; m_PreviousY=0; m_PreviousState=0; };
			//basic initialization to position 0,0 with no velocity

		void Move(bool);
			//move the sprite based on the current speed with some bounds checking

		void GetPosition(long &X, long &Y) const
			{ X = m_CurrentX; Y = m_CurrentY; };
			//get the X and Y position in the parameters - position corresponds to the upper left corner

        void GetPreviousPosition(long &X, long &Y) const
            { X = m_PreviousX;  Y=m_PreviousY;  };
            //get the stored, historical position

		void SetPosition(long &X, long &Y);
			//set the X and Y position, and return the old position in the parameters

		void SetMaxMinBounds(const long MinX, const long MaxX, const long MinY, const long MaxY)
			{ m_MinX = MinX; m_MaxX = MaxX; m_MinY = MinY; m_MaxY = MaxY; };
			//set the maximum and minimum X and Y positions for bounds checking

		void SetBoundaryWrap(const bool Wrap)
			{ m_BoundaryWrap = Wrap; };
			//set the ability for boundaries to auto wrap

		void GetSpeed(long &XSpeed, long &YSpeed) const
			{ XSpeed = m_SpeedX; YSpeed = m_SpeedY; };
			//return the X and Y speed in the parameters

        void GetPreviousSpeed(long &XSpeed, long &YSpeed)
            { XSpeed = m_PreviousSpeedX; YSpeed = m_PreviousSpeedY;  };

		void SetSpeed(long &XSpeed, long &YSpeed);
			//set the speed variables and return the old X and Y speed in the parameters

		void GetState(long &oldState) const
			{ oldState = m_State;};
			//return the state variable.

		void SetState(long &newState);
			//set the state and return the old state

        void RevertState();
        void RevertSpeed();
            //revert back to the old speed/state

		~SpriteModel() {}; //nothing to do for clean up
};

#endif
