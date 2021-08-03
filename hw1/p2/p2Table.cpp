/****************************************************************************
  FileName     [ p2Table.cpp ]
  PackageName  [ p2 ]
  Synopsis     [ Define member functions of class Row and Table ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2016-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include "p2Table.h"
#include<iostream>
#include<fstream>
#include <stdio.h>
#include <stdlib.h>
#include<iomanip>
#include<cstring>
#include<cmath>
using namespace std;

// Implement member functions of class Row and Table here
bool
Table::read(const string& csvFile)
{
   fstream myfile;
   string temp;
   char * cstr = new char [csvFile.length()+1];
   std::strcpy (cstr, csvFile.c_str());
   myfile.open(cstr) ;
   if(myfile.is_open() == 1){

       int length = 0;
       bool getn = 0, getr = 0;
       char getting;
       while(true){   // counting array length
    	   getting = myfile.get();
    	   if(getting == '\n'|| getting == '\r') break;
    	   length ++ ;
	   }
	     if(getting == '\n') getn = 1;
         else  getr = 1;

	   myfile.seekg (0, myfile.beg);  // return to the beginning
	   char* Array = new char[length];
	   int commanumber = 0;
	   for(int i=0;i<length;i++){     //counting column numbers
		  char getting2;
    	  getting2 = myfile.get();
    	  if(getting2 == ',' ) commanumber++;
	   }
       _nCols = commanumber+1;


       myfile.seekg (0, myfile.beg);  // return to the beginning
       bool lastoneiscomma = 0;


       if(getr==1) {  // condition 1  \n \r
          while((myfile.eof()!=1)){  // end when file ends
           Row newRow(_nCols);  // Row object
           string tmpstring;
           getline(myfile,tmpstring,'\r');  //get one row from myfile
           char dump = myfile.get();
           tmpstring += ',';

           int tmpstringindex = 0;   // index of tmpstring
           for(int i=0;i<_nCols;i++){
              string stringbtwcommas;
              int btwcommaindex = 0;

              while(true){
                 if(btwcommaindex == 0 && tmpstring.at(tmpstringindex) == ',' )  newRow[i] = INT_MAX;
                 if(tmpstring.at(tmpstringindex) == ','){
                    tmpstringindex++;
                    break;
                 }
                 stringbtwcommas += tmpstring.at(tmpstringindex);
                 tmpstringindex++;
                 btwcommaindex++;
              }
              if(newRow[i] == INT_MAX) continue ;

              int number = 0;
              if(stringbtwcommas.at(0) == '-'){   // <0
                 int expo = stringbtwcommas.size() - 2;
                 for(int i=1 ; i<stringbtwcommas.size() ; i++){
                    number += pow(10,expo)*(stringbtwcommas.at(i)-'0');
                    expo -- ;
                 }
                 newRow[i] = 0- number;
              }
              else{   //>0
                 int expo = stringbtwcommas.size() - 1;
                 for(int i=0 ; i<stringbtwcommas.size() ; i++){
                     number += pow(10,expo)*(stringbtwcommas.at(i)-'0');
                     expo -- ;
                 }
                 newRow[i] = number;
              }


           }

           _rows.push_back(newRow);

       }
       }
       else if(getn==1){   // condition 3  \n
         while((myfile.eof()!=1)){  // end when file ends
           Row newRow(_nCols);  // Row object
           string tmpstring;
           getline(myfile,tmpstring,'\n');  //get one row from myfile
           tmpstring += ',';

           int tmpstringindex = 0;   // index of tmpstring
           for(int i=0;i<_nCols;i++){
              string stringbtwcommas;
              int btwcommaindex = 0;

              while(true){
                 if(btwcommaindex == 0 && tmpstring.at(tmpstringindex) == ',' )  newRow[i] = INT_MAX;
                 if(tmpstring.at(tmpstringindex) == ','){
                    tmpstringindex++;
                    break;
                 }
                 stringbtwcommas += tmpstring.at(tmpstringindex);
                 tmpstringindex++;
                 btwcommaindex++;
              }
              if(newRow[i] == INT_MAX) continue ;

              int number = 0;
              if(stringbtwcommas.at(0) == '-'){   // <0
                 int expo = stringbtwcommas.size() - 2;
                 for(int i=1 ; i<stringbtwcommas.size() ; i++){
                    number += pow(10,expo)*(stringbtwcommas.at(i)-'0');
                    expo -- ;
                 }
              }
              else{   //>0
                 int expo = stringbtwcommas.size() - 1;
                 for(int i=0 ; i<stringbtwcommas.size() ; i++){
                     number += pow(10,expo)*(stringbtwcommas.at(i)-'0');
                     expo -- ;
                 }
              }

              newRow[i] = number;
           }

           _rows.push_back(newRow);

       }
       }

       else if(getr==1){   // condition 4  \r

       }

       return true; // TODO
   }
   else return false;

}  // didn't consider space

void
Table::print(){
	for(int i=0;i<_rows.size();i++){
		for(int j=0;j<_nCols;j++){
            if(_rows[i][j] == INT_MAX) cout << setw(4) << right << '.' ;
			else cout << setw(4) << right<< _rows[i][j]  ;
		}
		cout << endl;
	}


}
