/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"
#include "cirDef.h"
#include<string>
#include<set>

using namespace std;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;

enum CirParseError {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printint, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;
size_t CirGate::_globalRef = 0;

static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine constant (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
bool
CirMgr::readCircuit(const string& fileName)
{
   fstream myfile;
   string temp;
   char * cstr = new char [fileName.length()+1];
   std::strcpy (cstr, fileName.c_str());
   myfile.open(cstr) ;
   if(myfile.is_open() == 1){
   	  for(int i=0;i<6;i++){
	     string _get ;
         if(i == 5)  getline(myfile, _get);
		 else getline(myfile, _get, ' ');
         int num ;
         myStr2Int(_get, num);
		 if(i==1){   // m
		 	_miloa[0] = num;
		 }
		 else if(i==2){  // i
		 	_miloa[1] = num;
		 	for(int i=0;i<_miloa[1];i++){
		 	   	CirGate* cir = new CirGate(PI_GATE);
		 	   	_gateList.push_back(cir);
			}
		 }
		 else if(i==4){  //o
		 	_miloa[3] = num;
		 	for(int i=0;i<_miloa[3];i++){
		 	   	CirGate* cir = new CirGate(PO_GATE);
		 	   	_gateList.push_back(cir);
			}
		 }
		 else if(i==5){  // a
		 	_miloa[4] = num;
		 	for(int i=0;i<_miloa[4];i++){
		 	   	CirGate* cir = new CirGate(AIG_GATE);
		 	   	_gateList.push_back(cir);
			}
		 }

	 }


	 for(int i=0;i<_miloa[1];i++){  // input
	     string _get ;
	     getline(myfile, _get);
         int num ;
         myStr2Int(_get, num);
	 	 _idList.push_back(num/2);
	 	 _gateList.at(i)->setId(num/2);
	 }

	 int in_of_out[_miloa[3]]; // fanin of outputs

	 for(int i=0;i<_miloa[3];i++){  //output, push_back idList
	     string _get ;
         getline(myfile, _get);
         int num ;
         myStr2Int(_get, num);
	     in_of_out[i] = num;
	   	 _idList.push_back(_miloa[0]+i+1);
	     _gateList.at(_miloa[1]+i)->setId(_miloa[0]+i+1);
	 }

	 int in_of_aig[_miloa[4]][2];

	 for(int i=0;i<_miloa[4];i++){   //aigGate, push_back idList
	 	for(int j=1;j<=3;j++){
		  string _get ;
          if(j == 3) getline(myfile, _get);
		  else getline(myfile, _get, ' ');
          int num ;
          myStr2Int(_get, num);
           switch(j){
           	  case 1 :
           	    _idList.push_back(num/2);
				_gateList.at(_miloa[1]+_miloa[3]+i)->setId(num/2);
				    break;
           	  case 2 :
				in_of_aig[i][0] = num; break;
           	  case 3 :
				in_of_aig[i][1] = num; break;
		   }
		}
	 }

	 for(int i=0;i<_miloa[3];i++){   // handling output fanin
	 	_gateList.at(_miloa[1]+i)->_fanInList = new CirGateV[2];
	 	if( in_of_out[i]%2 == 0){ // non-inverting
	 	    int index = 0;
	 	    if(in_of_out[i] == 0){
	 	    		index = _idList.size();
        			CirGate* cir = new CirGate(CONST_GATE);
				   	_gateList.push_back(cir);
				   	_idList.push_back(0);
				   	_gateList.at(index)->setId(0);
			}
        	else{
        		while(in_of_out[i]/2 != _idList.at(index)){
				    index++;
				    if(index == _idList.size()){
				   	    CirGate* cir = new CirGate(UNDEF_GATE);
				   	    _gateList.push_back(cir);
				   	    _idList.push_back(in_of_out[i]/2);
				   	    _gateList.at(index)->setId(in_of_out[i]/2);
				   	    break;
				    }
				}
			}
	 		_gateList.at(_miloa[1]+i)->_fanInList[0] = CirGateV(_gateList.at(index), false);
	 		_gateList.at(_miloa[1]+i)->_fanInList[1] = CirGateV(0, false);
		}
		else{   // inverting
			int index = 0;
	 	    if(in_of_out[i] == 1){
	 	    	    index = _idList.size();
        			CirGate* cir = new CirGate(CONST_GATE);
				   	_gateList.push_back(cir);
				   	_idList.push_back(0);
					_gateList.at(index)->setId(0);
			}
        	else{
        		while((in_of_out[i]-1)/2 != _idList.at(index)){
				    index++;
				    if(index == _idList.size()){
				   	    CirGate* cir = new CirGate(UNDEF_GATE);
				   	    _gateList.push_back(cir);
				   	    _idList.push_back((in_of_out[i]-1)/2);
				   	    _gateList.at(index)->setId((in_of_out[i]-1)/2);
				   	    break;
				    }
				}
			}
	 		_gateList.at(_miloa[1]+i)->_fanInList[0] = CirGateV(_gateList.at(index), true);
	 		_gateList.at(_miloa[1]+i)->_fanInList[1] = CirGateV(0, false);
		}
	 }

	 for(int i=0;i<_miloa[4];i++){   // handling aig fanin
	 	_gateList.at(_miloa[1]+_miloa[3]+i)->_fanInList = new CirGateV[2];
        for(int j=0;j<=1;j++){
        	if(in_of_aig[i][j]%2 == 0){ // non-inverting
        		int index =0;
        		if(in_of_aig[i][j] == 0){
        			index = _idList.size();
        			CirGate* cir = new CirGate(CONST_GATE);
				   	_gateList.push_back(cir);
				   	_idList.push_back(0);
				   	_gateList.at(index)->setId(0);
				}
        		else{
        			while(in_of_aig[i][j]/2 != _idList.at(index)){
				       index++;
				       if(index == _idList.size()){
				   	      CirGate* cir = new CirGate(UNDEF_GATE);
				   	      _gateList.push_back(cir);
				   	      _idList.push_back(in_of_aig[i][j]/2);
				   	      _gateList.at(index)->setId(in_of_aig[i][j]/2);
				   	      break;
				       }
				    }
				}
        		_gateList.at(_miloa[1]+_miloa[3]+i)->_fanInList[j] = CirGateV(_gateList.at(index),false);
			}
			else{ //inverting
				int index =0;
        	    if(in_of_aig[i][j] == 1){
        	    	index = _idList.size();
        			CirGate* cir = new CirGate(CONST_GATE);
				   	_gateList.push_back(cir);
				   	_idList.push_back(0);
				   	_gateList.at(index)->setId(0);
				}
        		else{
        			while((in_of_aig[i][j]-1)/2 != _idList.at(index)){
				       index++;
				       if(index == _idList.size()){
				   	      CirGate* cir = new CirGate(UNDEF_GATE);
				   	      _gateList.push_back(cir);
				   	      _idList.push_back((in_of_aig[i][j]-1)/2);
				   	      _gateList.at(index)->setId((in_of_aig[i][j]-1)/2);
				   	      break;
				       }
				    }
				}
        		_gateList.at(_miloa[1]+_miloa[3]+i)->_fanInList[j] = CirGateV(_gateList.at(index),true);
			}
		}
	 }

	 while(true){
	 	string str;
	 	getline(myfile, str);
	 	string tok;
	 	int idx = myStrGetTok(str, tok);
	 	if(myfile.eof())break;
	 	if(tok.at(0) == 'i'){
            int num;
            myStr2Int(tok.substr(1) ,num);
		    _gateList.at(num)->_name = str.substr(idx);

		}
		else if(tok.at(0) == 'o'){
            int num;
            myStr2Int(tok.substr(1) ,num);
			_gateList.at(num+_miloa[1])->_name = str.substr(idx);
		}
		else if(tok.at(0) == 'c'){
			break;
		}
		else break;
	 }
	 for(size_t i=_miloa[1];i<_miloa[1]+_miloa[3];i++){
	     _gateList.at(i)->_fanInList[0].gate()->_fanOutList.push_back(CirGateV(_gateList.at(i), _gateList.at(i)->_fanInList[0].isInverted()));	
	 }
	 
	 for(size_t i=_miloa[1]+_miloa[3];i<_miloa[1]+_miloa[3]+_miloa[4];i++){
	     _gateList.at(i)->_fanInList[0].gate()->_fanOutList.push_back(CirGateV(_gateList.at(i), _gateList.at(i)->_fanInList[0].isInverted()));	
	     _gateList.at(i)->_fanInList[1].gate()->_fanOutList.push_back(CirGateV(_gateList.at(i), _gateList.at(i)->_fanInList[1].isInverted()));	
	 }
	 

	 return true;
   }
   else {
   	  cout << "cannot open file !!" << endl;
   	  return false;
   }
}

/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void
CirMgr::printSummary() const
{
	cout << "Circuit Statistics" << endl;
	cout << "==================" << endl;
	cout << "  PI" << setw(12) << _miloa[1] << endl;
	cout << "  PO" << setw(12) << _miloa[3] << endl;
	cout << "  AIG" << setw(11) << _miloa[4] << endl;
	cout << "------------------" << endl;
	cout << "  Total" << setw(9) << _miloa[1]+_miloa[3]+_miloa[4] << endl;

}

void
CirMgr::printNetlist() const
{    // end -> UN_DEF, CONST , INPUT
  int list_ind = 0;
  bool pr_const = 0;
  size_t now_ref = CirGate::getGlobalRef();
  for(int i=0;i<_miloa[3];i++){
  	 GateList trace;
  	 CirGate::setGlobalRef();

	 CirGate* _cur = _gateList.at(_miloa[1]+i);
	 trace.push_back(_cur);  // output node

	 _cur = _cur->_fanInList[0].gate();
	 trace.push_back(_cur);  // the fanin of output
	 while(_cur != _gateList.at(_miloa[1]+i)){

	 	 if(_cur->_gateType == UNDEF_GATE || _cur->_gateType == PI_GATE || _cur->_gateType == CONST_GATE ){
	 	 	int ori_ref = _cur->getRef();
	 	 	_cur->setRef();
	 	 	 if(ori_ref <= now_ref && _cur->_gateType != UNDEF_GATE  && !(_cur->_gateType == CONST_GATE && pr_const)){
	 	      cout << "[" << list_ind << "] " ;
	 	  	  switch(_cur->_gateType){
				 case PI_GATE :
					cout << "PI  " << _cur->getId() ;
					if(_cur->_name.size()) cout << " (" << _cur->_name << ")";
					cout << endl; break;
				 case UNDEF_GATE :
					cout << "UNDEF " << _cur->getId() << endl; break;
			 	 case CONST_GATE :
					if(!pr_const) cout << "CONST0" << endl; pr_const = 1; break;
			  }
			  list_ind++;
		    }
		    trace.pop_back();
		    _cur = trace.at(trace.size()-1);
		 }
	 	 else if(_cur->_fanInList[0].gate()->getRef() < CirGate::getGlobalRef()){  // not yet traverse
		    _cur = _cur->_fanInList[0].gate();
		    trace.push_back(_cur);
	     }
	     else if(_cur->_fanInList[1].gate()->getRef() < CirGate::getGlobalRef()){  // already traverse
		    _cur = _cur->_fanInList[1].gate();
		    trace.push_back(_cur);
	     }
	     else{
	     	int ori_ref = _cur->getRef();
	     	_cur->setRef();

	 	    if(ori_ref <= now_ref){
	 	      cout << "[" << list_ind << "] " ;
	 	  	  switch(_cur->_gateType){
				 case PI_GATE :
					cout << "PI  " << _cur->getId() ;
					if(_cur->_name.size()) cout << " (" << _cur->_name << ")";
					cout << endl; break;
				 case AIG_GATE :
					cout << "AIG " << _cur->getId() <<" ";
					if(_cur->_fanInList[0].gate()->_gateType == UNDEF_GATE) cout << "*";
					if(_cur->_fanInList[0].isInverted())  cout << "!";
					cout << _cur->_fanInList[0].gate()->getId() << " ";
					if(_cur->_fanInList[1].gate()->_gateType == UNDEF_GATE) cout << "*";
					if(_cur->_fanInList[1].isInverted())  cout << "!";
					cout << _cur->_fanInList[1].gate()->getId() << endl; break;

			  }
			  list_ind++;
		    }


		    trace.pop_back();
		    _cur = trace.at(trace.size()-1);

		 }


	 }
	 	cout << "[" << list_ind << "] " ;
		cout << "PO  " << _cur->getId() << " ";
		if(_cur->_fanInList[0].gate()->_gateType == UNDEF_GATE) cout << "*";
		if(_cur->_fanInList[0].isInverted())  cout << "!";
		cout <<  _cur->_fanInList[0].gate()->getId() ;
		if(_cur->_name.size()) cout << " (" << _cur->_name << ")";
		cout << endl;
		list_ind++;

  }

}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:" ;
   for(int i=0;i<_miloa[1];i++){
   	  cout << " " << _idList.at(i);
   }
   cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   for(int i=0;i<_miloa[3];i++){
   	  cout << " " << _idList.at(_miloa[1]+i);
   }
   cout << endl;
}

void
CirMgr::printFloatGates() const
{   std::set<unsigned> flo_fanin;
    std::set<unsigned> def_not_used;
	for(size_t i=0;i<_idList.size();i++){ // handling gates with floating fanin

		if(_gateList.at(i)->_gateType == PO_GATE && _gateList.at(i)->_fanInList[0].gate()->_gateType == UNDEF_GATE)  flo_fanin.insert(_idList.at(i));
		else if(_gateList.at(i)->_gateType == AIG_GATE && (_gateList.at(i)->_fanInList[0].gate()->_gateType == UNDEF_GATE || _gateList.at(i)->_fanInList[1].gate()->_gateType == UNDEF_GATE))  flo_fanin.insert(_idList.at(i));
	}
    if(flo_fanin.size() != 0){
        cout << "Gates with floating fanin(s):" ;
	    for (std::set<unsigned>::iterator it=flo_fanin.begin(); it!=flo_fanin.end(); ++it) cout << ' ' << *it;
        cout << endl;
    }



	for(size_t i=0;i<_miloa[1];i++){
		bool used = 0;
		for(size_t j=_miloa[1];j<_miloa[1]+_miloa[3]+_miloa[4];j++){
			if(_gateList.at(j)->_gateType == PO_GATE){
				if(_gateList.at(j)->_fanInList[0].gate()->getId() == _idList.at(i)){
					used = 1; break;
				}
			}
			else if(_gateList.at(j)->_gateType == AIG_GATE){
				if(_gateList.at(j)->_fanInList[0].gate()->getId() == _idList.at(i)){
					used = 1; break;
				}
				else if(_gateList.at(j)->_fanInList[1].gate()->getId() == _idList.at(i)){
					used = 1; break;
				}
			}
		}
		if(!used) def_not_used.insert(_idList.at(i));
	}
	for(size_t i=_miloa[1]+_miloa[3];i<_miloa[1]+_miloa[3]+_miloa[4];i++){
		bool used = 0;
		for(size_t j=_miloa[1];j<_miloa[1]+_miloa[3]+_miloa[4];j++){
			if(_gateList.at(j)->_gateType == PO_GATE){
				if(_gateList.at(j)->_fanInList[0].gate()->getId() == _idList.at(i)){
					used = 1; break;
				}
			}
			else if(_gateList.at(j)->_gateType == AIG_GATE){
				if(_gateList.at(j)->_fanInList[0].gate()->getId() == _idList.at(i)){
					used = 1; break;
				}
				else if(_gateList.at(j)->_fanInList[1].gate()->getId() == _idList.at(i)){
					used = 1; break;
				}
			}
		}
		if(!used) def_not_used.insert(_idList.at(i));
	}

	if(def_not_used.size() != 0){
        cout << "Gates defined but not used  :" ;
        for (std::set<unsigned>::iterator it=def_not_used.begin(); it!=def_not_used.end(); ++it) cout << ' ' << *it;
	}

}

void
CirMgr::writeAag(ostream& outfile) const
{
	GateList  _list;
	size_t now_ref = CirGate::getGlobalRef();
	for(int i=0;i<_miloa[3];i++){
  	 GateList trace;
  	 CirGate::setGlobalRef();

	 CirGate* _cur = _gateList.at(_miloa[1]+i);
	 trace.push_back(_cur);  // output node

	 _cur = _cur->_fanInList[0].gate();
	 trace.push_back(_cur);  // the fanin of output
	 while(_cur != _gateList.at(_miloa[1]+i)){

	 	 if(_cur->_gateType == UNDEF_GATE || _cur->_gateType == PI_GATE || _cur->_gateType == CONST_GATE ){
	 	 	int ori_ref = _cur->getRef();
	 	 	_cur->setRef();
		    trace.pop_back();
		    _cur = trace.at(trace.size()-1);
		 }
	 	 else if(_cur->_fanInList[0].gate()->getRef() < CirGate::getGlobalRef()){  // not yet traverse
		    _cur = _cur->_fanInList[0].gate();
		    trace.push_back(_cur);
	     }
	     else if(_cur->_fanInList[1].gate()->getRef() < CirGate::getGlobalRef()){  // already traverse
		    _cur = _cur->_fanInList[1].gate();
		    trace.push_back(_cur);
	     }
	     else{
	     	int ori_ref = _cur->getRef();
	     	_cur->setRef();
	     	if(ori_ref <= now_ref){_list.push_back(_cur);}
		    trace.pop_back();
		    _cur = trace.at(trace.size()-1);

		 }


	 }

  }
	outfile << "aag " << _miloa[0] << " " << _miloa[1] << " " << 0 << " "<< _miloa[3] << " " << _miloa[4] << endl;
	for(int i=0;i<_miloa[1];i++){
		outfile << _idList.at(i)*2 << endl;
	}
	for(int i=0;i<_miloa[3];i++){
		outfile << (_gateList.at(i+_miloa[1])->_fanInList[0].gate()->getId())*2 + (int)(_gateList.at(i+_miloa[1])->_fanInList[0].isInverted()) << endl;
	}
	for(GateList::iterator it=_list.begin();it!= _list.end();it++){
		outfile << ((*it)->getId())*2 << " " << ((*it)->_fanInList[0].gate()->getId())*2 + (int)((*it)->_fanInList[0].isInverted()) << " "
		<< ((*it)->_fanInList[1].gate()->getId())*2 + (int)((*it)->_fanInList[1].isInverted()) << endl;
	}
	for(int i=0;i<_miloa[1];i++){
		if(_gateList.at(i)->_name.size())outfile << "i" << i << " " << _gateList.at(i)->_name <<endl;
	}
	for(int i=0;i<_miloa[3];i++){
		if(_gateList.at(i+_miloa[1])->_name.size())outfile << "o" << i << " " << _gateList.at(i+_miloa[1])->_name <<endl;
	}

}
