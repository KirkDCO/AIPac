/*	WorldController.h
	Robert Kirk DeLisle
	23 August 2006

	Purpose:  Defines an interface between the WorldModel and the WorldView_Win32Resourced,
				Contains the primary game logic and is the central controller

	Modificaton History:

*/

#if !defined(WorldController_RKD_23Aug06)
#define WorldController_RKD_23Aug06

#include <windows.h>
#include <string>
using namespace std;

#include "WorldModel.h"
#include "WorldView.h"
#include "MapController.h"
#include "SpriteController.h"
#include "Utilities\\ParameterMap.h"

class WorldController
{
	private:

		WorldModel m_Model;
		WorldView m_View;
			//MVC links

		MapController m_MazeController;
		MapController m_DotsController;
			//access to the maze and the dots

		SpriteController m_PacManController;
		vector<SpriteController *> m_vecGhostControllers;
		vector<SpriteController *> m_vecPowerUpControllers;
		SpriteController m_FruitController;
			//access to all the sprites

		//ParameterMap to hold specific details and allow scripting
		ParameterMap m_Param;

		//how many ghosts do we want?
		long m_NumGhosts;  //allow 0 to 4 in the constructor from the param file

		long m_UpdateCount; //keeps a tally of the number of updates that have been called

		//initilizers for the various game pieces
		void InitTimers();

        void InitMaze();
        void InitDots();

        void InitPacMan();
        void InitGhosts();
        void InitPowerUps();

        void UpdateGhostSpeed( long i );

        bool m_MapDrawn;

        void CloseCage();

	public:

		WorldController(const string ParamFile);
			//standard constructor to get the world off to a good start

		~WorldController() {};

		void Draw(HDC TargetDC);
			//tell the worldview to draw to the target DC

        bool Update(bool loggit);
            //update for one clock tick

        void MovePacMan(bool loggit);
            //validate speed/direction and make the move

        void MoveGhosts();
            //validate speed/direction and make the move (part 2)

        void CheckDotCollisions();
            //look for collisions with dots and increment the score appropriately

        void CheckPowerUpCollisions();
            //look for powerup collisions and change states accordingly

        void CheckTimers();
            //check timers for everyone

        void CheckGhostCollisions();
            //check if PacMan collides with ghosts

        long GetScore()
            {  return m_Model.GetScore();  };
            //return the current score for the simulation

        long GetUpdateCount()
            {  return m_UpdateCount;  }

        long Width()
            {  return m_MazeController.GetPixelWidth();  };

        long Height()
            {  return m_MazeController.GetPixelHeight();  };

        void Reset();
            //reset the world to the base settings without destroying it.

        void SetPacBrain(IBrain *);
            //accept the incoming AI as the PacMan brain

        MapController *GetMazeController()
            {  return &m_MazeController; };

        MapController *GetDotController()
            {  return &m_DotsController;  };

        vector<SpriteController *> *GetGhostControllers()
            {  return &m_vecGhostControllers;  }

        vector<SpriteController *> *GetPowerUpControllers()
            {  return &m_vecPowerUpControllers;  };

        SpriteController *GetPacManController()
            {  return &m_PacManController;  }
};


#endif
