#include <sstream>
#include "CParams.h"

double CParams::dPi                       = 0;
double CParams::dHalfPi                   = 0;
double CParams::dTwoPi                    = 0;
int CParams::WindowWidth                  = 400;
int CParams::WindowHeight                 = 400;
int CParams::iFramesPerSecond             = 0;
int CParams::iNumInputs                   = 0;
int CParams::iNumOutputs                  = 0;
double CParams::dBias                     = -1;
double CParams::dMaxTurnRate              = 0;
int CParams::iSweeperScale                = 0;
int CParams::iNumSensors                  = 0;
double CParams::dSensorRange              = 0;
int CParams::iPopSize                     = 0;
int CParams::iNumTicks                    = 0;
double CParams::dCollisionDist            = 0;
double CParams::dCellSize                 = 0;
double CParams::dSigmoidResponse          = 1;
int CParams::iNumAddLinkAttempts          = 0;
int CParams::iNumTrysToFindLoopedLink     = 5;
int CParams::iNumTrysToFindOldLink        = 5;
double CParams::dYoungFitnessBonus        = 0;
int CParams::iYoungBonusAgeThreshhold     = 0;
double CParams::dSurvivalRate             = 0;
int CParams::InfoWindowWidth              = 400;
int CParams::InfoWindowHeight             = 400;
int CParams::iNumGensAllowedNoImprovement = 0;
int CParams::iMaxPermittedNeurons         = 0;
double CParams::dChanceAddLink            = 0;
double CParams::dChanceAddNode            = 0;
double CParams::dChanceAddRecurrentLink   = 0;
double CParams::dMutationRate             = 0;
double CParams::dMaxWeightPerturbation    = 0;
double CParams::dProbabilityWeightReplaced= 0;

double CParams::dActivationMutationRate   = 0;
double CParams::dMaxActivationPerturbation= 0;

double CParams::dCompatibilityThreshold   = 0;
int CParams::iNumBest                     = 4;
int CParams::iOldAgeThreshold             = 0;
double CParams::dOldAgePenalty            = 0;
double CParams::dCrossoverRate            = 0;
int CParams::iMaxNumberOfSpecies          = 0;

//this function loads in the parameters from a given file name. Returns
//false if there is a problem opening the file.
bool CParams::LoadInParameters(char* szFileName)
{
    //I've modifed the CParams file to use my singleton ParameterMap.
    //Here I'll extract the necessary parameters and feed them into CParams.
    //Defaults were left in place above, just in case I've missed something.
    ParameterMap m_Params;

    //extract the parameters into the CParam pieces
    dBias = m_Params.GetDoubleParam("Bias");
    iPopSize = m_Params.GetLongParam("PopSize");
    dSigmoidResponse = m_Params.GetDoubleParam("SigmoidResponse");
    iNumAddLinkAttempts = m_Params.GetLongParam("NumAddLinkAttempts");
    iNumTrysToFindLoopedLink = m_Params.GetLongParam("NumTrysToFindLoopedLink");
    iNumTrysToFindOldLink = m_Params.GetLongParam("NumTrysToFindOldLink");
    dYoungFitnessBonus = m_Params.GetDoubleParam("YoungFitnessBonus");
    iYoungBonusAgeThreshhold = m_Params.GetDoubleParam("YoungBonusAgeThreshhold");
    dSurvivalRate = m_Params.GetDoubleParam("SurvivalRate");
    iNumGensAllowedNoImprovement = m_Params.GetLongParam("NumGensAllowedNoImprovement");
    iMaxPermittedNeurons = m_Params.GetLongParam("MaxPermittedNeurons");
    dChanceAddLink = m_Params.GetDoubleParam("ChanceAddLink");
    dChanceAddNode = m_Params.GetDoubleParam("ChanceAddNode");
    dChanceAddRecurrentLink = m_Params.GetDoubleParam("ChanceAddRecurrentLink");
    dMutationRate = m_Params.GetDoubleParam("ChanceAddRecurrentLink");
    dMaxWeightPerturbation = m_Params.GetDoubleParam("MaxWeightPerturbation");
    dProbabilityWeightReplaced = m_Params.GetDoubleParam("ProbabilityWeightReplaced");
    dActivationMutationRate = m_Params.GetDoubleParam("ActivationMutationRate");
    dMaxActivationPerturbation = m_Params.GetDoubleParam("MaxActivationPerturbation");
    dCompatibilityThreshold = m_Params.GetDoubleParam("CompatibilityThreshold");
    iNumBest = m_Params.GetLongParam("NumBest");
    iOldAgeThreshold = m_Params.GetLongParam("OldAgeThreshold");
    dOldAgePenalty = m_Params.GetDoubleParam("OldAgePenalty");
    dCrossoverRate = m_Params.GetDoubleParam("CrossoverRate");
    iMaxNumberOfSpecies = m_Params.GetLongParam("MaxNumberOfSpecies");
    iNumOutputs = m_Params.GetLongParam("NumOutputs");

    //count up the number of inputs
    iNumInputs = 0;
    long multiplier;
    long radius;
    long maxScore;

    if ( m_Params.GetStringParam("AIType") == "Windowed" )
    {
        //establish the number of tiles in the input field
        radius = m_Params.GetLongParam("WindowRadius");
        iNumInputs = (radius*2+2) * (radius*2+2) - 4 ;  //2 cols/rows for PacMan + Radius on each side minus PacMan himself

        if ( m_Params.GetLongParam("DotsWallsOnly") == 0 ) //monitor both
        {
            if ( m_Params.GetLongParam("DotWallSingleInput") == 0 )
                multiplier = 2;  //one input set for dots, one for wall
            else
                multiplier = 1;  //dot and wall are combined
        }
        else
            multiplier = 1;  //single input or multiple input, either dots/walls ignored, so only 1 set

        if ( m_Params.GetLongParam("PelletInputs") !=0 )
            multiplier++;

        //multiplier determines how many inputs per tile
        //initially, each tile gives 1 input for dot, wall, and power pellet
        if ( m_Params.GetLongParam("NumberOfGhosts") > 0 )
        {
            if ( m_Params.GetLongParam("SingleGhostInput") == 1 )
                multiplier +=1;  //one input contains -1 for hunting, 0 for nothing/dead, 1 for blue
            else
                multiplier +=2;//for each ghost, a hunting and blue bit is added
        }

        iNumInputs *= multiplier;
    }
    else if (m_Params.GetStringParam("AIType") == "Global" )
    {
        if ( m_Params.GetLongParam("GhostXY") == 1 )
            iNumInputs += (2*m_Params.GetLongParam("NumberOfGhosts"));

        if ( m_Params.GetLongParam("GhostState") == 1 )
            iNumInputs += (m_Params.GetLongParam("NumberOfGhosts"));

        if ( m_Params.GetLongParam("PowerUpXY") == 1 )
            iNumInputs += 8;

        if ( m_Params.GetLongParam("PowerUpState") == 1 )
            iNumInputs += 4;

        if ( m_Params.GetLongParam("PacManXY") == 1 )
            iNumInputs += 2;

        if ( m_Params.GetLongParam("CheckForWalls") == 1 )
            iNumInputs += 4;

        if ( m_Params.GetLongParam("CheckDots") == 1 )
            iNumInputs += 4;

        if ( m_Params.GetLongParam("MaxDotDirection") == 1 )
            iNumInputs+=4;
    }
    else if (m_Params.GetStringParam("AIType") == "Vector")
    {
        if ( m_Params.GetLongParam("DotsWallsOnly") == 0 ||
             m_Params.GetLongParam("DotsWallsOnly") == -1 )
        {
            radius = m_Params.GetLongParam("WindowRadius");
            iNumInputs += (radius*2+2) * (radius*2+2) - 4 ;  //2 cols/rows for PacMan + Radius on each side minus PacMan himself
        }

        if ( m_Params.GetLongParam("DotsWallsOnly") == 0 ||
             m_Params.GetLongParam("DotsWallsOnly") == 1 )
             iNumInputs += 2;

        if ( m_Params.GetLongParam("PelletInputs") == 1 )
            iNumInputs += 8;

        iNumInputs += (3*m_Params.GetLongParam("NumberOfGhosts"));
    }
    else if (m_Params.GetStringParam("AIType") == "Console")
    {
        if (m_Params.GetStringParam("CompositeScheme") == "AllLayers")
        {
            iNumInputs = 4640;
        }
        else
        {
            iNumInputs = 1856;
        }
    }

    std::ostringstream inputParam;
    inputParam << iNumInputs;
    m_Params.SetParam("NumberOfInputs",inputParam.str());

    //compute the maximum possible score given the parameters
    maxScore = 3040; //standard - 284 dots * 10 points each + 4 Power Ups * 50 points each
    maxScore += 3000 * m_Params.GetLongParam("NumberOfGhosts"); //ghosts are 200, 400, 800, 1600 for each power up
    inputParam.str("");
    inputParam << maxScore;
    m_Params.SetParam("MaxScore", inputParam.str());

//  ifstream grab(szFileName);
//
//  check file exists
//  if (!grab)
//  {
//    return false;
//  }
//
//  load in from the file
//  char ParamDescription[40];
//
//  grab >> ParamDescription;
//  grab >> iFramesPerSecond;
//  grab >> ParamDescription;
//  grab >> dMaxTurnRate;
//  grab >> ParamDescription;
//  grab >> iSweeperScale;
//  grab >> ParamDescription;
//  grab >> iNumSensors;
//  grab >> ParamDescription;
//  grab >> dSensorRange;
//  grab >> ParamDescription;
//  grab >> iPopSize;
//  grab >> ParamDescription;
//  grab >> iNumTicks;
//  grab >> ParamDescription;
//  grab >> dCellSize;
//  grab >> ParamDescription;
//  grab >> iNumAddLinkAttempts;
//  grab >> ParamDescription;
//  grab >> dSurvivalRate;
//  grab >> ParamDescription;
//  grab >> iNumGensAllowedNoImprovement;
//  grab >> ParamDescription;
//  grab >> iMaxPermittedNeurons;
//  grab >> ParamDescription;
//  grab >> dChanceAddLink;
//  grab >> ParamDescription;
//  grab >> dChanceAddNode;
//  grab >> ParamDescription;
//  grab >> dChanceAddRecurrentLink;
//  grab >> ParamDescription;
//  grab >> dMutationRate;
//  grab >> ParamDescription;
//  grab >> dMaxWeightPerturbation;
//  grab >> ParamDescription;
//  grab >> dProbabilityWeightReplaced;
//  grab >> ParamDescription;
//  grab >> dActivationMutationRate;
//  grab >> ParamDescription;
//  grab >> dMaxActivationPerturbation;
//  grab >> ParamDescription;
//  grab >> dCompatibilityThreshold;
//  grab >> ParamDescription;
//  grab >>iOldAgeThreshold;
//  grab >> ParamDescription;
//  grab >>dOldAgePenalty;
//  grab >> ParamDescription;
//  grab >> dYoungFitnessBonus;
//  grab >> ParamDescription;
//  grab >> iYoungBonusAgeThreshhold;
//  grab >> ParamDescription;
//  grab >>dCrossoverRate;
//  grab >> ParamDescription;
//  grab >> iMaxNumberOfSpecies;

  return true;
}
