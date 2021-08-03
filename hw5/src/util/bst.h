/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef BST_H
#define BST_H

#include <cassert>

using namespace std;

template <class T> class BSTree;

// BSTreeNode is supposed to be a private class. User don't need to see it.
// Only BSTree and BSTree::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class BSTreeNode
{   
    friend class BSTree<T>;
    friend class BSTree<T>::iterator;
    
    BSTreeNode(const T& d, BSTreeNode<T>* p = 0, BSTreeNode<T>* q = 0, BSTreeNode<T>* n = 0 ):
      _data(d), _left(p), _right(q), _parent(n) {}
    
    BSTreeNode<T>* _left;
    BSTreeNode<T>* _right;
    BSTreeNode<T>* _parent;
    T _data;
   // TODO: design your own class!!
};


template <class T>
class BSTree
{
   // TODO: design your own class!!
  public :
   BSTree(){ 
      root = 0;
      _tail = new BSTreeNode<T>(T());     
   }
   ~BSTree(){ clear();   } 	
   	
   class iterator {
   	 friend class BSTree; 
    
	 public:
      iterator(BSTreeNode<T>* n= 0): _node(n) {}
      iterator(const iterator& i): _node(i._node) {}
   
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return _node->_data; }
      T& operator * () { return _node->_data; }
      iterator& operator ++ () { 
         _node = this->successor()._node;
		 return (*this);	     
	  }
      iterator operator ++ (int) { 
	      iterator i (_node) ; 
		  _node = this->successor()._node;
		  return (i);
	  }
      iterator& operator -- () {
         if(_node->_left){
   	  	    BSTreeNode<T>* _current = _node->_left ;
            while(_current->_right != 0){ _current = _current -> _right;  }        
	        _node = _current; 	  
         }    
     
	     else{  //no right child
	  	    BSTreeNode<T>* _current = _node;
	  	    while(_current){		   
		      if(_current->_parent->_right == _current){
		   	       _node = _current->_parent;
		   	       break;
		      }
		      _current = _current->_parent;
		    }
	     }
	      
		  return (*this);
	 }
      iterator operator -- (int) {
      	
     // 	cerr<<"in"<<endl;
	      iterator i (_node) ; 
		  if(_node->_left){
   	  	    BSTreeNode<T>* _current = _node->_left ;
            while(_current->_right != 0){ _current = _current -> _right;  }        
	        _node = _current; 	  
         }    
     
	     else{  //no left child
	  	    BSTreeNode<T>* _current = _node;
	  	    while(_current->_parent){		   
		      if(_current->_parent->_right == _current){
		   	       _node = _current->_parent;
		   	    //   cerr << "yep" <<  "node ->_data = " << _node->_data << endl;
		   	       break;
		      }
		      _current = _current->_parent;
		    }
	     }
		  return (i);
	  }

      iterator operator + (int i) const { 
	      iterator it(_node); 
	      for(size_t j=0;j<i;j++) it = successor(it);
		  return (*it); 
      }
      iterator& operator += (int i) { 
	     for(int j=0;j<i;j++) *this = successor();
	     return (*this);
	  }
	  
	  iterator successor() const{
   	     if(_node->_right){
   	  	    BSTreeNode<T>* _current = _node->_right ;
            while(_current->_left != 0){ _current = _current -> _left;  }        
	        return iterator (_current); 	  
         }    
     
	     else{  //no right child
	  	    BSTreeNode<T>* _current = _node;
	  	    while(_current){		   
		      if(_current->_parent->_left == _current){
		   	       return iterator(_current->_parent); 
		      }
		      _current = _current->_parent;
		    }
		    return 0;
	     }
      
      }

      iterator& operator = (const iterator& i) { return (*this); }

      bool operator != (const iterator& i) const { return i._node != _node ? true : false; }
      bool operator == (const iterator& i) const { return i._node == _node ? true : false; }

     private:
        BSTreeNode<T>*   _node;
   };
   
   
   iterator begin() const { 
       if(empty()) return _tail;
       BSTreeNode<T>* _current = root ;
	   while(_current->_left != 0){	 _current = _current -> _left;  }
	   iterator i(_current);
	   return i; 	   
   }
   iterator end() const {
	   return _tail; 
   }
   bool empty() const { return root == 0 ? true : false; }
   size_t size() const { int j = 0; for(iterator i=begin();i!=end();i++) j++; return j; }
     
   void insert(const T& a){
   	  if(empty()){
	     root = new BSTreeNode<T>(a);
	     root -> _right = _tail;
	     _tail -> _parent = root;
	  }
	  else{
	
	  	BSTreeNode<T>* x = new BSTreeNode<T>(a);
   	    BSTreeNode<T>* _curr = root;
   	    while( true ){
   	    	
	      if(a > _curr->_data){   //bigger
	        
	      	 if(_curr->_right == _tail ){
	   	        _curr->_right = x;
			    x->_right = _tail;
			    _tail->_parent = x;  
				break;	
		     }
	      	 else if(_curr->_right)  _curr = _curr->_right;	      	  
		     else{
		     	  _curr->_right = x;  
				   break;
			 }
		  }  
	      else {      // smaller
	       
	      	 if(_curr->_left) _curr = _curr->_left;
	      	 else {
	      	 	_curr->_left = x; 	   break;
			 }
		  }
	    } 
	
	    x->_parent = _curr;
	  }   	   
   } 
   void pop_front(){
   	if(!empty()){
   	   BSTreeNode<T>* tmp = begin()._node;
   	   if(begin()._node->_right != 0){
		 if(begin()._node->_right == _tail) {
		 	root =0; _tail->_parent = 0;
		 }
		 else if(tmp == root){
		    root = tmp->_right;
			tmp->_right->_parent = 0;				
		 }  	  	 
		 else{
		 	(begin()._node->_parent)->_left = begin()._node->_right;
   	  	    (tmp->_right)->_parent = tmp->_parent;
		 }  	  	      
	   }
       else {
      	 if(tmp == root) root =0;
      	 else (begin()._node->_parent)->_left = 0;
	   }
       delete tmp;	
	}
   	  

   }
   void pop_back(){
   	  if(!empty()) {
		BSTreeNode<T>* tmp = _tail->_parent;
   	   	if(tmp->_left != 0){
   	   		if(tmp->_parent) {
			   tmp->_parent->_right = tmp->_left;
			   tmp->_left->_parent = tmp->_parent;
			}   	   	    
   	   		else {
   	   			tmp->_left->_parent = 0; root = tmp->_left;
			}
			BSTreeNode<T>* _curr = tmp->_left;
			while(_curr ->_right) {
				_curr = _curr->_right;
			}
   	   		_curr->_right = _tail;
			_tail->_parent =  _curr;	   	
        } 
        else{
        	if(tmp ->_parent){
        		_tail ->_parent = (_tail ->_parent)->_parent;
        		(tmp ->_parent)->_right = _tail;
			} 
        	else {
        		root = 0; _tail->_parent = 0;
			}
            
		}
       
         delete tmp;
	 }
   	    
   	   	
   }
   bool erase(iterator pos){
   	if(empty())   return false;
       if(pos._node->_left &&pos._node-> _right){  // 2 children
         if(pos._node->_right == _tail) pop_back();
		 else{
           BSTreeNode<T>* del = pos.successor()._node;         //the successor's node(the node to be deleted)
		   *pos = *(pos.successor());
           if(del->_right){
           	  if(pos.successor()._node->_parent ->_right ==pos.successor()._node ){
			     (del->_parent) ->_right = del->_right;
			     (del->_right)->_parent = del->_parent;
			  }
			  else{
			     del->_parent ->_left = del->_right;
			     del->_right->_parent = del->_parent;
			  }      	  
           	  
		   }
		   else {
		   	   
		   	   if(pos.successor()._node->_parent ->_right == pos.successor()._node) del->_parent ->_right = 0;
		   	   else   	    pos.successor()._node->_parent ->_left = 0;
		   }  
		     
	       delete del;
		 }
           
	   }
	   else if(pos._node->_left){  // 1 child
	     if(pos._node == root){
	     	pos._node->_left->_parent = 0;	       
			root = 	pos._node->_left;
		 }
		 else{
		    if((pos._node->_parent)->_right == pos._node)  (pos._node->_parent)->_right = pos._node->_left;
		    else (pos._node->_parent)->_left = pos._node->_left;
		    (pos._node->_left)->_parent = pos._node->_parent;
		 }
	   	   delete pos._node;
	   }
	   else if(pos._node->_right){   // 1 child
	     if(pos._node->_right == _tail) {
	        pop_back();
		 }
	     else{
	     	if(pos._node == root){
	       	  pos._node->_right->_parent = 0;	       
			  root = pos._node->_right;
		    }
	        else{
	          if((pos._node->_parent)->_right == pos._node)  (pos._node->_parent)->_right = pos._node->_right;
		      else (pos._node->_parent)->_left = pos._node->_right;
		      (pos._node->_right)->_parent = pos._node->_parent;	
		    }  
	   	     delete pos._node;
		 } 
	   }
	   else{     // 0 child
	     if(pos._node == root){
	     	root = 0;
		 }
		 else{
		   if((pos._node->_parent)->_right == pos._node) {		      
			  (pos._node->_parent)->_right = 0;
		   } 
		   else (pos._node->_parent)->_left = 0;	
		 }   
		 delete pos._node;	   		   
	   }
	   
	   return true;
   }
   bool erase(const T& x){
   	if(empty())   return false;
      for(iterator it=begin();it!=end();it++){
      	 if(*it == x){
		   erase(it); return true;
		 } 
	  }
	  return false;
      
   }
      
   void clear(){
     while(_tail->_parent) pop_front();
   }
   void sort() const{;}  // no need
   void print() const{;}
   
   private :
   	  BSTreeNode<T>* root;
   	  BSTreeNode<T>* _tail;
};

#endif // BST_H
