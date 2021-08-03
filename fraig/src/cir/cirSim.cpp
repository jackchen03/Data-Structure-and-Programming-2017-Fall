/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir simulation functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"
#define SIZE_T 64
#include "myHashMap.h"
#include<cstdlib>
#include<climits>
#include "cirDef.h"

using namespace std;

// TODO: Keep "CirMgr::randimSim()" and "CirMgr::fileSim()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/************************************************/
/*   Public member functions about Simulation   */
/************************************************/

bool comp_gate(CirGate* p, CirGate* q){ return p->getId() < q->getId();}
bool comp_list(GateList & a, GateList& b){ return a.at(0)->getId() < b.at(0)->getId();}

void
CirMgr::randomSim()
{
	GateList fecGrp;
	for(size_t i=0;i<_DFSList.size();i++){
		if(_DFSList.at(i)->getGateType() == AIG_GATE ) fecGrp.push_back(_DFSList.at(i));
	}
	fecGrp.push_back(_gateList[0]);
	_fecGrps.push_back(fecGrp);
	
	int sim_rep = 0 ;
	int limit ;
	if((_miloa[1]+_miloa[4]/200) < 10 ) limit = (_miloa[1]+_miloa[4]/200);
	else if((_miloa[1]+_miloa[4]/200) > 10 && (_miloa[1]+_miloa[4]/200) < 50)  limit = (_miloa[1]+_miloa[4])/2;
	else if((_miloa[1]+_miloa[4]/200) > 50 && (_miloa[1]+_miloa[4]/200) < 200)  limit = (_miloa[1]+_miloa[4]/200) /3;
	else if((_miloa[1]+_miloa[4]/200) > 200 && (_miloa[1]+_miloa[4]/200) < 600)  limit = (_miloa[1]+_miloa[4]/200) /5;
	else if((_miloa[1]+_miloa[4]/200) > 600 && (_miloa[1]+_miloa[4]/200) < 1000)  limit = (_miloa[1]+_miloa[4]/200) /10;
	else if((_miloa[1]+_miloa[4]/200) > 1000 && (_miloa[1]+_miloa[4]/200) < 5000)  limit = (_miloa[1]+_miloa[4]/200) /18;
	else if((_miloa[1]+_miloa[4]/200) > 5000 && (_miloa[1]+_miloa[4]/200) < 10000)  limit = (_miloa[1]+_miloa[4]/200) /35;
    else if((_miloa[1]+_miloa[4]/200) > 10000 )  limit = (_miloa[1]+_miloa[4]/200) /60;

	int small_diff_count = 0;
	
	while(1){   // one loop per simulation	    
		for(size_t i=0;i<_miloa[1];i++){   // for each input, generate a pattern word(64bit)
			_gateList[_piList.at(i)]->addSimResult(genRandSim(SIZE_T));
		}
		_gateList[0]->addSimResult(0);
		sim_rep++; // simulate one more time
		
		for(size_t i=0;i<_DFSList.size();i++){  // iteration along the DFSList  (value simulation)
			if(_DFSList.at(i)->getGateType() == AIG_GATE || _DFSList.at(i)->getGateType() == UNDEF_GATE ){
				size_t p = _DFSList.at(i)->getFanIn(0).isInv() ? (~_DFSList.at(i)->getFanIn(0).getGate()->getSimResult()) : _DFSList.at(i)->getFanIn(0).getGate()->getSimResult();
				
				size_t q = _DFSList.at(i)->getFanIn(1).isInv() ? (~_DFSList.at(i)->getFanIn(1).getGate()->getSimResult()) : _DFSList.at(i)->getFanIn(1).getGate()->getSimResult();
				size_t k = p & q;
				_DFSList.at(i)->addSimResult(k);
			}
			else if(_DFSList.at(i)->getGateType() == PO_GATE){
				size_t p = _DFSList.at(i)->getFanIn(0).isInv() ? (~_DFSList.at(i)->getFanIn(0).getGate()->getSimResult()) : _DFSList.at(i)->getFanIn(0).getGate()->getSimResult();
				_DFSList.at(i)->addSimResult(p);
			}
		}		
		size_t orgFecNum =  _fecGrps.size();  // original FEC number

		
        vector<GateList> tmpFecGrps;   // temporary for storage, later assign to the one in CirMgr
	//	cout << _fecGrps.size() << endl;
		for(size_t i=0;i<_fecGrps.size();i++){ // for each FEC group
			for(size_t j=0;j<_fecGrps.at(i).size();j++){   // for each gate in one FEC group, check if they are still in the same group ( PUT IN HASH)
				HashKey _key = HashKey(_fecGrps.at(i).at(j)->getSimResult(), ~(_fecGrps.at(i).at(j)->getSimResult())) ;				
	            GateList g;
	            if(_simHash.query(_key, g)){
	            	g.push_back(_fecGrps.at(i).at(j));
					_simHash.update(_key, g);
				}
				else{
					g.push_back(_fecGrps.at(i).at(j));
					_simHash.insert(_key, g);
				}
			}
			
			for(size_t i=0;i<SIM_HASH_SIZE;i++){  // collect FEC GROUPS in HASH 
			  if(_simHash[i].size())
			    for(size_t j=0;j<_simHash[i].size();j++){
				    if(_simHash[i].at(j).second.size() > 1) tmpFecGrps.push_back(_simHash[i].at(j).second);
			    }
		    }
			_simHash.clear();
		}
		
		_fecGrps.clear();
        _fecGrps = tmpFecGrps;
		
		if(_simLog)
		for(size_t i=0;i<SIZE_T;i++){   // iteration for the length of a word
		   for(size_t j=0;j<_miloa[1];j++){
			   if(_gateList[_piList.at(j)]-> getSimResult() >> i & (size_t)(1) == 1) (*_simLog) << "1";
			   else (*_simLog) << "0";
		   }
		   (*_simLog) << " ";
		   for(size_t j=_miloa[0]+1;j<_listSize;j++){  
			   if(_gateList[j]->getSimResult() >> i & (size_t)(1) == 1) (*_simLog) << "1";
			   else (*_simLog) << "0";
		   }   
		   (*_simLog)<< endl;
	   }

		
		size_t diff_num_fec = orgFecNum > _fecGrps.size() ? orgFecNum - _fecGrps.size() :  _fecGrps.size() - orgFecNum ;
		if(diff_num_fec  < 2 || diff_num_fec < (limit/1000)) small_diff_count++ ;  // handling iteration times	
		else small_diff_count = 0;	
		if( small_diff_count > 8 || sim_rep > limit){
			if(small_diff_count > 8) cout << "small!!" << endl;
			else if(sim_rep > limit) cout << "limit!!" << endl;
			 break;
		}
		
    }
	cout << 64*sim_rep << " patterns simulated." << endl;
	
	for(size_t i=0;i<_fecGrps.size();i++){   // sort _fecGrps
		std::sort(_fecGrps.at(i).begin(), _fecGrps.at(i).end(), comp_gate);
	}
	std::sort(_fecGrps.begin(), _fecGrps.end(), comp_list);

	
	
}

size_t CirMgr::genRandSim(int l){  //  l = pattern length
	
	size_t sum = 0;
    sum+= (rand() << 32);
    sum += rand();
	return sum;
}
void
CirMgr::fileSim(ifstream& patternFile)
{
	GateList fecGrp;
	for(size_t i=0;i<_DFSList.size();i++){
		if(_DFSList.at(i)->getGateType() == AIG_GATE ) fecGrp.push_back(_DFSList.at(i));
	}
	fecGrp.push_back(_gateList[0]);
	_fecGrps.push_back(fecGrp);
   int sim_rep =0;	 
   
   while(true){   // whole simulation loop
   	  bool errPattern = 0; 
   	  bool lastLoop = 0;
   	  vector<string> ptn;
   	  for(size_t i=0;i<64;i++){   // per simulation (64bits), parse pattern from file & store it in an vector 
   	     string tmp;
   	     getline(patternFile, tmp);
   	     ptn.push_back(tmp);
   	     if(patternFile.eof()){  // check if reach end of file
			 lastLoop = 1;
			 string zero;
		 	 for(size_t j=0;j<_miloa[1];j++) zero += "0";
		 	 ptn.pop_back();
		 	 for(size_t j=i;j<64;j++)  ptn.push_back(zero); 
			 break;
		 }   

   	     if(ptn.at(i).size() != _miloa[1]) {     // checking length match
	        cout << "Error: Pattern(" << ptn.at(i) << ") length(" << ptn.at(i).size() << ") does not match the number of inputs(" << _miloa[1] << ") in a circuit!!" << endl;
	        errPattern = 1;
	        break;
		 }
		 if(errPattern) break;		 
      }
     if(errPattern) break;   // if pattern type error, no further simulation
      
      for(size_t i=0;i<_miloa[1];i++){   // create simulation value of input from the pattern stored
      	 size_t sim = 0;  // for every input
      	 for(size_t j=0;j<64;j++){
      	 	 if(ptn.at(j).at(i) == '1') sim += ((size_t)(1) << j);
			 else if(ptn.at(j).at(i) != '0'){
			 	cout << "Error: Pattern( "<< ptn.at(j) << ") contains a non-0/1 character("<< "'" << ptn.at(j).at(i) <<"').";
			 	errPattern = 1;
	            break;
			 }	  
		 }
		 _gateList[_piList.at(i)]->addSimResult(sim);
		 if(errPattern) break;
	  }
	  if(errPattern) break;   // if pattern type error, no further simulation
	  
	  for(size_t i=0;i<_DFSList.size();i++){  // iteration along the DFSList  (value simulation)
			if(_DFSList.at(i)->getGateType() == AIG_GATE ){
				size_t p = _DFSList.at(i)->getFanIn(0).isInv() ? (~_DFSList.at(i)->getFanIn(0).getGate()->getSimResult()) : _DFSList.at(i)->getFanIn(0).getGate()->getSimResult();
				
				size_t q = _DFSList.at(i)->getFanIn(1).isInv() ? (~_DFSList.at(i)->getFanIn(1).getGate()->getSimResult()) : _DFSList.at(i)->getFanIn(1).getGate()->getSimResult();
				size_t k = p & q;
				_DFSList.at(i)->addSimResult(k);
			}
			else if(_DFSList.at(i)->getGateType() == UNDEF_GATE){
				_DFSList.at(i)->addSimResult((size_t)(0));
			}
			else if(_DFSList.at(i)->getGateType() == PO_GATE){
				size_t p = _DFSList.at(i)->getFanIn(0).isInv() ? (~_DFSList.at(i)->getFanIn(0).getGate()->getSimResult()) : _DFSList.at(i)->getFanIn(0).getGate()->getSimResult();
				_DFSList.at(i)->addSimResult(p);
			}
	   }
	   _gateList[0]->addSimResult(0); // const gate has the value 0	
	   
	   sim_rep++;
	   
	    vector<GateList> tmpFecGrps;   // temporary for storage, later assign to the one in CirMgr
	
		for(size_t i=0;i<_fecGrps.size();i++){ // for each FEC group
			for(size_t j=0;j<_fecGrps.at(i).size();j++){   // for each gate in one FEC group, check if they are still in the same group ( PUT IN HASH)
				HashKey _key = HashKey(_fecGrps.at(i).at(j)->getSimResult(), ~(_fecGrps.at(i).at(j)->getSimResult())) ;				
	            GateList g;
	            if(_simHash.query(_key, g)){
	            	g.push_back(_fecGrps.at(i).at(j));
					_simHash.update(_key, g);
				}
				else{
					g.push_back(_fecGrps.at(i).at(j));
					_simHash.insert(_key, g);
				}
			}
			
			for(size_t i=0;i<SIM_HASH_SIZE;i++){  // collect FEC GROUPS in HASH 
			  if(_simHash[i].size())
			    for(size_t j=0;j<_simHash[i].size();j++){
				    if(_simHash[i].at(j).second.size() > 1) tmpFecGrps.push_back(_simHash[i].at(j).second);
			    }
		    }
			_simHash.clear();
		}
		
		_fecGrps.clear();
        _fecGrps = tmpFecGrps;
        
        for(size_t i=0;i<SIZE_T;i++){   //  Handling logfile  (iteration for the length of a word 
		   for(size_t j=0;j<_miloa[1];j++){
			   if(_gateList[_piList.at(j)]-> getSimResult() >> i & (size_t)(1) == 1) (*_simLog) << "1";
			   else (*_simLog) << "0";
		   }
		   (*_simLog) << " ";
		   for(size_t j=_miloa[0]+1;j<_listSize;j++){  
			   if(_gateList[j]->getSimResult() >> i & (size_t)(1) == 1) (*_simLog) << "1";
			   else (*_simLog) << "0";
		   }   
		   (*_simLog)<< endl;
	   }
        
        if(lastLoop == 1) break;  // if end of file -> break;        
   }	
       cout << 64*sim_rep << " patterns simulated." << endl;
	
	   for(size_t i=0;i<_fecGrps.size();i++){   // sort _fecGrps
		   std::sort(_fecGrps.at(i).begin(), _fecGrps.at(i).end(), comp_gate);
	   }
	   std::sort(_fecGrps.begin(), _fecGrps.end(), comp_list);
   
}







/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/
