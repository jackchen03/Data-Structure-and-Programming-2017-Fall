/****************************************************************************
  FileName     [ dbCmd.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include <cassert>
#include "util.h"
#include "dbCmd.h"
#include "dbTable.h"

// Global variable
DBTable dbtbl;

// Static functions
static bool checkColIdx(const string& token, int& c)
{
   if (!dbtbl) {
      cerr << "Error: Table is not yet created!!" << endl;
      return false;
   }

   if (!myStr2Int(token, c)) {
      cerr << "Error: " << token << " is not a number!!\n";
      return false;
   }
   if (c < 0 || c >= int(dbtbl.nCols())) {
      cerr << "Error: Column index " << c << " is out of range!!\n";
      return false;
   }
   return true;
}

static bool checkRowIdx(const string& token, int& c)
{
   if (!dbtbl) {
      cerr << "Error: Table is not yet created!!" << endl;
      return false;
   }

   if (!myStr2Int(token, c)) {
      cerr << "Error: " << token << " is not a number!!\n";
      return false;
   }
   if (c < 0 || c >= int(dbtbl.nRows())) {
      cerr << "Error: Row index " << c << " is out of range!!\n";
      return false;
   }
   return true;
}

bool
initDbCmd()
{
   // TODO...

   if (!(cmdMgr->regCmd("DBAPpend", 4, new DBAppendCmd) &&
         cmdMgr->regCmd("DBAVerage", 4, new DBAveCmd) &&
         cmdMgr->regCmd("DBCount", 3, new DBCountCmd) &&
         cmdMgr->regCmd("DBDelete", 3, new DBDelCmd) &&
         cmdMgr->regCmd("DBMAx", 4, new DBMaxCmd) &&
         cmdMgr->regCmd("DBMIn", 4, new DBMinCmd) &&
         cmdMgr->regCmd("DBPrint", 3, new DBPrintCmd) &&
         cmdMgr->regCmd("DBRead", 3, new DBReadCmd) &&
         cmdMgr->regCmd("DBSOrt", 4, new DBSortCmd) &&
         cmdMgr->regCmd("DBSUm", 4, new DBSumCmd) 
         
      )) {
      cerr << "Registering \"init\" commands fails... exiting" << endl;
      return false;
   }

   return true;
  
}

//----------------------------------------------------------------------
//    DBAPpend <-Row | -Column> <(int data)...>
//----------------------------------------------------------------------
CmdExecStatus
DBAppendCmd::exec(const string& option)
{
   // TODO...
   // check option
   
   string tok;    // tok = -r... or -c...
   int end = myStrGetTok(option, tok);
   
   string token;
   if(!CmdExec::lexSingleOption(tok, token, 0) ) return CMD_EXEC_ERROR; 
     
   
   if(myStrNCmp("-row", tok, 1) == 0){   // append row
     string numop; 
     for(int i=0; end+1+i < option.size() ;i++){    // store in numop	  
		  if(end==-1) break; 
   	      else numop.push_back( option.at(end+1+i));
		  if(numop.at(i) == 0 ) break;	 	
	 } 
	 
   	 if(numop.size() == 0 ) {
   	 	cerr << "Error: Missing option after (-row)!!";   
   	 	return CMD_EXEC_ERROR; 
	 }		 
		  
	 vector <int> emp ;  // create empty row
	 for(size_t i=0;i<dbtbl.nCols();i++) emp.push_back(INT_MAX);
	  
   	 if(numop.size() == 1 && numop.at(0) == '-' ) dbtbl.addRow(emp);
   	 else{
   	 	  vector <int> tmp ;
   	    while(true){  // generate number array
   	       string numTok;
   	 	   int numEnd = myStrGetTok(numop, numTok);
   	 	
   	 	   int num;
   	 	   bool empidx = 0;
   	  	   if(numTok == "-" )  empidx = 1;
   	 	   else if (!myStr2Int(numTok, num)) {
              cerr << "Error: " << token << " is not a number!!\n";
              return CMD_EXEC_ERROR;
           }
           if(empidx == 1) num = INT_MAX;
   	 	     
   	 	   tmp.push_back(num); 
   	 	
   	 	   if(numEnd == -1) break;
   	 	
   	 	       string tmpstr;   // tmpstr to store numop's original value
   	 		   for(int i =0;i<numop.size();i++){
			      tmpstr.push_back(numop.at(i));
			   }  	 		
   	 			 		
   	 		for(int i=0;i<numEnd+1;i++)  numop.erase(numop.begin());
	    } 
   	 	
	 
   	 
   	 
   	 
   	    if((tmp.size() < dbtbl.nCols())) {
   	       int size = tmp.size();
   	       for(int i=0;i<dbtbl.nCols()-size;i++) tmp.push_back(INT_MAX);	
   	       dbtbl.addRow(tmp);
	    }
   	    else if ((tmp.size() > dbtbl.nCols())) {
	       vector<int> tmp2 ;
	       for(size_t i=0;i<dbtbl.nCols();i++) tmp2.push_back(tmp.at(i));
	       dbtbl.addRow(tmp2);
	    }
	    else if ((tmp.size() == dbtbl.nCols())) dbtbl.addRow(tmp);
	 }
   	 
   	 
   }
   
   else if(myStrNCmp("-column", tok, 1) == 0){   // append column
   	   string numop; 
     for(int i=0; end+1+i < option.size() ;i++){    // store in numop  	  
		  if(end==-1) break; 
   	      else numop.push_back( option.at(end+1+i));
		  if(numop.at(i) == 0 ) break;	 	
	 } 
	 
   	 if(numop.size() == 0 ) {
   	 	cerr << "Error: Missing option after (-column)!!";   
   	 	return CMD_EXEC_ERROR; 
	 }	
		  
	 vector <int> emp ;  // create empty row
	 for(size_t i=0;i<dbtbl.nRows();i++) emp.push_back(INT_MAX);
	  
   	 if(numop.size() == 1 && numop.at(0) == '-' ) dbtbl.addCol(emp);
   	 else{
   	 	 vector <int> tmp ;
   	     while(true){  // generate number array
   	       string numTok;
   	 	   int numEnd = myStrGetTok(numop, numTok);
   	 	
   	       int num;
   	       bool empidx = 0;
   	 	   if(numTok == "-" )  empidx = 1;
   	 	   else if (!myStr2Int(numTok, num)) {
               cerr << "Error: " << token << " is not a number!!\n";
               return CMD_EXEC_ERROR;
           }
           if(empidx == 1) num = INT_MAX;
   	 	     
   	 	    tmp.push_back(num); 
   	 	
   	 	   if(numEnd == -1) break;
   	 	
   	 	   string tmpstr;   // tmpstr to store numop's original value
   	 		for(int i =0;i<numop.size();i++){
			   tmpstr.push_back(numop.at(i));
			}  	 		
   	 			 		
   	 		for(int i=0;i<numEnd+1;i++)  numop.erase(numop.begin());
	     }
   	 	
	 
   	 
   	 
   	 
   	    if((tmp.size() < dbtbl.nRows())) {
   	 	   int size = tmp.size();
   	       for(int i=0;i<dbtbl.nRows()-size;i++) tmp.push_back(INT_MAX);	
   	       dbtbl.addCol(tmp);
	    }
   	    else if ((tmp.size() > dbtbl.nRows())) {
	       vector<int> tmp2 ;
	       for(size_t i=0;i<dbtbl.nRows();i++) tmp2.push_back(tmp.at(i));
	       dbtbl.addCol(tmp2);
	    }
	    else if ((tmp.size() == dbtbl.nRows())) dbtbl.addCol(tmp);
	 }
   	 
   	 
   }
   
   else {
   	  return CmdExec::errorOption(CMD_OPT_ILLEGAL, tok);
   }
   

   return CMD_EXEC_DONE;
}

void
DBAppendCmd::usage(ostream& os) const
{
   os << "Usage: DBAPpend <-Row | -Column> <(int data)...>" << endl;
}

void
DBAppendCmd::help() const
{
   cout << setw(15) << left << "DBAPpend: "
        << "append a row or column of data to the table" << endl;
}


//----------------------------------------------------------------------
//    DBAVerage <(int colIdx)>
//----------------------------------------------------------------------
CmdExecStatus
DBAveCmd::exec(const string& option)
{  
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token, false))
      return CMD_EXEC_ERROR;
   int c;
   if (!checkColIdx(token, c)) return CMD_EXEC_ERROR;

   float a = dbtbl.getAve(c);
   ios_base::fmtflags origFlags = cout.flags();
   cout << "The average of column " << c << " is " << fixed
        << setprecision(2) << a << ".\n";
   cout.flags(origFlags);

   return CMD_EXEC_DONE;
}

void
DBAveCmd::usage(ostream& os) const
{     
   os << "Usage: DBAVerage <(int colIdx)>" << endl;
}

void
DBAveCmd::help() const
{
   cout << setw(15) << left << "DBAVerage: "
        << "compute the average of a column" << endl;
}


//----------------------------------------------------------------------
//    DBCount <(int colIdx)>
//----------------------------------------------------------------------
CmdExecStatus
DBCountCmd::exec(const string& option)
{  
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token, false))
      return CMD_EXEC_ERROR;
   int c;
   if (!checkColIdx(token, c)) return CMD_EXEC_ERROR;

   int n = dbtbl.getCount(c);
   cout << "The distinct count of column " << c << " is " << n << "." << endl;

   return CMD_EXEC_DONE;
}

void
DBCountCmd::usage(ostream& os) const
{     
   os << "Usage: DBCount <(int colIdx)>" << endl;
}

void
DBCountCmd::help() const
{
   cout << setw(15) << left << "DBCount: "
        << "report the distinct count of data in a column" << endl;
}


//----------------------------------------------------------------------
//    DBDelete <-Row | -Column> <(int index)>
//----------------------------------------------------------------------
CmdExecStatus
DBDelCmd::exec(const string& option)
{
   // check option
   vector<string> options;
   if (!CmdExec::lexOptions(option, options, 2))
      return CMD_EXEC_ERROR;

   bool doRow = false;
   if (myStrNCmp("-Row", options[0], 2) == 0) doRow = true;
   else if (myStrNCmp("-Column", options[0], 2) != 0)
      return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);

   int c;
   if (doRow) {
      if (!checkRowIdx(options[1], c)) return CMD_EXEC_ERROR;
      dbtbl.delRow(c);
   }
   else {
      if (!checkColIdx(options[1], c)) return CMD_EXEC_ERROR;
      dbtbl.delCol(c);
   }

   return CMD_EXEC_DONE;
}

void
DBDelCmd::usage(ostream& os) const
{
   os << "Usage: DBDelete <-Row | -Column> <(int index)>" << endl;
}

void
DBDelCmd::help() const
{
   cout << setw(15) << left << "DBDelete: "
        << "delete a row or column from the table" << endl;
}


//----------------------------------------------------------------------
//    DBMAx <(int colIdx)>
//----------------------------------------------------------------------
CmdExecStatus
DBMaxCmd::exec(const string& option)
{  
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token, false))
      return CMD_EXEC_ERROR;
   int c;
   if (!checkColIdx(token, c)) return CMD_EXEC_ERROR;

   float n = dbtbl.getMax(c);
   cout << "The max data of column " << c << " is " << n << "." << endl;

   return CMD_EXEC_DONE;
}

void
DBMaxCmd::usage(ostream& os) const
{     
   os << "Usage: DBMAx <(int colIdx)>" << endl;
}

void
DBMaxCmd::help() const
{
   cout << setw(15) << left << "DBMAx: "
        << "report the maximum number of a column" << endl;
}


//----------------------------------------------------------------------
//    DBMIn <(int colIdx)>
//----------------------------------------------------------------------
CmdExecStatus
DBMinCmd::exec(const string& option)
{  
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token, false))
      return CMD_EXEC_ERROR;
   int c;
   if (!checkColIdx(token, c)) return CMD_EXEC_ERROR;

   float n = dbtbl.getMin(c);
   cout << "The min data of column " << c << " is " << n << "." << endl;

   return CMD_EXEC_DONE;
}

void
DBMinCmd::usage(ostream& os) const
{     
   os << "Usage: DBMIn <(int colIdx)>" << endl;
}

void
DBMinCmd::help() const
{
   cout << setw(15) << left << "DBMIn: "
        << "report the minimum number of a column" << endl;
}


//----------------------------------------------------------------------
//    DBPrint < (int rowIdx) (int colIdx)
//            | -Row (int rowIdx) | -Column (colIdx) | -Table | -Summary>
//----------------------------------------------------------------------
CmdExecStatus
DBPrintCmd::exec(const string& option)
{  
   // TODO...
   string tok;
   int end = myStrGetTok(option, tok);
   
   
   if(myStrNCmp("-row", tok, 1) == 0){
   	  string numop;
   	  for(int i=0; end+1+i < option.size() ;i++){    // store in numop	  
		  if(end==-1) break; 
   	      else numop.push_back( option.at(end+1+i));
		  if(numop.at(i) == 0 ) break;	 	
	  }
	 string emptok ;
	 if(!CmdExec::lexSingleOption(numop, emptok, 0)) return CMD_EXEC_ERROR;
	 int idx;
	 if(!checkRowIdx(numop,idx)) return CMD_EXEC_ERROR;
	 cerr << dbtbl[idx] ;
   }
   else if(myStrNCmp("-column", tok, 1) == 0){
   	  string numop;
   	  for(int i=0; end+1+i < option.size() ;i++){    // store in numop	  
		  if(end==-1) break; 
   	      else numop.push_back( option.at(end+1+i));
		  if(numop.at(i) == 0 ) break;	 	
	  }
	 string emptok ;
	 if(!CmdExec::lexSingleOption(numop, emptok, 0)) return CMD_EXEC_ERROR;
	 int idx;
	 if(!checkColIdx(numop,idx)) return CMD_EXEC_ERROR;
	 dbtbl.printCol(idx);
   }
   else if(myStrNCmp("-table", tok, 1) == 0){
   	   string numop;
   	   for(int i=0; end+1+i < option.size() ;i++){    // store in numop	  
		  if(end==-1) break; 
   	      else numop.push_back( option.at(end+1+i));
		  if(numop.at(i) == 0 ) break;	 	
	   } 
   	   if(end != -1) return errorOption(CMD_OPT_EXTRA, numop);
   	   cerr << dbtbl ;
   }
   else if(myStrNCmp("-summary", tok, 1) == 0){
   	    string numop;
   	   for(int i=0; end+1+i < option.size() ;i++){    // store in numop	  
		  if(end==-1) break; 
   	      else numop.push_back( option.at(end+1+i));
		  if(numop.at(i) == 0 ) break;	 	
	   } 
   	   if(end != -1) return errorOption(CMD_OPT_EXTRA, numop);
   	   dbtbl.printSummary();
   }
   else if(option.size()!=0){
   	  vector <int> tmp ;
   	  int numEnd;
   	  string numop;
   	  for(int i=0; i < option.size() ;i++){    // store in numop	  
		  if(end==-1) break; 
   	      else numop.push_back( option.at(i));
		  if(numop.at(i) == 0 ) break;	 	
	   }
	   
	      
			
   	 for(int i =0;i<2;i++){  // generate number array
   	    string numTok;   	    
   	 	numEnd = myStrGetTok(numop, numTok);
   	 	if(i==0&&numEnd == -1) {
   	 	   cerr << "Illegal option!!" <<endl ;
		   return CMD_EXEC_ERROR;  	   	
		   break;
		}	
   	 	int num;
   	 	if(i ==0){   // checking row index
   	 		if(!checkRowIdx(numTok,num)) return CMD_EXEC_ERROR;  
   	 		else {  tmp.push_back(num); } 
		}   
		else if(i ==1){   // checking column index
   	 		if(!checkColIdx(numTok,num)) return CMD_EXEC_ERROR;  
   	 		else {  tmp.push_back(num); } 
		}   	 
   	 	     
   	 		 		
		for(int i=0;i<numEnd+1;i++){ if(numop.size() == 0) break;	numop.erase(numop.begin());   }	  	 		  
			
	 }
	 
	 
	
	 if(numEnd!= -1) return errorOption(CMD_OPT_EXTRA,numop);
     if(dbtbl[tmp.at(0)][tmp.at(1)] == INT_MAX) cerr << '.'; 
	 else cerr << dbtbl[tmp.at(0)][tmp.at(1)] ;
	 
   }
   else if(option.size()== 0) return errorOption(CMD_OPT_MISSING,"");
   
   
   
   

   return CMD_EXEC_DONE;
}

void
DBPrintCmd::usage(ostream& os) const
{
   os << "DBPrint < (int rowIdx) (int colIdx)\n"
      << "        | -Row (int rowIdx) | -Column (colIdx) | -Table | -Summary>"
      << endl;
}

void
DBPrintCmd::help() const
{
   cout << setw(15) << left << "DBPrint: "
        << "print the data in the table" << endl;
}


//----------------------------------------------------------------------
//    DBRead <(string csvFile)> [-Replace]
//----------------------------------------------------------------------
CmdExecStatus
DBReadCmd::exec(const string& option)
{
   // check option
   vector<string> options;
   if (!CmdExec::lexOptions(option, options))
      return CMD_EXEC_ERROR;

   if (options.empty())
      return CmdExec::errorOption(CMD_OPT_MISSING, "");

   bool doReplace = false;
   string fileName;
   for (size_t i = 0, n = options.size(); i < n; ++i) {
      if (myStrNCmp("-Replace", options[i], 2) == 0) {
         if (doReplace) return CmdExec::errorOption(CMD_OPT_EXTRA,options[i]);
         doReplace = true;
      }
      else {
         if (fileName.size())
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
         fileName = options[i];
      }
   }

   ifstream ifs(fileName.c_str());
   if (!ifs) {
      cerr << "Error: \"" << fileName << "\" does not exist!!" << endl;
      return CMD_EXEC_ERROR;
   }

   if (dbtbl) {
      if (!doReplace) {
         cerr << "Error: Table exists. Use \"-Replace\" option for "
              << "replacement.\n";
         return CMD_EXEC_ERROR;
      }
      cout << "Table is replaced..." << endl;
      dbtbl.reset();
   }
 //  if (!(ifs >> dbtbl)) return CMD_EXEC_ERROR;
   ifs >> dbtbl;
   cout << "\"" << fileName << "\" was read in successfully." << endl;

   return CMD_EXEC_DONE;
}

void
DBReadCmd::usage(ostream& os) const
{
   os << "Usage: DBRead <(string csvFile)> [-Replace]" << endl;
}

void
DBReadCmd::help() const
{
   cout << setw(15) << left << "DBRead: "
        << "read data from .csv file" << endl;
}


//----------------------------------------------------------------------
//    DBSOrt <(int colIdx)>...
//----------------------------------------------------------------------
CmdExecStatus
DBSortCmd::exec(const string& option)
{
   // check option
   vector<string> options;
   if (!CmdExec::lexOptions(option, options))
      return CMD_EXEC_ERROR;
   
   if (options.empty())
      return CmdExec::errorOption(CMD_OPT_MISSING, "");

   DBSort sortOrders;
   for (int i = 0, n = options.size(); i < n; ++i) {
      int s;
      if (!checkColIdx(options[i], s)) return CMD_EXEC_ERROR;
      sortOrders.pushOrder(s);
   }
   dbtbl.sort(sortOrders);

   return CMD_EXEC_DONE;
}

void
DBSortCmd::usage(ostream& os) const
{
   os << "Usage: DBSOrt <(int colIdx)>..." << endl;
}

void
DBSortCmd::help() const
{
   cout << setw(15) << left << "DBSort: "
        << "sort the data in the table" << endl;
}


//----------------------------------------------------------------------
//    DBSUm <(int colIdx)>
//----------------------------------------------------------------------
CmdExecStatus
DBSumCmd::exec(const string& option)
{  
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token, false))
      return CMD_EXEC_ERROR;
   int c;
   if (!checkColIdx(token, c)) return CMD_EXEC_ERROR;

   float n = dbtbl.getSum(c);
   cout << "The sum of column " << c << " is " << n << "." << endl;

   return CMD_EXEC_DONE;
}

void
DBSumCmd::usage(ostream& os) const
{     
   os << "Usage: DBSUm <(int colIdx)>" << endl;
}

void
DBSumCmd::help() const
{
   cout << setw(15) << left << "DBSUm: "
        << "compute the summation of a column" << endl;
}

