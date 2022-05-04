#include "PacNEATBrain.h"

//-----------------------------------constructor-------------------------
//
//-----------------------------------------------------------------------
PacNEATBrain::PacNEATBrain()
{

}



//-----------------------------Reset()------------------------------------
//
//	Resets the Brains details
//
//------------------------------------------------------------------------
void PacNEATBrain::Reset()
{
	//reset the fitness
	m_dFitness = 0;
}

//-------------------------------Update()--------------------------------
//
//	Get details from the World and feed them into the Neural Net
//
//	The inputs are:
//
//  PacMan's X and Y position
//  All ghost X and Y positions and states
//  Power Pellet X and Y positions and states
//  # of dots left of PacMan'x X postion
//  # of dots right of PacMan'x X postion
//  # of dots below PacMan'x Y postion
//  # of dots above PacMan'x Y postion
//  Fruit X and Y postion and state
//
//-----------------------------------------------------------------------
bool PacNEATBrain::Update()
{
    long X, Y;
    long state;
    long index;
    vector<SpriteController *> GhostControllers;
    vector<SpriteController *> PowerUpControllers;
    RECT rct;
    long MapWidth, MapHeight;
    static long StartingX=0, StartingY=0;
    static long DotsUp, DotsDown, DotsLeft, DotsRight;
    ParameterMap pm;
    vector<double> inputs;

    //avoid too many calls to GetLongParam() - based on profiler results
    static bool firstrun = true;
    static string AIType;
    static long WindowRadius;

    if ( firstrun )
    {
        AIType = pm.GetStringParam("AIType");
        WindowRadius = pm.GetLongParam("WindowRadius");
        firstrun = false;
    }

	//get the inputs for the window
	if ( AIType == "Windowed" )
        inputs = GetWindowInputs(WindowRadius);
    else if ( AIType == "Global" )
        inputs = GetInputs();
    else if ( AIType == "Vector" )
        inputs = GetVectorInputs(WindowRadius);
    else if ( AIType == "Console" )
        inputs = GetConsoleInputs();
    else
        return 0; //do nothing if one of these AITypes isn't present

    //update the brain and get feedback
	vector<double> output = m_pItsBrain->Update(inputs, CNeuralNet::active);

	//make sure there were no errors in calculating the
	//output
	if (output.size() < CParams::iNumOutputs)
      {
        return false;
      }

	return true;
}

//------------------------- EndOfRunCalculations() -----------------------
//
//------------------------------------------------------------------------
void PacNEATBrain::EndOfRunCalculations(vector<long> &scores)
{
    long median, index, size;
    ParameterMap params;

    if ( params.GetStringParam("ScoringMethod") == "Average" )
    {
        m_dFitness=0;
        for (index=0; index<scores.size(); index++)
        {
            m_dFitness += scores.at(index);
        }
        m_dFitness /= scores.size();
    }
    else if ( params.GetStringParam("ScoringMethod") == "Median" )
    {
        //calculate the median of the scores
        sort(scores.begin(), scores.end());
        size = scores.size();
        index = (scores.size()/2);

        if ( size % 2 == 0)
        {
            m_dFitness = (scores.at(index) + scores.at(index-1))/2;
        }
        else
        {
            m_dFitness = scores.at(index);
        }
    }
    else if ( params.GetStringParam("ScoringMethod") == "Minimum" )
    {
        sort(scores.begin(), scores.end());
        m_dFitness = scores.at(0);
    }

    m_dScore = m_dFitness;

    if ( params.GetIntParam("PenalizeLinks") == 1 )
    {
        m_dFitness /= m_pItsBrain->GetNumLinks();
    }
}


//for compatibility with the IBrain interface
long PacNEATBrain::GetNextDirection(long &State, long &XSpeed, long &YSpeed, bool loggit)
{
    /*	Purpose:	Get the next direction for PM based on the PM and world conditions.

		Parameters:	State - return parameter for the desired state of the ghost - defined in Enumerations.h
                    XSpeed, YSpeed - return parameters for the desired speed

		Return:		0 if no action is taken
                    1 if a change of state/speed is done

		Exceptions:

	*/

	double MaxOutput;
	long MaxDirection;
    long retval;
    ParameterMap pm;
    vector<double> inputs;

    static bool firstrun = true;
    static string AIType;
    static long WindowRadius;
    static long outsize;

    fstream lf;

    if ( firstrun )
    {
        AIType = pm.GetStringParam("AIType");
        WindowRadius = pm.GetLongParam("WindowRadius");
        outsize = pm.GetLongParam("NumOutputs");
        firstrun =false;
    }

    if ( AIType == "Windowed" )
    {
        inputs = GetWindowInputs(WindowRadius);
    }
    else if ( AIType == "Global" )
        inputs = GetInputs();
    else if ( AIType == "Vector" )
        inputs = GetVectorInputs(WindowRadius);
    else if ( AIType == "Console" )
        inputs = GetConsoleInputs();
    else
        return 0; //do nothing if one of these AITypes isn't present

	//run the brain!
	vector<double> output = m_pItsBrain->Update(inputs, CNeuralNet::active);

    //determine what the next direction should be
    MaxOutput = output.at(0);
    MaxDirection = 0;
    retval = 0;
    for (long x=1; x<outsize; x++)
    {
        if ( output.at(x) > MaxOutput )
        {
            MaxOutput = output.at(x);
            MaxDirection = x;
        }
//        else if ( output.at(x) == MaxOutput )
//        {
//            //resolve ties by random choice
//            if ( (rand() % 2) == 0 )
//            {
//                MaxOutput = output.at(x);
//                MaxDirection = x;
//            }
//        }
    }
//this block is used to monitor the inputs and outputs for each call to this function
//I commented it out to eliminate an extra comparison every time
//    if ( loggit )
//    {
//        lf.open("InOut.csv", ios::out|ios::app);
//        lf << "In," << inputs.at(0);
//        for ( int q=1; q<inputs.size(); q++)
//            lf << "," << inputs.at(q);
//        lf << ",Out," << output.at(0);
//        for ( int q=1; q<output.size(); q++)
//            lf << "," << output.at(q);
//        lf << ",Maxdirection," << MaxDirection << endl;
//        lf.close();
//    }
    //there shouold be 4 outputs, so let's just assume that for directional choices
    switch(MaxDirection)
    {
        case 0:
            XSpeed = 0;
            YSpeed = -8;
            State = PacMan_ActiveUp;
            retval = 1;
            break;

        case 1:
            XSpeed = 0;
            YSpeed = 8;
            State = PacMan_ActiveDown;
            retval = 1;
            break;
        case 2:
            XSpeed = -8;
            YSpeed = 0;
            State = PacMan_ActiveLeft;
            retval = 1;
            break;
        case 3:
            XSpeed = 8;
            YSpeed = 0;
            State = PacMan_ActiveRight;
            retval = 1;
            break;
        default:
            retval = 0;
    }

    return retval;
}

vector<double> PacNEATBrain::GetVectorInputs(long WindowRadius)
{
    //get inputs based on vector representation of ghosts, PowerPellets, and (maybe) dots
    //the window is similar to GetWindowInputs and supplies either dots or walls around PM

    //get inputs corresponding to a window around PacMan
    //WindowWidth represents the number of tiles around PacMan in each direction
    long PacX, PacY;
    long SpriteX, SpriteY;
    long TileWidth, TileHeight;
    vector<double> inputs;
    vector<double> dots;
    vector<double> walls;
    vector<double> pellets;
    vector<double> ghosts;

    long x,y, index;
    long TileX, TileY, angX, angY;
    long State;
    double dist, angle;
    vector<SpriteController *> *PowerUpControllers = m_pWorld->GetPowerUpControllers();
    vector<SpriteController *> *GhostControllers = m_pWorld->GetGhostControllers();

    static bool firstrun = true;
    static long NumGhosts;
    static long PelletInputs;
    static long DotsWallsOnly;
    ParameterMap params;

    if ( firstrun )
    {
         NumGhosts = params.GetLongParam("NumberOfGhosts");
         PelletInputs = params.GetLongParam("PelletInputs");
         DotsWallsOnly = params.GetLongParam("DotsWallsOnly");
         firstrun = false;
    }

    //Get PacMan's location in tiles - this is the center of the window
    m_pWorld->GetPacManController()->GetPosition(PacX, PacY);
    m_pWorld->GetMazeController()->GetTileDimensions(TileWidth, TileHeight);

    PacX /= 16;//TileWidth;
    PacY /= 16;//TileHeight;

    //walls
    if ( DotsWallsOnly == 0 || DotsWallsOnly == -1 )
    {
        for ( y=PacY-WindowRadius; y<=PacY+1+WindowRadius; y++ )
        {
            for ( x=PacX-WindowRadius; x<=PacX+1+WindowRadius; x++ )
            {
                TileX = x;
                TileY = y;

                //allow tunnel look-through
                if ( TileX<0 && (TileY == 14 || TileY == 15) )
                    TileX += 29;

                if ( TileX > 28 && (TileY == 14 || TileY == 15) )
                    TileX -= 29;

                //no need to look under PM himself
                if ( (TileX == PacX && TileY == PacY) ||
                     (TileX == PacX+1 && TileY == PacY) ||
                     (TileX == PacX && TileY == PacY+1) ||
                     (TileX == PacX+1 && TileY == PacY+1) )
                {
                    continue;
                }

                //wall?
                if ( m_pWorld->GetMazeController()->GetPosition(TileX, TileY) == 0 )
                    walls.push_back(0);
                else
                    walls.push_back(1);
            }
        }
    }

    //pellets
    if ( PelletInputs != 0 )
    {
        for ( index=0; index<PowerUpControllers->size(); index++)
        {
            PowerUpControllers->at(index)->GetPosition(SpriteX, SpriteY);
            PowerUpControllers->at(index)->GetState(State);
            SpriteX /= 16;
            SpriteY /= 16;

            if ( State == PowerUp_Active )
            {
                //make PM the origin
                angX = abs(PacX - SpriteX);
                angY = abs(PacY - SpriteY);
                if ( PacX > SpriteX )
                    angX = -angX;

                dist = sqrt( angX*angX + angY*angY);
                angle = acos( angX /dist ) * (180/3.141519);

                if ( angY < 0 )
                    angle = -angle;

                pellets.push_back(dist);
                pellets.push_back(angle);
            }
            else
            {
                pellets.push_back(0);
                pellets.push_back(0);
            }
        }
    }

    //ghosts
    for ( index=0; index<GhostControllers->size(); index++ )
    {
        //first, is a ghost here??
        GhostControllers->at(index)->GetPosition(SpriteX, SpriteY);
        GhostControllers->at(index)->GetState(State);
        SpriteX /= 16;
        SpriteY /= 16;

        //make PM the origin
        angX = abs(PacX - SpriteX);
        angY = abs(PacY - SpriteY);
        if ( PacX > SpriteX )
            angX = -angX;

        dist = sqrt( angX*angX + angY*angY);
        angle = acos( angX /dist ) * (180/3.141519);

        if ( angY < 0 )
            angle = -angle;

        ghosts.push_back(dist);
        ghosts.push_back(angle);

        //state of the ghost?
        if ( State == Ghost_HuntUp || State == Ghost_HuntDown || State == Ghost_HuntLeft || State == Ghost_HuntRight )
            ghosts.push_back(-1);
        else if ( State == Ghost_BlueLeft || State == Ghost_BlueRight || State == Ghost_BlueUp || State == Ghost_BlueDown )
            ghosts.push_back(1);
        else
            ghosts.push_back(0); //all other states
    }

    //dots
    if ( DotsWallsOnly == 0 || DotsWallsOnly == 1 )
    {
        long sumX=0, sumY=0, count=0;
        for (x=0; x<m_pWorld->GetDotController()->GetWidth(); x++)
        {
            for (y=0; y<m_pWorld->GetDotController()->GetHeight(); y++)
            {
                if ( m_pWorld->GetDotController()->GetPosition(x,y) == 1 )
                {
                    sumX+=x;
                    sumY+=y;
                    count++;
                }
            }
        }

        sumX/=count;
        sumY/=count; //compute the centroid of the dots

        //make PM the origin
        angX = abs(PacX - sumX);
        angY = abs(PacY - sumY);
        if ( PacX > sumX )
            angX = -angX;

        dist = sqrt( angX*angX + angY*angY);
        angle = acos( angX /dist ) * (180/3.141519);

        if ( angY < 0 )
            angle = -angle;

        dots.push_back(dist);
        dots.push_back(angle);
    }

    //combine the inputs into one vector
    if ( DotsWallsOnly == 0 || DotsWallsOnly == 1 )
    {
        inputs.insert(inputs.end(), dots.begin(), dots.end());
    }

    if ( DotsWallsOnly == 0 || DotsWallsOnly == -1 )
    {
        inputs.insert(inputs.end(), walls.begin(), walls.end());
    }

    if ( PelletInputs != 0 )
        inputs.insert(inputs.end(), pellets.begin(), pellets.end());

    inputs.insert(inputs.end(), ghosts.begin(), ghosts.end());

    return inputs;
}
vector<double> PacNEATBrain::GetWindowInputs(long WindowRadius)
{
    //get inputs corresponding to a window around PacMan
    //WindowWidth represents the number of tiles around PacMan in each direction
    long PacX, PacY;
    long SpriteX, SpriteY;
    long TileWidth, TileHeight;
    vector<double> inputs;
    vector<double> dots;
    vector<double> walls;
    vector<double> pellets;
    vector<double> ghosts;
    vector<double> blue;
    vector<double> dotwall;

    long x,y,index;
    long TileX, TileY;
    long State;
    vector<SpriteController *> *PowerUpControllers = m_pWorld->GetPowerUpControllers();
    vector<SpriteController *> *GhostControllers = m_pWorld->GetGhostControllers();
    bool found=false;
    bool bluefound = false;

    static bool firstrun = true;
    static long NumGhosts;
    static long DotWallSingleInput;
    static long SingleGhostInput;
    static long PelletInputs;
    static long DotsWallsOnly;
    ParameterMap params;

    if ( firstrun )
    {
         NumGhosts = params.GetLongParam("NumberOfGhosts");
         DotWallSingleInput = params.GetLongParam("DotWallSingleInput");
         SingleGhostInput = params.GetLongParam("SingleGhostInput");
         PelletInputs = params.GetLongParam("PelletInputs");
         DotsWallsOnly = params.GetLongParam("DotsWallsOnly");
         firstrun = false;
    }

    //Get PacMan's location in tiles - this is the center of the window
    m_pWorld->GetPacManController()->GetPosition(PacX, PacY);
    m_pWorld->GetMazeController()->GetTileDimensions(TileWidth, TileHeight);

    PacX /= 16;//TileWidth;
    PacY /= 16;//TileHeight;

    //step through all the tiles in the window and see what's there
    //we need 4 bits/inputs per tile - dot, hunting ghost, blue ghost, power pellet
    //each bit is 0 (false) or 1 (true) for the desired property/item
    //optionally, another bit represetns the walls 0=no wall, 1=wall
    //at this point I'm experiemtning with the walls - I'll decide to keep/remove them later

    for ( y=PacY-WindowRadius; y<=PacY+1+WindowRadius; y++ )
    {
        for ( x=PacX-WindowRadius; x<=PacX+1+WindowRadius; x++ )
        {
            TileX = x;
            TileY = y;

            //allow tunnel look-through
            if ( TileX<0 && (TileY == 14 || TileY == 15) )
                TileX += 29;

            if ( TileX > 28 && (TileY == 14 || TileY == 15) )
                TileX -= 29;

            //no need to look under PM himself
            if ( (TileX == PacX && TileY == PacY) ||
                 (TileX == PacX+1 && TileY == PacY) ||
                 (TileX == PacX && TileY == PacY+1) ||
                 (TileX == PacX+1 && TileY == PacY+1) )
            {
                continue;
            }

            if ( DotWallSingleInput == 0 )
            {
                if ( DotsWallsOnly == 1 || DotsWallsOnly == 0 )
                {
                    //dot?
                    if ( m_pWorld->GetDotController()->GetPosition(TileX, TileY) == 1 )
                    {
                        dots.push_back(1);
                    }
                    else
                    {
                        dots.push_back(0);
                    }
                }

                if ( DotsWallsOnly == 0 || DotsWallsOnly == -1 )
                {
                    //wall?
                    if ( m_pWorld->GetMazeController()->GetPosition(TileX, TileY) == 0 )
                    {
                        walls.push_back(0);
                    }
                    else
                    {
                        walls.push_back(1);
                    }
                }
            }
            else
            {
                if ( m_pWorld->GetDotController()->GetPosition(TileX,TileY) == 1 )
                {
                    if ( DotsWallsOnly == 1 || DotsWallsOnly == 0 )
                        dotwall.push_back(1);  //theres a dot and I'm monitoring dots only or both
                    else
                        dotwall.push_back(0); //I'm only looking for walls so ignore this dot
                }
                else if ( m_pWorld->GetMazeController()->GetPosition(TileX, TileY) != 0 )
                {
                    if ( DotsWallsOnly == -1 || DotsWallsOnly == 0 )
                        dotwall.push_back(-1);  //there's a wall and I'm monitoring walls only or both
                    else
                        dotwall.push_back(0);  //I'm only looking for dots so ignore this wall
                }
                else
                    dotwall.push_back(0); //there's nothing there - either way I want to know that
            }

            //pellets
            if ( PelletInputs != 0 )
            {
                for ( index=0; index<PowerUpControllers->size(); index++)
                {
                    found = false;
                    PowerUpControllers->at(index)->GetPosition(SpriteX, SpriteY);
                    SpriteX /= 16;
                    SpriteY /= 16;

                    if ( SpriteX == TileX && SpriteY == TileY )
                    {
                        PowerUpControllers->at(index)->GetState(State);
                        if ( State == PowerUp_Active )
                        {
                            //there's an active pellet here
                            found = true;
                        }
                    }
                }

                if ( found )
                {
                    pellets.push_back(1);
                }
                else
                {
                    pellets.push_back(0); //even if it is there, it is inactive
                }
            }

            //ghosts
            if ( GhostControllers->size() > 0 )
            {
                found=false;
                bluefound = false;

                for ( index=0; index<GhostControllers->size(); index++ )
                {
                    //first, is a ghost here??
                    GhostControllers->at(index)->GetPosition(SpriteX, SpriteY);
                    SpriteX /= 16;
                    SpriteY /= 16;
                    if ( SpriteX == TileX && SpriteY == TileY )
                    {
                        GhostControllers->at(index)->GetState(State);

                        //there's a ghost there - is he hunting?
                        if ( State == Ghost_HuntUp || State == Ghost_HuntDown || State == Ghost_HuntLeft || State == Ghost_HuntRight )
                        {
                            found=true;
                        }

                        //is he blue?
                        if ( State == Ghost_BlueLeft || State == Ghost_BlueRight || State == Ghost_BlueUp || State == Ghost_BlueDown )
                        {
                            bluefound=true;
                        }
                    }
                }
                if ( SingleGhostInput == 0 )
                {
                    if ( found )
                        ghosts.push_back(1);
                    else
                        ghosts.push_back(0);

                    if ( bluefound )
                        blue.push_back(1);
                    else
                        blue.push_back(0);
                }
                else
                {
                    if ( found )
                        ghosts.push_back(-1); //hunters take precedent if overlapped
                    else if ( bluefound )
                        ghosts.push_back(1);
                    else
                        ghosts.push_back(0);
                }
            }
        }
    }

    //combine the various inputs into one big glop
    //My hope was that this would allow visualization of which inputs were on off
    //in the neural net visualization while running.  May not work as well as hoped.
    if ( DotWallSingleInput == 0 )
    {
        if ( DotsWallsOnly == 0 || DotsWallsOnly == 1 )
        {
            inputs.insert(inputs.end(), dots.begin(), dots.end());
        }

        if ( DotsWallsOnly == 0 || DotsWallsOnly == -1 )
        {
            inputs.insert(inputs.end(), walls.begin(), walls.end());
        }
    }
    else
        inputs.insert(inputs.end(), dotwall.begin(), dotwall.end());

    if ( PelletInputs != 0 )
        inputs.insert(inputs.end(), pellets.begin(), pellets.end());

    if ( SingleGhostInput == 0 )
    {
        inputs.insert(inputs.end(), ghosts.begin(), ghosts.end());
        inputs.insert(inputs.end(), blue.begin(), blue.end());
    }
    else
        inputs.insert(inputs.end(), ghosts.begin(), ghosts.end());

    return inputs;
}

vector<double> PacNEATBrain::GetInputs()
{
    long X, Y;
    long state;
    long index;
    vector<SpriteController *> GhostControllers;
    vector<SpriteController *> PowerUpControllers;
    RECT rct;
    static long MaxDirection=0, MaxDots=0;

    long ret;
    ParameterMap Params;

    static long StartingX=0, StartingY=0;
    static long DotsUp=0, DotsDown=0, DotsLeft=0, DotsRight=0;

    //the following is strictly for speed up purposes
    //profiling the code showed too much time in GetLongParam
    static bool firstRun = true;
    static bool GhostXY=false, GhostState=false, PowerUpXY=false, PowerUpState=false, PacManXY=false, CheckForWalls=false, CheckDots=false, MaxDotDirection=false;

    if ( firstRun )
    {
        if ( Params.GetLongParam("GhostXY") == 1 )
            GhostXY = true;
        if ( Params.GetLongParam("GhostState") == 1 )
            GhostState = true;
        if ( Params.GetLongParam("PowerUpXY") == 1 )
            PowerUpXY = true;
        if ( Params.GetLongParam("PowerUpState") == 1 )
            PowerUpState = true;
        if ( Params.GetLongParam("PacManXY") == 1 )
            PacManXY = true;
        if ( Params.GetLongParam("CheckForWalls") == 1 )
            CheckForWalls = true;
        if ( Params.GetLongParam("CheckDots") == 1 )
            CheckDots = true;
        if ( Params.GetLongParam("MaxDotDirection") == 1 )
            MaxDotDirection = true;

        firstRun = false;
    }

    //put the inputs together and return the Vector
    //this will store all the inputs for the NN
	vector<double> inputs;

    //feed the inputs in the "inputs" vector
    //e.g., vector.push_back(# of Dots above, etc)
    GhostControllers = *(m_pWorld->GetGhostControllers());
    for (index = 0; index < GhostControllers.size(); index++)
    {
        if ( GhostXY )
        {
            GhostControllers.at(index)->GetPosition(X,Y);
            inputs.push_back(X/16);
            inputs.push_back(Y/16);
        }
        if ( GhostState )
        {
            GhostControllers.at(index)->GetState(state);
            inputs.push_back(state);
        }
    }

    PowerUpControllers = *(m_pWorld->GetPowerUpControllers());
    for (index = 0; index < PowerUpControllers.size(); index++)
    {
        if ( PowerUpXY )
        {
            PowerUpControllers.at(index)->GetPosition(X,Y);
            inputs.push_back(X/16);
            inputs.push_back(Y/16);
        }
        if ( PowerUpState )
        {
            PowerUpControllers.at(index)->GetState(state);
            inputs.push_back(state);
        }
    }

    m_pWorld->GetPacManController()->GetPosition(X,Y);
    if ( PacManXY )
    {
        inputs.push_back(X/16);
        inputs.push_back(Y/16);
    }

    if ( CheckForWalls )
    {
        if ( m_pWorld->GetMazeController()->CheckMapByPixels(X,X+31,Y-8,(Y-8)+31,ret))
            inputs.push_back(0);  //nothing in the up direction
        else
            inputs.push_back(1);  //a wall is in the way

        if ( m_pWorld->GetMazeController()->CheckMapByPixels(X,X+31,Y+8,(Y+8)+31,ret) )
            inputs.push_back(0);  //nothing in the down direction
        else
            inputs.push_back(1);  //a wall is in the way

        if ( m_pWorld->GetMazeController()->CheckMapByPixels(X-8,(X-8)+31,Y,Y+31,ret) )
            inputs.push_back(0);  //nothing in the left direction
        else
            inputs.push_back(1);  //a wall is in the way

        if ( m_pWorld->GetMazeController()->CheckMapByPixels(X+8,(X+8)+31,Y,Y+31,ret) )
            inputs.push_back(0);  //nothing in the right direction
        else
            inputs.push_back(1);  //a wall is in the way
    }

    //use the starting variables to limit the number of times we check the dot content
    if ( abs(StartingX - X) > 32 || abs(StartingY - Y) > 32 )
    {
        StartingX = X;
        StartingY = Y;

        //only update the number of dots every 8 steps
        //profiling suggested this would be a good place to save computation time
        rct.top = 0;
        rct.bottom = (Y+31)/16;
        rct.left = 0;
        rct.right = m_pWorld->GetDotController()->GetWidth()-1;
        DotsUp = m_pWorld->GetDotController()->CountTilesOfType(rct, 1);
        MaxDots = DotsUp;
        MaxDirection = 0;

        rct.top = (Y/16);
        rct.bottom = m_pWorld->GetDotController()->GetHeight()-1;
        DotsDown = m_pWorld->GetDotController()->CountTilesOfType(rct, 1);
        if ( MaxDots < DotsDown )
        {
            MaxDots = DotsDown;
            MaxDirection = 1;
        }

        rct.top = 0;
        rct.right = (X+31)/16;
        DotsLeft = m_pWorld->GetDotController()->CountTilesOfType(rct, 1);
        if ( MaxDots < DotsLeft )
        {
            MaxDots = DotsLeft;
            MaxDirection = 2;
        }

        rct.left = (X/16);
        rct.right = m_pWorld->GetDotController()->GetWidth()-1;
        DotsRight = m_pWorld->GetDotController()->CountTilesOfType(rct, 1);
        if ( MaxDots < DotsRight )
            MaxDirection = 3;
    }

    //the dot variables are static, but are only updated (above) every 32 steps, or, one PacMan width
    if ( CheckDots )
    {
        //push the number of dots in
        inputs.push_back( DotsUp );
        inputs.push_back( DotsDown );
        inputs.push_back( DotsLeft );
        inputs.push_back( DotsRight );
    }

    //check which direction has the most dots and push that in
    if ( MaxDotDirection )
    {
        for (index=0; index<MaxDirection; index++)
        {
            inputs.push_back(0);
        }

        inputs.push_back(1);

        for (index=MaxDirection+1; index<4; index++)
        {
            inputs.push_back(0);
        }
    }

    return inputs;
}

vector<double> PacNEATBrain::GetConsoleInputs()
{
    vector<double> inputs;

    int x_dim = 29;
    int y_dim = 32;

    vector<SpriteController *> SpriteControllers;
    SpriteController *PMController;

    long state;
    long X, Y;

    //dots and walls
    vector<double> walls(x_dim*y_dim, 0);
    vector<double> dots(x_dim*y_dim, 0);

    ParameterMap params;

    for( int TileY=0; TileY<y_dim; TileY++)
    {
        for( int TileX=0; TileX<x_dim; TileX++)
        {
            state = m_pWorld->GetMazeController()->GetPosition(TileX, TileY);
            if( m_pWorld->GetMazeController()->GetPosition(TileX, TileY) != 0)
            {
                walls[TileY*x_dim + TileX] = -1;
            }
            if( m_pWorld->GetDotController()->GetPosition(TileX, TileY) != 0 )
            {
                dots[TileY*x_dim + TileX] = 1;
            }
        }
    }

    //powerups
    vector<double> powerups(x_dim*y_dim, 0);
    SpriteControllers = *(m_pWorld->GetPowerUpControllers());
    for ( int index = 0; index < SpriteControllers.size(); index++)
    {
        SpriteControllers.at(index)->GetState(state);
        if ( state == PowerUp_Active )
        {
            SpriteControllers.at(index)->GetPosition(X,Y);
            powerups[int(Y/16)*x_dim + int(X/16)] = 1;
            powerups[int(Y/16)*x_dim + (int(X/16)+1)] = 1;
            powerups[(int(Y/16)+1)*x_dim + int(X/16)] = 1;
            powerups[(int(Y/16)+1)*x_dim + (int(X/16)+1)] = 1;
        }
    }

    //ghosts
    vector<double> ghosts(x_dim*y_dim, 0);
    SpriteControllers = *(m_pWorld->GetGhostControllers());
        for ( int index = 0; index < SpriteControllers.size(); index++)
        {
            SpriteControllers.at(index)->GetState(state);
            if ( state == Ghost_HuntUp | state == Ghost_HuntDown |
                 state == Ghost_HuntLeft | state == Ghost_HuntRight )
            {
                SpriteControllers.at(index)->GetPosition(X,Y);
                ghosts[int(Y/16)*x_dim + int(X/16)] = -1;
                ghosts[int(Y/16)*x_dim + (int(X/16)+1)] = -1;
                ghosts[(int(Y/16)+1)*x_dim + int(X/16)] = -1;
                ghosts[(int(Y/16)+1)*x_dim + (int(X/16)+1)] = -1;
            }

            if ( state == Ghost_BlueUp | state == Ghost_BlueDown |
                 state == Ghost_BlueLeft | state == Ghost_BlueRight )
            {
                SpriteControllers.at(index)->GetPosition(X,Y);
                ghosts[int(Y/16)*x_dim + int(X/16)] = 1;
                ghosts[int(Y/16)*x_dim + (int(X/16)+1)] = 1;
                ghosts[(int(Y/16)+1)*x_dim + int(X/16)] = 1;
                ghosts[(int(Y/16)+1)*x_dim + (int(X/16)+1)] = 1;
            }
        }
    //pacman
    vector<double> pacman(x_dim*y_dim, 0);
    PMController = m_pWorld->GetPacManController();
    PMController->GetPosition(X,Y);
    pacman[int(Y/16)*x_dim + int(X/16)] = 1;
    pacman[int(Y/16)*x_dim + (int(X/16)+1)] = 1;
    pacman[(int(Y/16)+1)*x_dim + int(X/16)] = 1;
    pacman[(int(Y/16)+1)*x_dim + (int(X/16)+1)] = 1;

    //gather all inputs together
    if( params.GetStringParam("CompositeScheme") == "AllLayers")
    {
        inputs.insert( inputs.end(), walls.begin(), walls.end());
        inputs.insert( inputs.end(), dots.begin(), dots.end());
        inputs.insert( inputs.end(), powerups.begin(), powerups.end());
        inputs.insert( inputs.end(), ghosts.begin(), ghosts.end());
        inputs.insert( inputs.end(), pacman.begin(), pacman.end());
    }
    else if( params.GetStringParam("CompositeScheme") == "Flatten")
    {
        inputs.insert( inputs.end(), walls.begin(), walls.end() );

        std::transform( inputs.begin(), inputs.end(),
                        dots.begin(), inputs.begin(),
                        std::plus<double>( ));

        std::transform( powerups.begin(), powerups.end(), powerups.begin(),
                        std::bind1st(std::multiplies<double>(), 50));
        std::transform( inputs.begin(), inputs.end(),
                        powerups.begin(), inputs.begin(),
                        std::plus<double>( ));

//        std::transform( ghosts.begin(), ghosts.end(), ghosts.begin(),
//                        std::bind1st(std::multiplies<double>(), 200));
//        std::transform( inputs.begin(), inputs.end(),
//                        ghosts.begin(), inputs.begin(),
//                        std::plus<double>( ));

        vector<double>::iterator res = ghosts.begin();
        while( res != ghosts.end() )
        {
            res = std::find(res, ghosts.end(), 1);
            if( res != ghosts.end() )
            {
                inputs[ std::distance(ghosts.begin(), res) ] = 200;
                res+=1;
            }
        }

        res = ghosts.begin();
        while( res != ghosts.end() )
        {
            res = std::find(res, ghosts.end(), -1);
            if( res != ghosts.end() )
            {
                inputs[ std::distance(ghosts.begin(), res) ] = -200;
                res+=1;
            }
        }

        inputs.insert( inputs.end(), pacman.begin(), pacman.end() );
    }

    return inputs;
}
