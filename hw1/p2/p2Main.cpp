/****************************************************************************
  FileName     [ p2Main.cpp ]
  PackageName  [ p2 ]
  Synopsis     [ Define main() function ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2016-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <string>
#include<stdlib.h>
#include "p2Table.h"
#include<iomanip>

using namespace std;

int main()
{
   Table table(0);
   int a = table.RowNumber();
   cout << a ;

   // Read in the csv file. Do NOT change this part of code.
   string csvFile;
   cout << "Please enter the file name: ";
   cin >> csvFile;
   if (table.read(csvFile))
      cout << "File \"" << csvFile << "\" was read in successfully." << endl;
   else {
      cerr << "Failed to read in file \"" << csvFile << "\"!" << endl;
      exit(-1); // csvFile does not exist.
   }
   while(true){
      string command;
      cin >> command;


      int col; // which column
      int notemp = 0;
      if(command == "PRINT" )  table.print();
      else if(command == "SUM"){
        int result = 0;
        cin >> col;
        for(int i=0;i<table.RowNumber();i++){
            if(table[i][col]!=INT_MAX) result += table[i][col];
        }
        cout << "The summation of data in column #" << col << " is " << result << endl;
      }
      else if(command == "AVE"){
        bool allempty = 1;
        for(int i=0;i<table.RowNumber();i++){
          if(table[i][col] != INT_MAX){
             allempty = 0;
             break;
          }
        }
        if(allempty == 1) cout << "Error: This is a NULL column!! " << endl;
        else{
          double result = 0.0;
          cin >> col;
          for(int i=0;i<table.RowNumber();i++){
             if(table[i][col]!=INT_MAX){
                result += table[i][col];
                notemp ++ ;
          }
          }
           result /= notemp ;
           cout << "The average of data in column #" << col << " is " << fixed << setprecision(1) << result << endl;
        }


      }
      else if(command == "MIN"){
         bool allempty = 1;
         for(int i=0;i<table.RowNumber();i++){
          if(table[i][col] != INT_MAX){
             allempty = 0;
             break;
          }
         }
         if(allempty == 1) cout << "Error: This is a NULL column!! " << endl;
         else{
            int nowmin = INT_MAX;
            cin >> col;
            for(int i=0;i<table.RowNumber();i++){
               if(table[i][col]<nowmin) nowmin = table[i][col]  ;
            }
            cout << "The minimum of data in column #" << col << " is "  << nowmin << endl;
         }

      }
      else if(command == "MAX"){
         bool allempty = 1;
         for(int i=0;i<table.RowNumber();i++){
          if(table[i][col] != INT_MAX){
             allempty = 0;
             break;
          }
         }
         if(allempty == 1) cout << "Error: This is a NULL column!! " << endl;
         else{
             int nowmax = INT_MIN;
             cin >> col;
             for(int i=0;i<table.RowNumber();i++){
               if(table[i][col]>nowmax&& table[i][col]!=INT_MAX) nowmax = table[i][col]    ;
             }
             cout << "The maximum of data in column #" << col << " is "  << nowmax << endl;
         }

      }
      else if(command == "DIST"){
         bool allempty = 1;
         for(int i=0;i<table.RowNumber();i++){
          if(table[i][col] != INT_MAX){
             allempty = 0;
             break;
          }
         }
         if(allempty == 1) cout << "Error: This is a NULL column!! " << endl;
         else{
            cin >> col;
            int* now = new int [table.RowNumber()];
            for(int i=0;i<table.RowNumber();i++){
                now[i] = INT_MAX;
            }

            int nowindex = 0;
            for(int i=0;i<table.RowNumber();i++){
               bool thesame = 0;
               if(i==0){
                  now[nowindex] = table[i][col];
               }
               for(int j=0;j<= nowindex;j++){
                  if(table[i][col] == now[j]&&table[i][col]<INT_MAX){
                    thesame = 1;
                    break;
                  }
               }
               if(thesame == 0 && table[i][col] < INT_MAX){
                  nowindex++  ;
                  now[nowindex] = table[i][col];
               }
             }
             int dist = nowindex+1;

             cout << "The distinct count of data in column #" << col << " is "  << dist << endl;
         }

      }
      else if(command == "ADD"){
         string tmp;
         getline(cin,tmp);
         int index = 0;
         Row tmprow(table.nCols());
         for(int i=0;i<table.nCols();i++){
            if(tmp.at(index) != ' '){
                tmprow[i] =  tmp.at(index) - '0';
            }
            else i--;
            index++;
         }
         table.addRow(tmprow);
      }
      else if(command == "EXIT"){
         break;
      }
   }

   // TODO read and execute commands
   /*while (true) {
   }*/
}
