/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"
#include<algorithm>

using namespace std;

// TODO: Keep "CirGate::reportGate()", "CirGate::reportFanin()" and
//       "CirGate::reportFanout()" for cir cmds. Feel free to define
//       your own variables and functions.

extern CirMgr *cirMgr;

/**************************************/
/*   class CirGate member functions   */
/**************************************/
void
CirGate::reportGate() const
{

	for(int i=0;i<80;i++) cout<<"=";
	cout << endl;
	
	cout << "= " << getTypeStr() << "(" << getIdStr() << ")";	
	
	if(getGateType() == PI_GATE || getGateType() == PO_GATE){
		if(getName() != "")
		cout << "\"" << (this)->getName() << "\"";
	}
	cout << ", line " << getLineNoStr();
	cout << endl;
	
	cout << "= FECs: ";
	for(size_t i=0;i<cirMgr->numFecGrps();i++){
		bool found = 0;
		if(_simResult == cirMgr->getFecGrps(i).at(0)->getSimResult() || ~_simResult == cirMgr->getFecGrps(i).at(0)->getSimResult() ){
			for(size_t j=0;j<cirMgr->getFecGrps(i).size();j++){
				if(cirMgr->getFecGrps(i).at(j)->getId() == _gateId){
					for(size_t k=0;k<cirMgr->getFecGrps(i).size();k++){
						if(k == j) continue;
						if(cirMgr->getFecGrps(i).at(k)->getSimResult() != _simResult) cout <<"!";
						cout << cirMgr->getFecGrps(i).at(k)->getId() << " ";
					}
					found = 1;
					break;
				}
				else if(cirMgr->getFecGrps(i).at(j)->getId() > _gateId) break;
			}
		}
		if(found) break;
	}

	cout << endl;
	// handling FEC cout
	
	cout << "= Value: " ;
	string value;
	   for(int i=0;;i++){
	   	   size_t t = ((_simResult) >> i) & (size_t)(1) ;
	   	   if(t == 1) value += "1";
	   	   else value += "0";
	   	   
		   if (i == 63) break;
	   	   if(i % 8 == 7 ) value +=  "_"; 
	   }
	
	cout << value;
		
	cout << endl;
	// handling Value cout
    for(int i=0;i<80;i++) cout<<"=";
    cout << endl;
}

void
CirGate::reportFanin(int level) 
{
   assert (level >= 0);
   setGlobalRef();
   dfsTraversal_Vin(0,level,0);
}

void
CirGate::reportFanout(int level) 
{
   assert (level >= 0);
   setGlobalRef();
   dfsTraversal_Vout(0,level,0);
}

//DFS traversal
void
CirGate::dfsTraversal(vector<CirGate*> &dfsList){
  //if no fanin
		if(!isGlobalRef())
		{
			setToGlobalRef();
			
			for(int i=0;i<getFanInSize();i++){
				(getFanIn(i).getGate())->dfsTraversal(dfsList);
			}
			dfsList.push_back(this);
			
		}
	
}

void
CirGate::dfsTraversal_nI(vector<CirGate*> &dfsList){
	if(getGateType() == AIG_GATE)
	{   //if no fanin
		if(!isGlobalRef())
		{
			setToGlobalRef();
			
			for(int i=0;i<getFanInSize();i++){
				(getFanIn(i).getGate())->dfsTraversal_nI(dfsList);
			}
			dfsList.push_back(this);
			
		}
	}
}

void 
CirGate::dfsTraversal_Vin(int l_count, int l_limit,bool isInv){
	  //if no fanin
	  
	    if(l_count == l_limit || getFanInSize() == 0){
			//if reach the limit
			for(int i=1;i<=2*l_count;i++)
			cout<<' ';
			cout << (isInv ? "!" : "" ) << getTypeStr() << ' ' << getId() << endl;
			return;
				  	
		}
		
		if(!isGlobalRef())
		{
			
		//	dfsList.push_back(CirGateV(this,isInv));
//		cout << getId() << " " << getFanInSize() << endl;
			for(int i=1;i<=2*l_count;i++)
			    cout<<' ';
			cout << (isInv ? "!" : "" ) << getTypeStr() << ' ' << getId() << endl;
            
			
			setToGlobalRef();
			for(int i=0;i<getFanInSize();i++){
				++l_count;
				(this->getFanIn(i).getGate())->dfsTraversal_Vin(l_count,l_limit,getFanIn(i).isInv());
				--l_count;
			}
				
			
			
		}
		else{
			//if already traversed
			for(int i=1;i<=2*l_count;i++)
			    cout<<' ';
			    
			cout << (isInv ? "!" : "" ) << getTypeStr() << ' ' << getId() << ' ' << "(*)" << endl;
		}
	
//	else{
//		cout << setw(2*l_count) << right << getTypeStr() << ' ' << getId() << endl; 
//	}
}

void 
CirGate::dfsTraversal_Vout(int l_count, int l_limit,bool isInv){
	  //if no fanin
	  
	    if(l_count == l_limit || getFanOutSize() == 0){
			//if reach the limit
			for(int i=1;i<=2*l_count;i++)
			cout<<' ';
			cout << (isInv ? "!" : "" ) << getTypeStr() << ' ' << getId() << endl;
			return;
				  	
		}
		
		if(!isGlobalRef())
		{
			
		//	dfsList.push_back(CirGateV(this,isInv));
//		cout << getId() << " " << getFanInSize() << endl;
			for(int i=1;i<=2*l_count;i++)
			    cout<<' ';
			cout << (isInv ? "!" : "" ) << getTypeStr() << ' ' << getId() << endl;
            
			
			setToGlobalRef();
			for(int i=0;i<getFanOutSize();i++){
				++l_count;
				this->getFanOut(i).getGate()->dfsTraversal_Vout(l_count,l_limit,getFanOut(i).isInv());
				--l_count;
			}
				
			
			
		}
		else{
			//if already traversed
			for(int i=1;i<=2*l_count;i++)
			    cout<<' ';
			    
			cout << (isInv ? "!" : "" ) << getTypeStr() << ' ' << getId() << ' ' << "(*)" << endl;
		}
	
//	else{
//		cout << setw(2*l_count) << right << getTypeStr() << ' ' << getId() << endl; 
//	}
}

