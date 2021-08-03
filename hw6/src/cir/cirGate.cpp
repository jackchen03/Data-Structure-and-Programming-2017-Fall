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
#include<cmath>

using namespace std;

extern CirMgr *cirMgr;

// TODO: Implement memeber functions for class(es) in cirGate.h

/**************************************/
/*   class CirGate member functions   */
/**************************************/
void
CirGate::reportGate() const
{
	cout << "==================================================" << endl;
    cout << "= ";
        string str;
		 switch(_gateType){
		 	case PI_GATE:
		 		cout << "PI" ;  str ="PI"; break;
		 	case PO_GATE :
		 		cout << "PO" ;  str = "PO"; break;
		 	case AIG_GATE :
		 		cout << "AIG" ; str = "AIG"; break;
		 }
		 cout << "("<< id << ")" ;
		 str+= "()";
		 
		 int num = 0;
		 while( id/pow(10, num) >= 1) num++;
		 for(int i=0;i<num;i++ ) str+= " "; 
		 if((_gateType == PI_GATE || _gateType == PO_GATE) && _name.size()!=0){
		 	cout << " \" " << _name << " \" ";   
			 str+= " \" ";
			 str += _name;
			 str+= " \" ";
		 }   
		 cout <<", ";
		 str+= ", ";

		 int index = 0;
		 while(cirMgr->_idList[index] != id) index ++;
		 cout << "line " << index ;
		 str += "line ";
		 int n = 0;
		 while( index/pow(10, n) >= 1) n++;
		 for(int i=0;i<n;i++ ) str+= " "; 
		 for(int i=0;i< 47-str.size();i++) cout << " ";
		 cout << "=" << endl;
		 
         cout << "==================================================" << endl;
}

void
CirGate::reportFanin(int level) const
{
   assert (level >= 0);

  size_t now_ref = CirGate::getGlobalRef();

  	 GateList trace;
  	 CirGate::setGlobalRef();

	 CirGate* _cur = (CirGate*)this;
	 trace.push_back(_cur);
	 switch(_cur->_gateType){
				 case PI_GATE :
					cout << "PI " << _cur->getId() << endl;  break;
                 case PO_GATE :
					cout << "PO " << _cur->getId() << endl;  break;
				 case UNDEF_GATE :
					cout << "UNDEF " << _cur->getId() << endl; break;
			 	 case CONST_GATE :
					 cout << "CONST0" << endl; break;
                 case AIG_GATE :
                    cout << "AIG " << _cur->getId() << endl; break;
    }
   for(int i=0;i<2;i++){
   	  if(_cur->_gateType == CONST_GATE || _cur->_gateType == PI_GATE) break;
   	 do{    int count = 0;
   	 	   int ori_ref = _cur->getRef();
	 	 if(_cur->_gateType == UNDEF_GATE || _cur->_gateType == PI_GATE || _cur->_gateType == CONST_GATE || level == trace.size() -1 || (ori_ref > now_ref && _cur->_gateType == AIG_GATE)){
	 	
             if(level == 0) break;
	 	 	
	 	 	_cur->setRef();

		    trace.pop_back();
		    _cur = trace.at(trace.size()-1);
		    
		 }
	 	 else if(_cur->_fanInList[0].gate()->getRef() < CirGate::getGlobalRef()){  // not yet traverse
	 	
	 	 	bool inv = _cur->_fanInList[0].isInverted();
		    _cur = _cur->_fanInList[0].gate();
		    for(int i=0;i<trace.size();i++) cout << "  " ;
		    if(inv) cout<< "!" ;
		    switch(_cur->_gateType){
				 case PI_GATE :
					cout << "PI " << _cur->getId() << endl;  break;
                 case PO_GATE :
					cout << "PO " << _cur->getId() << endl;  break;
				 case UNDEF_GATE :
					cout << "UNDEF " << _cur->getId() << endl; break;
			 	 case CONST_GATE :
					 cout << "CONST0" << endl;  break;
                 case AIG_GATE :
                    cout << "AIG " << _cur->getId() ;
					 if(_cur->getRef() > now_ref ) cout << " (*)" ;
					 cout << endl; break;
			  }

		    trace.push_back(_cur);
	     }
	     else if(_cur->_gateType == AIG_GATE){
            if (_cur->_fanInList[1].gate()->getRef() < CirGate::getGlobalRef() || ((_cur->_fanInList[1].gate() == _cur->_fanInList[0].gate()) &&( _cur->same_in_count < CirGate::getGlobalRef()))){  // already traverse
               
                    bool in_same = 0;
                	if(_cur->_fanInList[1].gate() == _cur->_fanInList[0].gate()) {
                		_cur->same_in_count = CirGate::getGlobalRef();
                		in_same= 1;
					}
                	bool inv = _cur->_fanInList[1].isInverted();
                _cur = _cur->_fanInList[1].gate();
                for(int i=0;i<trace.size();i++) cout << "  " ;
                if(inv) cout<< "!" ;
		       switch(_cur->_gateType){
				 case PI_GATE :
					cout << "PI " << _cur->getId() << endl;  break;
                 case PO_GATE :
					cout << "PO " << _cur->getId() << endl;  break;
				 case UNDEF_GATE :
					cout << "UNDEF " << _cur->getId() << endl; break;
			 	 case CONST_GATE :
					cout << "CONST0" << endl;  break;
                 case AIG_GATE :
                    cout << "AIG " << _cur->getId() ;
					 if(_cur->getRef() > now_ref && !in_same) cout << " (*)" ;
					 cout << endl; break;
			  }
		       
		       trace.push_back(_cur);
	        }
	        else {
	     	
	     	  int ori_ref = _cur->getRef();
	     	  _cur->setRef();
	     	  for(int i=0;i<2;i++){
	     	    if( _cur->_fanInList[i].gate()->_gateType == UNDEF_GATE ||  _cur->_fanInList[i].gate()->_gateType == CONST_GATE ||  _cur->_fanInList[i].gate()->_gateType == PI_GATE)
	     	      _cur->_fanInList[i].gate()->unsetRef();
			  }

		      trace.pop_back();
		      _cur = trace.at(trace.size()-1);
		   }
	     }
	    
		 
		 
	 } while(_cur != this);
	 if(this->_gateType != AIG_GATE) break;
   } 
	 



}

void
CirGate::reportFanout(int level) const
{
   assert (level >= 0);
   
  size_t now_ref = CirGate::getGlobalRef();

  	 GateList trace;
  	 CirGate::setGlobalRef();

	 CirGate* _cur = (CirGate*)this;
	 trace.push_back(_cur);
	 switch(_cur->_gateType){
				 case PI_GATE :
					cout << "PI " << _cur->getId() << endl;  break;
                 case PO_GATE :
					cout << "PO " << _cur->getId() << endl;  break;
				 case UNDEF_GATE :
					cout << "UNDEF " << _cur->getId() << endl; break;
			 	 case CONST_GATE :
					cout << "CONST0" << endl;  break;
                 case AIG_GATE :
                    cout << "AIG " << _cur->getId() << endl; break;
    }
   for(int a=0;a<this->_fanOutList.size();a++){
   	  if(_cur->_gateType == PO_GATE) break;
   	 do{   // int count = 0;
   	 	   int ori_ref = _cur->getRef();
	 	 if(_cur->_gateType == PO_GATE || level == trace.size() -1 || (ori_ref > now_ref && _cur->_gateType == AIG_GATE)){
	 	
             if(level == 0) break;
	 	 	
	 	 	_cur->setRef();

		    trace.pop_back();
		    _cur = trace.at(trace.size()-1);
		    
		 }
	 	 else if(_cur->_fanOutList.at(0).gate()->getRef() < CirGate::getGlobalRef()){  // not yet traverse
	 	
	 	 	bool inv = _cur->_fanOutList.at(0).isInverted();
		    _cur = _cur->_fanOutList.at(0).gate();
		    for(int i=0;i<trace.size();i++) cout << "  " ;
		    if(inv) cout<< "!" ;
		    switch(_cur->_gateType){
				 case PI_GATE :
					cout << "PI " << _cur->getId() << endl;  break;
                 case PO_GATE :
					cout << "PO " << _cur->getId() << endl;  break;
				 case UNDEF_GATE :
					cout << "UNDEF " << _cur->getId() << endl; break;
			 	 case CONST_GATE :
					cout << "CONST0" << endl;break;
                 case AIG_GATE :
                    cout << "AIG " << _cur->getId() ;
					 if(_cur->getRef() > now_ref ) cout << " (*)" ;
					 cout << endl; break;
			  }

		    trace.push_back(_cur);
	     }
	     else{
		   if(_cur->_fanOutList.size()>1 ){
		   	     bool all_traverse = 1;
		   	     int idx;
	             for(size_t i=1;i<_cur->_fanOutList.size();i++){
				    if(_cur->_fanOutList.at(i).gate()->getRef() <= now_ref ){
				    	all_traverse = 0;
				    	idx = i;
				    	break;
					}
				 } 
	        	 bool out_same = 0;
	        	 for(size_t j=idx-1;j>0;j--){
	        	 	if(_cur->_fanOutList.at(idx).gate() == _cur->_fanOutList.at(j).gate()) {
	        	 		out_same = 1; break;
					}
				 } 
	        	 if (all_traverse != 1 || (out_same == 1 &&( _cur->same_out_count < CirGate::getGlobalRef()))){  // already traverse
               
                    
                	if(out_same == 1) {
                		_cur->same_out_count = CirGate::getGlobalRef();
                		
					}
                	bool inv = _cur->_fanOutList.at(idx).isInverted();
                   _cur = _cur->_fanOutList.at(idx).gate();
                   for(int i=0;i<trace.size();i++) cout << "  " ;
                   if(inv) cout<< "!" ;
		           switch(_cur->_gateType){
				     case PI_GATE :
					   cout << "PI " << _cur->getId() << endl;  break;
                     case PO_GATE :
				  	   cout << "PO " << _cur->getId() << endl;  break;
				     case UNDEF_GATE :
					   cout << "UNDEF " << _cur->getId() << endl; break;
			 	     case CONST_GATE :
					   cout << "CONST0" << endl;  break;
                     case AIG_GATE :
                       cout << "AIG " << _cur->getId() ;
					 if(_cur->getRef() > now_ref && !out_same) cout << " (*)" ;
					 cout << endl; break;
			       }
		       
		         trace.push_back(_cur);
	           }
	           else {
	     	      _cur->setRef();
	     	      for(int i=0;i<_cur->_fanOutList.size();i++){
	     	         if( _cur->_fanOutList.at(i).gate()->_gateType == PO_GATE)
	     	         _cur->_fanOutList.at(i).gate()->unsetRef();
			      }

		          trace.pop_back();
		          _cur = trace.at(trace.size()-1);
		       }  
			
		
		   }
           
	        else {
	     	   _cur->setRef();
	     	   for(int i=0;i<_cur->_fanOutList.size();i++){
	     	      if( _cur->_fanOutList.at(i).gate()->_gateType == PO_GATE)
	     	      _cur->_fanOutList.at(i).gate()->unsetRef();
			   }

		       trace.pop_back();
		       _cur = trace.at(trace.size()-1);
		   }   
		 } 
	     
	    
		 
		 
	 } while(_cur != this);
	 
   } 
}

