#ifndef CGA_H
#define	CGA_H

//------------------------------------------------------------------------
//
//	Name: Cga.h
//
//  Author: Mat Buckland 2002
//
//  Desc: The evolutionary algorithm class  used in the implementation of
//        Kenneth Owen Stanley's and Risto Miikkulainen's NEAT idea.
//
//------------------------------------------------------------------------
#include <vector>
#include <windows.h>
#include <fstream>

#include "phenotype.h"
#include "genotype.h"
#include "CSpecies.h"
#include "CParams.h"

using namespace std;



//------------------------------------------------------------------------
//
//  this structure is used in the creation of a network depth lookup
//  table.
//------------------------------------------------------------------------
struct SplitDepth
{
  double val;

  int    depth;

  SplitDepth(double v, int d):val(v), depth(d){}
};


//------------------------------------------------------------------------
//
//------------------------------------------------------------------------
class Cga
{

private:

	//current population
	vector<CGenome>   m_vecGenomes;

	//keep a record of the last generations best genomes. (used to render
  //the best performers to the display if the user presses the 'B' key)
	vector<CGenome>   m_vecBestGenomes;

	CGenome m_BestEverGenome; //the best genome ever found

	//all the species
  vector<CSpecies>  m_vecSpecies;

	//to keep track of innovations
  CInnovation*      m_pInnovation;

	//current generation
  int               m_iGeneration;

	int               m_iNextGenomeID;

  int               m_iNextSpeciesID;

	int               m_iPopSize;

  //adjusted fitness scores
  double            m_dTotFitAdj,
                    m_dAvFitAdj;

  //index into the genomes for the fittest genome
  int               m_iFittestGenome;

  double            m_dBestEverFitness;
  double            m_dBestGenomeScore;
  int               m_iBestGenomeLinks;
  int               m_iBestGenomeNeurons;

  //this holds the precalculated split depths. They are used
  //to calculate a neurons x/y position for rendering and also
  //for calculating the flush depth of the network when a
  //phenotype is working in 'snapshot' mode.
  vector<SplitDepth> vecSplits;


	//used in Crossover
	void    AddNeuronID(int nodeID, vector<int> &vec);

  //this function resets some values ready for the next epoch, kills off
  //all the phenotypes and any poorly performing species.
  void    ResetAndKill();

  //separates each individual into its respective species by calculating
  //a compatibility score with every other member of the population and
  //niching accordingly. The function then adjusts the fitness scores of
  //each individual by species age and by sharing and also determines
  //how many offspring each individual should spawn.
  void    SpeciateAndCalculateSpawnLevels();

  //adjusts the fitness scores depending on the number sharing the
  //species and the age of the species.
  void    AdjustSpeciesFitnesses();

  CGenome Crossover(CGenome& mum, CGenome& dad);

  CGenome TournamentSelection(const int NumComparisons);

  //searches the lookup table for the dSplitY value of each node in the
  //genome and returns the depth of the network based on this figure
  void    CalculateNetDepth(CGenome &gen);

  //sorts the population into descending fitness, keeps a record of the
  //best n genomes and updates any fitness statistics accordingly
  void    SortAndRecord();

  //this automatically adjusts the compatibility threshold in an attempt
  //to keep the number of species at a constant value
  void    AdjustCompatibilityThreshold();

  //a recursive function used to calculate a lookup table of split
  //depths
  vector<SplitDepth> Split(double low, double high, int depth);


public:

	//normal constructor. Creates a population starting with minimal
  //genomes consisting of specified number of inputs and outputs.
  Cga(int   size,
      int   inputs,
      int   outputs);


	~Cga();

  //create a population from a genome defined in a data file
  bool                 CreatePopulationFromGenome(const char* szFileName);

	vector<CNeuralNet*>	 Epoch(const vector<double> &FitnessScores,
                               const vector<double> &GameScores);

	//iterates through the population and creates the phenotypes
  vector<CNeuralNet*>  CreatePhenotypes();

  //keeps a record of the n best genomes from the last population.
  //(used to display the best genomes)
  void                 StoreBestGenomes();

  //renders the best performing species statistics and a visual aid
  //showing the distribution.
  void                 RenderSpeciesInfo(HDC &surface, RECT db);

  //returns a vector of the n best phenotypes from the previous generation
  vector<CNeuralNet*>  GetBestPhenotypesFromLastGeneration();

  //writes the entire state of the GA to a file
  bool                 WriteState(char* szFileName);

  //given an index into m_vecGenomes, writes a single genome to a file
  bool                 WriteGenome(const char* szFileName, const int idxGenome);

  //writes the species info to a file
  bool                 SpeciesDump(const char* szFileName);


  //----------------------------------------------------accessor methods
  int     NumSpecies()const{return m_vecSpecies.size();}
  double  BestEverFitness()const{return m_dBestEverFitness;}
  double  BestGenomeScore()const{return m_dBestGenomeScore;}
  int     BestGenomeLinks()const{return m_iBestGenomeLinks;}
  int     BestGenomeNeurons()const{return m_iBestGenomeNeurons;}
  void    SetFitness(int genome, double fitness){m_vecGenomes[genome].SetFitness(fitness);}
  int     Generation(){return m_iGeneration;}

	CNeuralNet* GetBestPhenotypeEver();  //get the best genome ever found

	CGenome GetGenomeByID(long ID)
	{
	    int i;
	    for (i=0; i<m_vecGenomes.size(); i++)
        {
            if ( m_vecGenomes.at(i).ID() == ID )
            {
                return m_vecGenomes.at(i);
            }
        }
        return CGenome();

	}
};


#endif
