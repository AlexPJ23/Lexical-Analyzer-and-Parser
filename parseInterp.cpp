#include "parseInt.h"

map<string, bool> defVar;
map<string, Token> SymTable;
map<string, Value> TempsResults; //Container of temporary locations of Value objects for results of expressions, variables values and constants 
queue <Value> * ValQue; //declare a pointer variable to a queue of Value objects

namespace Parser {
	bool pushed_back = false;
	LexItem	pushed_token;

	static LexItem GetNextToken(istream& in, int& line) {
		if( pushed_back ) {
			pushed_back = false;
			return pushed_token;
		}
		return getNextToken(in, line);
	}

	static void PushBackToken(LexItem & t) {
		if( pushed_back ) {
			abort();
		}
		pushed_back = true;
		pushed_token = t;	
	}

}

static int error_count = 0;

int ErrCount()
{
    return error_count;
}

void ParseError(int line, string msg)
{
	++error_count;
	cout << line << ": " << msg << endl;
}
//PROGRAM IDENT StmtList END PROGRAM
bool Prog(istream& in, int& line)
{
	bool f1;
	LexItem tok = Parser::GetNextToken(in, line);	
	if (tok.GetToken() == PROGRAM) {
		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == IDENT) {
			f1 = StmtList(in, line); 
			if(f1) {
				tok = Parser::GetNextToken(in, line);
				if( tok == END){
					tok = Parser::GetNextToken(in, line);
					if(tok != PROGRAM){
						ParseError(line, "Missing PROGRAM Keyword.");
						return false;
					}
					return true;
				}
				else
				{
					ParseError(line, "Missing END of Program.");
					return false;
				}
				
			}
			else
			{
				ParseError(line, "Incorrect Program Body.");
				return false;
			}
		}
		else
		{
			ParseError(line, "Missing Program Name.");
			return false;
		}
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else if(tok.GetToken() == DONE && tok.GetLinenum() <= 1){
		ParseError(line, "Empty File");
		return true;
	}
	ParseError(line, "Missing PROGRAM.");
	return false;
}
//StmtList ::= Stmt; { Stmt; }
bool StmtList(istream& in, int& line){
    LexItem tok;
    bool f1 = Stmt(in, line);
    while(f1){
        tok = Parser::GetNextToken(in,line);
        if(tok.GetToken() !=SEMICOL){
            ParseError(line,"Missing SemiColon!");
            return false;
        }
        f1 = Stmt(in,line);
    }
    tok = Parser::GetNextToken(in,line);
    if(tok.GetToken() == END)
    {
        Parser::PushBackToken(tok);
        return true;
    }
    else if(tok.GetToken() == ELSE)
    {
        Parser::PushBackToken(tok);
        return true;
    }
    else{
        ParseError(line,"Sytactic error in Program!");
        return false;
    }
    return false;
}
//DeclStmt ::= ( INT | FLOAT | BOOL ) VarList 
bool DeclStmt(istream& in, int& line){
    LexItem tok = Parser::GetNextToken(in,line);
    if (tok.GetToken() == INT || tok.GetToken() == FLOAT || tok.GetToken() == BOOL){
        if(!VarList(in,line,tok))
        {
            ParseError(line, "Incorrect Var!");
            return false;
        }

    }else
    {
        ParseError(line,"Not a type!");
        return false;
    }
    return true;

}
//ControlStmt ::= AssigStmt | IfStmt | PrintStmt
bool ControlStmt(istream& in, int& line)
{
	bool status;
	
	LexItem t = Parser::GetNextToken(in, line);
	
	switch( t.GetToken() ) {

	case PRINT:
        Parser::PushBackToken(t);
		status = PrintStmt(in, line);
		break;

	case IF:
        Parser::PushBackToken(t);
		status = IfStmt(in, line);
		break;

	case IDENT:
		Parser::PushBackToken(t);
        status = AssignStmt(in, line);
		break;
		
	default:
		Parser::PushBackToken(t);
		return false;
	}

	return status;
}
//Stmt ::= DeclStmt | ControlStmt 
bool Stmt(istream& in, int& line){
	bool status=false;
	
	LexItem t = Parser::GetNextToken(in, line);
	
	switch( t.GetToken() ) {
	case INT: case FLOAT: case BOOL:
		Parser::PushBackToken(t);
		status = DeclStmt(in, line);
		if(!status)
		{
			ParseError(line, "Incorrect Declaration Statement.");
			return status;
		}
		break;
	case IF: case PRINT: case IDENT: 
		Parser::PushBackToken(t);
		status = ControlStmt(in, line);
		
		if(!status)
		{
			ParseError(line, "Incorrect control Statement.");
			return status;
		}
		break;
	default:
		Parser::PushBackToken(t);
	}
	return status;
}
// IfStmt ::= IF (Expr) THEN StmtList [ ELSE StmtList ] END IF
bool IfStmt(istream& in, int& line){
    LexItem tok;
    Value val1,val2;
    bool status;
    if ((tok=Parser::GetNextToken(in,line)).GetToken() == IF){
        tok = Parser::GetNextToken(in,line);
        if(tok.GetToken() == LPAREN){
            status = Expr(in,line,val1);
            if(!status)
            {
                ParseError(line,"Invalid Expression!");
                return false;
            }
            if((tok=Parser::GetNextToken(in,line)).GetToken() == RPAREN){
                tok = Parser::GetNextToken(in,line);
                if(tok.GetToken() != THEN){
                    ParseError(line, "No Then KeyWord!");
                    return false;
                }
                status = StmtList(in,line);
                if(!status){
                    ParseError(line,"Statement List is invalid!");
                    return false;
                }
                if((tok=Parser::GetNextToken(in,line)).GetToken() == ELSE ){
                    status = StmtList(in,line);
                    if(!status)
                    {
                        ParseError(line,"Statment List is invalid in Else!");
                        return false;
                    }

                }else if ((tok=Parser::GetNextToken(in,line)).GetToken() == END){
                    if((tok=Parser::GetNextToken(in,line)).GetToken() == IF){
                        return true;
                    } else{
                        ParseError(line,"No End-IF!");
                        return false;
                    }
                }else{
                    ParseError(line,"No END Statment!");
                    return false;
                }
            }
        }
    }
    return true;
}
//AssignStmt ::= Var = Expr
bool AssignStmt(istream& in, int& line){
    LexItem t;
    Value val1;
    bool status;
    status = Var(in,line,(t=Parser::GetNextToken(in,line)));
    if(!status){
        ParseError(line,"Missing LHS Var");
        return false;
    }
    t = Parser::GetNextToken(in,line);
    if(t.GetToken() != ASSOP || t.GetToken() == ERR){
        ParseError(line,"No Equal operator");
        cout << "(" << t.GetLexeme() << ")" << endl;
        return false;
    }
    
    status = Expr(in,line,val1);
    if (!status){
        ParseError(line,"Expression is invalid");
        return false;
    }
    return true;
}
// VarList ::= Var { ,Var }
bool VarList(istream& in, int& line, LexItem& type)
{
	bool status = false;
	string identstr;
	
	LexItem tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == IDENT)
	{
		identstr = tok.GetLexeme();
		if (!(defVar.find(identstr)->second))
		{
			defVar[identstr] = true;
		}	
		else
		{
			ParseError(line, "Variable Redefinition");
			return false;
		}
		
	}
	else
	{
		Parser::PushBackToken(tok);
		return true;
	}
	
	tok = Parser::GetNextToken(in, line);
	
	if (tok == COMMA) {
		status = VarList(in, line, tok);
	}
	else {
		Parser::PushBackToken(tok);
		return true;
	}
	return status;	
}
// Var ::= IDENT
bool Var(istream& in, int& line, LexItem& type){
    string identstr;
	
	if (type.GetToken() == IDENT){
		identstr = type.GetLexeme();
		if (!(defVar.find(identstr)->second))
		{
			ParseError(line, "Undeclared Variable");
			return false;
		}	
		return true;
	}
	else if(type.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << type.GetLexeme() << ")" << endl;
		return false;
	}
	
	return false;

}
//ExprList ::= Expr { , Expr }
bool ExprList(istream& in, int& line){
    bool status;
    Value val1;
    LexItem t;
    status = Expr(in,line,val1);
    if(!status){
        ParseError(line,"Missing Expression");
        return false;
    }
    t = Parser::GetNextToken(in,line);
    if (t.GetToken() == COMMA){
        status = ExprList(in,line);
    } else if (t.GetToken() == ERR){
        ParseError(line,"Unrecognized Input");
        return false;
    }
    else{
        Parser::PushBackToken(t);
        return true;
    }
    return true;
}
//Expr ::= LogORExpr ::= LogANDExpr { || LogANDRxpr }
bool Expr(istream& in, int& line,Value& retVal){
    LexItem tok;
    Value val1,val2;
	bool t1 = LogANDExpr(in, line, val1);
	if( !t1 ) {
		return false;
	}
	
	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	while ( tok == OR ) 
	{
		t1 = LogANDExpr(in, line, val2);
		if( !t1 ) 
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}
		
		tok = Parser::GetNextToken(in, line);
		if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}		
	}
	Parser::PushBackToken(tok);
	return true;

}
//EqualExpr ::= RelExpr [== RelExpr ] 
bool EqualExpr(istream& in, int& line, Value & retVal){
    LexItem tok;
    Value val1,val2;
	bool t1 = RelExpr(in, line,val1);
		
	if( !t1 ) {
		return false;
	}
	
	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	if ( tok == EQUAL ) 
	{
		t1 = RelExpr(in,line,val2);
		if( !t1 ) 
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}
		
		tok = Parser::GetNextToken(in, line);
		if(tok == EQUAL)
		{
			ParseError(line, "Illegal Equality Expression.");
			return false;
		}
		else if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}		
	}
	Parser::PushBackToken(tok);
	return true;

}
//RelExpr ::= AddExpr [ ( < | > ) AddExpr ]
bool RelExpr(istream& in, int& line, Value & retVal){
	LexItem tok;
    Value val1,val2;
	bool t1 = AddExpr(in, line,val1);
		
	if( !t1 ) {
		return false;
	}
	
	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	if ( tok == LTHAN || tok == GTHAN) 
	{
		t1 = AddExpr(in, line,val2);
		if( !t1 ) 
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}
		
		tok = Parser::GetNextToken(in, line);
		
		if(tok == LTHAN || tok == GTHAN)
		{
			ParseError(line, "Illegal Relational Expression.");
			return false;
		}
		else if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}		
	}
	Parser::PushBackToken(tok);
	return true;
}
// AddExpr :: MultExpr { ( + | - ) MultExpr }
bool AddExpr(istream& in, int& line, Value & retVal){
    LexItem tok;
    Value val1,val2;
    bool t1 = MultExpr(in, line,val1);
	
	if( !t1 ) {
		return false;
	}
	
	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	while ( tok == PLUS || tok == MINUS ) 
	{
		t1 = MultExpr(in, line,val2);
		if( !t1 ) 
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}
		
		tok = Parser::GetNextToken(in, line);
		if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}		
	}
	Parser::PushBackToken(tok);
	return true;

}
//MultExpr ::= UnaryExpr { ( * | / ) UnaryExpr }
bool MultExpr(istream& in, int& line, Value & retVal){
    LexItem tok;
    Value val1,val2;
    bool t1 = UnaryExpr(in, line,val1);
	
	if( !t1 ) {
		return false;
	}
	
	tok	= Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
	}
	while ( tok == MULT || tok == DIV  )
	{
		t1 = UnaryExpr(in, line,val2);
		
		if( !t1 ) {
			ParseError(line, "Missing operand after operator");
			return false;
		}
		
		tok	= Parser::GetNextToken(in, line);
		if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}
	}
	Parser::PushBackToken(tok);
	return true;
}
//UnaryExpr ::= ( - | + | ! ) PrimaryExpr | PrimaryExpr
bool UnaryExpr(istream& in, int& line, Value & retVal){
    LexItem t = Parser::GetNextToken(in, line);
	bool status;
	int sign = 0;
	if(t.GetToken() == MINUS )
	{
		sign = -1;
	}
	else if(t.GetToken() == PLUS)
	{
		sign = 1;
	}else if(t.GetToken() == NOT)
    {
        retVal = !retVal;
    }
	else
		Parser::PushBackToken(t);
		
	status = PrimaryExpr(in, line, sign,retVal);
	return status;

}
//PrimaryExpr ::= IDENT | ICONST | RCONST | SCONST | BCONST | ( Expr ))
bool PrimaryExpr(istream& in, int& line,int sign, Value & retVal){
    LexItem tok = Parser::GetNextToken(in, line);
	

	if( tok == IDENT ) {
		
		string lexeme = tok.GetLexeme();
		if (!(defVar.find(lexeme)->second))
		{
			ParseError(line, "Using Undeclared Variable");
			return false;	
		}
		return true;
	}
	else if( tok == ICONST ) {
		
		return true;
	}
	else if( tok == SCONST ) {
		
		return true;
	}
	else if( tok == RCONST ) {
		
		return true;
	}
	else if( tok == BCONST ) {
		
		return true;
	}
	else if( tok == LPAREN ) {
		bool ex = Expr(in, line,retVal);
		if( !ex ) {
			ParseError(line, "Missing expression after Left Parenthesis");
			return false;
		}
		if( Parser::GetNextToken(in, line) == RPAREN )
			return ex;
		else 
		{
			Parser::PushBackToken(tok);
			ParseError(line, "Missing right Parenthesis after expression");
			return false;
		}
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	return false;
}
bool IdentList(istream& in, int& line);


//PrintStmt:= PRINT (ExpreList) 
bool PrintStmt(istream& in, int& line) {
	LexItem t;
	//cout << "in PrintStmt" << endl;
	ValQue = new queue<Value>;
	t = Parser::GetNextToken(in, line);
	if( t != LPAREN ) {
		
		ParseError(line, "Missing Left Parenthesis");
		return false;
	}
	
	bool ex = ExprList(in, line);
	
	if( !ex ) {
		ParseError(line, "Missing expression list after Print");
		while(!(*ValQue).empty())
		{
			ValQue->pop();		
		}
		delete ValQue;
		return false;
	}
	
	//Evaluate: print out the list of expressions' values
	while (!(*ValQue).empty())
	{
		Value nextVal = (*ValQue).front();
		cout << nextVal;
		ValQue->pop();
	}
	cout << endl;
	
	t = Parser::GetNextToken(in, line);
	if(t != RPAREN ) {
		
		ParseError(line, "Missing Right Parenthesis");
		return false;
	}
	
	return true;
}//End of PrintStmt

//LogANDExpr ::= EqualExpr { && EqualExpr }
bool LogANDExpr(istream& in, int& line, Value & retVal) {
	LexItem tok;
	Value val1, val2;
	bool t1 = EqualExpr(in, line, val1);
	//cout << "status of EqualExpr and val1: " << t1<< " " << val1.IsErr() << " " << val1.IsBool() << endl;
	//cout << "status of EqualExpr: " << t1<< endl;
		
	if( !t1 ) {
		return false;
	}
	//cout << "value of var1 in AND op " << val1.GetBool() << endl;
	retVal = val1;
	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	while ( tok == AND ) 
	{
		t1 = EqualExpr(in, line, val2);
		//cout << "value of var1 in AND op " << val1.GetBool() << endl;
		if( !t1 ) 
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}
		//evaluate expression for Logical AND
		retVal = retVal && val2;
		//cout << "AND op result " << retVal.IsBool() << " " << retVal.GetBool() << endl;
		if(retVal.IsErr())
		{
			ParseError(line, "Illegal AND operation.");
			//cout << "(" << tok.GetLexeme() << ")" << endl;		
			return false;
		}
		tok = Parser::GetNextToken(in, line);
		if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}		
	}
	
	Parser::PushBackToken(tok);
	return true;
}//End of LogANDExpr
