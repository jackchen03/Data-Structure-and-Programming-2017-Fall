/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include "memCmd.h"
#include "memTest.h"
#include "cmdParser.h"
#include "util.h"

using namespace std;

extern MemTest mtest;  // defined in memTest.cpp

bool
initMemCmd()
{
   if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
         cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
         cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
         cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)
      )) {
      cerr << "Registering \"mem\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}


//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string& option)
{
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token))
      return CMD_EXEC_ERROR;
   if (token.size()) {
      int b;
      if (!myStr2Int(token, b) || b < int(toSizeT(sizeof(MemTestObj)))) {
         cerr << "Illegal block size (" << token << ")!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
      }
      #ifdef MEM_MGR_H
      mtest.reset(toSizeT(b));
      #else
      mtest.reset();
      #endif // MEM_MGR_H
   }
   else
      mtest.reset();
   return CMD_EXEC_DONE;
}

void
MTResetCmd::usage(ostream& os) const
{  
   os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void
MTResetCmd::help() const
{  
   cout << setw(15) << left << "MTReset: " 
        << "(memory test) reset memory manager" << endl;
}


//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus
MTNewCmd::exec(const string& option)
{
   // TODO
   string token;   
   size_t end  = myStrGetTok(option , token);
   if(token.size() == 0) return CmdExec::errorOption(CMD_OPT_MISSING, option);
   
   int num, num2 ;
   if(end == -1){
      if(!myStr2Int(token, num) ) {
   	      cerr << "Error : Illegal option!!" << endl;
		  return CMD_EXEC_ERROR;	
      }    	
	  if(num <= 0){
	  	  cerr << "Error : Not positive number!!" << endl;
		  return CMD_EXEC_ERROR;
	  }  
      try {mtest.newObjs(num);}  //for object  
	  catch (bad_alloc){cerr << "Error : Exceed block size!!"	;	 return CMD_EXEC_ERROR;  } 
   }
   else{
   	  string tmp = option.substr(end+1);
   	  string token2;
   	  size_t end2 = myStrGetTok(tmp, token2);
   	  
   	  string tmp2 = tmp.substr(end2+1);
      string token3;
      size_t end3 = myStrGetTok(tmp2, token3);
      
      if((myStrNCmp("-array", token, 2) != 0 ) && (myStrNCmp("-array", token2, 2) != 0 ) ){
      	 cerr << "Error : Illegal option!!" <<endl;
      	  return CMD_EXEC_ERROR;
	  }
	  else if(end3!=-1) return CmdExec::errorOption(CMD_OPT_EXTRA, option);
	  else if(myStrNCmp("-array", token, 2) == 0 ){
	  	 if(!myStr2Int(token2, num) ) {
   	        cerr << "Error : Illegal option!!" << endl;
		    return CMD_EXEC_ERROR;	
         } 
          if(!myStr2Int(token3, num2) ) {
   	         cerr << "Error : Illegal option!!" << endl;
		     return CMD_EXEC_ERROR;	
         } 
         if(num<=0 || num2<=0 ){
         	 cerr << "Error : Not positive number!!" << endl;
		     return CMD_EXEC_ERROR;
		 }
		 try {	mtest.newArrs(num2, num);   }  
	     catch(bad_alloc){  cerr << "Error : Exceed block size!!"	;  }	  
		 
	  }
	  else if(myStrNCmp("-array", token2, 2) == 0) {
	  	 if(!myStr2Int(token, num) ) {
   	        cerr << "Error : Illegal option!!" << endl;
		    return CMD_EXEC_ERROR;	
         } 
         if(!myStr2Int(token3, num2) ) {
   	         cerr << "Error : Illegal option!!" << endl;
		     return CMD_EXEC_ERROR;	
         } 
         if(num<= 0 || num2<= 0 ){
         	 cerr << "Error : Not positive number!!" << endl;
		     return CMD_EXEC_ERROR;
		 }
		 try {	mtest.newArrs(num, num2);   }  
	     catch(bad_alloc){  cerr << "Error : Exceed block size!!"	;  }	  
	  }
   } 

   return CMD_EXEC_DONE;
}

void
MTNewCmd::usage(ostream& os) const
{  
   os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void
MTNewCmd::help() const
{  
   cout << setw(15) << left << "MTNew: " 
        << "(memory test) new objects" << endl;
}


//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus
MTDeleteCmd::exec(const string& option)
{
   // TODO
   string token;   
   size_t end  = myStrGetTok(option , token);
   if(token.size() == 0) return CmdExec::errorOption(CMD_OPT_MISSING, option);
   
   
   if(end == -1) return CmdExec::errorOption(CMD_OPT_MISSING, option);
   if(myStrNCmp("-index", token, 2) != 0  && myStrNCmp("-random", token, 2) != 0  && (myStrNCmp("-array", token, 2) != 0 )){
   	      cerr << "Error : Illegal option!!" << endl;
		  return CMD_EXEC_ERROR;	
    }
   else if(myStrNCmp("-array", token, 2) == 0){
   	  string tmp = option.substr(end+1);
   	  string token2;
   	  size_t end2 = myStrGetTok(tmp, token2);
   	  if(end2 == -1) return CmdExec::errorOption(CMD_OPT_MISSING, option);  // only two token
   	  
   	  string tmp2 = tmp.substr(end2+1);
      string token3;
      myStrGetTok(tmp2, token3);
   	  
	  int num;  
   	  
   	  if(!myStr2Int(token3, num)) {
   	      cerr << "Error : Illegal option!!" << endl;
		  return CMD_EXEC_ERROR;	
      }
	  if(num < 0) {
	  	 cerr << "Error : Negative number!!" <<endl;
	  	 return CMD_EXEC_ERROR;	
	  } 
      
      else{
      	 if(myStrNCmp("-index", token2, 2) == 0 ){
      	    if(num < mtest.getArrListSize() )
		        mtest.deleteArr(num);
		    else {
		    	cerr << "Error : The number is out of range !!" <<endl;
	  	        return CMD_EXEC_ERROR;	
			}    
		 }
		 else if((myStrNCmp("-random", token2, 2) == 0 )){		 	
		 	for(int i=0;i<num;i++){
		 	   int number;
		 	   if(mtest.getArrListSize()){
			      number = rnGen( mtest.getArrListSize() );
			      mtest.deleteArr(number);	
			   }
			   else {
			   	  cerr << "Error : _arrList is empty !! " << endl;
			   	  return CMD_EXEC_ERROR;	
			   }
			}
		 }
	  }
	  

   }  
	  	
   else{   	
   	  string tmp = option.substr(end+1);
   	  string token2;
   	  size_t end2 = myStrGetTok(tmp, token2);
   	  int num;
   	  
   	  if(!myStr2Int(token2, num)) {
   	      cerr << "Error : Illegal option!!" << endl;
		  return CMD_EXEC_ERROR;	
      }
	  if(num < 0) {
	  	 cerr << "Error : Negative number!!" <<endl;
	  	 return CMD_EXEC_ERROR;	
	  } 
      
      if(end2 == -1){
      	 if(token == "-i"){
      	 	if(num < mtest.getObjListSize()  )
		       mtest.deleteObj(num);
		    else {
		    	cerr << "Error : The number is out of range !!" <<endl;
	  	        return CMD_EXEC_ERROR;	
			}
		 }
		 else if(token == "-r"){
		 	for(int i=0;i<num;i++){
		 	   int number;
			   if(mtest.getObjListSize()){
			   	   number = rnGen( mtest.getObjListSize() );
				   mtest.deleteObj(number);	
			   }
			   else {
			   	  cerr << "Error : _objList is empty !! " << endl;
			   	  return CMD_EXEC_ERROR;	
			   }
			}
		 }
	  }
	  
	  else if(end2 != -1){
	  	 string tmp2 = tmp.substr(end2+1);
         string token3;
         myStrGetTok(tmp2, token3);
         if(myStrNCmp("-array", token3, 2) != 0 ) {
   	       cerr << "Error : Illegal option!!" << endl;
		   return CMD_EXEC_ERROR;	
         }
         if(token == "-i"){
         	if(num < mtest.getArrListSize() )
		        mtest.deleteArr(num);
		    else {
		    	cerr << "Error : The number is out of range !!" <<endl;
	  	        return CMD_EXEC_ERROR;	
			}    
		    
		 }
		 else if(token == "-r"){
		 	
		 	for(int i=0;i<num;i++){
		 	   int number;
		 	   if(mtest.getArrListSize()){
			      number = rnGen( mtest.getArrListSize() );
			      mtest.deleteArr(number);	
			   }
			   else {
			   	  cerr << "Error : _arrList is empty !! " << endl;
			   	  return CMD_EXEC_ERROR;	
			   }
			}
		 }
	  }
        	  
   } 

   return CMD_EXEC_DONE;
}

void
MTDeleteCmd::usage(ostream& os) const
{  
   os << "Usage: MTDelete <-Index (size_t objId) | "
      << "-Random (size_t numRandId)> [-Array]" << endl;
}

void
MTDeleteCmd::help() const
{  
   cout << setw(15) << left << "MTDelete: " 
        << "(memory test) delete objects" << endl;
}


//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string& option)
{
   // check option
   if (option.size())
      return CmdExec::errorOption(CMD_OPT_EXTRA, option);      
   mtest.print();

   return CMD_EXEC_DONE;
}

void
MTPrintCmd::usage(ostream& os) const
{  
   os << "Usage: MTPrint" << endl;
}

void
MTPrintCmd::help() const
{  
   cout << setw(15) << left << "MTPrint: " 
        << "(memory test) print memory manager info" << endl;
}


