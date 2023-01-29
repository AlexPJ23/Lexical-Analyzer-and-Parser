
#include <fstream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <cstring>
#include <stdio.h>
#include <map>
#include "lex.h"
using namespace std;

LexItem getNextToken(istream& in, int& lineNum){
    enum TokState {START, INID, INSTRING, ININT, INREAL, INCOMMENT};
    int lexstate = START;
    Token t = ERR;
    string lexeme;
    char ch;
    LexItem newLex = LexItem(t, lexeme, lineNum);
    while(in.get(ch)){
        switch(lexstate){
            case START:
                if(ch == '\n'){
                    lineNum++;
                }
                if(( in.peek() == '*'  && ch == '/' )|| ch == '#' ){
                    //lexeme+=ch;
                    lexstate = INCOMMENT;
                    break;
                }
                if(!isdigit(ch) && in.peek() == '.'){ // change this
                    lexeme +=ch;//changed this
                    cerr<<"Error in line "<<lineNum+1<<" ("<<lexeme<<")"<<endl;
                    exit(1);
                }
                if(isspace(ch))
                    break;
                if(isalpha(ch) || ch == '_'){
                    lexstate =  INID;
                    lexeme += ch;
                    if(in.peek() == '$'){
                       cerr<<"Error in line "<<lineNum+1<<" ($)"<<endl;
                       exit(1);
                    }
                        
                    if(ispunct(in.peek()) && (in.peek() != '_' && in.peek() != '@'))
                        return id_or_kw(lexeme, lineNum);
                    break;
                }
                if(isdigit(ch)){
                    lexstate = ININT;
                    lexeme += ch;
                    if(in.peek() == '.'){
                        lexeme+=in.peek(); 
                        lexstate = INREAL;
                    }
                    if(in.peek() == ';' || in.peek() == ',')
                        return LexItem(ICONST,lexeme,lineNum);
                    break;
                }
                if(ch == '\"'){
                    lexstate = INSTRING;
                    break;
                }
                if(ispunct(ch)){
                    lexeme+=ch;
                    if(in.peek() == '=' || in.peek() == '|' || in.peek() == '&')
                        break;
                    return id_or_kw(lexeme,lineNum);
                }
                break;
            case ININT:
                if(isdigit(ch))
                    lexeme+= ch;
                if(ch == '\n'){
                    lineNum++;
                }
                
                if(in.peek() == '.'){
                    lexeme += in.peek(); // this
                    lexstate = INREAL;
                    break;
                }
                else if(isspace(ch) || ch == ';' ){
                    t = ICONST;
                    newLex = LexItem(t,lexeme, lineNum);
                    return newLex;
                }

                if (in.peek() == ';' || in.peek() == '_'){
                    t = ICONST;
                    newLex = LexItem(t,lexeme, lineNum);
                    return newLex;
                }
                break;
            case INREAL:
                if(ch == '\n'){
                        lineNum++;
                 }
                if(ch == '.' && !isdigit(in.peek())){
                    cerr<<"Error in line "<<lineNum+1<<" ("<<lexeme<<")"<<endl;
                    exit(1);
                }
                if(in.peek() == '.'){
                    lexeme += ch;
                    cerr<<"Error in line "<<lineNum+1<<" ("<<lexeme<<".)"<<endl;
                    exit(1);
                }
                
                if(isdigit(ch)){
                    lexeme += ch;
                }
                
                if(isspace(ch)|| ch == ';'){
                    t = RCONST;
                    newLex = LexItem(t,lexeme, lineNum);
                    return newLex;
                }
                if (in.peek() == ';' || in.peek() == ','){
                    t = RCONST;
                    newLex = LexItem(t,lexeme, lineNum);
                    return newLex;
                }
                break;
            case INSTRING:
                if(ch == '\n'){
                   cerr<<"Error in line "<<lineNum+1<<" (\""<<lexeme<<")"<<endl;
                    exit(1);
                }
                if(ch == '\"'){
                    //cout<<"This is also entered"<<endl;
                    t = SCONST;
                    newLex = LexItem(t,lexeme, lineNum);
                    return newLex;
                } else{
                    lexeme+=ch;
                }
                 if(in.peek() == '\''){
                    cerr<<"Error in line "<<lineNum+1<<" (\""<<lexeme<<"\'"<<" )"<<endl;
                     exit(1);

                }
                break;
            case INID:
                //cout<<"In IDENT Rn lex:"<<lexeme<<endl;
                if(isalpha(ch) || isdigit(ch) || ch =='_' || ch =='@'){
                    lexeme += ch;
                }
                
                    
                if(isspace(in.peek()) || ispunct(in.peek()))
                    return id_or_kw(lexeme,lineNum);
                break;
            case INCOMMENT:
                    if(ch == '\n'){
                        lineNum++;
                    }
                
                
                
                if(in.peek() == EOF){
                    cout<<""<<endl;
                    cerr<<"Missing a comment end delimiters \'*/\' at line "<<lineNum<<endl;
                    t = ERR;
                    return LexItem(t,lexeme,lineNum);
                }
                if(ch == '*' && in.peek() == '/'){
                    lexstate = START;
                    in.get(ch);
                    break;
                }
                
                break;
        }
    }
    newLex = LexItem(t,lexeme,lineNum);
    return newLex;
}



LexItem id_or_kw(const string& lexeme, int linenum){
    Token t = IDENT;
    string upWord = "";
    static const string EnumString[9]= {"PROGRAM","PRINT","INT","END","IF","FLOAT","BOOL","ELSE","THEN"};
    static const string OpString[15]= {"+","-","*","/","=","(",")",",","==",">","<",";","&&","||","!"};
    for(int k = 0; k < lexeme.length(); k++){
         upWord += toupper(lexeme.at(k));
    }
    if(upWord == "FALSE" || upWord == "TRUE"){
        t = BCONST;
        return LexItem(t, upWord, linenum);
    }
    //Finds if the lexeme is a keyword
    for(int i = 0; i <= 8; i++){
         if(upWord == EnumString[i]){
             t = Token(i);
             return LexItem(t, upWord, linenum);
         }
    }
    //Finds if the lexeme is an operator
    for(int x = 0; x<= 14; x++){
         if(upWord == OpString[x]){
             t = Token(x+16);
             return LexItem(t, lexeme, linenum);
         }
    }
    return LexItem(t, upWord, linenum);
}

ostream& operator<<(ostream& out, const LexItem& tok){
    	map <string, Token> tokString = {
		{"PROGRAM", PROGRAM},
		{"PRINT", PRINT},
		{"INT", INT},
		{ "END", END },
		{ "FLOAT", FLOAT},
		{ "BOOL", BOOL },
		
		{ "ELSE", ELSE },
		{ "IF", IF },
		
		{"THEN", THEN},
		{ "TRUE", TRUE },
		{ "FALSE", FALSE },
		
		{ "IDENT", IDENT},

		{ "ICONST", ICONST },
		{ "RCONST", RCONST },
		{ "SCONST", SCONST },
		{ "BCONST", BCONST },
		
		{ "PLUS", PLUS },
		{ "MINUS", MINUS },
		{ "MULT", MULT },
		{ "DIV", DIV },
		{ "ASSOP", ASSOP},
		{ "EQUAL", EQUAL },
		{ "GTHAN", GTHAN },
		{ "LTHAN", LTHAN },
		{ "AND", AND },
		{ "OR", OR },
		{ "NOT", NOT },
            
		{ "COMMA", COMMA},
		{ "LPAREN", LPAREN },
		{ "RPAREN", RPAREN },
		
		
		{ "SEMICOL", SEMICOL },
		
		{ "ERR", ERR },

		{ "DONE", DONE },
	};
    
    for (auto i : tokString){
      if(tok.GetToken() <= 10 && tok.GetToken() == i.second){
        return out<<i.first<<endl;
    }
      if(tok.GetToken() >= 16 && tok.GetToken() <= 31 && tok.GetToken() == i.second){
          if(tok.GetToken() == 31)
              return out<<"Error: : \""<< tok.GetLexeme()<<"\" at Line " << tok.GetLinenum()<<endl;
        return out<<i.first<<endl;
    }
      if(tok.GetToken() >= 12 && tok.GetToken() <= 15 && tok.GetToken() == i.second){
        if(tok.GetToken() == 14 && tok.GetToken() == i.second)
            return out<<"SCONST("<<tok.GetLexeme()<<")"<<endl;
        if(tok.GetToken() == 15 && tok.GetToken() == i.second)
            return out<<"BCONST("<<tok.GetLexeme()<<")"<<endl;
        return out<<i.first<<"("<<tok.GetLexeme()<<")"<<endl;
    }
        if(tok.GetLexeme() == i.first && tok.GetToken() == 11)
            return out<<i.first<<"("<<tok.GetLexeme()<<")"<<endl;
        
    }
    return out<<"IDENT("<<tok.GetLexeme()<<")" <<endl;
}
