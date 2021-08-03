/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"
#include "cirDef.h"
#include<vector>
#include<algorithm>
using namespace std;

// TODO: Please keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
// Remove unused gates
// DFS list should NOT be changed
// UNDEF, float and unused list may be changed
void
CirMgr::sweep()
{
	IdList dfs_id;
	for(int i=0;i<_DFSList.size();i++){
		dfs_id.push_back(_DFSList.at(i)->getId());
	}
	std::sort(dfs_id.begin(),dfs_id.end());
	
	int idx = 0;
	GateList swp_list;
	for(size_t j=0;j<dfs_id.size();j++){
	     for(int i=idx;i<dfs_id.at(j);i++){
	     	if(_gateList[i])  if(_gateList[i]->getGateType() == AIG_GATE || _gateList[i]->getGateType() == UNDEF_GATE) swp_list.push_back(_gateList[i])	;
		 }
		 idx = dfs_id.at(j)+1;
	}
	
    for(int i=idx;i<_listSize;i++){
	    if(_gateList[i])  if(_gateList[i]->getGateType() == AIG_GATE || _gateList[i]->getGateType() == UNDEF_GATE) swp_list.push_back(_gateList[i])	;
	}
	
	for(size_t i=0;i<swp_list.size();i++){
		 if(swp_list.at(i)->getGateType() == AIG_GATE){
		     cout << "Sweeping: AIG"  << "(" << swp_list.at(i)->getId() << ") removed..." << endl;
		     for(int j=0;j<2;j++){
			 	 size_t k = 0;
				 for(;k<swp_list.at(i)->getFanIn(j).getGate()->getFanOutSize();k++) if(swp_list.at(i)->getFanIn(j).getGate()->getFanOut(k).getGate() == swp_list.at(i)) break;
				 swp_list.at(i)->getFanIn(j).getGate()->delFanOut(k);
			 }
			 _gateList[swp_list.at(i)->getId()] =0;
			 _miloa[4]--;	
			 
		 }
		 else if(swp_list.at(i)->getGateType() == UNDEF_GATE) {
		     cout << "Sweeping: UNDEF"  << "(" << swp_list.at(i)->getId() << ") removed..." << endl;
			 _gateList[swp_list.at(i)->getId()] =0;
		 }
	}
	
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void
CirMgr::optimize()
{
	for(size_t i=0;i<_DFSList.size();i++){
		
		
	  if(_DFSList.at(i)->getGateType() == AIG_GATE){
	  	
	  	if(_DFSList.at(i)->getFanIn(0).getGate()->getGateType() == CONST_GATE && _DFSList.at(i)->getFanIn(0).isInv() ){  //fanin(0) == const 1, replace with fanin of another 
	  	    for(size_t j=0;j<2;j++){   //deleting the two fanin's fanout to the deleted aig_gate 
	  	       size_t k = 0;    
			   for(;k<_DFSList.at(i)->getFanIn(j).getGate()->getFanOutSize();k++) if(_DFSList.at(i)->getFanIn(j).getGate()->getFanOut(k).getGate() == _DFSList.at(i)) break; 
			   _DFSList.at(i)->getFanIn(j).getGate()->delFanOut(k);	
		    } 
	  	     
	  	    for(size_t j=0;j<_DFSList.at(i)->getFanOutSize();j++){
				if(_DFSList.at(i)->getFanOut(j).getGate()->getFanIn(0).getGate() == _DFSList.at(i)){   // fanout(j) 's fanin(0) == current gate, replace with fanin(1)
					_DFSList.at(i)->getFanOut(j).getGate()->setFanIn(0, CirGateV(_DFSList.at(i)->getFanIn(1).getGate(), _DFSList.at(i)->getFanIn(1).isInv()^_DFSList.at(i)->getFanOut(j).isInv())); 
					
					
					_DFSList.at(i)->getFanIn(1).getGate()->pushFanOut(CirGateV(_DFSList.at(i)->getFanOut(j).getGate(), _DFSList.at(i)->getFanIn(1).isInv()^_DFSList.at(i)->getFanOut(j).isInv()));
				}
				else if(_DFSList.at(i)->getFanOut(j).getGate()->getFanIn(1).getGate() == _DFSList.at(i)){  // fanout(j) 's fanin(1) == current gate, replace with fanin(0)
					_DFSList.at(i)->getFanOut(j).getGate()->setFanIn(1, CirGateV(_DFSList.at(i)->getFanIn(1).getGate(), _DFSList.at(i)->getFanIn(1).isInv()^_DFSList.at(i)->getFanOut(j).isInv()));	
					
					_DFSList.at(i)->getFanIn(1).getGate()->pushFanOut(CirGateV(_DFSList.at(i)->getFanOut(j).getGate(), _DFSList.at(i)->getFanIn(1).isInv()^_DFSList.at(i)->getFanOut(j).isInv()));
				}
			}
			cout <<  "Simplifying: "<< _DFSList.at(i)->getFanIn(1).getGate()->getId() << " merging " ;
			if(_DFSList.at(i)->getFanIn(1).isInv()) cout << "!" ;
			cout << _DFSList.at(i)->getId() << "..."<< endl;
			
	       _gateList[_DFSList.at(i)->getId()] = 0;
	       _miloa[4]--;
		}
		
		else if(_DFSList.at(i)->getFanIn(0).getGate()->getGateType() == CONST_GATE && !(_DFSList.at(i)->getFanIn(0).isInv())){ // fanin(0) == const0, replace with 0
		    for(size_t j=0;j<2;j++){   //deleting the two fanin's fanout to the deleted aig_gate 
	  	       size_t k = 0;    
			   for(;k<_DFSList.at(i)->getFanIn(j).getGate()->getFanOutSize();k++) if(_DFSList.at(i)->getFanIn(j).getGate()->getFanOut(k).getGate() == _DFSList.at(i)) break; 
			   _DFSList.at(i)->getFanIn(j).getGate()->delFanOut(k);	
		    }  
		    
			for(size_t j=0;j<_DFSList.at(i)->getFanOutSize();j++){
				if(_DFSList.at(i)->getFanOut(j).getGate()->getFanIn(0).getGate() == _DFSList.at(i)){   // fanout(j) 's fanin(0) == current gate, 
					_DFSList.at(i)->getFanOut(j).getGate()->setFanIn(0, CirGateV(_DFSList.at(i)->getFanIn(0).getGate(), _DFSList.at(i)->getFanOut(j).isInv()));
					    // replace with 0
					
					_DFSList.at(i)->getFanIn(0).getGate()->pushFanOut(CirGateV(_DFSList.at(i)->getFanOut(j).getGate(), _DFSList.at(i)->getFanOut(j).isInv()));    
					    
				
				}
				else if(_DFSList.at(i)->getFanOut(j).getGate()->getFanIn(1).getGate() == _DFSList.at(i)){  // fanout(j) 's fanin(1) == current gate
					_DFSList.at(i)->getFanOut(j).getGate()->setFanIn(1, CirGateV(_DFSList.at(i)->getFanIn(0).getGate(), _DFSList.at(i)->getFanOut(j).isInv()));
						  // replace with 0
			        
					_DFSList.at(i)->getFanIn(0).getGate()->pushFanOut(CirGateV(_DFSList.at(i)->getFanOut(j).getGate(), _DFSList.at(i)->getFanOut(j).isInv()));     	
				
				}
			}
				cout <<  "Simplifying: 0"<<  " merging " ;
				cout << _DFSList.at(i)->getId() << "..."<< endl;
			_gateList[_DFSList.at(i)->getId()] = 0;
			_miloa[4]--;
		}
		
		else if(_DFSList.at(i)->getFanIn(1).getGate()->getGateType() == CONST_GATE && _DFSList.at(i)->getFanIn(1).isInv()){  //fanin(1) == const 1, replace with fanin of another 
		    for(size_t j=0;j<2;j++){   //deleting the two fanin's fanout to the deleted aig_gate 
	  	       size_t k = 0;    
			   for(;k<_DFSList.at(i)->getFanIn(j).getGate()->getFanOutSize();k++) if(_DFSList.at(i)->getFanIn(j).getGate()->getFanOut(k).getGate() == _DFSList.at(i)) break; 
			   _DFSList.at(i)->getFanIn(j).getGate()->delFanOut(k);	
		    } 

			for(size_t j=0;j<_DFSList.at(i)->getFanOutSize();j++){
				if(_DFSList.at(i)->getFanOut(j).getGate()->getFanIn(0).getGate() == _DFSList.at(i)){   // fanout(j) 's fanin(0) == current gate, replace with fanin(0)
					_DFSList.at(i)->getFanOut(j).getGate()->setFanIn(0, CirGateV(_DFSList.at(i)->getFanIn(0).getGate(),  _DFSList.at(i)->getFanIn(0).isInv()^_DFSList.at(i)->getFanOut(j).isInv()));
					
					_DFSList.at(i)->getFanIn(0).getGate()->pushFanOut(CirGateV(_DFSList.at(i)->getFanOut(j).getGate(), _DFSList.at(i)->getFanIn(0).isInv()^_DFSList.at(i)->getFanOut(j).isInv()));
				}
				else if(_DFSList.at(i)->getFanOut(j).getGate()->getFanIn(1).getGate() == _DFSList.at(i)){  // fanout(j) 's fanin(1) == current gate, replace with fanin(0)
					_DFSList.at(i)->getFanOut(j).getGate()->setFanIn(1, CirGateV(_DFSList.at(i)->getFanIn(0).getGate(),  _DFSList.at(i)->getFanIn(0).isInv()^_DFSList.at(i)->getFanOut(j).isInv()));	
					
					_DFSList.at(i)->getFanIn(0).getGate()->pushFanOut(CirGateV(_DFSList.at(i)->getFanOut(j).getGate(), _DFSList.at(i)->getFanIn(0).isInv()^_DFSList.at(i)->getFanOut(j).isInv()));
				}
			}
			cout <<  "Simplifying: "<< _DFSList.at(i)->getFanIn(0).getGate()->getId() << " merging " ;
			if( _DFSList.at(i)->getFanIn(0).isInv()) cout << "!" ;
			cout << _DFSList.at(i)->getId() << "..."<< endl;
			
			_gateList[_DFSList.at(i)->getId()] = 0;
			_miloa[4]--;
		}
		
		else if(_DFSList.at(i)->getFanIn(1).getGate()->getGateType() == CONST_GATE && !(_DFSList.at(i)->getFanIn(1).isInv())){ // fanin(1) == const0, replace with 0
		   for(size_t j=0;j<2;j++){   //deleting the two fanin's fanout to the deleted aig_gate 
	  	       size_t k = 0;    
			   for(;k<_DFSList.at(i)->getFanIn(j).getGate()->getFanOutSize();k++) if(_DFSList.at(i)->getFanIn(j).getGate()->getFanOut(k).getGate() == _DFSList.at(i)) break; 
			   _DFSList.at(i)->getFanIn(j).getGate()->delFanOut(k);	
		    } 

		   for(size_t j=0;j<_DFSList.at(i)->getFanOutSize();j++){
				if(_DFSList.at(i)->getFanOut(j).getGate()->getFanIn(0).getGate() == _DFSList.at(i)){   // fanout(j) 's fanin(0) == current gate, replace with 0
					_DFSList.at(i)->getFanOut(j).getGate()->setFanIn(0, CirGateV(_DFSList.at(i)->getFanIn(1).getGate(), _DFSList.at(i)->getFanOut(j).isInv()));
				    
					_DFSList.at(i)->getFanIn(1).getGate()->pushFanOut(CirGateV(_DFSList.at(i)->getFanOut(j).getGate(), _DFSList.at(i)->getFanOut(j).isInv()));
				}
				else if(_DFSList.at(i)->getFanOut(j).getGate()->getFanIn(1).getGate() == _DFSList.at(i)){  // fanout(j) 's fanin(1) == current gate
					_DFSList.at(i)->getFanOut(j).getGate()->setFanIn(1, CirGateV(_DFSList.at(i)->getFanIn(1).getGate(), _DFSList.at(i)->getFanOut(j).isInv()));
					
					_DFSList.at(i)->getFanIn(1).getGate()->pushFanOut(CirGateV(_DFSList.at(i)->getFanOut(j).getGate(), _DFSList.at(i)->getFanOut(j).isInv()));
				}
					cout <<  "Simplifying: 0"<<  " merging " ;
					cout << _DFSList.at(i)->getId() << "..." << endl;	
			}  
			_gateList[_DFSList.at(i)->getId()] = 0;
			_miloa[4]--;
		}
		
		else if(_DFSList.at(i)->getFanIn(0).getGate() == _DFSList.at(i)->getFanIn(1).getGate()){
		  for(size_t j=0;j<2;j++){   //deleting the two fanin's fanout to the deleted aig_gate 
	  	       size_t k = 0;    
			   for(;k<_DFSList.at(i)->getFanIn(j).getGate()->getFanOutSize();k++) if(_DFSList.at(i)->getFanIn(j).getGate()->getFanOut(k).getGate() == _DFSList.at(i)) break; 
			   _DFSList.at(i)->getFanIn(j).getGate()->delFanOut(k);	
		  } 	 
		  if(_DFSList.at(i)->getFanIn(0).isInv() == _DFSList.at(i)->getFanIn(1).isInv()){  // the two fanin is the same, replace the gate with one of the fanin

		  	for(size_t j=0;j<_DFSList.at(i)->getFanOutSize();j++){
			    if(_DFSList.at(i)->getFanOut(j).getGate()->getFanIn(0).getGate() == _DFSList.at(i)){   // fanout(j) 's fanin(0) == current gate, replace with 1
					_DFSList.at(i)->getFanOut(j).getGate()->setFanIn(0, CirGateV(_DFSList.at(i)->getFanIn(0).getGate(), _DFSList.at(i)->getFanIn(0).isInv()^_DFSList.at(i)->getFanOut(j).isInv()));
				
					_DFSList.at(i)->getFanIn(0).getGate()->pushFanOut(CirGateV(_DFSList.at(i)->getFanOut(j).getGate(), _DFSList.at(i)->getFanIn(0).isInv()^_DFSList.at(i)->getFanOut(j).isInv()));
				}
				else if(_DFSList.at(i)->getFanOut(j).getGate()->getFanIn(1).getGate() == _DFSList.at(i)){  // fanout(j) 's fanin(1) == current gate
					_DFSList.at(i)->getFanOut(j).getGate()->setFanIn(1, CirGateV(_DFSList.at(i)->getFanIn(0).getGate(), _DFSList.at(i)->getFanIn(0).isInv()^_DFSList.at(i)->getFanOut(j).isInv()));	
					
					_DFSList.at(i)->getFanIn(0).getGate()->pushFanOut(CirGateV(_DFSList.at(i)->getFanOut(j).getGate(), _DFSList.at(i)->getFanIn(0).isInv()^_DFSList.at(i)->getFanOut(j).isInv()));
				}
			}
			        cout <<  "Simplifying: "<< _DFSList.at(i)->getFanIn(0).getGate()->getId() << " merging " ;
					if(_DFSList.at(i)->getFanIn(0).isInv()) cout << "!" ;
					cout << _DFSList.at(i)->getId() << "..." << endl;
		  }
		  else{
		  	for(size_t j=0;j<_DFSList.at(i)->getFanOutSize();j++){      // the two fanin is inverted, replace the gate with 0
			    if(_DFSList.at(i)->getFanOut(j).getGate()->getFanIn(0).getGate() == _DFSList.at(i)){   // fanout(j) 's fanin(0) == current gate, replace with 0
					_DFSList.at(i)->getFanOut(j).getGate()->setFanIn(0, CirGateV(_gateList[0], _DFSList.at(i)->getFanOut(j).isInv()));
					
					_gateList[0]->pushFanOut(CirGateV(_DFSList.at(i)->getFanOut(j).getGate(), _DFSList.at(i)->getFanOut(j).isInv()));
				}
				else if(_DFSList.at(i)->getFanOut(j).getGate()->getFanIn(1).getGate() == _DFSList.at(i)){  // fanout(j) 's fanin(1) == current gate
					_DFSList.at(i)->getFanOut(j).getGate()->setFanIn(1, CirGateV(_gateList[0], _DFSList.at(i)->getFanOut(j).isInv()));	
					
					_gateList[0]->pushFanOut(CirGateV(_DFSList.at(i)->getFanOut(j).getGate(), _DFSList.at(i)->getFanOut(j).isInv()));
				}
			}
			cout <<  "Simplifying: 0"<<  " merging " ;
			cout << _DFSList.at(i)->getId() << "..." << endl;
		  }
			_gateList[_DFSList.at(i)->getId()] = 0;
			_miloa[4]--;
		}
	  }	
		
	
		
	}
	
	_DFSList.clear();
	genDFSList();
	
	
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/

