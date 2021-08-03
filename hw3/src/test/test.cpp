/****************************************************************************
  FileName     [ test.cpp ]
  PackageName  [ test ]
  Synopsis     [ Test program for simple database db ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <cstdlib>
#include "dbTable.h"

using namespace std;

extern DBTable dbtbl;

class CmdParser;
CmdParser* cmdMgr = 0; // for linking purpose

int
main(int argc, char** argv)
{
   
   if (argc != 2) {  // testdb <cvsfile>
      cerr << "Error: using testdb <cvsfile>!!" << endl;
      exit(-1);
   }

   ifstream inf(argv[1]);

   if (!inf) {
      cerr << "Error: cannot open file \"" << argv[1] << "\"!!\n";
      exit(-1);
   }

   if (dbtbl) {
      cout << "Table is resetting..." << endl;
      dbtbl.reset();
   }
 /*  if (!(inf >> dbtbl)) {
      cerr << "Error in reading csv file!!" << endl;
      exit(-1);
   }*/
   
   
   inf>>dbtbl;
    
   cout << "========================" << endl;
   cout << " Print table " << endl;
   cout << "========================" << endl;
   cout << dbtbl << endl;
   

   // TODO
   // Insert what you want to test here by calling DBTable's member functions
/*   vector<int> test1;
   test1.push_back(1);
   test1.push_back(INT_MAX);*/
   cerr << "0";
   
//   dbtbl.reset();
 //  for(int i=0;i<5;i++)
 //      dbtbl.addRow(DBRow(test1));
 
    DBSort d;
   d._sortOrder.push_back(2);
   d._sortOrder.push_back(3);
   
   
   dbtbl.sort(d);
   
   cout<<dbtbl<<endl;
   int col=1;
   cout<<"ave"<<dbtbl.getAve(col)<<endl;
   cout<<"sum"<<dbtbl.getSum(col)<<endl;
   cout<<"max"<<dbtbl.getMax(col)<<endl;
   cout<<"min"<<dbtbl.getMin(col)<<endl;
   cout<<"count"<<dbtbl.getCount(col)<<endl;      

   
   return 0;
}
