/*	Brain_Keyboard.cpp
	Robert Kirk DeLisle
	20 September 2006

	Purpose:  Defines an interface the keyboard controlling directional choices.

	Modificaton History:

*/

#include "Brain_Keyboard.h"
#include "Enumerations.h"

long Brain_Keyboard::GetNextDirection(long &State, long &XSpeed, long &YSpeed, bool loggit)
{
    /*	Purpose:	Get the next direction using the keyboard.

		Parameters:	none

		Return:		Next direction based on keyboard state.
                    Return values are defined in Enumerations.h

		Exceptions:

	*/

	if ( GetAsyncKeyState(VK_UP) & 0x8000 )
    {
        XSpeed = 0;
        YSpeed = -8;
        State = PacMan_ActiveUp;
        return 1;
    }
    else if ( GetAsyncKeyState(VK_DOWN) & 0x8000 )
    {
        XSpeed = 0;
        YSpeed = 8;
        State = PacMan_ActiveDown;
        return 1;
    }
    else if ( GetAsyncKeyState(VK_LEFT) & 0x8000 )
    {
        XSpeed = -8;
        YSpeed = 0;
        State = PacMan_ActiveLeft;
        return 1;
    }
    else if ( GetAsyncKeyState(VK_RIGHT) & 0x8000 )
    {
        XSpeed = 8;
        YSpeed = 0;
        State = PacMan_ActiveRight;
        return 1;
    }
    else
    {
        //no direction keys found
        return 0;
    }
}




