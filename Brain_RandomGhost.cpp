/*	Brain_RandomGhost.cpp
	Robert Kirk DeLisle
	5 November 2006

	Purpose:  Ghost AI in which the ghost moves toward the PacMan or in a random direction with random probability.

	Modificaton History:

*/

#include "Brain_RandomGhost.h"

long Brain_RandomGhost::GetNextDirection(long &State, long &XSpeed, long &YSpeed, bool loggit)
{
    /*	Purpose:	Get the next direction for a ghost based on the ghost and world conditions.

		Parameters:	State - return parameter for the desired state of the ghost - defined in Enumerations.h
                    XSpeed, YSpeed - return parameters for the desired speed

		Return:		0 if no action is taken
                    1 if a change of state/speed is done

		Exceptions:

	*/

    long decision = int((rand() %100) +1);  //are we going toward the pacman or random?
    long retval = 0; //success or failure return
    long direction;

    long MeXPos, MeYPos;
    long PacXPos, PacYPos;
    long Xdist, Ydist; //distances to PacMan

    long spriteState;

    //get the state of the sprite for later use
    m_pSprite->GetState(spriteState);

    if ( spriteState == Ghost_DeadUp || spriteState == Ghost_DeadDown ||
         spriteState == Ghost_DeadLeft || spriteState == Ghost_DeadRight )
    {
        //head back to the cage!
        direction = int((rand() %2) +1); //move up/down or left/right
        m_pSprite->GetPosition(MeXPos, MeYPos);
        if ( (rand() % 2) > 0 )
        {
            switch(direction)
            {
                case 1:
                    if ( MeYPos<224 )
                    {
                        State = Ghost_DeadDown;
                        XSpeed = 0;
                        YSpeed = 8;
                        retval = 1;
                    }
                    else
                    {
                        State = Ghost_DeadUp;
                        XSpeed = 0;
                        YSpeed = -8;
                        retval = 1;
                    }
                    break;

                case 2:
                    if ( MeXPos<224 )
                    {
                        State = Ghost_DeadRight;
                        XSpeed = 8;
                        YSpeed = 0;
                        retval = 1;
                    }
                    else
                    {
                        State = Ghost_DeadLeft;
                        XSpeed = -8;
                        YSpeed = 0;
                        retval = 1;
                    }
                    break;
            }
        }
        else
        {
            direction = int((rand() %4) +1);

            switch(direction)
            {
                case 1:
                    State = Ghost_DeadUp;
                    XSpeed = 0;
                    YSpeed = -8;
                    retval = 1;
                    break;

                case 2:
                    State = Ghost_DeadDown;
                    XSpeed = 0;
                    YSpeed = 8;
                    retval = 1;
                    break;

                case 3:
                    State = Ghost_DeadLeft;
                    XSpeed = -8;
                    YSpeed = 0;
                    retval = 1;
                    break;

                case 4:
                    State = Ghost_DeadRight;
                    XSpeed = 8;
                    YSpeed = 0;
                    retval = 1;
                    break;
            }
        }
    }
    else
    {

        if ( decision > (m_MoveProbability*100) )
        {
            direction = int((rand() %4) +1); //pick a direction

            //move in a random direction
            switch(direction)
            {
                case 1:
                    if ( spriteState == Ghost_HuntUp || spriteState == Ghost_HuntDown ||
                         spriteState == Ghost_HuntRight || spriteState == Ghost_HuntLeft )
                        State = Ghost_HuntUp;
                    else if ( spriteState == Ghost_BlueUp || spriteState == Ghost_BlueDown ||
                         spriteState == Ghost_BlueRight || spriteState == Ghost_BlueLeft )
                        State = Ghost_BlueUp;
                    XSpeed = 0;
                    YSpeed = -8;
                    retval = 1;
                    break;

                case 2:
                    if ( spriteState == Ghost_HuntUp || spriteState == Ghost_HuntDown ||
                         spriteState == Ghost_HuntRight || spriteState == Ghost_HuntLeft )
                        State = Ghost_HuntDown;
                    else if ( spriteState == Ghost_BlueUp || spriteState == Ghost_BlueDown ||
                         spriteState == Ghost_BlueRight || spriteState == Ghost_BlueLeft )
                        State = Ghost_BlueDown;
                    XSpeed = 0;
                    YSpeed = 8;
                    retval = 1;
                    break;

                case 3:
                    if ( spriteState == Ghost_HuntUp || spriteState == Ghost_HuntDown ||
                         spriteState == Ghost_HuntRight || spriteState == Ghost_HuntLeft )
                        State = Ghost_HuntLeft;
                    else if ( spriteState == Ghost_BlueUp || spriteState == Ghost_BlueDown ||
                         spriteState == Ghost_BlueRight || spriteState == Ghost_BlueLeft )
                        State = Ghost_BlueLeft;
                    XSpeed = -8;
                    YSpeed = 0;
                    retval = 1;
                    break;

                case 4:
                    if ( spriteState == Ghost_HuntUp || spriteState == Ghost_HuntDown ||
                         spriteState == Ghost_HuntRight || spriteState == Ghost_HuntLeft )
                        State = Ghost_HuntRight;
                    else if ( spriteState == Ghost_BlueUp || spriteState == Ghost_BlueDown ||
                         spriteState == Ghost_BlueRight || spriteState == Ghost_BlueLeft )
                        State = Ghost_BlueRight;
                    XSpeed = 8;
                    YSpeed = 0;
                    retval = 1;
                    break;
            }
        }
        else
        {
            //move toward/away from the pacman if hunt/blue
            m_pWorld->GetPacManController()->GetPosition(PacXPos, PacYPos);
            m_pSprite->GetPosition(MeXPos, MeYPos);

            Xdist = MeXPos - PacXPos;
            Ydist = MeYPos - PacYPos;

            if ( abs(Xdist) > abs(Ydist) )
            {
                if ( Xdist > 0 )
                {
                    if ( spriteState == Ghost_HuntUp || spriteState == Ghost_HuntDown ||
                         spriteState == Ghost_HuntRight || spriteState == Ghost_HuntLeft )
                    {
                        State = Ghost_HuntLeft;
                        XSpeed = -8;
                        YSpeed = 0;
                        retval = 1;
                    }
                    else if ( spriteState == Ghost_BlueUp || spriteState == Ghost_BlueDown ||
                         spriteState == Ghost_BlueRight || spriteState == Ghost_BlueLeft )
                    {
                        State = Ghost_BlueRight;
                        XSpeed = 8;
                        YSpeed = 0;
                        retval = 1;
                    }
                }
                else
                {
                    if ( spriteState == Ghost_HuntUp || spriteState == Ghost_HuntDown ||
                         spriteState == Ghost_HuntRight || spriteState == Ghost_HuntLeft )
                    {
                        State = Ghost_HuntRight;
                        XSpeed = 8;
                        YSpeed = 0;
                        retval = 1;
                    }
                    else if ( spriteState == Ghost_BlueUp || spriteState == Ghost_BlueDown ||
                         spriteState == Ghost_BlueRight || spriteState == Ghost_BlueLeft )
                    {
                        State = Ghost_BlueLeft;
                        XSpeed = -8;
                        YSpeed = 0;
                        retval = 1;
                    }
                }
            }
            else
            {
                if ( Ydist > 0 )
                {
                    if ( spriteState == Ghost_HuntUp || spriteState == Ghost_HuntDown ||
                         spriteState == Ghost_HuntRight || spriteState == Ghost_HuntLeft )
                        State = Ghost_HuntUp;
                    else if ( spriteState == Ghost_BlueUp || spriteState == Ghost_BlueDown ||
                         spriteState == Ghost_BlueRight || spriteState == Ghost_BlueLeft )
                        State = Ghost_BlueDown;
                    XSpeed = 0;
                    YSpeed = -8;
                    retval = 1;
                }
                else
                {
                    if ( spriteState == Ghost_HuntUp || spriteState == Ghost_HuntDown ||
                         spriteState == Ghost_HuntRight || spriteState == Ghost_HuntLeft )
                        State = Ghost_HuntDown;
                    else if ( spriteState == Ghost_BlueUp || spriteState == Ghost_BlueDown ||
                         spriteState == Ghost_BlueRight || spriteState == Ghost_BlueLeft )
                        State = Ghost_BlueUp;
                    XSpeed = 0;
                    YSpeed = 8;
                    retval = 1;
                }
            }
        }
    }

    return retval;
}
