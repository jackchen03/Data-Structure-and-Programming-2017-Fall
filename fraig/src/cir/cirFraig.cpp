/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"
#include "myHashMap.h"
#include "util.h"
#include "cirDef.h"

using namespace std;

// TODO: Please keep "CirMgr::strash()" and "CirMgr::fraig()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/*******************************************/
/*   Public member functions about fraig   */
/*******************************************/
// _floatList may be changed.
// _unusedList and _undefList won't be changed

void
CirMgr::strash()
{
	HashMap<HashKey, CirGate*>* _hashMap = new HashMap<HashKey, CirGate*>(HASH_SIZE);
	for(size_t i=0;i<_DFSList.size();i++){
		HashKey _key = HashKey(_DFSList.at(i)->getFanIn(0).getGateV(), _DFSList.at(i)->getFanIn(1).getGateV());
		
		if(!_hashMap->insert(_key, _DFSList.at(i))){
			CirGate* g;
			_hashMap->query(_key, g);
			if(g->getFanIn(0) == _DFSList.at(i)->getFanIn(0) && g->getFanIn(1) == _DFSList.at(i)->getFanIn(1) || g->getFanIn(0) == _DFSList.at(i)->getFanIn(1) && g->getFanIn(1) == _DFSList.at(i)->getFanIn(0)){
				for(size_t k=0;k<2;k++){   // deleting fanin's fanout
					size_t j=0;
				    for(;j<_DFSList.at(i)->getFanIn(k).getGate()->getFanOutSize();j++) if(_DFSList.at(i)->getFanIn(k).getGate()->getFanOut(j).getGate() == _DFSList.at(i)) break;
				    _DFSList.at(i)->getFanIn(k).getGate()->delFanOut(j);
				}
				for(size_t j=0;j<_DFSList.at(i)->getFanOutSize();j++){ // handling fanout's fanin (replace with equivalent gate)
					for(size_t k=0;k<_DFSList.at(i)->getFanOut(j).getGate()->getFanInSize();k++){
				        if(_DFSList.at(i)->getFanOut(j).getGate()->getFanIn(k).getGate() == _DFSList.at(i))
						   _DFSList.at(i)->getFanOut(j).getGate()->setFanIn(k, CirGateV(g ,_DFSList.at(i)->getFanOut(j).getGate()->getFanIn(k).isInv()));       
					}
					g->pushFanOut(_DFSList.at(i)->getFanOut(j));
				}
				cout << "Strashing: "<< g->getId() << " merging "<< _DFSList.at(i)->getId() << "..." << endl;
				_gateList[_DFSList.at(i)->getId()] = 0;
				_miloa[4]--;
		
			}
		}
			
		
	}
	_DFSList.clear();
	genDFSList();
	
}




void
CirMgr::genProofModel(SatSolver& s)
{
   // Allocate and record variables; No Var ID for POs
   Var z = s.newVar();
   _gateList[0]->setVar(z);
   
   for (size_t i = 0, n = _DFSList.size(); i < n; ++i) {
   	  if(_DFSList.at(i)->getGateType() == UNDEF_GATE){ _DFSList.at(i)->setVar(z);}
	  else if(_DFSList.at(i)->getGateType() != CONST_GATE){
	  	  Var v = s.newVar();
         _DFSList.at(i)->setVar(v);
	  }      
   }
   
   for(size_t i=0;i<_DFSList.size();i++){
   	  if(_DFSList.at(i)->getGateType() == AIG_GATE){
   	      	s.addAigCNF(_DFSList.at(i)->getVar(), _DFSList.at(i)->getFanIn(0).getGate()->getVar(), _DFSList.at(i)->getFanIn(0).isInv()
				 , _DFSList.at(i)->getFanIn(1).getGate()->getVar(), _DFSList.at(i)->getFanIn(1).isInv());
	  }
   }

}

void reportResult(const SatSolver& solver, bool result)
{
   solver.printStats();
   cout << (result? "SAT" : "UNSAT") << endl;
  /* if (result) {
      for (size_t i = 0, n = _DFSList.size(); i < n; ++i)
         cout << solver.getValue(gates[i]->getVar()) << endl;
   }*/
}


void
CirMgr::fraig()
{
   
   SatSolver solver;
   solver.initialize();
   genProofModel(solver);
   bool result;
   
   for(size_t i=0;i<_DFSList.size();i++){ //along DFSList
     if(_DFSList.at(i)){	 
	   // if the gate still exists
      if(_DFSList.at(i)->getGateType() == AIG_GATE || _DFSList.at(i)->getGateType() == CONST_GATE ){  // for AIG_GATE
          size_t _fecIdx;
          if(!_fecGrps.size()) break;
          
          bool found = 0;
          for(_fecIdx=0;_fecIdx<_fecGrps.size();_fecIdx++){  // find FEC group 
            if(_fecGrps.at(_fecIdx).size())
		      if(_DFSList.at(i)->getSimResult() == _fecGrps.at(_fecIdx).at(0)->getSimResult() || ~_DFSList.at(i)->getSimResult() == _fecGrps.at(_fecIdx).at(0)->getSimResult() ){
			      for(size_t j=0;j<_fecGrps.at(_fecIdx).size();j++){
				     if(_fecGrps.at(_fecIdx).at(j) == _DFSList.at(i)){
                          found = 1; break;
				     }
				     else if(_fecGrps.at(_fecIdx).at(j)->getId() > _DFSList.at(i)->getId()) break;
			      }
		      }
		      if(found) break;
 	       }  
 	       if(!found) continue; // if did not find FEC group, skip a loop 
 	       
 	       for(size_t j=0;j<_fecGrps.at(_fecIdx).size();j++){  // deleting the gate itself from the FEC group
			  if(_fecGrps.at(_fecIdx).at(j) == _DFSList.at(i)) {
			      _fecGrps.at(_fecIdx).erase(_fecGrps.at(_fecIdx).begin()+j);  
				  break;
			  }			
		   }
		   	
		   CirGate* _testGate = _DFSList.at(i);  	 
           for(size_t j=0;j<_fecGrps.at(_fecIdx).size();j++){  // iteration along the FEC group
      	       Var newV = solver.newVar();
      	       bool inverted  = (_testGate->getSimResult() != _fecGrps.at(_fecIdx).at(j)->getSimResult());
               solver.addXorCNF(newV, _testGate->getVar(), false, _fecGrps.at(_fecIdx).at(j)->getVar(), inverted);
               solver.assumeRelease();  // Clear assumptionsb
               solver.assumeProperty(_gateList[0]->getVar(), false);
               solver.assumeProperty(newV, true);  // k = 1
               result = solver.assumpSolve();
            //   reportResult(solver, result);

		       if(!result){  // the same, merge 
		          if(_fecGrps.at(_fecIdx).at(j)->getId() != 0 && _testGate->getFanIn(0).getGate() != _fecGrps.at(_fecIdx).at(j) && _testGate->getFanIn(1).getGate() != _fecGrps.at(_fecIdx).at(j) ){  // the other gate isn't const 0
			  	          for(size_t k=0;k<_fecGrps.at(_fecIdx).at(j)->getFanOutSize();k++){  // handling fanout issue
		  	                 _testGate->pushFanOut(_fecGrps.at(_fecIdx).at(j)->getFanOut(k));   // handling fanout's fanin's fanout
					         if(_fecGrps.at(_fecIdx).at(j)->getFanOut(k).getGate()->getFanIn(0).getGate() == _fecGrps.at(_fecIdx).at(j))  _fecGrps.at(_fecIdx).at(j)->getFanOut(k).getGate()->setFanIn(0, CirGateV(_testGate, inverted^_fecGrps.at(_fecIdx).at(j)->getFanOut(k).isInv()));
						     else  	_fecGrps.at(_fecIdx).at(j)->getFanOut(k).getGate()->setFanIn(1, CirGateV(_testGate, inverted^_fecGrps.at(_fecIdx).at(j)->getFanOut(k).isInv()));
				          }	
				          for(size_t k=0;k<2;k++){  // for each fanin, delete its fanout
				      	      for(size_t f=0;f<_fecGrps.at(_fecIdx).at(j)->getFanIn(k).getGate()->getFanOutSize();f++){
					              if(_fecGrps.at(_fecIdx).at(j)->getFanIn(k).getGate()->getFanOut(f).getGate() == _fecGrps.at(_fecIdx).at(j)){
					          	     _fecGrps.at(_fecIdx).at(j)->getFanIn(k).getGate()->delFanOut(f);
					          	     break;
							      }
						      }
				          }
				          for(size_t t =0;t<_DFSList.size();t++){  // deleting the gate from the DFSList
				  	         if(_DFSList.at(t) == _fecGrps.at(_fecIdx).at(j)) {
				  	 	       _DFSList.at(t) = 0;
				  	 	       break;
					         }
				          }
				          cout << "Fraig : " << _testGate->getId() << " merging ";
						  if(inverted) cout << "!" ;
						  cout << _fecGrps.at(_fecIdx).at(j)->getId() << endl;
						  _gateList[_fecGrps.at(_fecIdx).at(j)->getId()] = 0; 
			      }
			      else{  // hard case
			              CirGate* _mergeGate = _testGate; 
			 	          _testGate = _fecGrps.at(_fecIdx).at(j);
			 	          for(size_t k=0;k<_mergeGate->getFanOutSize();k++){  // handling fanout issue
		  	                 _testGate->pushFanOut(_mergeGate->getFanOut(k));   // handling fanout's fanin's fanout
					         if(_mergeGate->getFanOut(k).getGate()->getFanIn(0).getGate() == _mergeGate) _mergeGate->getFanOut(k).getGate()->setFanIn(0, CirGateV(_testGate, inverted^_mergeGate->getFanOut(k).isInv()));
						     else  	_mergeGate->getFanOut(k).getGate()->setFanIn(1, CirGateV(_testGate, inverted^_mergeGate->getFanOut(k).isInv()));
				          }	
				          for(size_t k=0;k<2;k++){  // for each fanin, delete its fanout
				      	      for(size_t f=0;f<_mergeGate->getFanIn(k).getGate()->getFanOutSize();f++){
					              if(_mergeGate->getFanIn(k).getGate()->getFanOut(f).getGate() == _mergeGate){
					          	     _mergeGate->getFanIn(k).getGate()->delFanOut(f);
					          	     break;
							      }
						      }
				          }
				          for(size_t t =0;t<_DFSList.size();t++){  // deleting the gate from the DFSList
				  	         if(_DFSList.at(t) == _mergeGate) {
				  	 	       _DFSList.at(t) = 0;
				  	 	       break;
					         }
				          }
				          cout << "Fraig : " << _testGate->getId() << " merging ";
						  if(inverted) cout << "!" ;
						  cout << _mergeGate->getId() << endl;
						  _gateList[_mergeGate->getId()] = 0;			 	          
			      }			          
			      _fecGrps.at(_fecIdx).erase(_fecGrps.at(_fecIdx).begin()+j);   // removing the merged gate from FEC group
				  j--;	
				  _miloa[4]--;
			   }		   
		    }
		    if(_fecGrps.at(_fecIdx).size() == 0){
		           _fecGrps.erase(_fecGrps.begin()+_fecIdx);
		    }
		    
 	       
	    } 
     }
	}
	solver.reset();   
   _DFSList.clear();
   genDFSList();
}






/********************************************/
/*   Private member functions about fraig   */
/********************************************/
