/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"
#include<algorithm>

using namespace std;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;
size_t CirGate::_globalRef = 0;

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
              << ": Cannot redefine const (" << errInt << ")!!" << endl;
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
   fstream file(fileName);
   //does not handle error
   if(file.is_open()){
   	string firstLine;
   	getline(file,firstLine);
   	//assume "aag "
   	size_t begin = 4;
   	
   	//read in the first line( MILOA )
   	int miloa_int[5];
   	for(int i=0;i<5;i++){
   		size_t end = firstLine.find_first_of(' ',begin);
   		string strNum = firstLine.substr(begin, (end - begin));    //the number in string form
   		if(!myStr2Int(strNum, miloa_int[i]))                          //convert and store in miloa
   		    return false;
   		    
   		_miloa[i] = unsigned(miloa_int[i]);                         //convert int to unsigned
   		
   		if(i == 4){
   			if(end != string::npos)
   			    return false;
		}
		else 	begin = end + 1;
	}

    //create an empty array
    _listSize = _miloa[0]+_miloa[3]+1;
	_gateList = new CirGate*[_listSize];
	_gateList[0] = new ConstGate(0,0,0);
	for(int i=1;i<_listSize;i++){
		_gateList[i] = 0;
	} 
	
//	bool hasZero = false;
	/***read the input (repeat I times)***/
	for(int i=1;i<=int(_miloa[1]);i++){
		string strNum;
		int id;
		getline(file,strNum);
		if(!myStr2Int(strNum,id))       //read in input ID
		    return false;
		
		id/=2;     //because it is always noninverted
		
		_gateList[id] = new PIGate(0,i+1,id,"");

	    _piList.push_back(id);
	}
	
	/*** read the output ***/
	for(int i=1;i<=_miloa[3];i++){  
		//first construct the POGate
		CirGate* po = new POGate(0,i+1+_miloa[1],_miloa[0]+i,"");   //the output gate to pushin
	    _gateList[_miloa[0]+i] = po;
	//	_idList.push_back(_miloa[0]+i);
		
		int fanInId;
		string strNum;
		size_t isInv;
		
		getline(file,strNum);
		if(!myStr2Int(strNum,fanInId) || fanInId < 0)       //read in input ID
		    return false;
		
		///have to consider inverting input
		isInv = size_t(fanInId%2);
		fanInId/=2;                          //get the real fanin ID
		
		
		
		//check if fanin is input
		if(_gateList[fanInId] != 0){      //if is input-> setFanIn, setFanout for input
			po->pushFanIn(CirGateV(_gateList[fanInId],isInv));
			_gateList[fanInId]->pushFanOut(CirGateV(po,isInv));
		}
		else{                         //if not input->construct a new gate, and set its fanOut
		    CirGate* fan_in;
			
			fan_in = new AigGate(0,0,fanInId);
			
			
			po->pushFanIn(CirGateV(fan_in, isInv));
			fan_in->pushFanOut(CirGateV(po,isInv));
			
			//push back the newly constructed gate
			_gateList[fanInId] = fan_in;
	//		_idList.push_back(fanInId);
		}
	    	
	}
	
	
	/*** read in the def of AIG ***/	
	unsigned line_idx = _miloa[1]+_miloa[3]+1;

	while(true){
		++line_idx;
		unsigned gate_id;
		unsigned _inputId[2];
		bool isInv[2] = {0,0};
		CirGate* toBeDef = 0;
		CirGate* fanin[2] = {0,0};
		

	
		//read in the numbers
		if(!(file>>gate_id)){       //failbit or EOF
			file.clear();
		    
			break;
		}
		gate_id/=2;
		    
		for(int i=0;i<2;i++){
			file.ignore();
			file>>_inputId[i];
			if(_inputId[i]%2)
			    isInv[i] = true;
			    
			_inputId[i] = _inputId[i]/2;
		}
		
	
		//概念：先確定parse進來的input和要define的gate是否已經在List裡面，如果有 就把pointer那些設成想要的；
		//如果沒有，就new一個新的gate，然後再把pointer設好 
		//start to parse
		
		/*handle the tobeDefined gate first*/
		//check if already pushed in
		if(_gateList[gate_id] != 0){
			toBeDef = _gateList[gate_id];
			toBeDef->setLineNo(line_idx);
		}
		
		//check if already defined, if not, create one
		if(toBeDef == 0){
			toBeDef = new AigGate(0,line_idx,gate_id);
			_gateList[gate_id] = toBeDef;
	//		_idList.push_back(gate_id);
		}
		   
		   
		/**then handle the fanin**/
		if(_inputId[0] != _inputId[1]) 
		{   //if the two fanins aren't the same
		    for(int i=0;i<2;i++){
		    	if(_gateList[_inputId[i]] != 0){
		    		fanin[i] = _gateList[_inputId[i]];
				}
			}
		}  
		else   
		{   //if they are the same
		    if(_gateList[_inputId[0]] != 0){
		    	fanin[0] = fanin[1] = _gateList[_inputId[0]];
			}			
		}
		
		//check if not two same input
		if(_inputId[0] != _inputId[1])  
		  for(int i=0;i<2;i++){
			if(fanin[i] == 0){
			    fanin[i] = new AigGate(0,0,_inputId[i]);
		   	   
		   	   _gateList[_inputId[i]] = fanin[i];
	//	   	   _idList.push_back(_inputId[i]);
		   }
		  }
		else{
			if(fanin[0] == 0){
			   fanin[0] = new AigGate(0,0,_inputId[0]);
		   	   
		   	   fanin[1] = fanin[0];
		   	   _gateList[_inputId[0]] = fanin[0];
	//	   	   _idList.push_back(_inputId[0]);
		    }
		    
		}
		   
		//start to assign fanin, fanout
		for(int i=0;i<2;i++){
			toBeDef->pushFanIn(CirGateV(fanin[i],isInv[i]));
			fanin[i]->pushFanOut(CirGateV(toBeDef, isInv[i]));
		}
		 
	}
	
	/***read in the names***/
	while(true){
		char firChar;
		file.get(firChar);
		string strName;
		if(firChar == 'c' || file.eof())  //if comment section begins or eof
		    break;
		
		int i_o_Idx;
		file>>i_o_Idx;
		file.ignore();
		getline(file,strName);
		
		if(firChar == 'i'){       //if input
			_gateList[_piList[i_o_Idx]]->setName(strName);
		}
		else if(firChar == 'o'){  //if output
		    _gateList[_miloa[0]+1+i_o_Idx]->setName(strName);
			
		}
		else return false;
	}
	
	//generate the DFSList
	genDFSList();
	
	return true;
   }
   else{    //file not open
      cerr << "Cannot open file " << "\""  <<  fileName << " !"  << "\"" << endl;
   	  return false;
   	  
   }
}

void 
CirMgr::genDFSList()
{
	//setGlobalRef
	_gateList[0]->setGlobalRef();
	for(int i=_miloa[0]+1;i<_miloa[0]+1+_miloa[3];i++){
		_gateList[i]->dfsTraversal(_DFSList);   //push the node inside the output into the dfsList
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
	cout<<"\nCircuit Statistics"<<endl;
	cout<<"=================="<<endl;
	cout<<"  PI";
	cout<<setw(12)<<right<<_miloa[1]<<endl;
	cout<<"  PO";
	cout<<setw(12)<<right<<_miloa[3]<<endl;
	cout<<"  AIG";
	cout<<setw(11)<<right<<_miloa[4]<<endl;
	cout<<"------------------"<<endl;
	cout<<"  Total"<<setw(9)<<(_miloa[1]+_miloa[3]+_miloa[4])<<endl;
}

void
CirMgr::printNetlist() const
{

   cout<<endl;

	int lineCount = 0;

	for(int i=0;i<_DFSList.size();i++){
		if(_DFSList.at(i)->getGateType() != UNDEF_GATE){
			cout << "[" << lineCount << "] " << setw(4) << left << _DFSList[i]->getTypeStr() << _DFSList[i]->getId();
			//for each of its fanin
			for(int fidx = 0; fidx < _DFSList[i]->getFanInSize(); fidx++){
				cout << ' ';
				cout << (((_DFSList[i]->getFanIn(fidx)).getGate()->getGateType() == UNDEF_GATE) ? "*" : "") << ((_DFSList[i]->getFanIn(fidx).isInv()) ? "!" :"" ) << (_DFSList[i]->getFanIn(fidx)).getGate()->getId();
				
			}
			if(_DFSList[i]->getName() != ""){
				cout << ' ' << "(" << _DFSList[i]->getName() << ")";
			}
			cout<<endl;
			lineCount++;
		}
		
	}
	
	
}

void
CirMgr::printPIs() const
{
    cout << "PIs of the circuit:";
   for(int i=0;i<_piList.size();i++){
   	 cout<<' '<<_piList[i];
   }
   cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   //if no output
   if(_miloa[3] == 0){
   	cout << endl;
   	return;
   }
   
  
   int ogCount = 0;
   int gate_id = _miloa[0]+1;
   for(int i=0;i<_miloa[3];i++){
   	cout<<' '<<gate_id;
   	gate_id++;
   }
   cout<<endl;
}

void
CirMgr::printFloatGates() const
{
	vector<unsigned> _floating;
	vector<unsigned> _defBNU;
	vector<unsigned>::iterator it;
	
	bool haveFloating = false;
	bool haveNotUsed = false;
	//gate with floating fanins
	for(int i= 0;i<_listSize;i++){
		//if PO or AIG
		if(_gateList[i] != 0)
		if(_gateList[i]->getGateType() == AIG_GATE || _gateList[i]->getGateType() == PO_GATE)
			for(int j=0;j<_gateList[i]->getFanInSize();j++){
				if((_gateList[i]->getFanIn(j)).getGate()->getGateType() == UNDEF_GATE){
					_floating.push_back(_gateList[i]->getId());
					break;
				}
			}
	}
	
	if(!_floating.empty()){
		std::sort(_floating.begin(),_floating.end());
		cout << "Gates with floating fanin(s):";
		it = _floating.begin();
		while(it != _floating.end()){
		    cout << ' ' << (*it);
		    ++it;
		}
		cout<<endl;
	}
	    
	    
	//define but not used
	for(int i=0;i<=_miloa[0];i++){
		if(_gateList[i] != 0)
		if(_gateList[i]->getGateType() == AIG_GATE || _gateList[i]->getGateType() == PI_GATE){
			if(_gateList[i]->getFanOutSize() == 0){
			    _defBNU.push_back(_gateList[i]->getId());
			} 
		}
		
	}
	
	if(!_defBNU.empty()){
		std::sort(_defBNU.begin(),_defBNU.end());
		cout << "Gates defined but not used  :";
		it = _defBNU.begin();
		while(it != _defBNU.end()){
		    cout << ' ' << (*it);
		    ++it;
		}
		cout<<endl;
	}
}

void
CirMgr::printFECPairs() const
{
    /*for(size_t i=0;i<_DFSList.size();i++){
		cout << "[" << _DFSList.at(i)->getId() << "] " ;
		for(size_t j=0;j<_DFSList.at(i)->_simResult.size();j++) cout << _DFSList.at(i)->_simResult.at(j) << " ";
		cout << endl;
	}*/
	
	for(size_t i=0;i<_fecGrps.size();i++){
		cout << "[" << i << "] " ;
		size_t positive_value = _fecGrps.at(i).at(0)->getSimResult();
		for(size_t j=0;j<_fecGrps.at(i).size();j++) {
			if(_fecGrps.at(i).at(j)->getSimResult() != positive_value) cout << "!";
			cout << _fecGrps.at(i).at(j)->getId() << " ";
		}
		cout << endl;
	}
}

void
CirMgr::writeAag(ostream& outfile) const
{
	//first get the number of AIG_GATES
	//setGlobalRef
	_gateList[0]->setGlobalRef();
	
	vector<CirGate*> _dfsList;
	for(int i=_miloa[0]+1;i<_listSize;i++){
		//check if PO
		if(_gateList[i] != 0)
		if(_gateList[i]->getGateType() == PO_GATE){
			(_gateList[i]->getFanIn(0).getGate())->dfsTraversal_nI(_dfsList);   //push the node inside the output into the dfsList
			
		}
	}
	
	//print out 1st line
	outfile<<"aag";
	for(int i=0;i<4;i++){
		outfile<<" "<<_miloa[i];
	}
	outfile << " " << _dfsList.size();
	
	//print out input
	for(int i=0;i<_miloa[1];i++){
		outfile<<"\n";
		outfile<<2*(_piList[i]);
	}
	//print out the output
	int outCount = 0;
	for(int i=_miloa[0]+1;i<_listSize;i++){
		unsigned fan_in_id = (_gateList[i]->getFanIn(0)).getGate()->getId();
		outfile<<endl;
		outfile << (((_gateList[i]->getFanIn(0)).isInv()) ? (2*(fan_in_id)+1) : 2*(fan_in_id));
		outCount++;
	
	}
	
	
	/****the definition only consists of those in the DFS list***/
	//print out definition  這邊有用到原本的模式fanin fanout 之後改調
	
	
	
	for(int j=0;j<_dfsList.size();j++){
			outfile << endl;
			outfile << 2*(_dfsList[j]->getId());
			for(int fidx = 0;fidx < 2; fidx++){
				outfile << ' ' << 2*((_dfsList[j]->getFanIn(fidx)).getGate()->getId()) + (((_dfsList[j]->getFanIn(fidx)).isInv()) ? 1 : 0);
			}	
	} 
	 
	 
	 
	
	//print out the names
	//1. print out inputName
	for(int i=0;i<_piList.size();i++){
		if(_gateList[_piList[i]]->getName() != ""){
			outfile<<endl;
			outfile<<'i'<<i<<' '<<_gateList[_piList[i]]->getName();
		}
		
		
	}
	//2.print out outputName
	int ogCount = 0;
	
	if(_miloa[3] != 0)
	  for(int i=_miloa[0]+1;i<_listSize;i++){
			if(_gateList[i]->getName() != ""){
				outfile<<endl;
				outfile<<'o'<<ogCount<<' '<<_gateList[i]->getName();
			}
			ogCount++;
		  
	  }  
	outfile<<endl;
}

void
CirMgr::writeGate(ostream& outfile, CirGate *g) const
{
}


