/*	Brain_Keyboard.h
	Robert Kirk DeLisle
	20 September 2006

	Purpose:  Defines an interface the keyboard controlling directional choices.

	Modificaton History:

*/

#if !defined(Brain_Keyboard_20Sep2006_RKD)
#define Brain_Keyboard_20Sep2006_RKD

#include "IBrain.h"
#include <windows.h>

class Brain_Keyboard : public IBrain
{
    public:
        Brain_Keyboard() {};
        ~Brain_Keyboard() {};

        long GetNextDirection(long &State, long &XSpeed, long &YSpeed, bool loggit);

    private:

};

#endif
