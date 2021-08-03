/****************************************************************************
  FileName     [ myHashSet.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashSet ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_SET_H
#define MY_HASH_SET_H

#include <vector>

using namespace std;

//---------------------
// Define HashSet class
//---------------------
// To use HashSet ADT,
// the class "Data" should at least overload the "()" and "==" operators.
//
// "operator ()" is to generate the hash key (size_t)
// that will be % by _numBuckets to get the bucket number.
// ==> See "bucketNum()"
//
// "operator ==" is to check whether there has already been
// an equivalent "Data" object in the HashSet.
// Note that HashSet does not allow equivalent nodes to be inserted
//
template <class Data>
class HashSet
{
public:
   HashSet(size_t b = 0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashSet() { reset(); }

   // TODO: implement the HashSet<Data>::iterator
   // o An iterator should be able to go through all the valid Data
   //   in the Hash
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashSet<Data>;
      
   public:
   	
   	  iterator(const HashSet<Data>* h, size_t p, size_t q) : _hash(h), _bucketIdx(p), _slotIdx(q){	  }
      ~iterator(){;  }
   	
      const Data& operator * () const {  

	     return (*_hash)[_bucketIdx].at(_slotIdx); 
	  }
      iterator operator ++(int) {
	     iterator i(_hash, _bucketIdx, _slotIdx);
	     _bucketIdx++;
	     if((*_hash)[_bucketIdx].size() != 0){
	     	if(_slotIdx >= (*_hash)[_bucketIdx].size()-1){
			   _bucketIdx++;
			   _slotIdx = 0;
		    }
		    else _slotIdx++;
		 }
      	 else while((*_hash)[_bucketIdx].size() == 0) _bucketIdx++;
		    
		    return i;
	  }
      iterator& operator ++ () {
      	 _bucketIdx++;
      	 if((*_hash)[_bucketIdx].size() != 0){
	     	if(_slotIdx >= (*_hash)[_bucketIdx].size()-1){
			   _bucketIdx++;
			   _slotIdx = 0;
		    }
		    else _slotIdx++;
		 }
      	 else while((*_hash)[_bucketIdx].size() == 0) _bucketIdx++;
		 
		 return (*this);
	  }
	  iterator operator --(int) {
	     iterator i(_bucketIdx, _slotIdx);
	     _bucketIdx--;
	     if((*_hash)[_bucketIdx].size() != 0){
		      if(_slotIdx == 0){
                if(_bucketIdx == 0) _bucketIdx == _numBuckets;
                else _bucketIdx--;
		     }
		     else _slotIdx--;
		 }
	     else while((*_hash)[_bucketIdx].size() == 0) _bucketIdx--;
		 return i;
	  }
	  
	  iterator& operator --() {
	  	_bucketIdx--;
	    if((*_hash)[_bucketIdx].size() != 0){
		      if(_slotIdx == 0){
                if(_bucketIdx == 0) _bucketIdx == _numBuckets;
                else _bucketIdx--;
		     }
		     else _slotIdx--;
		 }
	     else while((*_hash)[_bucketIdx].size() == 0) _bucketIdx--;	 
		
		 return (*this);
	  }
	  
      bool operator != (const iterator& i) const {  
	     if(i._bucketIdx == _bucketIdx && i._slotIdx == _slotIdx) return false;
		 else return true;
	  }
	  bool operator == (const iterator& i) const {  
	      if(i._bucketIdx == _bucketIdx && i._slotIdx == _slotIdx) return true;
		 else return false;
	  }
	  
	  iterator& operator = (const iterator& i){ 
	     _bucketIdx = i._bucketIdx;
	     _slotIdx = i._slotIdx;
	     _hash = i._hash;
	     return (*this); 
	  }
	 
	  // access function
	  size_t getBucketIdx(){ return _bucketIdx;	  }
	  size_t getSlotIdx(){ return _slotIdx;	  }
   private:
   	  size_t _bucketIdx;
   	  size_t _slotIdx;
   	  const HashSet<Data>* _hash;
   };

   void init(size_t b) { 
      _numBuckets = b; 
	  _buckets = new vector<Data>[b+1];  
	  _buckets[b].push_back(Data("end",0));
   }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<Data>& operator [] (size_t i) { return _buckets[i]; }
   const vector<Data>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
   // Point to the first valid data
   iterator begin() const { 
       if(!empty()){
       	   int idx = 0; 
		   while(_buckets[idx].size() == 0) idx++;
		   return iterator(this, idx, 0);
	   }
   }
   // Pass the end
   iterator end() const { return iterator(this, _numBuckets, 0); }
   // return true if no valid data
   bool empty() const { 
      for(int i=0;i<_numBuckets;i++){
      	 if(_buckets[i].size()!=0){
      	    return false;
		 }
	  }
     return true;
   }
   // number of valid data
   size_t size() const { 
      size_t s = 0; 
	  
	  for(int i=0;i<_numBuckets;i++){
	  	 s += _buckets[i].size();
	  }
	  return s;
   }

   // check if d is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const Data& d) const {  
      for(int i=0;i<_buckets[bucketNum(d)].size();i++){
	  	 if(_buckets[bucketNum(d)].at(i) == d) return true;
	  } 
	  return false;
   }

   // query if d is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(Data& d) const {
     for(int i=0;i<_buckets[bucketNum(d)].size();i++){
	  	 if(_buckets[bucketNum(d)].at(i) == d){
	  	 	d = _buckets[bucketNum(d)].at(i);     return true;
		 } 
	  }
	  return false;
   }

   // update the entry in hash that is equal to d (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const Data& d) { 
      for(int i=0;i<_buckets[bucketNum(d)].size();i++){
	  	 if(_buckets[bucketNum(d)].at(i) == d){
	  	 	_buckets[bucketNum(d)].at(i) == d = d;     return true;
		 } 
	  }
      _buckets[bucketNum].push_back(d);
	  return false;

   }

   // return true if inserted successfully (i.e. d is not in the hash)
   // return false is d is already in the hash ==> will not insert
   bool insert(const Data& d) {
      
	  for(int i=0;i<_buckets[bucketNum(d)].size();i++){
	  	if(_buckets[bucketNum(d)].at(i) == d)
	  	    return false;
	  } 
      
	  
	  	 _buckets[bucketNum(d)].push_back(d);
	  	 return true;
	   
   }

   // return true if removed successfully (i.e. d is in the hash)
   // return fasle otherwise (i.e. nothing is removed)
   bool remove(const Data& d) { 

      for(int i=0;i<_buckets[bucketNum(d)].size();i++){
	  	if(_buckets[bucketNum(d)].at(i) == d){
		    _buckets[bucketNum(d)].pop_back(); return true;
		}
	  } 
	  return false;
   }

private:
   // Do not add any extra data member
   
   size_t            _numBuckets;
   vector<Data>*     _buckets;

   size_t bucketNum(const Data& d) const {
      return (d() % _numBuckets); }
};

#endif // MY_HASH_SET_H
