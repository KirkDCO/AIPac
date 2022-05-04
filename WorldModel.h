/*	WorldModel.h
	Robert Kirk DeLisle
	18 August 2006

	Purpose:  Representation for the PacMan world.
				primary logic center - game engine

	Modificaton History:

*/

#if !defined(WorldModel_RKD_18Aug2006)
#define	WorldModel_RKD_18Aug2006

#include <windows.h>

#include "MapModel.h"
#include "SpriteModel.h"

class WorldModel
{
	private:

		//counters used for various timings
		long m_FruitTimer;			//when does fruit get released
		long m_GhostTimer;	        //when do ghosts get released from the pen
		long m_PacManTimer;			//used to check PacMan's movement status - don't let him stagnate
		long m_PowerUpTimer;		//how long do powerups last

		long m_Score;
			//what is PacMan's score

		long m_GhostValue;
			//keep a check on scores for eating ghosts

		long m_Ticks;
			//how many updates/ticks/steps did it take to get here

        long m_GhostTicks;      //keeps track of ticks for ghosts
        long m_FruitTicks;      //keeps track of ticks for fruit
        long m_PacManTicks;     //keeps track of ticks for pacman
        long m_PowerUpTicks;    //keeps track of ticks for the powerups

        bool m_PowerUpActive;   //is the powerup timer being used or has it expired?

        bool m_GhostTimerActive;

	public:

		WorldModel()
			{  m_Score = 0;  m_GhostValue = 200;  m_Ticks = 0; m_PowerUpActive=false; m_GhostTimerActive = false; };
			//basic constructor

		~WorldModel() {};
			//clear out references

		void SetFruitTimer(const long TickCount)
			{  m_FruitTimer = TickCount;  };

		void SetGhostTimer(const long TickCount)
			{  m_GhostTimer = TickCount;  };

		void SetPacManTimer(const long TickCount)
			{  m_PacManTimer = TickCount;  };

		void SetPowerUpTimer(const long TickCount)
			{  m_PowerUpTimer = TickCount;  };

        void IncrementScore(long Points)
            {  m_Score += Points;  };

        long GetScore()
            {  return m_Score;  };

        long SetScore(long score)
            {  m_Score = score;  };

        void IncrementGameTimer()
            {  m_Ticks++;  };

        void StartPowerUpTimer()
            {
                m_PowerUpTicks = m_Ticks + m_PowerUpTimer;
                m_PowerUpActive = true;
                m_GhostValue = 200;  //initialize the ghost value
                return;
            };
            //set the expiration point for the powerups

        void StartGhostTimer()
            {
                m_GhostTicks = m_Ticks + m_GhostTimer;
                m_GhostTimerActive = true;
                return;
            }

        bool CheckGhostTimer()
            {   if (m_GhostTicks > m_Ticks)
                    return false;  //timer still going
                else
                {
                    m_GhostTimerActive = false;
                    return true;  //timer up
                }
            };

        bool IsGhostTimeRunning()
            {
                return m_GhostTimerActive;
            };

        bool CheckFruitTimer()
            {   if (m_FruitTicks > m_Ticks)
                    return false;  //timer still going
                else
                    return true;  //timer up
            };

        bool CheckPacManTimer()
            {   if (m_PacManTicks > m_Ticks)
                    return false;  //timer still going
                else
                    return true;  //timer up
            };

        bool CheckPowerUpTimer()
            {   if (m_PowerUpTicks > m_Ticks)
                    return false;  //timer still going
                else
                {
                    m_PowerUpActive = false;
                    return true;  //timer up
                }
            };

        bool PowerUpActive()
            {
                return m_PowerUpActive;
            };

        long CurrentGhostValue()
            {  return m_GhostValue;  };

        void IncrementGhostValue()
            {  m_GhostValue *= 2;  }
};

#endif
