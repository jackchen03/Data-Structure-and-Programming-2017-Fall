/****************************************************************************
  FileName     [ p2Table.h]
  PackageName  [ p2 ]
  Synopsis     [ Header file for class Row and Table ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2016-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#ifndef P2_TABLE_H
#define P2_TABLE_H

#include <vector>
#include <string>
#include<climits> 
using namespace std;

class Row
{
public:
   // TODO: define constructor and member functions on your own
   Row(int a){ _data = new int [a];   }

   // Basic access functions
   const int operator[] (size_t i) const { return _data[i]; }
   int& operator[] (size_t i) { return _data[i]; }


private:
   int    *_data; // DO NOT change this definition. Use it to store data.
};

class Table
{
public:
   // TODO: define constructor and member functions on your own
   Table(size_t a){ _rows.reserve(a);    } // constructor
   size_t RowNumber(){ return _rows.size();  }
   bool read(const string&);
   void setColumnNumber(size_t a){ _nCols = a;  }
   void addRow(Row a){ _rows.push_back(a);}
   void print();

   // Basic access functions
   size_t nCols() const { return _nCols; }
   const Row& operator[] (size_t i) const { return _rows[i]; }
   Row& operator[] (size_t i) { return _rows[i]; }

private:
   size_t       _nCols; // You should record the number of columns.
   vector<Row>  _rows;  // DO NOT change this definition. Use it to store rows.
};

#endif // P2_TABLE_H
