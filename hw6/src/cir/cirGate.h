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

using namespace std;

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes

class CirGateV
{
	#define NEG 0x1
public:
	CirGateV(){;	}
	CirGateV(CirGate* g, bool phase) : _gate(g), _inv(phase){	}
	CirGate* gate(){return _gate;	}
	bool isInverted(){return _inv;	}
private:
	CirGate* _gate;
	bool _inv;
};


class CirGate
{
	
public:
   CirGate(int a) {_gateType = a; _ref = 0; same_in_count = 0 ; same_out_count = 0; }
    ~CirGate() {}

   // Basic access methods
   int getId(){ return id; }
   void setId(int a){ id = a;  }
   string getTypeStr() const { return ""; }
   unsigned getLineNo() const { return 0; }
   CirGate* gate() ;
   
   // traverse functions
   size_t getRef(){return _ref;	}
   void setRef(){_ref = _globalRef;	}
   void unsetRef(){ _ref--;  }
   static size_t getGlobalRef(){return _globalRef;	}
   static void setGlobalRef(){_globalRef++;	}
	

   // Printing functions
   void printGate() const ;
   void reportGate() const;
   void reportFanin(int level) const;
   void reportFanout(int level) const;
   int _gateType;
   CirGateV* _fanInList;
   vector<CirGateV> _fanOutList;
   string _name;
   int same_in_count ;
   int same_out_count;


private:
	int id;
    size_t _ref;
    static size_t _globalRef;  
    
    
protected:

};





	


#endif // CIR_GATE_H
