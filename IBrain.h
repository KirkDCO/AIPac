/*	IBrain.h
	Robert Kirk DeLisle
	20 September 2006

	Purpose:  Defines an interface to AI (or otherwise) controlling directional choices.

	Modificaton History:

*/

#if !defined(IBrain_20Sep06_RKD)
#define IBrain_20Sep06_RKD

#include "Enumerations.h"

class IBrain
{
    private:

    public:
        IBrain() {};
        ~IBrain() {};

        virtual long GetNextDirection(long &State, long &XSpeed, long &YSpeed, bool loggit)=0;
            //returns the next direction to move
            //directions defined in Enumerations.h
};

#endif
