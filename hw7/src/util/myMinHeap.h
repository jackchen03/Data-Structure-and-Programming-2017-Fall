/****************************************************************************
  FileName     [ myMinHeap.h ]
  PackageName  [ util ]
  Synopsis     [ Define MinHeap ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_MIN_HEAP_H
#define MY_MIN_HEAP_H

#include <algorithm>
#include <vector>

template <class Data>
class MinHeap
{
public:
   MinHeap(size_t s = 0) { if (s != 0) _data.reserve(s); }
   ~MinHeap() {}

   void clear() { _data.clear(); }

   // For the following member functions,
   // We don't respond for the case vector "_data" is empty!
   const Data& operator [] (size_t i) const { return _data[i]; }   
   Data& operator [] (size_t i) { return _data[i]; }

   size_t size() const { return _data.size(); }

   // TODO
   const Data& min() const { return _data[0]; }
   void insert(const Data& d) { 
      int n = _data.size()+1;
      _data.push_back(d);
      while(n > 1){
      	 int p = n/2;
      	 if(_data[p-1] < _data[n-1]){
      	    break;
		 } 
		 Data tmp;
		 tmp = _data[n-1];
      	 _data[n-1] = _data[p-1];
      	 _data[p-1] = tmp;
      	 n = p;
	  }
	 
	  
   }
   void delMin() { 
      int n = 0 ;  
      int p = n;
      while(n*2+1 <= _data.size()-1){
      	 if(n*2+2 <= _data.size()-1) {   // has right child
		    if(_data[n*2+2] < _data[n*2+1]) n = n*2+2;
      	    else n = n*2+1;
		 }
		 else n = n*2+1;
      	 
		 _data[p] = _data[n];
		 p = n;    
	  }
	  if(n != _data.size()-1) _data[n] = _data[_data.size()-1];
	  _data.pop_back();
   }
   void delData(size_t i) {
      if(i == 0) delMin();
      else{
      	 int p = i;
       	 while(i*2+1 <= _data.size()-1){
      	  	if(i*2+2 <= _data.size()-1) {   // has right child
		       if(_data[i*2+2] < _data[i*2+1]) i = i*2+2;
      	       else i = i*2+1;
		    }
		    else i = i*2+1;
		    
		    _data[p] = _data[i];
		    p = i; 
		 }
      	 if(i != _data.size()-1) _data[i] = _data[_data.size()-1];
	     _data.pop_back();
	  }
   }

private:
   // DO NOT add or change data members
   vector<Data>   _data;
};

#endif // MY_MIN_HEAP_H
