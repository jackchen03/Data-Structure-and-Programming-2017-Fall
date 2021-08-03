/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include "myHashMap.h"
#include<ctime>
#define SIM_HASH_SIZE 59 


using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

#include "cirDef.h"

extern CirMgr *cirMgr;

class CirMgr
{
public:
   CirMgr() { _miloa[2] = 0; _gateList = 0; _simHash.init(SIM_HASH_SIZE);  srand(time(0));}
   ~CirMgr() {  if(!_gateList)  delete[]_gateList;
   } 

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const { 
      if(gid >= _listSize)
   	      return 0;
	  else if(_gateList[gid] == 0)
   	      return 0;
      else return _gateList[gid];
    }
    
    unsigned getMiloa(int i){
   	if(i<5 && i>=0)
   	   return _miloa[i];
   	else  return 0;
    }

   // Member functions about circuit construction
   bool readCircuit(const string&);

   // Member functions about circuit optimization
   void sweep();
   void optimize();

   // Member functions about simulation
   void randomSim();
   void fileSim(ifstream&);
  // void ColSim(vector<ValueList>& );
   void setSimLog(ofstream *logFile) { _simLog = logFile; }
   
   // Member funtions about simulation (on my own)
   size_t genRandSim(int l);

   // Member functions about fraig
   void strash();
   void printFEC() const;
   void fraig();

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void printFECPairs() const;
   void writeAag(ostream&) const;
   void writeGate(ostream&, CirGate*) const;
   
   //member functions about Hash
   void queryHash(HashKey _key, GateList g) const { _simHash.query(_key, g);  }   
   
   // member functions about FEC Groups
   GateList& getFecGrps(size_t i){ return _fecGrps.at(i);   }
   size_t numFecGrps(){ return _fecGrps.size();   }
   
   //member function about fraig
   void genProofModel(SatSolver& s);

private:
	//help function
	void genDFSList();
   ofstream  *_simLog;
   unsigned _miloa[5];
   CirGate** _gateList;
   unsigned   _listSize;
   IdList     _piList;
   GateList   _DFSList; 
   HashMap<HashKey, GateList> _simHash; 
   vector<GateList> _fecGrps;

};

#endif // CIR_MGR_H
