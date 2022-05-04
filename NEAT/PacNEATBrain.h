/*	PacNEATBrain.h
	Robert Kirk DeLisle
	4 March 2007

	Purpose:  Define a NEAT and AIPac compatible neural net/brain interface

	Modificaton History:

	Originally from Mat Bucland's MineSweeper's project:
//------------------------------------------------------------------------
//
//	Name: CMineSweeper.h
//
//  Author: Mat Buckland 2002 (fup@btinternet.com)
//
//  Desc: Class to create a minesweeper object
//
//------------------------------------------------------------------------

*/

#if !defined(PacNEATBrain_04Mar07_RKD)
#define PacNEATBrain_04Mar07_RKD

#include "../IBrain.h"
#include "../WorldController.h"
#include "../Enumerations.h"
#include <windows.h>

#include <vector>
#include <algorithm>
#include <math.h>

#include "phenotype.h"
#include "utils.h"
#include "CParams.h"
#include "collision.h"

using namespace std;

class PacNEATBrain : public IBrain
{

private:



	//the sweepers fitness score.
	double			m_dFitness;
    double          m_dScore; //what score did the brain get in the game?

	WorldController *m_pWorld; //used to access details about the world at large

public:

    CNeuralNet*  m_pItsBrain;

    int GetGenomeID()
    {
        return m_pItsBrain->myDNA_ID;
    }

    PacNEATBrain();

    void SetWorld(WorldController *wrld)
    {  m_pWorld = wrld;  }

	//updates the ANN with information from the sweepers environment
	bool Update();

	void Reset();

    void EndOfRunCalculations(vector<long> &scores);

    void DrawNet(HDC &surface, int cxLeft, int cxRight, int cyTop, int cyBot)
    {   m_pItsBrain->DrawNet(surface, cxLeft, cxRight, cyTop, cyBot);   }

	float Fitness()const{return m_dFitness;}
    float Score()const{return m_dScore;}
    void InsertNewBrain(CNeuralNet* brain){m_pItsBrain = brain;}

    virtual long GetNextDirection(long &State, long &XSpeed, long &YSpeed, bool loggit);

    vector<double> GetInputs();

    vector<double> GetWindowInputs(long WindowRadius);

    vector<double> GetVectorInputs(long WindowRadius);

    vector<double> GetConsoleInputs();

};


#endif
