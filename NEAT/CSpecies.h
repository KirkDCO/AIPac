#ifndef CSPECIES_H
#define CSPECIES_H
//-----------------------------------------------------------------------
//
//  Name: CSpecies.h
//
//  Author: Mat Buckland 2002
//
//	Desc: speciazation class used in the implementation of Kenneth Owen
//        Stanley's and Risto Miikkulainen's NEAT idea.
//
//-----------------------------------------------------------------------
#include <vector>
#include <math.h>
#include <iomanip>
#include <iostream>

#include "genotype.h"

using namespace std;


//------------------------------------------------------------------------
//
//  class to hold all the genomes of a given species
//------------------------------------------------------------------------
class CSpecies
{

private:

  //keep a local copy of the first member of this species
  CGenome           m_Leader;

  //pointers to all the genomes within this species
  vector<CGenome*>  m_vecMembers;

  //the species needs an identification number
  int               m_iSpeciesID;

  //best fitness found so far by this species
  double            m_dBestFitness;

  //generations since fitness has improved, we can use
  //this info to kill off a species if required
  int               m_iGensNoImprovement;

  //age of species
  int               m_iAge;

  //how many of this species should be spawned for
  //the next population
  double            m_dSpawnsRqd;


public:

  CSpecies(CGenome &FirstOrg, int SpeciesID);

  //this method boosts the fitnesses of the young, penalizes the
  //fitnesses of the old and then performs fitness sharing over
  //all the members of the species
  void    AdjustFitnesses();

  //adds a new individual to the species
  void    AddMember(CGenome& new_org);

  void    Purge();

  //calculates how many offspring this species should
  void    CalculateSpawnAmount();

  //spawns an individual from the species selected at random
  //from the best CParams::dSurvivalRate percent
  CGenome Spawn();


  //--------------------------------------accessor methods
  CGenome  Leader()const{return m_Leader;}

  double   NumToSpawn()const{return m_dSpawnsRqd;}

  int      NumMembers()const{return m_vecMembers.size();}

  int      GensNoImprovement()const{return m_iGensNoImprovement;}

  int      ID()const{return m_iSpeciesID;}

  double   SpeciesLeaderFitness()const{return m_Leader.Fitness();}

  double   BestFitness()const{return m_dBestFitness;}

  int      Age()const{return m_iAge;}

  vector<int> GetGenomeIDs();
    //added by R.K. DeLisle in order to facilitate deleting genomes associated with a particular species
    //31 Jan 2008

  //so we can sort species by best fitness. Largest first
  friend bool operator<(const CSpecies &lhs, const CSpecies &rhs)
  {
    return lhs.m_dBestFitness > rhs.m_dBestFitness;
  }

  //overload << for streaming
  friend ostream& operator<<(ostream &os, const CSpecies &rhs)
  {
    os << "\n\n------------------------------ Species " << rhs.m_iSpeciesID << " --------------------------------"
       << "\nAge: " << rhs.m_iAge << "  Gens No improvement: " << rhs.m_iGensNoImprovement
       << " Leader ID: " << rhs.m_Leader.ID();

    os << "\n\nBestFitness: " << rhs.m_dBestFitness << "\n\nMembers\n";

    for (int i=0; i<rhs.m_vecMembers.size(); i++)
    {
      os << "\nGenomeID: " << setw(3) << rhs.m_vecMembers[i]->ID()
         << "  Fitness: "  << setw(3) << rhs.m_vecMembers[i]->Fitness()
         << "  Adjusted: " << setw(3) << rhs.m_vecMembers[i]->GetAdjFitness()
         << "  Spawn: "    << setw(3) << rhs.m_vecMembers[i]->AmountToSpawn();

    }

    os << "\n\nAmount to spawn: " << rhs.m_dSpawnsRqd;

    return os;
  }
};

#endif
