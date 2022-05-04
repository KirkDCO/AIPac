/*	SpriteModel.cpp
	Robert Kirk DeLisle
	30 July 2006

	Purpose:  Data model for a sprite.

	Modificaton History:

*/

#include "SpriteModel.h"
#include "Utilities\\ExceptionRKD.h"
#include <stdlib.h>
#include <fstream>

using namespace std;

void SpriteModel::Move(bool loggit)
{
	/*	Purpose:	move the sprite based on the current speed with some bounds checking
					bondary behavior determined by m_BoundaryWrap - either crisp or wrapping

		Parameters:	none

		Return:		none

		Exceptions:	none
	*/

    //store previous state
    m_PreviousState = m_State;

    //store previous speed
    m_PreviousSpeedX = m_SpeedX;
    m_PreviousSpeedY = m_SpeedY;

    //store the position before changing it
    m_PreviousX = m_CurrentX;
    m_PreviousY = m_CurrentY;

	//make the move
	m_CurrentX += m_SpeedX;
	m_CurrentY += m_SpeedY;

	//check boundary conditions
	if ( m_BoundaryWrap )
	{
		if ( m_CurrentX < m_MinX )
			m_CurrentX = m_MaxX-abs(m_SpeedX);
		else if ( m_CurrentX > m_MaxX-abs(m_SpeedX) )
			m_CurrentX = m_MinX;

		if ( m_CurrentY < m_MinY )
			m_CurrentY = m_MaxY-abs(m_SpeedY);
		else if ( m_CurrentY > m_MaxY-abs(m_SpeedY) )
			m_CurrentY = m_MinY;
	}
	else
	{
		if ( m_CurrentX < m_MinX )
			m_CurrentX = m_MinX;
		else if ( m_CurrentX > m_MaxX )
			m_CurrentX = m_MaxX;

		if ( m_CurrentY < m_MinY )
			m_CurrentY = m_MinY;
		else if ( m_CurrentY > m_MaxY )
			m_CurrentY = m_MaxY;
	}
//this block is used to monitor details during different runs for each call to this function
//I commented it out to eliminate an extra comparison every time
//    if (loggit)
//    {
//        fstream lf;
//        lf.open("InOut.csv", ios::out|ios::app);
//        lf << m_CurrentX << "," << m_CurrentY << endl;
//        lf.close();
//    }

	return;
}


void SpriteModel::SetPosition(long &X, long &Y)
{
	/*	Purpose:	set the sprite to a specific position
					and return the old position in the parameters

		Parameters:	X,Y - target position

		Return:		old X and Y in parameter

		Exceptions:	Target position out of bounds
	*/

	long tempX, tempY;  //swap variables

	//do some bounds checking
	if ( (X > m_MaxX) || (X < m_MinX) || (Y > m_MaxY) || (Y < m_MinY) )
		throw ExceptionRKD(201,"SpriteModel::SetPosition","Target position out of bounds.");

	tempX = m_CurrentX;
	tempY = m_CurrentY;

	m_CurrentX = X;
	m_CurrentY = Y;

	X = tempX;
	Y = tempY;

	//hold onto the position as "previous" for erase purposes
	m_PreviousX=m_CurrentX;
	m_PreviousY=m_CurrentY;

	return;
}


void SpriteModel::SetSpeed(long &XSpeed, long &YSpeed)
{
	/*	Purpose:	set the speed variables and return the old X and Y speed in the parameters

		Parameters:	XSpeed, YSpeed - new speeds

		Return:		old X and YSpeed in parameters

		Exceptions:	none
	*/

	long tempX, tempY;

	tempX = m_SpeedX;
	tempY = m_SpeedY;

	m_SpeedX = XSpeed;
	m_SpeedY = YSpeed;

	XSpeed = tempX;
	YSpeed = tempY;

	m_PreviousSpeedX = tempX;
	m_PreviousSpeedY = tempY;
}

void SpriteModel::SetState(long &newState)
{
	/*	Purpose:	set the state variable and return the old state in the parameter

		Parameters:	newState - the new state of the sprite

		Return:		old state in parameters

		Exceptions:	none
	*/

	long tempState;

	tempState = m_State;
	m_State = newState;
	newState = tempState;

	m_PreviousState = tempState;
}

void SpriteModel::RevertSpeed()
{
    /*	Purpose:	go back to the previous speed
                    if the previous is the same as the current - full stop

		Parameters:	none

		Return:		none

		Exceptions:	none
	*/

	if ( m_SpeedX == m_PreviousSpeedX && m_SpeedY == m_PreviousSpeedY)
	{
        m_SpeedX = 0;
        m_SpeedY = 0;
	}
	else
	{
        m_SpeedX = m_PreviousSpeedX;
        m_SpeedY = m_PreviousSpeedY;
	}

	return;
}

void SpriteModel::RevertState()
{
    /*	Purpose:	go back to the previous state

		Parameters:	none

		Return:		none

		Exceptions:	none
	*/

    m_State = m_PreviousState;
}
