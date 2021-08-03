/****************************************************************************
  FileName     [ dbTable.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Table member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iomanip>
#include <string>
#include <cctype>
#include <cassert>
#include <set>
#include <algorithm>
#include<climits>
#include "dbTable.h"
#include "util.h"

using namespace std;

/*****************************************/
/*          Global Functions             */
/*****************************************/
ostream& operator << (ostream& os, const DBRow& r)
{
   // TODO: to print out a row.
   // - Data are seperated by a space. No trailing space at the end.
   // - Null cells are printed as '.'
   for(int i=0;i<r.size();i++){
   	  if(r[i] == INT_MAX) os << "." ;
   	  else os << r[i] ;
   	  if(i!=r.size()-1) os << " ";
   }
   return os;
}

ostream& operator << (ostream& os, const DBTable& t)
{
   // TODO: to print out a table
   // - Data are seperated by setw(6) and aligned right.
   // - Null cells are printed as '.'
   for(int i=0; i< t.nRows(); i++){
   	   for(int j=0; j< t[i].size(); j++){
   	   	  if(t[i][j] == INT_MAX) os  << setw(6) << right <<  ".";
   	   	  else os << setw(6) << right <<  t[i][j];
   	   	  if(j!=t[i].size()-1) os << " ";
	   }
	   os << endl;
   }
   
   return os;
}

ifstream& operator >> (ifstream& ifs, DBTable& t)
{
   // TODO: to read in data from csv file and store them in a table
   // - You can assume the input file is with correct csv file format
   // - NO NEED to handle error file format
  
      

       int length = 0;
       bool getn = 0, getr = 0;
       char getting;
       while(true){   // counting array length
    	   getting = ifs.get();
    	   if(getting == '\n'|| getting == '\r') break;
    	   length ++ ;
	   }
	     if(getting == '\n') getn = 1;
         else  getr = 1;

	   ifs.seekg (0, ifs.beg);  // return to the beginning
	   char* Array = new char[length];
	   int commanumber = 0;
	   for(int i=0;i<length;i++){     //counting column numbers
		  char getting2;
    	  getting2 = ifs.get();
    	  if(getting2 == ',' ) commanumber++;
	   }
       int colNum = commanumber+1;  // Column


       ifs.seekg (0, ifs.beg);  // return to the beginning
       bool lastoneiscomma = 0;
       


       if(getr==1) {  // condition 1  \n \r
          while((ifs.eof()!=1)){  // end when file ends
           DBRow newRow;  // Row object          
           string tmpstring;
           getline(ifs,tmpstring,'\r');  //get one row from ifs
           char dump = ifs.get();
           tmpstring += ',';

           int tmpstringindex = 0;   // index of tmpstring
           for(int i=0;i<colNum ;i++){
              string stringbtwcommas;
              int btwcommaindex = 0;
              bool emp = 0;  // to see if this element is empty (have to continue)
                   
              while(true){
                 if(btwcommaindex == 0 && tmpstring.at(tmpstringindex) == ',' ) {
                 	newRow.addData(INT_MAX);
                 	emp= 1;
				 } 
                 if(tmpstring.at(tmpstringindex) == ','){
                    tmpstringindex++;
                    break;
                 }
                 stringbtwcommas += tmpstring.at(tmpstringindex);
                 tmpstringindex++;
                 btwcommaindex++;
              }             
              if(emp == 1) continue ;  
                
              int number = 0;
              
              if(stringbtwcommas.at(0) == '-'){   // <0
                 int expo = stringbtwcommas.size() - 2;
                 for(int i=1 ; i<stringbtwcommas.size() ; i++){
                    number += pow(10,expo)*(stringbtwcommas.at(i)-'0');
                    expo -- ;
                 }
                 newRow.addData(0- number);
              }
              else{   //>0
                 int expo = stringbtwcommas.size() - 1;
                 for(int i=0 ; i<stringbtwcommas.size() ; i++){
                     number += pow(10,expo)*(stringbtwcommas.at(i)-'0');
                     expo -- ;
                 }
                 newRow.addData(number);
              }


           }

           t.addRow(newRow);

       }
       }
       else if(getn==1){   // condition 3  \n
         while((ifs.eof()!=1)){  // end when file ends
           DBRow newRow;  // Row object
           string tmpstring;
           getline(ifs,tmpstring,'\n');  //get one row from myfile
           tmpstring += ',';

           int tmpstringindex = 0;   // index of tmpstring
           for(int i=0;i<colNum ;i++){
              string stringbtwcommas;
              int btwcommaindex = 0;
              bool emp =0;

              while(true){
                 if(btwcommaindex == 0 && tmpstring.at(tmpstringindex) == ',' ){
                 	newRow.addData(INT_MAX);
                 	emp = 1;
				 }  
                 if(tmpstring.at(tmpstringindex) == ','){
                    tmpstringindex++;
                    break;
                 }
                 stringbtwcommas += tmpstring.at(tmpstringindex);
                 tmpstringindex++;
                 btwcommaindex++;
              }
              if(emp == 1) continue ;

              int number = 0;
              if(stringbtwcommas.at(0) == '-'){   // <0
                 int expo = stringbtwcommas.size() - 2;
                 for(int i=1 ; i<stringbtwcommas.size() ; i++){
                    number += pow(10,expo)*(stringbtwcommas.at(i)-'0');
                    expo -- ;
                 }
                  newRow.addData(0- number);
              }
              else{   //>0
                 int expo = stringbtwcommas.size() - 1;
                 for(int i=0 ; i<stringbtwcommas.size() ; i++){
                     number += pow(10,expo)*(stringbtwcommas.at(i)-'0');
                     expo -- ;
                 }
                 newRow.addData(number);
              }

              
           }

           t.addRow(newRow);

       }
       }

    
   return ifs;
}

/*****************************************/
/*   Member Functions for class DBRow    */
/*****************************************/
void
DBRow::removeCell(size_t c)
{
   // TODO
   _data.erase(_data.begin()+c);
}

/*****************************************/
/*   Member Functions for struct DBSort  */
/*****************************************/
bool
DBSort::operator() (const DBRow& r1, const DBRow& r2) const    // true -> r1 > r2
{
   // TODO: called as a functional object that compares the data in r1 and r2
   //       based on the order defined in _sortOrder
   for(size_t i=0;i<_sortOrder.size();i++){
   	  if(r1[_sortOrder.at(i)]!=r2[_sortOrder.at(i)]){
   	  	  if(r1[_sortOrder.at(i)]>r2[_sortOrder.at(i)]) return true;
   	  	  else return false;
	  } 
   }
   
   // still have bug
}

/*****************************************/
/*   Member Functions for class DBTable  */
/*****************************************/
void
DBTable::reset()
{
   // TODO
   vector<DBRow> tmp; tmp.swap(_table);
}

void
DBTable::addCol(const vector<int>& d) 
{
   // TODO: add a column to the right of the table. Data are in 'd'.
   if(d.size() <= _table.size()) {
   	  for(size_t i=0;i<d.size();i++)  _table.at(i).addData(d.at(i));	
	  
	  for(size_t i=d.size();i<_table.size();i++) _table.at(i).addData(INT_MAX);
   }
   
   else {   //>=
   	  for(size_t i=0;i<_table.size();i++)  _table.at(i).addData(d.at(i));		  
   }
   
}

void
DBTable::delRow(int c)  //ok
{
   // TODO: delete row #c. Note #0 is the first row.
   _table.erase(_table.begin()+c);
}

void
DBTable::delCol(int c)  //ok
{
   // delete col #c. Note #0 is the first row.
   for (size_t i = 0, n = _table.size(); i < n; ++i)
      _table[i].removeCell(c);
}

// For the following getXXX() functions...  (except for getCount())
// - Ignore null cells
// - If all the cells in column #c are null, return NAN
// - Return "float" because NAN is a float.
float
DBTable::getMax(size_t c) const
{
   // TODO: get the max data in column #c
   
   if((*this).emptyColumn(c) == 1) return NAN ;
   else {
   	  bool emp = 1;
   	  size_t idx = 0;
   	  for(;idx<_table.size();idx++){   	     
		 if((*this)[idx][c] != INT_MAX){
		 	 emp =0;
		 	 break;
		 } 		
	  }
   	  int max = (*this)[idx][c];
   	  for(size_t i=0;i<_table.size();i++){
   	  	if((*this)[i][c] != INT_MAX){
		   if((*this)[i][c] > max) max = (*this)[i][c];
		}   	     
	  }
	  return max ;
   }
}

float
DBTable::getMin(size_t c) const
{
   // TODO: get the min data in column #c
   if((*this).emptyColumn(c) == 1) return NAN ;
   else {
   	  int min = (*this)[0][c];
   	  for(size_t i=0;i<_table.size();i++){
   	  	 if((*this)[i][c] != INT_MAX){
   	  	 	if((*this)[i][c] < min) min = (*this)[i][c];
		 }   	     
	  }
	  return min ;
   }
}

float 
DBTable::getSum(size_t c) const
{
   // TODO: compute the sum of data in column #c
   if((*this).emptyColumn(c) == 1) return NAN ;
   int sum =0;
   for(size_t i=0;i<_table.size();i++) {
   	 if((*this)[i][c] != INT_MAX) sum += (*this)[i][c];
   }
   return sum;
}

int
DBTable::getCount(size_t c) const
{
   // TODO: compute the number of distinct data in column #c
   // - Ignore null cells
   vector<int> dist ;
   bool emp = 1;
   	  size_t idx = 0;
   	  for(;idx<_table.size();idx++){   	     
		 if((*this)[idx][c] != INT_MAX){
		 	 emp =0;
		 	 break;
		 } 		
	  }
   dist.push_back((*this)[idx][c]);
   for(size_t i=idx;i<_table.size();i++){
   	   if((*this)[i][c] == INT_MAX) continue;
   	   bool newNum = 1;
   	   for(size_t j=0;j<dist.size();j++){
	      if(dist.at(j)== (*this)[i][c] ){
	      	 newNum = 0;
			 break;      	
		  }
	   }
	   if(newNum == 1) dist.push_back((*this)[i][c]);
   }
   return dist.size();
}

float
DBTable::getAve(size_t c) const
{
   // TODO: compute the average of data in column #c
   if((*this).emptyColumn(c) == 1) return NAN ;
   float sum =0.0;
   float nonemp = 0.0;
   for(size_t i=0;i<_table.size();i++){
   	  if((*this)[i][c] != INT_MAX){
   	  	sum += (*this)[i][c]; 
   	  	nonemp++;
	  }      	  
   } 
   return sum/nonemp;
   
   
}

void
DBTable::sort(const struct DBSort& s)    // not ok
{
   // TODO: sort the data according to the order of columns in 's'
   for(size_t i=1;i<_table.size();i++){   // the row doing sorting 
   	   for(size_t j=i;j>0;j--){
   	   	   
   	       if(s((*this)[j],(*this)[j-1]) == 1){
   	       	  vector<int> tmp;
   	       	  for(size_t i=0;i<(*this)[j-1].size();i++){
   	       	     	tmp.push_back((*this)[j-1][i]) ;
			  }   	       	  
			  
   	       	  _table.erase(_table.begin()+j-1);   	       	 
   	       	  _table.insert(_table.begin()+j ,tmp);
		   }
		   else break;	  
	   }
   }
}

void
DBTable::printCol(size_t c) const
{
   // TODO: to print out a column.
   // - Data are seperated by a space. No trailing space at the end.
   // - Null cells are printed as '.'
   for(size_t i=0;i<_table.size();i++) {
   	  if(_table.at(i)[c] == INT_MAX ) cout << ".";
   	  else cout << (*this)[i][c];
   	  if(i!=_table.size()-1) cout << " ";
   }
}

void
DBTable::printSummary() const
{
   size_t nr = nRows(), nc = nCols(), nv = 0;
   for (size_t i = 0; i < nr; ++i)
      for (size_t j = 0; j < nc; ++j)
         if (_table[i][j] != INT_MAX) ++nv;
   cout << "(#rows, #cols, #data) = (" << nr << ", " << nc << ", "
        << nv << ")" << endl;
}

