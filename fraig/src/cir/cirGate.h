/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>
#include "cirDef.h"
#include "sat.h"

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------

class CirGateV         //a class for fanin definition
{
public:
    #define NEG 0x1
    CirGateV() { 	}
    CirGateV(const CirGateV& g){
    	_gatev = (size_t(g.getGate())+size_t(g.isInv()));
	}
    CirGateV(CirGate* g, size_t inv) :_gatev(size_t(g) + inv){	 }
    bool isInv() const {
    	return (_gatev & NEG);
	}
	CirGate* getGate() const {
		return (CirGate*)(_gatev & ~(size_t(NEG)));
	}
	size_t getGateV() const{ return _gatev; }
	bool operator == (CirGateV g){
		return g._gatev == _gatev;
	}
    
	
	

	
private:
    size_t _gatev;	
    
};

class CirGate
{
public:
   CirGate(size_t r = 0, unsigned l = 0, unsigned i = 0) :_ref(r), _lineNo(l), _gateId(i) { _simResult = 0;}
   virtual ~CirGate() {}

   // Basic access methods
   virtual string getTypeStr() const = 0;
   virtual bool isAig() const { return false; }
   unsigned getLineNo() const { return _lineNo; }
   unsigned getId() const {  return _gateId;}
   string getIdStr() const {
   	   string ret;
   	   int gate_id = int(_gateId);
   	   if(gate_id < 10){
   	   	   return (ret + char(gate_id+48));
		}
		
   	   
   	   	int exp = 10;     // the largest digit
   	   	while(gate_id / exp >= 10){
   	   		exp *= 10;
		}
		while(exp > 0){
			ret += char((gate_id/exp)+48);
			gate_id -= exp*(gate_id/exp);
			exp /= 10;
		}
	   
   	   
   	   return ret;
   }
   string getLineNoStr() const {
   	   string ret;
   	   int lineNo = int(_lineNo);
   	   if(lineNo < 10){
   	   	   return (ret + char(lineNo+48));
		}
		
   	   
   	   	int exp = 10;     // the largest digit
   	   	while(lineNo / exp >= 10){
   	   		exp *= 10;
		}
		while(exp > 0){
			ret += char((lineNo/exp)+48);
			lineNo -= exp*(lineNo/exp);
			exp /= 10;
		}
	   
   	   
   	   return ret;
   }
   
   virtual string getName() const = 0;
   virtual GateType getGateType() const = 0;
   CirGateV getFanOut(unsigned i) const {   return (i>=_fanoutList.size() ? CirGateV() : _fanoutList[i]);}
   CirGateV getFanIn(unsigned i) const {   return (i>=_faninList.size() ? CirGateV() : _faninList[i]);}
   unsigned getFanInSize() const {   return _faninList.size();}
   unsigned getFanOutSize() const {   return _fanoutList.size();}
   //set functions
 
   void setLineNo(unsigned i){     //for previously called gate
   	_lineNo = i;
   }
   void setId(unsigned id){
   	_gateId = id;
   }
   //construct memory space for fanin&fanout
 // void newFanIn(int);
  // void newFanOut();

   virtual void setName(string s) = 0;
   void delFanIn(int d){
   	_faninList.erase(_faninList.begin()+d);
   }
   void delFanOut(int d){
   	_fanoutList.erase(_fanoutList.begin()+d);
   }
   void pushFanIn(const CirGateV &g){
   	_faninList.push_back(g);
   }
   void pushFanOut(const CirGateV &g){
   	_fanoutList.push_back(g);
   }
   void setFanIn(int d, CirGateV g){
   	_faninList.at(d) = g;
   }

   //function overloading
   bool operator < (const CirGate &g) const {
   	return _gateId < g.getId();
   }
   
   
   // Printing functions
   virtual void printGate() const = 0;
   
   void reportGate() const;
   void reportFanin(int level) ;
   void reportFanout(int level) ;
   
   //about DFS search 
   void dfsTraversal(vector<CirGate*> &v);
   void dfsTraversal_nI(vector<CirGate*> &v);  //dfs without input and const
   void dfsTraversal_Vin( int l_count, int l_limit,bool isInv) ;
   void dfsTraversal_Vout( int l_count, int l_limit,bool isInv) ;
   	static size_t _globalRef;
	
	void setGlobalRef(){	_globalRef++;}
	void setToGlobalRef(){	_ref = _globalRef;}
	bool isGlobalRef(){		return (_ref == _globalRef);} 
	
	// about simulation
	void addSimResult(size_t t){ _simResult = t;	}
	size_t getSimResult(){ return _simResult;	}
	size_t getFecIdx() {return _fecIdx; }
	void setFecIdx(size_t i ){ _fecIdx = i;	}
	
	
	//about var
   Var getVar() const { return _var; }
   void setVar(const Var& v) { _var = v; }

private:
    size_t _fecIdx;
	unsigned _lineNo;
	unsigned _gateId;
	size_t _simResult;
	Var      _var;

protected:
	
	
	//data members for inherited class
	vector<CirGateV> _faninList;    //array, to be defined later
	vector<CirGateV> _fanoutList;
	size_t _ref;    


};

class AigGate : public CirGate{
public:
	//constructor
	AigGate(size_t r = 0, unsigned l = 0, unsigned i = 0) :CirGate(r,l,i) {}
	
	virtual string getTypeStr() const {
		if(_faninList.size() == 0)
		    return "UNDEF";
	    else return "AIG";
	}
	virtual GateType getGateType() const {
	   if(_faninList.size() == 0)
		    return UNDEF_GATE;
	   else return AIG_GATE;
	}
	virtual void setName(string s){	}
	virtual string getName() const {   return "";}
	virtual void printGate() const { }
}; 

class PIGate : public CirGate{  //no faninList
public:
	//constructor
	PIGate(size_t r = 0, unsigned l = 0, unsigned i = 0, string n = "") :CirGate(r,l,i), _name(n) {}
	
	virtual string getTypeStr() const {    return "PI";}
	virtual GateType getGateType() const {   return PI_GATE;}
	virtual string getName() const {   return _name;}
	virtual void setName(string s){   _name = s;}
	
	virtual void printGate() const { }
private:
    string _name;	
}; 

class POGate : public CirGate{  //no fanoutList
public:
    //constructor
	POGate(size_t r = 0, unsigned l = 0, unsigned i = 0, string n = "") :CirGate(r,l,i), _name(n) {}
	
	virtual string getTypeStr() const {    return "PO";}
	virtual GateType getGateType() const {   return PO_GATE;}
	virtual string getName() const {   return _name;}
	virtual void setName(string s){   _name = s;}
	
	virtual void printGate() const {    }
private:
	string _name;
}; 


class ConstGate : public CirGate{  //no faninList
public:
	//constructor
	ConstGate(size_t r = 0, unsigned l = 0, unsigned i = 0) :CirGate(r,l,i) {}
	
	virtual string getTypeStr() const {    return "CONST";}
	virtual GateType getGateType() const {   return CONST_GATE;}
	virtual void setName(string s){	}
	virtual string getName() const { return "";}
	virtual void printGate() const {   } 
};  

#endif // CIR_GATE_H
