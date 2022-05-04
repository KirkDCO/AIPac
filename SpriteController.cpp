/*	SpriteController.cpp
	Robert Kirk DeLisle
	28 August 2006

	Purpose:	Controller to serve as an interface between SpriteModel, SpriteView, and other classes

	Modification History:

*/

#if !defined(SpriteController_RKD_28Aug06)
#define SpriteController_RKD_28Aug06

#include "SpriteController.h"
#include <fstream>
SpriteController::SpriteController()
{
	/*	Purpose:	Really just makes the connections

		Parameters:	none

		Return:		none

		Exceptions:

	*/

    m_pBrain = 0; //set the brain to null for error checking later
	m_View.Connect(&m_Model);

	return;
}

void SpriteController::UpdateSpeed(bool loggit)
{
    /*	Purpose:	What direction are we being told to go??  Set speed and state appropriately.

		Parameters:	none

		Return:		none

		Exceptions:

	*/

	long state;
    long XSpeed, YSpeed;

	if ( m_pBrain->GetNextDirection(state, XSpeed, YSpeed, loggit) == 1 )
	{
        m_Model.SetState(state);
        m_Model.SetSpeed(XSpeed, YSpeed);
	}
//this block is used to monitor details during different runs for each call to this function
//I commented it out to eliminate an extra comparison every time
//    if (loggit)
//    {
//        fstream lf;
//        lf.open("InOut.csv", ios::out|ios::app);
//        lf << state << "," << XSpeed << "," << YSpeed << endl;
//        lf.close();
//    }

	return;
}

#endif
