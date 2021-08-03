/****************************************************************************
  FileName     [ dlist.h ]
  PackageName  [ util ]
  Synopsis     [ Define doubly linked list package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef DLIST_H
#define DLIST_H

#include <cassert>

template <class T> class DList;

// DListNode is supposed to be a private class. User don't need to see it.
// Only DList and DList::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class DListNode
{
   friend class DList<T>;
   friend class DList<T>::iterator;

   DListNode(const T& d, DListNode<T>* p = 0, DListNode<T>* n = 0):
      _data(d), _prev(p), _next(n) {}

   // [NOTE] DO NOT ADD or REMOVE any data member
   T              _data;
   DListNode<T>*  _prev;
   DListNode<T>*  _next;
};


template <class T>
class DList
{
public:
   // TODO: decide the initial value for _isSorted
   DList() {
      _head = new DListNode<T>(T());
      _head->_prev = _head->_next = _head; // _head is a dummy node
   }
   ~DList() { clear(); delete _head; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class DList;

   public:
      iterator(DListNode<T>* n= 0): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return _node->_data; }
      T& operator * () { return _node->_data; }
      iterator& operator ++ () {  _node = _node->_next; return *(this); }
      iterator operator ++ (int) { iterator i(_node); _node = _node->_next; return i; }
      iterator& operator -- () { _node = _node->_prev; return *(this); }
      iterator operator -- (int) {  iterator i(_node);_node = _node->_prev; return i; }

      iterator& operator = (const iterator& i) { return *(this); }

      bool operator != (const iterator& i) const { return _node != i._node? true : false; }
      bool operator == (const iterator& i) const { return _node == i._node? true : false; }

   private:
      DListNode<T>* _node;
   };

   // TODO: implement these functions
   iterator begin() const { iterator i(_head); return i; }
   iterator end() const { iterator i(_head->_prev); return i; }
   bool empty() const { return _head->_prev == _head ? true : false; }
   size_t size() const {  
        DListNode<T>* tmp = _head; 
        for(size_t i=0;;i++){           
   	       if(tmp == _head-> _prev) return i;
   	       tmp = tmp->_next;
        }; 
   }

   void push_back(const T& x) {    
        DListNode<T>* newNode = new DListNode<T>(T()) ; 
		  
      if(empty()) {      	
      	 newNode->_prev = newNode->_next = _head;
      	 _head->_prev = _head -> _next = newNode;
      	 _head = newNode;
	  }
	  else{
	  	newNode->_prev = end()._node->_prev;         
        (newNode->_prev)->_next = newNode; 
        end()._node->_prev = newNode; 
	    newNode->_next = end()._node;	    
	  }
      	   newNode->_data = x;
   }
   void pop_front() {
     if(!empty()){
	   (_head->_prev)->_next = _head->_next;
       (_head-> _next)->_prev = _head->_prev;
     
       delete _head;
       _head = _head -> _next;
	 }
       
     
     
   }
   void pop_back() {
   	if(!empty()){
   	  DListNode<T>* tmp = end()._node->_prev;
   	  if(tmp == _head) {
      	_head = end()._node;
      	_head->_prev = _head->_next = _head;
      }
      else{
      //	cerr<<"fuck"<<endl;
      	  end()._node->_prev = tmp->_prev;
          (tmp->_prev)->_next = end()._node;
	  }     
    
       delete tmp;	
	}
   	  
   }

   // return false if nothing to erase
   bool erase(iterator pos) { 
       if(empty()) return false; 
       DListNode<T>* tmp = pos._node;
       if(tmp->_prev == tmp->_next){
       	_head->_next = _head = _head->_prev;
       	_head->_prev = _head;
	   }
	   else{
	   	if(tmp == _head){
	   		 _head = tmp->_next;
		   }
	   	tmp->_prev->_next = tmp->_next;
	   tmp->_next->_prev = tmp-> _prev; 
	   }
	   	
		delete tmp;   
		return true;   
   }
   bool erase(const T& x) {
      if(empty()) return false; 
      iterator it = begin();
      while(it != end()){
      	 if(*it == x) {
      	    erase(it);
      	    return true;
		 }
		 ++it;
	  }
	  return false;
   }

   void clear() {
   	  DListNode<T>* _current = _head;
   	  DListNode<T>* _dummy = _head->_prev;
      while(_current != _dummy){
	     DListNode<T>* tmp = _current;
	     _current = _current->_next;
	     delete tmp;
	  }
	  _head = _dummy;
	  _head->_prev = _head->_next = _head;
   }  // delete all nodes except for the dummy node

   void sort() const {
   	    
       for(iterator it = begin();it != end();it++){
       	   for(iterator j=it; j!= begin() ; j--) {
		      iterator k = j;
		      k--;
		      if(*j < *k ){
		      	 T tmp;
		      	 tmp = *j;
		      	 *j = *k;
		      	 *k = tmp;
			  }
			  else break;  
		   }
	   }
   }

private:
   // [NOTE] DO NOT ADD or REMOVE any data member
   DListNode<T>*  _head;     // = dummy node if list is empty
   mutable bool   _isSorted; // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] helper functions; called by public member functions
};

#endif // DLIST_H
