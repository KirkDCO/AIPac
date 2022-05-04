/*	Brain_RandomGhost.h
	Robert Kirk DeLisle
	5 November 2006

	Purpose:  Ghost AI in which the ghost moves toward the PacMan or in a random direction with random probability.

	Modificaton History:

*/

#if !defined(Brain_RandomGhhost_20Sep2006_RKD)
#define Brain_RandomGhhost_20Sep2006_RKD

#include "IBrain.h"
#include <windows.h>
#include "SpriteController.h"
#include "WorldController.h"
#include <stdlib.h>
#include <time.h>

class Brain_RandomGhost : public IBrain
{
    public:
        Brain_RandomGhost(SpriteController *sprite, WorldController *world, double MoveProbability)
            { m_pSprite = sprite;  m_pWorld = world; m_MoveProbability = MoveProbability; };

        ~Brain_RandomGhost() {};

        long GetNextDirection(long &State, long &XSpeed, long &YSpeed, bool loggit);

    private:
        double m_MoveProbability;
            //what fraction of the time will the ghost move toward the PacMan

        Brain_RandomGhost() {};
            //privatized to prevent default construction

        SpriteController *m_pSprite;
            //ghost sprite being controlled by this AI
            //used to get details about the sprite

        WorldController *m_pWorld;
            //world in which the sprite lives
            //used to extract detailed information about the environment
};

#endif
