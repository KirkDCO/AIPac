#include "CController.h"

bool Terminated = false;  //used by DoEvents()

//these hold the geometry of the sweepers and the mines
//const int	 NumSweeperVerts = 16;
/*const SPoint sweeper[NumSweeperVerts] = {SPoint(-1, -1),
                                         SPoint(-1, 1),
                                         SPoint(-0.5, 1),
                                         SPoint(-0.5, -1),

                                         SPoint(0.5, -1),
                                         SPoint(1, -1),
                                         SPoint(1, 1),
                                         SPoint(0.5, 1),

                                         SPoint(-0.5, -0.5),
                                         SPoint(0.5, -0.5),

                                         SPoint(-0.5, 0.5),
                                         SPoint(-0.25, 0.5),
                                         SPoint(-0.25, 1.75),
                                         SPoint(0.25, 1.75),
                                         SPoint(0.25, 0.5),
                                         SPoint(0.5, 0.5)};



const int NumMineVerts = 4;
const SPoint mine[NumMineVerts] = {SPoint(-1, -1),
                                   SPoint(-1, 1),
                                   SPoint(1, 1),
                                   SPoint(1, -1)};


const int NumObjectVerts = 42;
const SPoint objects[NumObjectVerts] = {SPoint(80, 60),
                                        SPoint(200,60),
                                        SPoint(200,60),
                                        SPoint(200,100),
                                        SPoint(200,100),
                                        SPoint(160,100),
                                        SPoint(160,100),
                                        SPoint(160,200),
                                        SPoint(160,200),
                                        SPoint(80,200),
                                        SPoint(80,200),
                                        SPoint(80,60),

                                        SPoint(250,100),
                                        SPoint(300,40),
                                        SPoint(300,40),
                                        SPoint(350,100),
                                        SPoint(350,100),
                                        SPoint(250, 100),

                                        SPoint(220,180),
                                        SPoint(320,180),
                                        SPoint(320,180),
                                        SPoint(320,300),
                                        SPoint(320,300),
                                        SPoint(220,300),
                                        SPoint(220,300),
                                        SPoint(220,180),

                                        SPoint(12,15),
                                        SPoint(380, 15),
                                        SPoint(380,15),
                                        SPoint(380,360),
                                        SPoint(380,360),
                                        SPoint(12,360),
                                        SPoint(12,360),
                                        SPoint(12,340),
                                        SPoint(12,340),
                                        SPoint(100,290),
                                        SPoint(100,290),
                                        SPoint(12,240),
                                        SPoint(12,240),
                                        SPoint(12,15)};




*/
//---------------------------------------constructor---------------------
//
//	initilaize the sweepers, their brains and the GA factory
//
//-----------------------------------------------------------------------
CController::CController(HWND hwndMain,
                         int  cxClient,
                         int  cyClient): m_NumNets(CParams::iPopSize),
                                         m_bFastRender(true),
                                         m_bRenderBest(false),
                                         m_iTicks(0),
                                         m_hwndMain(hwndMain),
                                         m_hwndInfo(NULL),
                                         m_iGenerations(0),
                                         m_cxClient(cxClient),
                                         m_cyClient(cyClient)
{
	//create the world controller
   	m_pGameEngine = new WorldController("AIPac.param");

	//let's create the mine sweepers
	for (int i=0; i<m_NumNets; i++)
	{
		m_vecNeuralNets.push_back(PacNEATBrain());
	}

  //and the vector of sweepers which will hold the best performing sweeprs
  for (int i=0; i<CParams::iNumBest; i++)
	{
		m_vecBestNets.push_back(PacNEATBrain());
	}



  m_pPop = new Cga(CParams::iPopSize,
                   CParams::iNumInputs,
                   CParams::iNumOutputs);

  //create the phenotypes
   vector<CNeuralNet*> pBrains = m_pPop->CreatePhenotypes();

	//assign the phenotypes
  for (int i=0; i<m_NumNets; i++)
  {
    m_vecNeuralNets[i].InsertNewBrain(pBrains[i]);
  }

	//create a pen for the graph drawing
	m_BluePen        = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
	m_RedPen         = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
	m_GreenPen       = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
  m_GreyPenDotted  = CreatePen(PS_DOT, 1, RGB(100, 100, 100));
  m_RedPenDotted   = CreatePen(PS_DOT, 1, RGB(200, 0, 0));

	m_OldPen	= NULL;

  //and the brushes
  m_BlueBrush = CreateSolidBrush(RGB(0,0,244));
  m_RedBrush  = CreateSolidBrush(RGB(150,0,0));

//	//fill the vertex buffers
//	for (i=0; i<NumSweeperVerts; ++i)
//	{
//		m_SweeperVB.push_back(sweeper[i]);
//	}
//
//for (i=0; i<NumObjectVerts; ++i)
//  {
//    m_ObjectsVB.push_back(objects[i]);
//  }
}

//--------------------------------------destructor-------------------------------------
//
//--------------------------------------------------------------------------------------
CController::~CController()
{
	delete m_pGameEngine;

  if (m_pPop)
  {
    delete m_pPop;
  }

	DeleteObject(m_BluePen);
	DeleteObject(m_RedPen);
	DeleteObject(m_GreenPen);
	DeleteObject(m_OldPen);
  DeleteObject(m_GreyPenDotted);
  DeleteObject(m_RedPenDotted);
  DeleteObject(m_BlueBrush);
  DeleteObject(m_RedBrush);
}


//-------------------------------------Update---------------------------------------
//
//	This is the main workhorse. The entire simulation is controlled from here.
//
//--------------------------------------------------------------------------------------
bool CController::Update()
{
    static bool firstRun=true;
    static long Reps;
    ParameterMap params;
    long index;
    vector<long> scores;
    vector<double> inputs, outputs;

//    static long bestever=0;;
//    fstream log;
//    log.open("BestMonitor.txt", ios::out|ios::app);

    if ( firstRun )
    {
        Reps = params.GetLongParam("Replicates");
        firstRun = false;
    }

	//run the sweepers through NUM_TICKS amount of cycles. During this loop each
	//sweepers NN is constantly updated with the appropriate information from its
	//surroundings. The output from the NN is obtained and the sweeper is moved.
	for (int i=0; i<m_NumNets; i++)
	{
	//evaluate the NN through the simulator:
        //reset the worldcontroller
        m_pGameEngine->Reset();

        //push the net in
        m_pGameEngine->SetPacBrain(&m_vecNeuralNets[i]);

        //tell the brain what world it is in
        m_vecNeuralNets[i].SetWorld(m_pGameEngine);
		m_vecNeuralNets[i].Update();
		scores.clear();

        //run the simulation
        for ( index = 0; index < Reps; index++ )
        {
           while (m_pGameEngine->Update(false))
            {
               ;
            } //just keep repeating until we get a false back - time's up or PacMan died

            scores.push_back(m_pGameEngine->GetScore());
            m_pGameEngine->Reset();
        }

        if ( i % 10 == 0 )
        {
            DoEvents( m_hwndMain );    //I put these in to prevent the graphical windows
            DoEvents( m_hwndInfo );    //from freezing up during the evolutionary step
            DoEvents( m_hwndConsole );

            if (Terminated)
                exit(0);
        }

        //do end of run calculations to get accurate score
        (m_vecNeuralNets[i]).EndOfRunCalculations(scores);
	}

//monitor the best results
//        if ( bestever < m_vecNeuralNets[i].Fitness() )
//        {
//            bestever = m_vecNeuralNets[i].Fitness();
//This block of code runs through the NN's inputs and outputs to verify
//it generates the same each time.
//            log << "New Best: " << bestever << endl;
//            log << "Generation: " << m_pPop->Generation() << endl;
//            m_pPop->GetGenomeByID(m_vecNeuralNets[i].GetGenomeID()).Write(log);
//            log << endl << endl;
//            log.flush();
//            log << "GenomeID: " << m_vecNeuralNets[i].GetGenomeID() << endl;
//            for (int w=0; w<=1; w++)
//            {
//                for (int x=0; x<=1; x++)
//                {
//                    for (int y=0; y<=1; y++)
//                    {
//                        for (int z=0; z<=1; z++)
//                        {
//                            inputs.clear();
//                            inputs.push_back(w);inputs.push_back(x);inputs.push_back(y);inputs.push_back(z);
//                            outputs = (m_vecNeuralNets[i].m_pItsBrain)->Update(inputs, CNeuralNet::active);
//                            log << "Inputs: " << w << x << y << z << " -- " << outputs[0] << "," << outputs[1] << ","
//                                        << outputs[2] << "," << outputs[3] << endl;
//                        }
//                    }
//                }
//            }
//        log << endl << endl << endl;
//        log.flush();
//        }
//	}

    //draw the networks
    InvalidateRect(m_hwndInfo, NULL, TRUE);
    UpdateWindow(m_hwndInfo);

	//We have completed another generation so now we need to run the GA

		//increment the generation counter
		++m_iGenerations;

    //perform an epoch and grab the new brains
    vector<CNeuralNet*> pBrains = m_pPop->Epoch(GetFitnessScores(), GetGameScores());

		//insert the new  brains back into the sweepers and reset their
    //positions
    for (int i=0; i<m_NumNets; i++)
		{
			m_vecNeuralNets[i].InsertNewBrain(pBrains[i]);
			//m_vecNeuralNets[i].Reset();  //seems irrelevant
		}

    //grab the NNs of the best performers from the last generation
    vector<CNeuralNet*> pBestBrains = m_pPop->GetBestPhenotypesFromLastGeneration();

    //put them into our record of the best sweepers
    for (int i=0; i<m_vecBestNets.size(); i++)
    {
        m_vecBestNets[i].InsertNewBrain(pBestBrains[i]);
        //m_vecBestNets[i].Reset();//seems irrelevant
    }

    if ( m_bRenderBest )
    {
        ParameterMap params;
        long visDelay = params.GetLongParam("VisDelay");
        if ( visDelay < 0 )
            visDelay = 0;

        m_BestBrainEver.InsertNewBrain(m_pPop->GetBestPhenotypeEver());
        m_pGameEngine->Reset();
        m_pGameEngine->SetPacBrain(&m_BestBrainEver);
        m_BestBrainEver.SetWorld(m_pGameEngine);
        m_BestBrainEver.Update();

//        long score1, score2, score3, score4;
//        log2.open("InOut.csv", ios::out|ios::app);
//        log2 << "Score 1" << endl;
//        log2.close();
//        while ( m_pGameEngine->Update(false) )
//        {
//            ;
//        }
//        score1 = m_pGameEngine->GetScore();
//        log2.open("InOut.csv", ios::out|ios::app);
//        log2 << endl << endl << endl << "Score 4" << endl;
//        log2.close();
//
//        m_pGameEngine->Reset();
//        m_BestBrainEver.Update();
//        while ( m_pGameEngine->Update(false) )
//        {
//            ;
//        }
//        score4 = m_pGameEngine->GetScore();
//        log2.open("InOut.csv", ios::out|ios::app);
//        log2 << endl << endl << endl << "Score 2" << endl;
//        log2.close();
//
//        m_pGameEngine->Reset();
//        m_BestBrainEver.Update();
        while ( m_pGameEngine->Update(false) )
        {
            InvalidateRect(m_hwndConsole, NULL, TRUE);
            UpdateWindow(m_hwndConsole);
            DoEvents(m_hwndConsole);  //see if this solves a crash problem
            Sleep(visDelay);
        }

//        score2 = m_pGameEngine->GetScore();
//        log2.open("InOut.csv", ios::out|ios::app);
//        log2 << endl << endl << endl << "Score 3" << endl;
//        log2.close();
//
//        m_pGameEngine->Reset();
//        m_BestBrainEver.Update();
//        while ( m_pGameEngine->Update(false) )
//        {
//            ;
//        }
//
//        score3 = m_pGameEngine->GetScore();
//        log2.open("InOut.csv", ios::out|ios::app);
//        log2 << "Done" << endl << endl << endl;
//        log2.close();
//
//        log << "Best: " << bestever << " 1: " << score1 << " 4: " << score4 << " 2:" << score2 << " 3: " << score3 << endl;
//        log.flush();

    }
    else
    {
        m_pGameEngine->Reset();
        InvalidateRect(m_hwndConsole, NULL, TRUE);
        UpdateWindow(m_hwndConsole);
    }

    return true;
}


//This block of code checked multiple runs of the NN to see
//if the results were consistent.  I found that NNs were
//accumulating values leading to different outputs for the same inputs
//between runs.  The results was that you couldn't watch a valid PacMan
//and there were non-reproducible improvements creeping into the simulation.
//long score1, score2, score3, score4;
//if ( m_bRenderBest )
//    {
//        ParameterMap params;
//        long visDelay = params.GetLongParam("VisDelay");
//        if ( visDelay < 0 )
//            visDelay = 0;
//
//        m_BestBrainEver.InsertNewBrain(m_pPop->GetBestPhenotypeEver());
//        m_pGameEngine->Reset();
//        m_pGameEngine->SetPacBrain(&m_BestBrainEver);
//        m_BestBrainEver.SetWorld(m_pGameEngine);
//        m_BestBrainEver.Update();
//
//        while ( m_pGameEngine->Update(false) )
//        {
//            ;
//        }
//        score1 = m_pGameEngine->GetScore();
////        log2.open("InOut.csv", ios::out|ios::app);
////        log2 << endl << endl << endl;
////        log2.close();
//
//        while ( m_pGameEngine->Update(false) )
//        {
//            ;
//        }
//        score4 = m_pGameEngine->GetScore();
////        log2.open("InOut.csv", ios::out|ios::app);
////        log2 << endl << endl << endl;
////        log2.close();
//
//        m_pGameEngine->Reset();
//        m_BestBrainEver.Update();
//        while ( m_pGameEngine->Update(false) )
//        {
//            InvalidateRect(m_hwndConsole, NULL, TRUE);
//            UpdateWindow(m_hwndConsole);
//            DoEvents(m_hwndConsole);  //see if this solves a crash problem
//            Sleep(visDelay);
//        }
//
//        score2 = m_pGameEngine->GetScore();
////        log2.open("InOut.csv", ios::out|ios::app);
////        log2 << endl << endl << endl;
////        log2.close();
//
//        m_pGameEngine->Reset();
//        m_BestBrainEver.Update();
//
//        while ( m_pGameEngine->Update(false) )
//        {
//            ;
//        }
//
//        score3 = m_pGameEngine->GetScore();
////        log2.open("InOut.csv", ios::out|ios::app);
////        log2 << endl << endl << endl;
////        log2.close();
//
//        log << "Best: " << bestever << " 1: " << score1 << " 4: " << score4 << " 2:" << score2 << " 3: " << score3 << endl;
//        log.flush();


//this block checked neural net inputs and outputs
//        if ( m_pGameEngine->GetScore() == bestever )
//        {
//            flag = true;
//        }
//        else
//        {
//            log2 << "GenomeID: " << m_BestBrainEver.GetGenomeID() << endl;
//            for (int w=0; w<=1; w++)
//            {
//                for (int x=0; x<=1; x++)
//                {
//                    for (int y=0; y<=1; y++)
//                    {
//                        for (int z=0; z<=1; z++)
//                        {
//                            inputs.clear();
//                            inputs.push_back(w);inputs.push_back(x);inputs.push_back(y);inputs.push_back(z);
//                            outputs = (m_BestBrainEver.m_pItsBrain)->Update(inputs, CNeuralNet::active);
//                            log2 << "Inputs: " << w << x << y << z << " -- " << outputs[0] << "," << outputs[1] << ","
//                                        << outputs[2] << "," << outputs[3] << endl;
//                        }
//                    }
//                }
//            }
//        log2 << endl << endl << endl;
//        log2.flush();
//        }
//        InvalidateRect(m_hwndConsole, NULL, TRUE);
//            UpdateWindow(m_hwndConsole);
//        log2 << "Score: " << m_pGameEngine->GetScore() << endl;
//        m_pPop->GetGenomeByID(m_BestBrainEver.GetGenomeID()).Write(log2);
//        log2 << endl << endl;;
//        log2.flush();

//    }
//    else
//    {
//        m_pGameEngine->Reset();
//        InvalidateRect(m_hwndConsole, NULL, TRUE);
//        UpdateWindow(m_hwndConsole);
//    }

//    log.close();
//    log2.close();
//	return true;
//}

//---------------------------------- RenderNetworks ----------------------
//
//  Renders the best four phenotypes from the previous generation
//------------------------------------------------------------------------
void CController::RenderNetworks(HDC &surface)
{
  if (m_iGenerations < 1)
  {
    return;
  }

  //draw the network of the best 4 genomes. First get the dimensions of the
   //info window
   RECT rect;
	GetClientRect(m_hwndInfo, &rect);

	int	cxInfo = rect.right;
	int	cyInfo = rect.bottom;

   //now draw the 4 best networks
   m_vecBestNets[0].DrawNet(surface, 0, cxInfo/2, cyInfo/2, 0);
   m_vecBestNets[1].DrawNet(surface, cxInfo/2, cxInfo, cyInfo/2, 0);
   m_vecBestNets[2].DrawNet(surface, 0, cxInfo/2, cyInfo, cyInfo/2);
   m_vecBestNets[3].DrawNet(surface, cxInfo/2, cxInfo, cyInfo, cyInfo/2);
}

//------------------------------------Render()--------------------------------------
//
//----------------------------------------------------------------------------------
void CController::Render(HDC &surface)
{
	//do not render if running at accelerated speed
	if (!m_bFastRender)
	{
    string s = "Generation: " + itos(m_iGenerations);
	  TextOut(surface, 5, 0, s.c_str(), s.size());

    //select in the blue pen
    m_OldPen = (HPEN)SelectObject(surface, m_BluePen);

//    if (m_bRenderBest)
//    {
//      //render the best sweepers memory cells
////      m_vecBestNets[m_iViewThisSweeper].Render(surface);
//
//      //render the best sweepers from the last generation
////      RenderSweepers(surface, m_vecBestNets);
//
//      //render the best sweepers sensors
////      RenderSensors(surface, m_vecBestNets);
//    }
//
//    else
//    {
//		  //render the sweepers
////      RenderSweepers(surface, m_vecSweepers);
//    }

    SelectObject(surface, m_OldPen);

    //render the objects
//    for (int i=0; i<NumObjectVerts; i+=2)
//    {
  //    MoveToEx(surface, m_ObjectsVB[i].x, m_ObjectsVB[i].y, NULL);
//
//      LineTo(surface, m_ObjectsVB[i+1].x, m_ObjectsVB[i+1].y);
//    }

	}//end if

  else
  {
    PlotStats(surface);

    RECT sr;
    sr.top    = m_cyClient-50;
    sr.bottom = m_cyClient;
    sr.left   = 0;
    sr.right  = m_cxClient;

    //render the species chart
    m_pPop->RenderSpeciesInfo(surface, sr);

  }

}
//------------------------- RenderSweepers -------------------------------
//
//  given a vector of sweepers this function renders them to the screen
//------------------------------------------------------------------------
//void CController::RenderSweepers(HDC &surface, vector<CMinesweeper> &sweepers)
//{
//  for (int i=0; i<sweepers.size(); ++i)
//	{
//
//    //if they have crashed into an obstacle draw
//    if ( sweepers[i].Collided())
//    {
//      SelectObject(surface, m_RedPen);
//    }
//
//    else
//    {
//      SelectObject(surface, m_BluePen);
//    }
//
//    //grab the sweeper vertices
//	  vector<SPoint> sweeperVB = m_SweeperVB;
//
//	  //transform the vertex buffer
//	  sweepers[i].WorldTransform(sweeperVB, sweepers[i].Scale());
//
//	  //draw the sweeper left track
//		MoveToEx(surface, (int)sweeperVB[0].x, (int)sweeperVB[0].y, NULL);
//
//		for (int vert=1; vert<4; ++vert)
//		{
//		  LineTo(surface, (int)sweeperVB[vert].x, (int)sweeperVB[vert].y);
//		}
//
//    LineTo(surface, (int)sweeperVB[0].x, (int)sweeperVB[0].y);
//
//    //draw the sweeper right track
//	  MoveToEx(surface, (int)sweeperVB[4].x, (int)sweeperVB[4].y, NULL);
//
//		for (vert=5; vert<8; ++vert)
//		{
//	    LineTo(surface, (int)sweeperVB[vert].x, (int)sweeperVB[vert].y);
//		}
//
//    LineTo(surface, (int)sweeperVB[4].x, (int)sweeperVB[4].y);
//
//    MoveToEx(surface, (int)sweeperVB[8].x, (int)sweeperVB[8].y, NULL);
//    LineTo(surface, (int)sweeperVB[9].x, (int)sweeperVB[9].y);
//
//    MoveToEx(surface, (int)sweeperVB[10].x, (int)sweeperVB[10].y, NULL);
//
//    for (vert=11; vert<16; ++vert)
//    {
//		   LineTo(surface, (int)sweeperVB[vert].x, (int)sweeperVB[vert].y);
//		}
//	}//next sweeper
//}
//
////----------------------------- RenderSensors ----------------------------
////
////  renders the sensors of a given vector of sweepers
////------------------------------------------------------------------------
//void CController::RenderSensors(HDC &surface, vector<CMinesweeper> &sweepers)
//{
//   //render the sensors
//    for (int i=0; i<sweepers.size(); ++i)
//    {
//      //grab each sweepers sensor data
//      vector<SPoint> tranSensors    = sweepers[i].Sensors();
//      vector<double> SensorReadings = sweepers[i].SensorReadings();
//      vector<double> MemoryReadings = sweepers[i].MemoryReadings();
//
//      for (int sr=0; sr<tranSensors.size(); ++sr)
//      {
//        if (SensorReadings[sr] > 0)
//        {
//          SelectObject(surface, m_RedPen);
//        }
//
//        else
//        {
//          SelectObject(surface, m_GreyPenDotted);
//        }
//
//        //make sure we clip the drawing of the sensors or we will get
//        //unwanted artifacts appearing
//        if (!((fabs(sweepers[i].Position().x - tranSensors[sr].x) >
//              (CParams::dSensorRange+1))||
//              (fabs(sweepers[i].Position().y - tranSensors[sr].y) >
//              (CParams::dSensorRange+1))))
//        {
//
//          MoveToEx(surface,
//                   (int)sweepers[i].Position().x,
//                   (int)sweepers[i].Position().y,
//                   NULL);
//
//          LineTo(surface, (int)tranSensors[sr].x, (int)tranSensors[sr].y);
//
//          //render the cell sensors
//          RECT rect;
//          rect.left  = (int)tranSensors[sr].x - 2;
//          rect.right = (int)tranSensors[sr].x + 2;
//          rect.top   = (int)tranSensors[sr].y - 2;
//          rect.bottom= (int)tranSensors[sr].y + 2;
//
//          if (MemoryReadings[sr] < 0)
//          {
//
//            FillRect(surface, &rect, m_BlueBrush);
//          }
//
//          else
//          {
//            FillRect(surface, &rect, m_RedBrush);
//          }
//
//        }
//      }
//    }
//}


//--------------------------PlotStats-------------------------------------
//
//  Given a surface to draw on this function displays some simple stats
//------------------------------------------------------------------------
void CController::PlotStats(HDC surface)const
{
    string s = "Generation:                 " + itos(m_iGenerations);
	  TextOut(surface, 5, 25, s.c_str(), s.size());

    s = "Num Species:             " + itos(m_pPop->NumSpecies());
	  TextOut(surface, 5, 45, s.c_str(), s.size());

    s = "Best Fitness so far:    " + ftos(m_pPop->BestEverFitness());
    TextOut(surface, 5, 5, s.c_str(), s.size());

    s = "Best Genome Score:   " + ftos(m_pPop->BestGenomeScore());
    TextOut(surface, 5, 65, s.c_str(), s.size());

    s = "Best Genome Links:   " + ftos(m_pPop->BestGenomeLinks());
    TextOut(surface, 5, 85, s.c_str(), s.size());

    s = "Best Genome Neurons: " + ftos(m_pPop->BestGenomeNeurons());
    TextOut(surface, 5, 105, s.c_str(), s.size());
}


//------------------------------- GetFitnessScores -----------------------
//
//  returns a std::vector containing the genomes fitness scores
//------------------------------------------------------------------------
vector<double> CController::GetFitnessScores()const
{
  vector<double> scores;

  for (int i=0; i<m_vecNeuralNets.size(); i++)
  {
     scores.push_back(m_vecNeuralNets[i].Fitness());
  }
  return scores;
}

vector<double> CController::GetGameScores()const
{
    vector<double> scores;
    for (int i=0; i<m_vecNeuralNets.size(); i++)
    {
        scores.push_back(m_vecNeuralNets[i].Score());
    }
    return scores;
}

void CController::DoEvents( HWND hWnd )
{
    MSG msg;
//    long sts;

    while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
        {
            Terminated = true;
            return;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
//    do
//    {
//        if (sts = PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE))
//        {
//            TranslateMessage(&msg);
//            DispatchMessage(&msg);
//        }
//    } while (sts);
}
