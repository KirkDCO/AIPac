/*	WorldController.cpp
	Robert Kirk DeLisle
	23 August 2006

	Purpose:  Defines an interface between the WorldModel and the WorldView_Win32Resourced
				Contains the primary game logic and is the central controller

	Modificaton History:

*/

#include "WorldController.h"
#include "WorldView.h"
#include "Brain_Keyboard.h"
#include "Brain_RandomGhost.h"

WorldController::WorldController(const string ParamFile)
{
	/*	Purpose:	Build the necessary pieces for the World, make appropriate connections, start
					the process rolling.

		Parameters:	hWnd	-	Handle to the window that will become the target for drawing
					hInstance - Handle to the application instance - required for some bitmap operations

		Return:		none

		Exceptions:

	*/

    m_MapDrawn = false; //the map has yet to be drawn, so flag it

	//get the parameters for this world
//	m_Param.SetParamFile(ParamFile);
    //these are already set elsewhere. I need to make sure things are properly ordered.

	//connect the world view to the world model
	m_View.Connect(&m_Model);

    //execute various initialization routines
    InitTimers();
    InitMaze();
    InitDots();

	//tell the view how big it needs to be
	m_View.SetViewDimensions(m_MazeController.GetPixelHeight(), m_MazeController.GetPixelWidth());

	InitPacMan();

	m_NumGhosts = m_Param.GetLongParam("NumberOfGhosts");
	InitGhosts();
	InitPowerUps();

	m_UpdateCount=0;

	return;
}

void WorldController::Draw(HDC TargetDC)
{
	/*	Purpose:	Draw all the world to the TargetDC

		Parameters:	TargetDC - where the drawing will occur

		Return:		none

		Exceptions:

	*/

    long state;  //used to check state of powerups


    //draw the map, if necessary
	if ( !m_MapDrawn )
	{
	    //clear out everything first
	    m_View.Clear();
		//draw the maze and the dots
		m_MazeController.Draw();
		//m_DotsController.Draw();
		m_MapDrawn = true;
	}

    //erase the sprites
    m_PacManController.Erase();

    for (int x=0; x<4; x++)
	{
		m_vecPowerUpControllers.at(x)->GetState(state);
		if ( state == PowerUp_Active )
            m_vecPowerUpControllers.at(x)->Erase();
	}

	for (int x=0; x<m_NumGhosts; x++)
	{
		m_vecGhostControllers.at(x)->Erase();
	}

    //draw the dots - necessary to redraw due to changes
    m_DotsController.Draw();

    //capture the section being drawn to for later erasure
    for (int x=0; x<4; x++)
    {
        m_vecPowerUpControllers.at(x)->GetState(state);
        if ( state == PowerUp_Active )
            m_vecPowerUpControllers.at(x)->StoreEraseBuffer();
    }

    for (int x=0; x<m_NumGhosts; x++)
    {
        m_vecGhostControllers.at(x)->StoreEraseBuffer();
    }


    m_PacManController.StoreEraseBuffer();

	//draw the sprites
	for (int x=0; x<4; x++)
	{
		//check state of Powerup and erase if needed
		m_vecPowerUpControllers.at(x)->GetState(state);
		if ( state == PowerUp_Active )
           m_vecPowerUpControllers.at(x)->Draw();
	}

	for (int x=0; x<m_NumGhosts; x++)
	{
		m_vecGhostControllers.at(x)->Draw();
	}

    m_PacManController.Draw();

	//everybody is done, so draw the world
	m_View.Draw(TargetDC);

	return;
}

void WorldController::InitTimers()
{
    /*	Purpose:	Initialize various timers related to game events

		Parameters:	none

		Return:		none

		Exceptions:

	*/

    //set the timers for the world model
	m_Model.SetFruitTimer(m_Param.GetLongParam("FruitTimer"));
	m_Model.SetGhostTimer(m_Param.GetLongParam("GhostTimer"));
	m_Model.SetPacManTimer(m_Param.GetLongParam("PacManTimer"));
	m_Model.SetPowerUpTimer(m_Param.GetLongParam("PowerUpTimer"));

    return;
}

void WorldController::InitMaze()
{
    /*	Purpose:	Initialize details related to the maze.

		Parameters:	none

		Return:		none

		Exceptions:

	*/

	//create the maze
	m_MazeController.LoadMap(m_Param.GetStringParam("MazeMap"));
	m_MazeController.SetBitmap(m_Param.GetStringParam("MazeBitmap"),
                                m_Param.GetLongParam("MazeBitmapTiles"));
	m_MazeController.SetTargetDC(m_View.GetBackBuffer());

	return;
}

void WorldController::InitDots()
{
    /*	Purpose:	Initialize details related to the dots.

		Parameters:	none

		Return:		none

		Exceptions:

	*/

	m_DotsController.LoadMap(m_Param.GetStringParam("DotMap"));
	m_DotsController.SetBitmap(m_Param.GetStringParam("DotBitmap"),
                                m_Param.GetLongParam("DotBitmapTiles"));
	m_DotsController.SetTargetDC(m_View.GetBackBuffer());

	return;
}

void WorldController::InitPacMan()
{
    /*	Purpose:	Initialize details related to the PacMan.

		Parameters:	none

		Return:		none

		Exceptions:

	*/

    long index;
    long state;
    vector<long> AnimSeq;

    m_PacManController.SetBitmap(m_Param.GetStringParam("PacManBitmap"),
                                    m_Param.GetLongParam("PacManBitmapTiles"));
	m_PacManController.SetBoundaryWrap(true);
	m_PacManController.SetMaxMinBounds(0,m_MazeController.GetPixelWidth(),
										0,m_MazeController.GetPixelHeight());
	m_PacManController.SetPosition(m_Param.GetLongParam("PacManPositionX"),
                                    m_Param.GetLongParam("PacManPositionY"));
	m_PacManController.SetTargetDC(m_View.GetBackBuffer());

    if (m_Param.GetStringParam("PacManBoundaryWrap") == "true")
        m_PacManController.SetBoundaryWrap(true);
    else
        m_PacManController.SetBoundaryWrap(false);

    state = PacMan_ActiveRight;
	m_PacManController.SetState(state);

    //I'm doing this the hard way right now - someday I'll get back to doing it properly
	//set the animation sequences
	for (index=0; index<15; index++)
    {
        if ( (index % 3 == 0) && (index != 0) )
        {
            //we've hit a break in the animation seq, so push it into the view (via the controller)
            switch(index)
            {
                case 3:
                    AnimSeq.push_back(index-2);  //get that intermediate frame
                    m_PacManController.SetAnimationSequence(PacMan_ActiveRight,AnimSeq);
                    AnimSeq.clear();
                    break;

                case 6:
                    AnimSeq.push_back(index-2);  //get that intermediate frame
                    m_PacManController.SetAnimationSequence(PacMan_ActiveUp,AnimSeq);
                    AnimSeq.clear();
                    break;

                case 9:
                    AnimSeq.push_back(index-2);  //get that intermediate frame
                    m_PacManController.SetAnimationSequence(PacMan_ActiveLeft,AnimSeq);
                    AnimSeq.clear();
                    break;

                case 12:
                    AnimSeq.push_back(index-2);  //get that intermediate frame
                    m_PacManController.SetAnimationSequence(PacMan_ActiveDown,AnimSeq);
                    AnimSeq.clear();
                    break;

                case 15:
                    AnimSeq.push_back(index-2);  //get that intermediate frame
                    m_PacManController.SetAnimationSequence(PacMan_Dead,AnimSeq);
                    AnimSeq.clear();
            }
        }

        //store the current frame number
        AnimSeq.push_back(index);

        //create a new AI and tell the sprite
//in this version the AI is created elsewhere and pushed in

    }

    return;
}

void WorldController::InitGhosts()
{
    /*	Purpose:	Initialize details related to the Ghosts.

		Parameters:	none

		Return:		none

		Exceptions:

	*/

    long index;
    long state;
    long XSpeed = 2, YSpeed = 0;
    vector<long> AnimSeq;

	for (index=0; index<m_NumGhosts; index++)
	{
		m_vecGhostControllers.push_back(new SpriteController());

		m_vecGhostControllers.at(index)->SetBitmap(m_Param.GetStringParam("GhostBitmap"),
                                                    m_Param.GetLongParam("GhostBitmapTiles"));

        if (m_Param.GetStringParam("GhostBoundaryWrap") == "true")
            m_vecGhostControllers.at(index)->SetBoundaryWrap(true);
        else
            m_vecGhostControllers.at(index)->SetBoundaryWrap(false);

		m_vecGhostControllers.at(index)->SetMaxMinBounds(0, m_MazeController.GetPixelWidth(),
														0, m_MazeController.GetPixelHeight());

		m_vecGhostControllers.at(index)->SetPosition(m_Param.GetLongParam("GhostPositionX")+(index*16),
                                                      m_Param.GetLongParam("GhostPositionY"));
		m_vecGhostControllers.at(index)->SetTargetDC(m_View.GetBackBuffer());

		state = Ghost_CagedRight;
		m_vecGhostControllers.at(index)->SetState(state);
		m_vecGhostControllers.at(index)->SetSpeed(XSpeed, YSpeed);
		XSpeed=2; YSpeed=0;
		state = Ghost_CagedRight;
		m_vecGhostControllers.at(index)->SetState(state); //done a second time to store the state for reversion
        m_vecGhostControllers.at(index)->SetSpeed(XSpeed, YSpeed);

        //create a new AI and tell the sprite
        IBrain *pBrain = new Brain_RandomGhost(m_vecGhostControllers.at(index), this, 0.5+(0.1*((double)(index))));
        m_vecGhostControllers.at(index)->SetSpriteBrain( pBrain );

		//set the animation sequences
		//yes, this is ugly - one day I'll do it properly...maybe...
		switch(index)
		{
		    case 0: //red
                AnimSeq.clear();
                AnimSeq.push_back(2);
                AnimSeq.push_back(6);
                m_vecGhostControllers.at(index)->SetAnimationSequence(Ghost_CagedLeft,AnimSeq);
                m_vecGhostControllers.at(index)->SetAnimationSequence(Ghost_ActiveLeft,AnimSeq);
                m_vecGhostControllers.at(index)->SetAnimationSequence(Ghost_HuntLeft,AnimSeq);

                AnimSeq.clear();
                AnimSeq.push_back(0);
                AnimSeq.push_back(4);
                m_vecGhostControllers.at(index)->SetAnimationSequence(Ghost_CagedRight,AnimSeq);
                m_vecGhostControllers.at(index)->SetAnimationSequence(Ghost_ActiveRight,AnimSeq);
                m_vecGhostControllers.at(index)->SetAnimationSequence(Ghost_HuntRight,AnimSeq);

                AnimSeq.clear();
                AnimSeq.push_back(1);
                AnimSeq.push_back(5);
                m_vecGhostControllers.at(index)->SetAnimationSequence(Ghost_ActiveUp,AnimSeq);
                m_vecGhostControllers.at(index)->SetAnimationSequence(Ghost_HuntUp,AnimSeq);

                AnimSeq.clear();
                AnimSeq.push_back(3);
                AnimSeq.push_back(7);
                m_vecGhostControllers.at(index)->SetAnimationSequence(Ghost_HuntDown,AnimSeq);
                break;

            case 1: //yellow
                AnimSeq.clear();
                AnimSeq.push_back(10);
                AnimSeq.push_back(14);
                m_vecGhostControllers.at(index)->SetAnimationSequence(Ghost_CagedLeft,AnimSeq);
                m_vecGhostControllers.at(index)->SetAnimationSequence(Ghost_ActiveLeft,AnimSeq);
                m_vecGhostControllers.at(index)->SetAnimationSequence(Ghost_HuntLeft,AnimSeq);

                AnimSeq.clear();
                AnimSeq.push_back(8);
                AnimSeq.push_back(12);
                m_vecGhostControllers.at(index)->SetAnimationSequence(Ghost_CagedRight,AnimSeq);
                m_vecGhostControllers.at(index)->SetAnimationSequence(Ghost_ActiveRight,AnimSeq);
                m_vecGhostControllers.at(index)->SetAnimationSequence(Ghost_HuntRight,AnimSeq);

                AnimSeq.clear();
                AnimSeq.push_back(9);
                AnimSeq.push_back(13);
                m_vecGhostControllers.at(index)->SetAnimationSequence(Ghost_ActiveUp,AnimSeq);
                m_vecGhostControllers.at(index)->SetAnimationSequence(Ghost_HuntUp,AnimSeq);

                AnimSeq.clear();
                AnimSeq.push_back(11);
                AnimSeq.push_back(15);
                m_vecGhostControllers.at(index)->SetAnimationSequence(Ghost_HuntDown,AnimSeq);
                break;

            case 2: //green
                AnimSeq.clear();
                AnimSeq.push_back(18);
                AnimSeq.push_back(22);
                m_vecGhostControllers.at(index)->SetAnimationSequence(Ghost_CagedLeft,AnimSeq);
                m_vecGhostControllers.at(index)->SetAnimationSequence(Ghost_ActiveLeft,AnimSeq);
                m_vecGhostControllers.at(index)->SetAnimationSequence(Ghost_HuntLeft,AnimSeq);

                AnimSeq.clear();
                AnimSeq.push_back(16);
                AnimSeq.push_back(20);
                m_vecGhostControllers.at(index)->SetAnimationSequence(Ghost_CagedRight,AnimSeq);
                m_vecGhostControllers.at(index)->SetAnimationSequence(Ghost_ActiveRight,AnimSeq);
                m_vecGhostControllers.at(index)->SetAnimationSequence(Ghost_HuntRight,AnimSeq);

                AnimSeq.clear();
                AnimSeq.push_back(17);
                AnimSeq.push_back(21);
                m_vecGhostControllers.at(index)->SetAnimationSequence(Ghost_ActiveUp,AnimSeq);
                m_vecGhostControllers.at(index)->SetAnimationSequence(Ghost_HuntUp,AnimSeq);

                AnimSeq.clear();
                AnimSeq.push_back(19);
                AnimSeq.push_back(23);
                m_vecGhostControllers.at(index)->SetAnimationSequence(Ghost_HuntDown,AnimSeq);
                break;

            case 3:  //purple
                AnimSeq.clear();
                AnimSeq.push_back(26);
                AnimSeq.push_back(30);
                m_vecGhostControllers.at(index)->SetAnimationSequence(Ghost_CagedLeft,AnimSeq);
                m_vecGhostControllers.at(index)->SetAnimationSequence(Ghost_ActiveLeft,AnimSeq);
                m_vecGhostControllers.at(index)->SetAnimationSequence(Ghost_HuntLeft,AnimSeq);

                AnimSeq.clear();
                AnimSeq.push_back(24);
                AnimSeq.push_back(28);
                m_vecGhostControllers.at(index)->SetAnimationSequence(Ghost_CagedRight,AnimSeq);
                m_vecGhostControllers.at(index)->SetAnimationSequence(Ghost_ActiveRight,AnimSeq);
                m_vecGhostControllers.at(index)->SetAnimationSequence(Ghost_HuntRight,AnimSeq);

                AnimSeq.clear();
                AnimSeq.push_back(25);
                AnimSeq.push_back(29);
                m_vecGhostControllers.at(index)->SetAnimationSequence(Ghost_ActiveUp,AnimSeq);
                m_vecGhostControllers.at(index)->SetAnimationSequence(Ghost_HuntUp,AnimSeq);

                AnimSeq.clear();
                AnimSeq.push_back(27);
                AnimSeq.push_back(31);
                m_vecGhostControllers.at(index)->SetAnimationSequence(Ghost_HuntDown,AnimSeq);
		}

		//blue
		AnimSeq.clear();
		AnimSeq.push_back(32);
		AnimSeq.push_back(33);
		m_vecGhostControllers.at(index)->SetAnimationSequence(Ghost_BlueUp,AnimSeq);

		AnimSeq.clear();
		AnimSeq.push_back(32);
		AnimSeq.push_back(33);
		m_vecGhostControllers.at(index)->SetAnimationSequence(Ghost_BlueDown,AnimSeq);

		AnimSeq.clear();
		AnimSeq.push_back(32);
		AnimSeq.push_back(33);
		m_vecGhostControllers.at(index)->SetAnimationSequence(Ghost_BlueLeft,AnimSeq);

		AnimSeq.clear();
		AnimSeq.push_back(32);
		AnimSeq.push_back(33);
		m_vecGhostControllers.at(index)->SetAnimationSequence(Ghost_BlueRight,AnimSeq);

		//dead
		AnimSeq.clear();
		AnimSeq.push_back(36);
		m_vecGhostControllers.at(index)->SetAnimationSequence(Ghost_DeadLeft,AnimSeq);

		AnimSeq.clear();
		AnimSeq.push_back(34);
		m_vecGhostControllers.at(index)->SetAnimationSequence(Ghost_DeadRight,AnimSeq);

		AnimSeq.clear();
		AnimSeq.push_back(35);
		m_vecGhostControllers.at(index)->SetAnimationSequence(Ghost_DeadUp,AnimSeq);

		AnimSeq.clear();
		AnimSeq.push_back(37);
		m_vecGhostControllers.at(index)->SetAnimationSequence(Ghost_DeadDown,AnimSeq);
    }

    m_Model.StartGhostTimer();

    return;
}

void WorldController::InitPowerUps()
{
    /*	Purpose:	Initialize details related to the PowerUps.

		Parameters:	none

		Return:		none

		Exceptions:

	*/

    long state;
    long index;
    vector<long> AnimSeq;

	for (index=0; index<4; index++)
	{
		m_vecPowerUpControllers.push_back(new SpriteController());
		m_vecPowerUpControllers.at(index)->SetBitmap("Resources\\PowerPill.bmp",4);
		m_vecPowerUpControllers.at(index)->SetBoundaryWrap(true);
		m_vecPowerUpControllers.at(index)->SetMaxMinBounds(0, m_MazeController.GetPixelWidth(),
															0, m_MazeController.GetPixelHeight());

		state = PowerUp_Active;
		m_vecPowerUpControllers.at(index)->SetState(state);

		switch(index)
		{
			case 0:
				m_vecPowerUpControllers.at(index)->SetPosition(16,48);
				break;

			case 1:
				m_vecPowerUpControllers.at(index)->SetPosition(416,48);
				break;

			case 2:
				m_vecPowerUpControllers.at(index)->SetPosition(16,352);
				break;

			case 3:
				m_vecPowerUpControllers.at(index)->SetPosition(416,352);
				break;
		}
		m_vecPowerUpControllers.at(index)->SetTargetDC(m_View.GetBackBuffer());
	}


	//PowerUps
	for (index=0; index<4; index++)
		AnimSeq.push_back(index);

	for (index=0; index<4; index++)
		m_vecPowerUpControllers.at(index)->SetAnimationSequence(PowerUp_Active,AnimSeq);

    return;
}

bool WorldController::Update(bool loggit)
{
    /*	Purpose:	Update all parts of the world for one time step

		Parameters:	none

		Return:		none

		Exceptions:

	*/
	long PacState;
    static long DeadSeq=0;
    long XSpeed=0, YSpeed=0;
    static long OldX, OldY, MaximumUpdates;
    long CurrX, CurrY;
    static bool firstRun = true;
    static long stallCount=0;
    static long MaximumStall;
    static long OldScore;
    static long ScoreStall;
    static long ScoreStallCount=0;
    ParameterMap params;

    static long index = 0;
    fstream logfile;
    //logfile.open("update.log", ios::out|ios::app);
    //logfile << "Update: " << index++ << endl;
    //logfile.flush();

    if ( firstRun )
    {
        m_PacManController.GetPosition(OldX, OldY);
        MaximumUpdates = m_Param.GetLongParam("MaximumUpdates");
        MaximumStall = m_Param.GetLongParam("MaximumStall");

        OldScore = m_Model.GetScore();
        ScoreStall = m_Param.GetLongParam("ScoreStall");
        firstRun = false;
    }

    //logfile << "Checktimers" << endl;
    //logfile.flush();
    //check everyone's timers
    CheckTimers();

    //logfile << "MovePacMan" << endl;
    //logfile.flush();
	//check for fruit on/off
	//move the PacMan
	MovePacMan(loggit);

	//move ghosts
	//logfile << "MoveGhosts" << endl;
    //logfile.flush();
	MoveGhosts();

    //logfile << "CheckDots" << endl;
    //logfile.flush();
    //check for dot collisions
    CheckDotCollisions();

    //logfile << "CheckPowerups" << endl;
    //logfile.flush();
    //check for power up collisions
    CheckPowerUpCollisions();

    //logfile << "Checkghosts" << endl;
    //logfile.flush();
    //check for ghost collisions
    CheckGhostCollisions();

    //logfile << "prevent stall" << endl;
    //logfile.flush();
    //keep PacMan from stalling out - no sense in wasting time if he's stuck
    m_PacManController.GetPosition(CurrX, CurrY);
    if ( CurrX == OldX && CurrY == OldY )
    {
        stallCount++;
    }
    else
    {
        OldX = CurrX;
        OldY = CurrY;
        stallCount = 0;
    }

    if ( stallCount > MaximumStall )
    {
        stallCount = 0;
        return false;
    }

    //logfile << "Prevent stuck" << endl;
    //logfile.flush();
    //keep PacMan from getting stuck in non-scoring cycles
    if ( m_Model.GetScore() - OldScore == 0 )
        ScoreStallCount++;
    else
    {
        OldScore = m_Model.GetScore();
        ScoreStallCount=0;
    }

    if ( ScoreStallCount > ScoreStall )
    {
        ScoreStallCount = 0;
        return false;
    }

    //logfile << "PacDead" << endl;
    //logfile.flush();
    //if PacMan dies, end early
    m_PacManController.GetState(PacState);
    if ( PacState == PacMan_Dead )
    {
        return false;
    }

    //logfile << "ClearMaze" << endl;
    //logfile.flush();
    //if PacMan clears the maze, end
    if ( m_Model.GetScore() == params.GetLongParam("MaxScore") )
        return false;

    //keep a tally of the number of updates and only allow a specific amount
    m_UpdateCount++;

    if ( m_UpdateCount < MaximumUpdates )
    {
        return true;
    }

    //logfile << "Done" << endl << endl << endl;
    //logfile.close();
    return false;
}

void WorldController::UpdateGhostSpeed(long i)
{
    /*	Purpose:	Make changes to the ghost speed based on state

		Parameters:	i - index of the ghost to adjust

		Return:		none

		Exceptions:

	*/

    long state;
//    long i;
    long CurrentX, CurrentY;
    long XSpeed, YSpeed;
    bool speedUpdated = false;
    long minX, minY, maxX, maxY;
    long ret;

    //step through the ghosts and update their speed
//    for (i=0; i<4; i++)
//    {
        m_vecGhostControllers.at(i)->GetState(state);
        m_vecGhostControllers.at(i)->GetPosition(CurrentX, CurrentY);
        m_vecGhostControllers.at(i)->GetSpeed(XSpeed, YSpeed);

        if ( state == Ghost_ActiveUp || state == Ghost_ActiveLeft || state == Ghost_ActiveRight )
        {
            //we're trying to get out of the cage
            //get the X-value right first, then move out of the cage
            if ( CurrentX < 216 )
            {
                state = Ghost_ActiveRight;
                XSpeed = 2;  YSpeed = 0;
                m_vecGhostControllers.at(i)->SetState(state);
                m_vecGhostControllers.at(i)->SetSpeed(XSpeed, YSpeed);
                speedUpdated=true;
            }
            else if ( CurrentX > 216 )
            {
                state = Ghost_ActiveLeft;
                XSpeed = -2;  YSpeed = 0;
                m_vecGhostControllers.at(i)->SetState(state);
                m_vecGhostControllers.at(i)->SetSpeed(XSpeed, YSpeed);
                speedUpdated=true;
            }
            else
            {
                if ( CurrentY > 176 )
                {
                    //open the cage - make sure it is open to avoid errors later
                    m_MazeController.ModifyTile(13,13,0);
                    m_MazeController.ModifyTile(14,13,0);
                    m_MazeController.ModifyTile(15,13,0);

                    state = Ghost_ActiveUp;
                    XSpeed = 0;  YSpeed = -2;
                    m_vecGhostControllers.at(i)->SetState(state);
                    m_vecGhostControllers.at(i)->SetSpeed(XSpeed, YSpeed);
                    speedUpdated=true;
                }
                else
                {
                    //pick a starting direction at random
                    if ( (rand() % 2) == 0 )
                    {
                        XSpeed = -8;
                        state = Ghost_HuntLeft;
                    }
                    else
                    {
                        XSpeed = 8;
                        state = Ghost_HuntRight;
                    }
                    YSpeed = 0;
                    m_vecGhostControllers.at(i)->SetState(state);
                    m_vecGhostControllers.at(i)->SetSpeed(XSpeed, YSpeed);
                    speedUpdated=true;

                    //close the cage
                    CloseCage();
//                    m_MazeController.ModifyTile(13,13,16);
//                    m_MazeController.ModifyTile(14,13,17);
//                    m_MazeController.ModifyTile(15,13,18);

                    m_MapDrawn = false; //force a maze redraw
                }
            }
        }
        else if ( state == Ghost_HuntUp || state == Ghost_HuntDown ||
                  state == Ghost_HuntLeft || state == Ghost_HuntRight ||
                  state == Ghost_BlueUp || state == Ghost_BlueDown ||
                  state == Ghost_BlueLeft || state == Ghost_BlueRight ||
                  state == Ghost_DeadUp || state == Ghost_DeadDown ||
                  state == Ghost_DeadLeft || state == Ghost_DeadRight )
        {
            //we're on the hunt, so look to the AI
            //check to see if we're at an intersection
            if ( state == Ghost_HuntUp || state == Ghost_HuntDown ||
                 state == Ghost_BlueUp || state == Ghost_BlueDown ||
                 state == Ghost_DeadUp || state == Ghost_DeadDown )
            {
                if ( state == Ghost_DeadDown )
                {
                    if ( CurrentX == 216 && ( CurrentY > 176 && CurrentY < 224 ))
                    {
                        //we're dead and trying to move into the cage
                        XSpeed = 0;  YSpeed = 2;
                        m_vecGhostControllers.at(i)->SetState(state);
                        m_vecGhostControllers.at(i)->SetSpeed(XSpeed, YSpeed);
                        speedUpdated = true;
                    }
                    else if ( CurrentX == 216 && CurrentY == 224 )
                    {
                        //we've made it back into the cage - revert to Caged state
                        state = Ghost_CagedRight;
                        XSpeed = 2;  YSpeed = 0;
                        m_vecGhostControllers.at(i)->SetState(state);
                        m_vecGhostControllers.at(i)->SetSpeed(XSpeed, YSpeed);
                        speedUpdated = true;
                        if ( !m_Model.IsGhostTimeRunning() )
                            m_Model.StartGhostTimer();

                        //close the cage
                        CloseCage();
//                        m_MazeController.ModifyTile(13,13,16);
//                        m_MazeController.ModifyTile(14,13,17);
//                        m_MazeController.ModifyTile(15,13,18);

                        m_MapDrawn = false; //force a maze redraw
                    }
                }
                if ( !speedUpdated )
                {
                    //we're moving up/down, so we need to find an opening for left/right
                    minX = CurrentX-8;
                    minY = CurrentY;
                    maxX = minX + m_vecGhostControllers.at(i)->GetWidth()-1;
                    maxY = minY + m_vecGhostControllers.at(i)->GetHeight()-1;

                    if ( m_MazeController.CheckMapByPixels( minX, maxX, minY, maxY,ret) )
                    {
                        m_vecGhostControllers.at(i)->UpdateSpeed(false);
                        speedUpdated = true;
                    }

                    minX = CurrentX+8;
                    minY = CurrentY;
                    maxX = minX + m_vecGhostControllers.at(i)->GetWidth()-1;
                    maxY = minY + m_vecGhostControllers.at(i)->GetHeight()-1;

                    if ( m_MazeController.CheckMapByPixels( minX, maxX, minY, maxY,ret) )
                    {
                        m_vecGhostControllers.at(i)->UpdateSpeed(false);
                        speedUpdated = true;
                    }
                }
            }
            else
            {
                if ( state == Ghost_HuntRight || state == Ghost_HuntLeft ||
                     state == Ghost_BlueRight || state == Ghost_BlueLeft )
                {
                    //prevent going back in the cage
                    if ( !( (CurrentX >= 208 && CurrentX <= 224) && CurrentY == 176) )
                    {
                        //we're moving left/right, we need an up/down opening
                        minX = CurrentX;
                        minY = CurrentY-8;
                        maxX = minX + m_vecGhostControllers.at(i)->GetWidth()-1;
                        maxY = minY + m_vecGhostControllers.at(i)->GetHeight()-1;

                        if ( m_MazeController.CheckMapByPixels( minX, maxX, minY, maxY,ret) )
                        {
                            m_vecGhostControllers.at(i)->UpdateSpeed(false);
                            speedUpdated = true;
                        }

                        minX = CurrentX;
                        minY = CurrentY+8;
                        maxX = minX + m_vecGhostControllers.at(i)->GetWidth()-1;
                        maxY = minY + m_vecGhostControllers.at(i)->GetHeight()-1;

                        if ( m_MazeController.CheckMapByPixels( minX, maxX, minY, maxY,ret) )
                        {
                            m_vecGhostControllers.at(i)->UpdateSpeed(false);
                            speedUpdated = true;
                        }
                    }
                }
                else
                {
                    //we're dead
                    if ( (CurrentX == 216 && ( CurrentY >= 176 && CurrentY <= 192 )) )
                    {
                        //open the cage
                        m_MazeController.ModifyTile(13,13,0);
                        m_MazeController.ModifyTile(14,13,0);
                        m_MazeController.ModifyTile(15,13,0);

                        m_MapDrawn = false; //force a maze redraw

                        //and trying to get back in the cage
                        state = Ghost_DeadDown;
                        XSpeed = 0; YSpeed = 2;
//                        speedUpdated = true;
                    }
                    else if ( CurrentX == 216 && CurrentY == 224 )
                    {
//                        //we're in - revert to Active state
//                        state = Ghost_ActiveRight;
//                        XSpeed = 2;  YSpeed = 0;
////                        speedUpdated = true;
//
//                        //close the cage
                        CloseCage();
//                        m_MazeController.ModifyTile(13,13,16);
//                        m_MazeController.ModifyTile(14,13,17);
//                        m_MazeController.ModifyTile(15,13,18);

                        m_MapDrawn = false; //force a maze redraw
                    }
                    else
                    {
                        if ( !( (CurrentX >= 208 && CurrentX <= 224) && CurrentY == 176) )
                        {
                            //we're moving left/right, we need an up/down opening
                            minX = CurrentX;
                            minY = CurrentY-8;
                            maxX = minX + m_vecGhostControllers.at(i)->GetWidth()-1;
                            maxY = minY + m_vecGhostControllers.at(i)->GetHeight()-1;

                            if ( m_MazeController.CheckMapByPixels( minX, maxX, minY, maxY,ret) )
                            {
                                m_vecGhostControllers.at(i)->UpdateSpeed(false);
                                speedUpdated = true;
                            }

                            minX = CurrentX;
                            minY = CurrentY+8;
                            maxX = minX + m_vecGhostControllers.at(i)->GetWidth()-1;
                            maxY = minY + m_vecGhostControllers.at(i)->GetHeight()-1;

                            if ( m_MazeController.CheckMapByPixels( minX, maxX, minY, maxY,ret) )
                            {
                                m_vecGhostControllers.at(i)->UpdateSpeed(false);
                                speedUpdated = true;
                            }
                        }
                    }
                }
            }
        }
        else if ( state == Ghost_CagedLeft || state == Ghost_CagedRight )
        {
            //we're still caged so just pace
            minX = CurrentX+XSpeed;
            minY = CurrentY+YSpeed;
            maxX = minX + m_vecGhostControllers.at(i)->GetWidth()-1;
            maxY = minY + m_vecGhostControllers.at(i)->GetHeight()-1;

            if ( !m_MazeController.CheckMapByPixels( minX, maxX, minY, maxY,ret ) )
            {
                // if its not OK to keep going the same direction
                speedUpdated = false;
                XSpeed = -XSpeed;
                YSpeed = 0;

                if ( state == Ghost_CagedLeft )
                {
                    state = Ghost_CagedRight;
                    m_vecGhostControllers.at(i)->SetState(state);
                }
                else
                {
                    state = Ghost_CagedLeft;
                    m_vecGhostControllers.at(i)->SetState(state);
                }
            }
            m_vecGhostControllers.at(i)->SetSpeed(XSpeed, YSpeed);
            speedUpdated = true;
        }

        if ( !speedUpdated )
        {
            //change state and direction
            m_vecGhostControllers.at(i)->SetState(state);
            m_vecGhostControllers.at(i)->SetSpeed(XSpeed, YSpeed);
        }

        speedUpdated = false;  //reset for other ghosts
//    }
    return;
}

void WorldController::CloseCage()
{
    //close the cage, but do some checks first
    //errors occured when two ghosts pass each other in and out of the cage

    bool closecage = true;
    long X, Y;

    for ( int k=0; k<m_NumGhosts; k++)
    {
       m_vecGhostControllers.at(k)->GetPosition(X,Y);
       if ( X >= 208 && X <= 224)
       {
           if ( Y > 176 && Y < 224 )
           {
               closecage=false;
           }
       }
    }
    if ( closecage )
    {
        m_MazeController.ModifyTile(13,13,16);
        m_MazeController.ModifyTile(14,13,17);
        m_MazeController.ModifyTile(15,13,18);
    }
}
void WorldController::MoveGhosts()
{
    /*	Purpose:	Validate Ghost movement and make the move

		Parameters:	none

		Return:		none

		Exceptions:

	*/

	long index;  //counter for the ghosts
    long state;  //stores state of the current ghost

    //variables to validate the speed
	long currentX, currentY;
	long XSpeed, YSpeed, PreviousXSpeed, PreviousYSpeed;
	long maxX, minX, maxY, minY;

    bool moveValid=true; //assume the next move is valid
    long ret;

//this first block causes some ghost stalling but is likely more appropriate for learning
//	for (index=0; index<4; index++)
//	{
//        UpdateGhostSpeed(index);
//	    //validate the direction
//        m_vecGhostControllers.at(index)->GetPosition(currentX, currentY);
//        m_vecGhostControllers.at(index)->GetSpeed(XSpeed, YSpeed);
//
//        minX = currentX+XSpeed;
//        minY = currentY+YSpeed;
//        maxX = minX + m_vecGhostControllers.at(index)->GetWidth()-1;
//        maxY = minY + m_vecGhostControllers.at(index)->GetHeight()-1;
//
//        if ( !m_MazeController.CheckMapByPixels( minX, maxX, minY, maxY ) )
//        {
//            XSpeed = 0;  YSpeed = 0;
//            m_vecGhostControllers.at(index)->SetSpeed(XSpeed, YSpeed);
//        }
//
    for (index=0; index<m_NumGhosts; index++)
	{
	    do
	    {
            //update the speed/direction based on the state
            UpdateGhostSpeed(index);

            //validate the direction
            m_vecGhostControllers.at(index)->GetPosition(currentX, currentY);
            m_vecGhostControllers.at(index)->GetSpeed(XSpeed, YSpeed);

            minX = currentX+XSpeed;
            minY = currentY+YSpeed;
            maxX = minX + m_vecGhostControllers.at(index)->GetWidth()-1;
            maxY = minY + m_vecGhostControllers.at(index)->GetHeight()-1;

	    } while (! m_MazeController.CheckMapByPixels( minX, maxX, minY, maxY,ret ));

        //all validated - move the ghost
        m_vecGhostControllers.at(index)->Move(false);

        moveValid=true;  //reset this for other ghosts
	}

	return;
}

void WorldController::MovePacMan(bool loggit)
{
    /*	Purpose:	Validate PacMan movement and make the move

		Parameters:	none

		Return:		none

		Exceptions:

	*/

    ParameterMap params;
    bool update = false;
    long CurrentX, CurrentY, state;
    long minX, minY, maxX, maxY, ret;

//    if ( !m_MapDrawn ) //I'm hijacking this as a first run indicator
//    {
//        update = true;
//    }
//
//    if ( params.GetLongParam("EnforceIntersections") == 0 ||
//         params.GetStringParam("Mode") == "Play" )
//        update = true;
//    else
//    {
//        m_PacManController.GetState(state);
//        m_PacManController.GetPosition(CurrentX, CurrentY);
//
//        if ( state == PacMan_ActiveUp || state == PacMan_ActiveDown )
//        {
//            //we're moving up/down, so we need to find an opening for left/right
//            minX = CurrentX-8;
//            minY = CurrentY;
//            maxX = minX + m_PacManController.GetWidth()-1;
//            maxY = minY + m_PacManController.GetHeight()-1;
//
//            if ( m_MazeController.CheckMapByPixels( minX, maxX, minY, maxY,ret) )
//                update = true;
//
//            minX = CurrentX+8;
//            minY = CurrentY;
//            maxX = minX + m_PacManController.GetWidth()-1;
//            maxY = minY + m_PacManController.GetHeight()-1;
//
//            if ( m_MazeController.CheckMapByPixels( minX, maxX, minY, maxY,ret) )
//                update=true;
//        }
//        else
//        {
//            //we're moving left/right, we need an up/down opening
//            minX = CurrentX;
//            minY = CurrentY-8;
//            maxX = minX + m_PacManController.GetWidth()-1;
//            maxY = minY + m_PacManController.GetHeight()-1;
//
//            if ( m_MazeController.CheckMapByPixels( minX, maxX, minY, maxY,ret) )
//                update=true;
//
//            minX = CurrentX;
//            minY = CurrentY+8;
//            maxX = minX + m_PacManController.GetWidth()-1;
//            maxY = minY + m_PacManController.GetHeight()-1;
//
//            if ( m_MazeController.CheckMapByPixels( minX, maxX, minY, maxY,ret) )
//                update=true;
//        }
//    }

//    if ( update )
    {
        //move the PacMan
        m_PacManController.UpdateSpeed(loggit);  //get the desired next direction to move

        //validate the new speed
        long currentX, currentY;
        long XSpeed, YSpeed, PreviousXSpeed, PreviousYSpeed;
        long maxX, minX, maxY, minY;
        m_PacManController.GetPosition(currentX, currentY);
        m_PacManController.GetSpeed(XSpeed, YSpeed);

        minX = currentX+XSpeed;
        minY = currentY+YSpeed;
        maxX = minX + m_PacManController.GetWidth()-1;
        maxY = minY + m_PacManController.GetHeight()-1;

        if ( !m_MazeController.CheckMapByPixels( minX, maxX, minY, maxY,ret ) )
        {
            //revert to old speed appropriately
            m_PacManController.RevertSpeed();
            m_PacManController.RevertState();

            //now we have to verify the reverted speed also
            m_PacManController.GetSpeed(XSpeed, YSpeed);
            minX = currentX+XSpeed;
            minY = currentY+YSpeed;
            maxX = minX + m_PacManController.GetWidth()-1;
            maxY = minY + m_PacManController.GetHeight()-1;
            if ( !m_MazeController.CheckMapByPixels( minX, maxX, minY, maxY,ret ) )
            {
                //this time should lead to a full stop
                XSpeed = 0;  YSpeed = 0;
                m_PacManController.SetSpeed(XSpeed, YSpeed);
            }
        }
    }

//this block is used to monitor details during different runs for each call to this function
//I commented it out to eliminate an extra comparison every time
//    if ( loggit )
//    {
//        fstream lf;
//        long XSpeed, YSpeed;
//        m_PacManController.GetSpeed(XSpeed, YSpeed);
//        lf.open("InOut.csv", ios::out|ios::app);
//        lf << XSpeed << "," << YSpeed << "," << "Update=";
//        if ( update )
//            lf << "True" << endl;
//        else
//            lf << "False" << endl;
//        lf.close();
//    }

    m_PacManController.Move( loggit ); //move the sprite


    return;
}

void WorldController::CheckDotCollisions()
{
    /*	Purpose:	Check for dot collisions, upadate the score appropriately, and erase the dot.

		Parameters:	none

		Return:		none

		Exceptions:

	*/

	long currentX, currentY;
	long minX, maxX, minY, maxY;
    long mods, ret;

	//get PacMan's current location
	m_PacManController.GetPosition(currentX, currentY);

	//shrink the bounding box for more realistic dot eating
	minX = currentX+5;
	minY = currentY+5;
	maxX = currentX + m_PacManController.GetWidth()-15;
	maxY = currentY + m_PacManController.GetHeight()-15;

	if ( !m_DotsController.CheckMapByPixels( minX, maxX, minY, maxY,ret ) )
	{
	    //there's a dot in the bounding box
	    mods = m_DotsController.ModifyTileByPixel(currentX, currentX+m_PacManController.GetWidth()-1,
                                            currentY, currentY+m_PacManController.GetHeight()-1);
        m_Model.IncrementScore(10*mods);
	}

	return;
}

void WorldController::CheckPowerUpCollisions()
{
    /*	Purpose:	Check for power up collisions, upadate the score appropriately, and change states accordingly.

		Parameters:	none

		Return:		none

		Exceptions:

	*/

    long i, j; //index
    long PacX, PacY;
    long PacMinX, PacMaxX, PacMinY, PacMaxY;

    long PowerUpX, PowerUpY;
    long PUMinX, PUMaxX, PUMinY, PUMaxY;

    long state;

    //get PacMan's location
    m_PacManController.GetPosition(PacX, PacY);

    //define his collision box
    PacMinX = PacX+5;
	PacMinY = PacY+5;
	PacMaxX = PacX + m_PacManController.GetWidth()-15;
	PacMaxY = PacY + m_PacManController.GetHeight()-15;

    //step through the PowerUps
    for (i=0; i<4; i++)
    {
        //only if the powerup is active
        m_vecPowerUpControllers.at(i)->GetState(state);
        if ( state == PowerUp_Active )
        {
            //get the powerup location
            m_vecPowerUpControllers.at(i)->GetPosition(PowerUpX, PowerUpY);

            //define the powerup collision box
            PUMinX = PowerUpX+5;
            PUMaxX = PowerUpX + m_vecPowerUpControllers.at(i)->GetWidth()-15;
            PUMinY = PowerUpY+5;
            PUMaxY = PowerUpY + m_vecPowerUpControllers.at(i)->GetHeight()-15;

            //do the boxes overlap??
            if ( ((PUMinX <= PacMinX) && (PUMaxX >= PacMinX)) ||
                 ((PUMinX <= PacMaxX) && (PUMaxX >= PacMaxX)) )
            {
                //X coords coincident
                if ( ((PUMinY <= PacMinY) && (PUMaxY >= PacMinY)) ||
                     ((PUMinY <= PacMaxY) && (PUMaxY >= PacMaxY)) )
                {
                    //Y coords coincident
                    //increment the score
                    m_Model.IncrementScore(50);

                    //set the state of the power up
                    state = PowerUp_Inactive;
                    m_vecPowerUpControllers.at(i)->SetState(state);
                    m_vecPowerUpControllers.at(i)->Erase();

                    //set the ghost states to blue
                    for ( j=0; j<m_NumGhosts; j++)
                    {
                        m_vecGhostControllers.at(j)->GetState(state);

                        switch(state)
                        {
                            case Ghost_HuntUp:
                                state = Ghost_BlueUp;
                                break;

                            case Ghost_HuntDown:
                                state = Ghost_BlueDown;
                                break;

                            case Ghost_HuntRight:
                                state = Ghost_BlueRight;
                                break;

                            case Ghost_HuntLeft:
                                state = Ghost_BlueLeft;
                                break;
                        }
                        m_vecGhostControllers.at(j)->SetState(state);
                        UpdateGhostSpeed(j);  //let the ghosts change direction after power up eaten
                    }

                    //start the PowerUp timer
                    m_Model.StartPowerUpTimer();

                    //we can only collide with one powerup at a time, so jump out
                    break;
                }
            }
        }
    }
}

void WorldController::CheckTimers()
{
    /*	Purpose:	Check timers and update states as necessary

		Parameters:	none

		Return:		none

		Exceptions:

	*/

    long i;
    long state;

    //increment the timers first
    m_Model.IncrementGameTimer();

    //check the powerups
    if ( m_Model.PowerUpActive() )
    {
        if ( m_Model.CheckPowerUpTimer() )
        {
            for (i=0; i<m_NumGhosts; i++)
            {
                //check the current state
                m_vecGhostControllers.at(i)->GetState(state);

                switch(state)
                {
                    case Ghost_BlueUp:
                        state = Ghost_HuntUp;
                        break;

                    case Ghost_BlueDown:
                        state = Ghost_HuntDown;
                        break;

                    case Ghost_BlueRight:
                        state = Ghost_HuntRight;
                        break;

                    case Ghost_BlueLeft:
                        state = Ghost_HuntLeft;
                        break;
                }
                m_vecGhostControllers.at(i)->SetState(state);
            }
        }
    }

    //check for ghosts to exit the cage
    if ( m_Model.CheckGhostTimer() )
    {
        for (i=0; i<m_NumGhosts; i++)
        {
            //time to leave the cage
            m_vecGhostControllers.at(i)->GetState(state);
            if ( state == Ghost_CagedLeft )
            {
                //open the cage
                m_MazeController.ModifyTile(13,13,0);
                m_MazeController.ModifyTile(14,13,0);
                m_MazeController.ModifyTile(15,13,0);

                m_MapDrawn = false; //force a maze redraw

                state = Ghost_ActiveLeft;
                m_vecGhostControllers.at(i)->SetState(state);
                m_Model.StartGhostTimer();  //start the timer for the next guy
                break;
            }
            else if (state == Ghost_CagedRight )
            {
                //open the cage
                m_MazeController.ModifyTile(13,13,0);
                m_MazeController.ModifyTile(14,13,0);
                m_MazeController.ModifyTile(15,13,0);

                m_MapDrawn = false; //force a maze redraw

                state = Ghost_ActiveRight;
                m_vecGhostControllers.at(i)->SetState(state);
                m_Model.StartGhostTimer();  //start the timer for the next guy
                break;
            }
        }
    }

    return;
}

void WorldController::CheckGhostCollisions()
{
    /*	Purpose:	Check if PacMan has collided with a ghost

		Parameters:	none

		Return:		none

		Exceptions:

	*/

	long index;
	long PacX, PacY, GhostX, GhostY;
	long MinPacX, MaxPacX, MinPacY, MaxPacY;
	long MinGhostX, MaxGhostX, MinGhostY, MaxGhostY;
    long GhostState;
    long PacState;

    //make a bounding box for PacMan
    m_PacManController.GetPosition(PacX, PacY);
    MinPacX = PacX + 4;
    MaxPacX = PacX + m_PacManController.GetWidth()-4;
    MinPacY = PacY + 4;
    MaxPacY = PacY + m_PacManController.GetHeight()-4;

    //step through the ghosts
    for (index=0; index<m_NumGhosts; index++)
    {
        //make a bounding box for the ghost
        m_vecGhostControllers.at(index)->GetState(GhostState);
        m_vecGhostControllers.at(index)->GetPosition(GhostX, GhostY);
        MinGhostX = GhostX-2;
        MaxGhostX = GhostX + m_vecGhostControllers.at(index)->GetWidth()-2;
        MinGhostY = GhostY-2;
        MaxGhostY = GhostY + m_vecGhostControllers.at(index)->GetHeight()-2;

        //do the boxes overlap??
        if ( ((MinGhostX <= MinPacX) && (MaxGhostX >= MinPacX)) ||
             ((MinGhostX <= MaxPacX) && (MaxGhostX >= MaxPacX)) )
        {
            //X coords coincident
            if ( ((MinGhostY <= MinPacY) && (MaxGhostY >= MinPacY)) ||
                 ((MinGhostY <= MaxPacY) && (MaxGhostY >= MaxPacY)) )
            {
                //we have a ghost - PacMan collision!
                if ( GhostState == Ghost_BlueLeft || GhostState == Ghost_BlueRight ||
                     GhostState == Ghost_BlueUp || GhostState == Ghost_BlueDown )
                {
                    //Y coords coincident
                    //increment the score
                    m_Model.IncrementScore(m_Model.CurrentGhostValue());
                    m_Model.IncrementGhostValue();

                    //change the ghost state
                    switch(GhostState)
                    {
                        case Ghost_BlueLeft:
                            GhostState = Ghost_DeadLeft;
                            break;

                        case Ghost_BlueRight:
                            GhostState = Ghost_DeadRight;
                            break;

                        case Ghost_BlueUp:
                            GhostState = Ghost_DeadUp;
                            break;

                        case Ghost_BlueDown:
                            GhostState = Ghost_DeadDown;
                            break;
                    }
                    m_vecGhostControllers.at(index)->SetState(GhostState);
                }

                if ( GhostState == Ghost_HuntUp || GhostState == Ghost_HuntDown ||
                     GhostState == Ghost_HuntLeft || GhostState == Ghost_HuntRight )
                {
                    PacState = PacMan_Dead;
                    m_PacManController.SetState(PacState);
                }
            }
        }
    }
}

void WorldController::Reset()
{
	/*	Purpose:	Reset the world settings in order to restart the simulation cleanly

		Parameters:	none

		Return:		none

		Exceptions:

	*/

    long index;
    long state;
    long XSpeed = 0, YSpeed = 0;

	//execute various initialization routines
    //timers are ever present and don't need to be reset.  They continue undeterred.
    //the maze is also unchanging - no need to reload the bitmap, etc.
    m_UpdateCount = 0;  //reset the updates
    m_Model.SetScore(0); //reset the scores
    m_DotsController.Reset();  //reset the dot map to its orignal contents

	//reset the PacMan
	m_PacManController.SetPosition(m_Param.GetLongParam("PacManPositionX"),
                                    m_Param.GetLongParam("PacManPositionY"));
    state = PacMan_ActiveRight;
    m_PacManController.SetSpeed(XSpeed,YSpeed);
    m_PacManController.SetState(state);

	//do the ghosts
	for (index=0; index<m_NumGhosts; index++)
	{
            m_vecGhostControllers.at(index)->SetPosition(m_Param.GetLongParam("GhostPositionX")+(index*16),
                                                        m_Param.GetLongParam("GhostPositionY"));

            state = Ghost_CagedRight;
            m_vecGhostControllers.at(index)->SetState(state);
            m_vecGhostControllers.at(index)->SetSpeed(XSpeed, YSpeed);
            XSpeed=2; YSpeed=0;
            state = Ghost_CagedRight;
            m_vecGhostControllers.at(index)->SetState(state); //done a second time to store the state for reversion
            m_vecGhostControllers.at(index)->SetSpeed(XSpeed, YSpeed);
            m_vecGhostControllers.at(index)->ClearBackbuffer();
	}

	//need to reset powerups, too
	for ( index=0; index<4; index++)
	{
	    state = PowerUp_Active;
	    m_vecPowerUpControllers.at(index)->SetState(state);
	}

	m_View.Clear(); //clean up the backbuffer
	m_MapDrawn=false;  //the map will have to be redrawn

	//make sure the cage is closed
	m_MazeController.ModifyTile(13,13,16);
    m_MazeController.ModifyTile(14,13,17);
    m_MazeController.ModifyTile(15,13,18);
}

void WorldController::SetPacBrain(IBrain *Brain)
{
    //accept the incoming AI as the PacMan brain

    m_PacManController.SetSpriteBrain(Brain);
}

