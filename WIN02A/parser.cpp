////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
Begin license text.

Copyright 2023- MicomScript Project

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and 
associated documentation files (the “Software”), to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial 
portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
IN THE SOFTWARE.

End license text.
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 *	parser.cpp
 *	構文解析
 */

#include<iostream>
#include<string.h>
#include "common.h"
#ifdef T_ESP32
#include <FS.h>
#include <SPIFFS.h>
#include "fsystem.h"
#include "comedit.h"
#endif
#include "expression.h"
#include "token.h"
#include "tokenizer.h"
#include "logic.h"
#include "sentence.h"
#include "function.h"
#include "parser.h"
#include "varmap.h"

using namespace std;

int _sentence_depth;
bool print_Mode;

extern VarMapStack _varMap;
extern FunctionMap _funcMap;
extern int _sentence_depth;
extern int _line_number;
extern int _system_mode;

#ifdef T_ESP32
extern MSFileSystem	_fileSystem;
#endif

int Parser::setScript(char *s) {
	return _tokenizer.setScript(s);
}

//字句を一つ読み取り、字句種別コードを返す
int Parser::nextToken() {
	if(_token != NULL)
		_token->clear();	//直前の字句を解放
	_token = _tokenizer.getToken();
	_code = _token->getCode();
	_keyCode = _token->getKeyCode();
	return _code;
}

void Parser::putBack() {
	_tokenizer.putBack(_token);
	_token = NULL;
}

// 文法に従い、入力された文字列の構文を解析し、
// 数式を処理する。
void Parser::parse() {
	Sentence *s = NULL;
	while(true) {
		_sentence_depth = 0;
		try {
			s = sentence();
			if(s) {
				print_Mode = s->printMode();
				s->execute();
				delete s;
				print_Mode = false;
			}
		} catch(ExitException e) {
#ifdef T_WINDOWS
			clearSysFuncs();
			clearConstants();
			return;
#endif
		}
#ifdef T_ESP32
		catch(ReturnException e) {
			delete s;
		}
#endif
		catch(ErrorException e) {
			delete s;
		}
	}
}

Sentence *Parser::sentence() {
	nextToken();
	if(_code == T_KeyWord) {
		if(_keyCode == K_Exit)				// exit
			throw ExitException();
		else if(_keyCode == K_Var)			// var
			return var_sentence();
		else if(_keyCode == K_Function)		// function
			return func_sentence();
		else if(_keyCode == K_If)			// if
			return if_sentence();
		else if(_keyCode == K_While)		// while
			return while_sentence(false);
		else if(_keyCode == K_Do)			// do-while
			return while_sentence(true);
		else if(_keyCode == K_For)			// for
			return for_sentence();
		else if(_keyCode == K_Foreach)		// foreach
			return foreach_sentence();
		else if(_keyCode == K_Times)		// times
			return times_sentence();
		else if(_keyCode == K_Repeat)		// repeat
			return repeat_sentence();
		else if(_keyCode == K_Break)		// break
			return new Break();
		else if(_keyCode == K_Continue)		// continue
			return new Continue();
		else if(_keyCode == K_Return)		// return
			return return_sentence();
		else if(_keyCode == K_Tree)			// tree
			return tree_sentence();
		else if(_keyCode == K_VarList) {	// vlist
			_varMap.printVars();
			return NULL;
		} else if(_keyCode == K_ConList) {	// clist
			printConstants();
			return NULL;
		} else if(_keyCode == K_FuncList) { // flist
			_funcMap.printFunctions();
			return NULL;
		} else if(_keyCode == K_Undef) {	// undef
			return undef_sentence();
		} else if(_keyCode == K_Help) {		// help
			return help_sentence();
#ifdef T_ESP32
		} else if(_keyCode == K_ESPCon) {	// clist_ESP
			printESPCon();
			return NULL;
		} else if(_keyCode == K_Reset) {	// reset
			return new Reset();
		} else if(_keyCode == K_Files) {	// files
			return file_list();
		} else if(_keyCode == K_Save) {		// save
			return save_file();
		} else if(_keyCode == K_Load) {		// load
			return load_file();
		} else if(_keyCode == K_Show) {		// show
			return show_file();
		} else if(_keyCode == K_Remove) {	// remove
			return remove_file();
#endif
		} else {
			printf("Error : Unimplemented name(keyCode = %d, line = %d)\n",_token->getKeyCode(),_line_number);
			return NULL;
		}
	} else if(_code == T_EOL) {
		return NULL;
	} else if(_code == T_LCurly) {
		return block_sentence();
	}
	// 式文
	putBack();
	return express_sentence();
}

#ifdef T_ESP32
Sentence *Parser::file_list(){
	int code = nextToken();
	if (code == T_EOL){
		_fileSystem.listFiles("/");
	} else if (code == T_Text){
		_fileSystem.listFiles(_token->getString());
	} else {
		printf("files \"path-name\" (line = %d)\n",_line_number);
	}
	return NULL;
}

Sentence *Parser::save_file(){
	int code = nextToken();
	if (code == T_Text){
		_fileSystem.saveFile(_token->getString());
	} else {
		printf("save \"path-name\" (line = %d)\n",_line_number);
	}
	return NULL;
}

Sentence *Parser::show_file(){
	int code = nextToken();
	if (code == T_Text){
		_fileSystem.showFile(_token->getString());
	} else {
		printf("show \"path-name\" (line = %d)\n",_line_number);
	}
	return NULL;
}

Sentence *Parser::load_file(){
	int code = nextToken();
	char *path;
	if (code != T_Text){
		printf("load \"path-name\" (line = %d)\n",_line_number);
		return NULL;
	}
	path= _token->getString();
	int lineNumber = _line_number;
	if (!setScript(path)){
		printf("failed to open script(name = %s, line = %d)\n", path,_line_number);
		return NULL;
	}
	parse();
	_tokenizer.closeScript();
	_line_number = lineNumber;
	return NULL;
}

void Parser::execute_file(char *path){
	int lineNumber = _line_number;
	if (setScript(path)){
		parse();
		_tokenizer.closeScript();
	}
	_line_number = lineNumber;
}

Sentence *Parser::remove_file(){
	int code = nextToken();
	if (code == T_Text){
		_fileSystem.removeFile(_token->getString());
	} else {
		printf("remove \"path-name\" (line = %d)\n",_line_number);
	}
	return NULL;
}
#endif

// tree
Sentence *Parser::tree_sentence() {
	if(nextToken() != T_LPar) {
		printf("SyntaxError : Missing '(' (line = %d)\n",_line_number);
		return NULL;
	}
	Expression *e = expression();
	if(e == NULL)
		return NULL;

	if(nextToken() != T_RPar) {
		printf("SyntaxError : Missing ')' (line = %d)\n",_line_number);
		delete e;
		return NULL;
	}
	return new TreeSentence(e);
}

// return
Sentence *Parser::return_sentence() {
	ReturnSentence *s = new ReturnSentence();
	if(nextToken() != T_EOL) {
		putBack();
		Expression *e = expression();
		if(e)
			s->setExpression(e);
	}
	return s;
}

// 複文
Sentence *Parser::block_sentence() {
	BlockSentence *block = new BlockSentence();
	Sentence *s;

	while(true) {
		while(nextToken() == T_EOL)
			;
		if(_code == T_RCurly) {
			break;
		}
		putBack();
		s = sentence();
		if(s != NULL)
			block->addSentence(s);
		else {
			delete block;
			return NULL;
		}
	}
	return block;
}

// var の後に続く分の処理を行う関数
Sentence *Parser::var_sentence(){
	Expression *e = NULL;
	VarSentence *s = new VarSentence();
	if (nextToken() != T_Name){
		printf("SyntaxError : Unexpected name (line = %d)\n",_line_number);
		delete s;
		return NULL;
	}
	char name[MAX_NAME+1];
	strcpy(name, _token->getString());
	if(nextToken() == T_Equal) {
		e = expression();
		if(e == NULL) {
			delete s;
			return NULL;
		}
	} else {
		putBack();
	}
	s->append(name, e);
	while (nextToken() == T_Comma) {
		e = NULL;
		if (nextToken() != T_Name) {
			printf("SyntaxError : Unexpected name (line = %d)\n",_line_number);
			delete s;
			return NULL;
		}
		strcpy(name, _token->getString());
		if(nextToken() == T_Equal) {
			e = expression();
			if(e == NULL) {
				delete s;
				return NULL;
			}
		} else {
			putBack();
		}
		s->append(name, e);
	}
	return s;
}

// function
Sentence *Parser::func_sentence() {
	char *func_name;
	UserFunction *user_func = new UserFunction();
	if(nextToken() != T_Name) {
		printf("SyntaxError : Unexpected name (line = %d)\n",_line_number);
		delete user_func;
		return NULL;
	}
	func_name = new char[strlen(_token->getString())+1];
	strcpy(func_name,_token->getString());
	user_func->setName(func_name);
	if(nextToken() != T_LPar) {
		printf("SyntaxError : Unexpected token '(' (line = %d)\n",_line_number);
		delete user_func;
		return NULL;
	}
	if(nextToken() != T_RPar) {
		while(_code == T_Name) {
			user_func->addParam(_token->getString());
			if(nextToken() != T_Comma) {
				break;
			}
			nextToken();
		}
	}
	if(_code != T_RPar) {
		printf("SyntaxError : Unexpected token ')' (line = %d)\n",_line_number);
		delete user_func;
		return NULL;
	}
	_sentence_depth++;
	while(nextToken() == T_EOL)
		;
	putBack();

	Sentence *s = sentence();
	_sentence_depth--;
	if(s == NULL) {
		printf("SyntaxError : Unexpected sentence (line = %d)\n",_line_number);
		delete user_func;
		return NULL;
	}
	user_func->setSentence(s);
	_funcMap.putFunction(func_name,user_func);
	return NULL;
}

Sentence *Parser::undef_sentence() {
	do {
		if(nextToken() != T_Name) {
			printf("SyntaxError : Unexpected name (line = %d)\n",_line_number);
			return NULL;
		}
		char *name = new char[strlen(_token->getString())+1];
		strcpy(name,_token->getString());
		if(nextToken() == T_LPar) {
			if(nextToken() != T_RPar) {
				printf("SyntaxError : missing token ')' (line = %d)\n",_line_number);
				delete name;
				return NULL;
			}
			if(strcmp(name,"all") == 0)
				_funcMap.clearFunctions();
			else
				_funcMap.removeFunction(name);
			return NULL;
		}
		putBack();
		if(strcmp(name,"all") == 0)
			_varMap.clearAll();
		else
			_varMap.removeVar(name);
	} while(nextToken() == T_Comma);
	putBack();
	return NULL;
}

Sentence *Parser::help_sentence() {
	if(nextToken() == T_EOL) {
		putBack();
		printf("[Command　List]\n");
		printf("  vlist			- print Variable  list\n");
		printf("  clist			- print Constants list\n");
		printf("  flist			- print Function  list\n");
		printf("  undef			- undefine Variable/Function\n");
		printf("  exit			- finish execution\n");
		return NULL;
	}
	if(_code == T_KeyWord) {
		if(_keyCode == K_VarList) {
			printf("定義した変数の変数名と値を表示します。\n");
		} else if(_keyCode == K_ConList) {
			printf("定数の名前とその値を表示します。\n");
		} else if(_keyCode == K_FuncList) {
			printf("定義した関数の名前を表示します。\n");
		} else if(_keyCode == K_Undef) {
			printf("変数、または関数を削除します。\n");
			printf("undef 変数名 で変数を削除し、undef 関数名() で関数を削除します。\n");
		} else if(_keyCode == K_Exit) {
			printf("作業を終了し、処理を終了します。\n");
		} else {
			printf("\"%s\" というコマンドは存在しません。\n",_token->getString());
		}
	}
	return NULL;
}

// -----------------------------------------------------------------------

// if文
Sentence *Parser::if_sentence() {
	Sentence *s;
	IfSentence *if_sentence = new IfSentence();
	if(nextToken() != T_LPar) {
		printf("SyntaxError : Unexpected token '(' (line = %d)\n",_line_number);
		delete if_sentence;
		return NULL;
	}
	Expression *logic = expression();
	if(logic == NULL) {
		delete if_sentence;
		return NULL;
	}
	if(nextToken() != T_RPar) {
		printf("SyntaxError : Unexpected token ')' (line = %d)\n",_line_number);
		delete logic;
		delete if_sentence;
		return NULL;
	}
	_sentence_depth++;
	while(nextToken() == T_EOL)
		;
	putBack();

	s = sentence();
	_sentence_depth--;
	if(s == NULL) {
		delete if_sentence;
		delete logic;
		return NULL;
	}

	if_sentence->addIfSentence(new IfNode(logic,s));

	_sentence_depth++;
	while(nextToken() == T_EOL)
		;
	putBack();
	if(nextToken() == T_SemiColon){
		_sentence_depth--;
		return if_sentence;
	}
	_sentence_depth--;
	putBack();

	while(nextToken() == T_KeyWord && _keyCode == K_Else) {
		if(nextToken() == T_KeyWord && _keyCode == K_If) {
			if(nextToken() != T_LPar) {
				printf("SyntaxError : Unexpected token '(' (line = %d)\n",_line_number);
				delete if_sentence;
				return NULL;
			}
			Expression *logic = expression();
			if(logic == NULL) {
				delete if_sentence;
				return NULL;
			}
			if(nextToken() != T_RPar) {
				printf("SyntaxError : Unexpected token ')' (line = %d)\n",_line_number);
				delete logic;
				delete if_sentence;
				return NULL;
			}
			_sentence_depth++;
			while(nextToken() == T_EOL)
				;
			putBack();

			s = sentence();
			_sentence_depth--;
			if(s == NULL) {
				delete if_sentence;
				delete logic;
				return NULL;
			}

			if_sentence->addIfSentence(new IfNode(logic,s));

			while(nextToken() == T_EOL)
				;
			putBack();

			if(nextToken() == T_SemiColon)
				return if_sentence;
			putBack();
		} else {
			putBack();
			_sentence_depth++;
			while(nextToken() == T_EOL)
				;
			putBack();

			s = sentence();
			_sentence_depth--;
			if(s == NULL) {
				delete if_sentence;
				return NULL;
			}

			if_sentence->addIfSentence(new IfNode(NULL,s));
			return if_sentence;
		}
	}
	putBack();
	return if_sentence;
}

// while文
Sentence *Parser::while_sentence(bool ch) {
	Sentence *s = NULL;
	if(ch == true) {	// do_while文
		_sentence_depth++;
		while(nextToken() == T_EOL)
			;
		putBack();

		s = sentence();
		_sentence_depth--;
		if(s == NULL)
			return NULL;

		if(nextToken() != T_KeyWord || _keyCode != K_While) {
			printf("SyntaxError : Unexpected token (line = %d)\n",_line_number);
			delete s;
			return NULL;
		}
	}
	if(nextToken() != T_LPar) {
		printf("SyntaxError : Unexpected token '(' (line = %d)\n",_line_number);
		delete s;
		return NULL;
	}
	Expression *logic = expression();
	if(logic == NULL) {
		delete s;
		return NULL;
	}
	if(nextToken() != T_RPar) {
		printf("SyntaxError : Unexpected token ')' (line = %d)\n",_line_number);
		delete logic;
		delete s;
		return NULL;
	}
	if(ch == false) {
		_sentence_depth++;
		while(nextToken() == T_EOL)
			;
		putBack();

		s = sentence();
		_sentence_depth--;
		if(s == NULL) {
			delete logic;
			return NULL;
		}
		return new WhileSentence(logic,s,ch);
	}
	return new WhileSentence(logic,s,ch);
}

// for文
Sentence *Parser::for_sentence() {
	char vname[MAX_NAME+1];
	char *name;
	bool var_ch;
	InitialVal *initial_val= new InitialVal();

	Expression *e;
	if(nextToken() != T_LPar) {
		printf("SyntaxError : Unexpected token '(' (line = %d)\n",_line_number);
		delete initial_val;
		return NULL;
	}

	if(nextToken() != T_SemiColon) {
		if((_code == T_KeyWord) && (_keyCode == K_Var)) {
			var_ch = true;
		} else {
			putBack();
			var_ch = false;
		}
		do {
			if(nextToken() != T_Name) {
				printf("SyntaxError : Unexpected name (line = %d)\n",_line_number);
				delete initial_val;
				return NULL;
			}
			strcpy(vname, _token->getString());
			name = vname;
			if(nextToken() != T_Equal) {
				printf("SyntaxError : missing token '=' (line = %d)\n",_line_number);
				delete initial_val;
				delete name;
				return NULL;
			}
			e = expression();
			if(e == NULL) {
				delete initial_val;
				delete name;
				return NULL;
			}
			initial_val->setInitialVal(new InitialNode(var_ch,name,e));
		} while(nextToken() == T_Comma);

		if(_code != T_SemiColon) {
			printf("SyntaxError : missing token ';' (line = %d)\n",_line_number);
			delete initial_val;
			return NULL;
		}
	}

	Expression *logic = expression();
	if(logic == NULL) {
		delete initial_val;
		return NULL;
	}

	if(nextToken() != T_SemiColon) {
		printf("SyntaxError : missing token ';' (line = %d)\n",_line_number);
		delete initial_val;
		delete logic;
		return NULL;
	}

	ExprList *expr = new ExprList();
	if(nextToken() != T_RPar) {
		putBack();
		do {
			e = expression();
			if(e == NULL) {
				delete initial_val;
				delete logic;
				delete expr;
				return NULL;
			}
			expr->append(e);
		} while(nextToken() == T_Comma);

		if(_code != T_RPar) {
			printf("SyntaxError : missing token ')' (line = %d)\n",_line_number);
			delete initial_val;
			delete logic;
			delete expr;
			return NULL;
		}
	}
	_sentence_depth++;
	while(nextToken() == T_EOL)
		;
	putBack();

	Sentence *s = sentence();
	_sentence_depth--;
	if(s == NULL) {
		delete initial_val;
		delete logic;
		delete expr;
		return NULL;
	}
	return new ForSentence(initial_val, logic, expr, s);
}

// foreach文
Sentence *Parser::foreach_sentence() {
	char *var, *list;
	if(nextToken() != T_LPar) {
		printf("SyntaxError : missing token ( (line = %d)\n",_line_number);
		return NULL;
	}
	if(nextToken() != T_Name) {
		printf("SyntaxError : Unexpected name (line = %d)\n",_line_number);
		return NULL;
	}
	var = new char[strlen(_token->getString())+1];
	strcpy(var,_token->getString());

	nextToken();
	if(strcmp(_token->getString(),"in") != 0) {
		printf("SyntaxError : Please enter \"in\" (line = %d)\n",_line_number);
		delete var;
		return NULL;
	}

	if(nextToken() != T_Name) {
		printf("SyntaxError : Unexpected name '('line = %d)\n",_line_number);
		delete var;
		return NULL;
	}
	list = new char[strlen(_token->getString())+1];
	strcpy(list,_token->getString());

	if(nextToken() != T_RPar) {
		printf("SyntaxError : missing token ')' (line = %d)\n",_line_number);
		delete var;
		delete list;
		return NULL;
	}
	_sentence_depth++;
	while(nextToken() == T_EOL)
		;
	putBack();

	Sentence *s = sentence();
	_sentence_depth--;
	if(s == NULL) {
		delete var;
		delete list;
		return NULL;
	}
	return new ForeachSentence(var,list,s);
}

// times文
Sentence *Parser::times_sentence() {
	if(nextToken() != T_LPar) {
		printf("SyntaxError : missing token '(' (line = %d)\n",_line_number);
		return NULL;
	}
	Expression *cnt = expression();
	if(cnt == NULL) {
		return NULL;
	}
	
	if(nextToken() != T_RPar) {
		printf("SyntaxError : missing token ')' (line = %d)\n",_line_number);
		delete cnt;
		return NULL;
	}
	_sentence_depth++;
	while(nextToken() == T_EOL)
		;
	putBack();
	Sentence *s = sentence();
	_sentence_depth--;
	if(s == NULL) {
		delete cnt;
		return NULL;
	}
	return new TimesSentence(cnt, s);
}

// repeat文
Sentence *Parser::repeat_sentence() {
	char *vname;

	if(nextToken() != T_LPar) {
		printf("SyntaxError : missing token '(' (line = %d)\n",_line_number);
		return NULL;
	}

	if(nextToken() != T_Name) {
		printf("SyntaxError : Unexpected name (line = %d)\n",_line_number);
		return NULL;
	}
	vname = new char[strlen(_token->getString())+1];
	strcpy(vname, _token->getString());

	if(nextToken() != T_Equal) {
		printf("SyntaxError : missing token '=' (line = %d)\n",_line_number);
		delete vname;
		return NULL;
	}
	Expression *e1 = expression();
	if(e1 == NULL) {
		delete vname;
		return NULL;
	}

	if(nextToken() != T_Comma) {
		printf("SyntaxError : missing token ',' (line = %d)\n",_line_number);
		delete vname;
		delete e1;
		return NULL;
	}

	Expression *e2 = expression();
	if(e2 == NULL) {
		delete vname;
		delete e1;
		return NULL;
	}

	Expression *e3 = NULL;
	if(nextToken() == T_Comma) {
		e3 = expression();
		if(e3 == NULL) {
			delete vname;
			delete e1;
			delete e2;
			return NULL;
		}
	} else {
		putBack();
	}

	if(nextToken() != T_RPar) {
		printf("SyntaxError : missing token ')' (line = %d)\n",_line_number);
		delete vname;
		delete e1;
		delete e2;
		delete e3;
		return NULL;
	}
	_sentence_depth++;
	while(nextToken() == T_EOL)
		;
	putBack();

	Sentence *s = sentence();
	_sentence_depth--;
	if(s == NULL) {
		delete vname;
		delete e1;
		delete e2;
		delete e3;
		return NULL;
	}

	return new RepeatSentence(vname, e1, e2, e3, s);
}

Sentence *Parser::express_sentence() {
	Expression *e = expression();
	if(e == NULL)
		return NULL;
	if(nextToken() != T_EOL) {
		printf("SyntaxError (line = %d)\n",_line_number);
		delete e;
		return NULL;
	}
	return new FormulaSentence(e);
}

// ----------------------------------------------------------
// 式の解析

//<式>
Expression *Parser::expression(){
	char name[MAX_NAME+1];
	Expression *e, *e2 = NULL;
	nextToken();

	if(_code == T_Name) {
		strcpy(name, _token->getString());
		nextToken();
		if(_code == T_LPar) {
			FuncName *func = new FuncName(name);
			if(nextToken() == T_RPar)
				return logic_or(func);
			putBack();
			do {
				if(nextToken() != T_EOL)
					putBack();
				e = expression();
				if(e == NULL) {
					delete func;
					return NULL;
				}
				func->addParam(e);
			} while(nextToken() == T_Comma);

			if(_code == T_RPar) {
				return logic_or(func);
			}

			printf("SyntaxError : missing token ')' (line = %d)\n",_line_number);
			delete func;
			return NULL;
		//メソッド
		} else if(_code == T_Dot) {
			MethodCall *method;
			nextToken();
			if(_code == T_Name)
				method = new MethodCall(name, _token->getString());
			else if(_code == T_KeyWord) {
				char key[MAX_NAME+1];
				char *k = searchKeyName(_keyCode);
				if(k == NULL) {
					printf("SyntaxError : Unexpected name (line = %d)\n",_line_number);
					return NULL;
				}
				strcpy(key,k);
				method = new MethodCall(name,key);
			} else {
				printf("SyntaxError : Unexpected name (line = %d)\n",_line_number);
				return NULL;
			}
			if(nextToken() != T_LPar) {
				printf("SyntaxError : missing token '(' (line = %d)\n",_line_number);
			}
			if(nextToken() != T_RPar) {
				putBack();
				Expression *e;
				do {
					e = expression();
					if(e == NULL)
						return NULL;
					method->addParam(e);
				} while(nextToken() == T_Comma);

				if(_code != T_RPar) {
					printf("SyntaxError : missing token ')' (line = %d)\n",_line_number);
				}
			}
			return logic_or(method);
		}

		VarObject *vobj = new VarName(name);
		if(_code == T_Equal) {
			e = expression();
			if(e == NULL) {
				return NULL;
			}
			return new EqSubstitution(vobj, e);
		} else if(_code == T_PlusEQ) {
			e = expression();
			if(e == NULL) {
				return NULL;
			}
			return new AddSubstitution(vobj, e);
		} else if(_code == T_MinusEQ) {
			e = expression();
			if(e == NULL) {
				return NULL;
			}
			return new SubSubstitution(vobj, e);
		} else if(_code == T_AsterEQ) {
			e = expression();
			if(e == NULL) {
				return NULL;
			}
			return new MulSubstitution(vobj, e);
		} else if(_code == T_SlashEQ) {
			e = expression();
			if(e == NULL) {
				return NULL;
			}
			return new DivSubstitution(vobj, e);
		} else if(_code == T_PercentEQ) {
			e = expression();
			if(e == NULL) {
				return NULL;
			}
			return new ModSubstitution(vobj, e);
		} else if(_code == T_Aster2EQ) {
			e = expression();
			if(e == NULL) {
				return NULL;
			}
			return new PowSubstitution(vobj, e);
		} else if(_code == T_AndEQ) {
			e = expression();
			if(e == NULL) {
				return NULL;
			}
			return new AndSubstitution(vobj, e);
		} else if(_code == T_OrEQ) {
			e = expression();
			if(e == NULL) {
				return NULL;
			}
			return new OrSubstitution(vobj, e);
		} else if(_code == T_ExorEQ) {
			e = expression();
			if(e == NULL) {
				return NULL;
			}
			return new ExorSubstitution(vobj, e);
		} else if(_code == T_LShiftEQ) {
			e = expression();
			if(e == NULL) {
				return NULL;
			}
			return new LShiftSubstitution(vobj, e);
		} else if(_code == T_RShiftEQ) {
			e = expression();
			if(e == NULL) {
				return NULL;
			}
			return new RShiftSubstitution(vobj, e);
		}

		if(_code == T_LBracket) {
			while(_code == T_LBracket) {
				e = expression();
				if(e == NULL) {
					delete vobj;
					return NULL;
				}
				if(nextToken() != T_RBracket) {
					printf("SyntaxError :  missing token '[' (line = %d)\n",_line_number);
					delete e;
					delete vobj;
					return NULL;
				}
				vobj = new Index(vobj, e);
				if(nextToken() == T_Equal) {
					e2 = expression();
					if(e2 == NULL) {
						delete e;
						delete vobj;
						return NULL;
					}
					return new EqSubstitution(vobj, e2);
				} else if(_code == T_PlusEQ) {
					e2 = expression();
					if(e2 == NULL) {
						delete e;
						delete vobj;
						return NULL;
					}
					return new AddSubstitution(vobj, e2);
				} else if(_code == T_MinusEQ) {
					e2 = expression();
					if(e2 == NULL) {
						delete e;
						delete vobj;
						return NULL;
					}
					return new SubSubstitution(vobj, e2);
				} else if(_code == T_AsterEQ) {
					e2 = expression();
					if(e2 == NULL) {
						delete e;
						delete vobj;
						return NULL;
					}
					return new MulSubstitution(vobj, e2);
				} else if(_code == T_SlashEQ) {
					e2 = expression();
					if(e2 == NULL) {
						delete e;
						delete vobj;
						return NULL;
					}
					return new DivSubstitution(vobj, e2);
				} else if(_code == T_PercentEQ) {
					e2 = expression();
					if(e2 == NULL) {
						delete e;
						delete vobj;
						return NULL;
					}
					return new ModSubstitution(vobj, e2);
				} else if(_code == T_Aster2EQ) {
					e2 = expression();
					if(e2 == NULL) {
						delete e;
						delete vobj;
						return NULL;
					}
					return new PowSubstitution(vobj, e2);
				} else if(_code == T_AndEQ) {
					e2 = expression();
					if(e2 == NULL) {
						delete e;
						delete vobj;
						return NULL;
					}
					return new AndSubstitution(vobj, e2);
				} else if(_code == T_OrEQ) {
					e2 = expression();
					if(e2 == NULL) {
						delete e;
						delete vobj;
						return NULL;
					}
					return new OrSubstitution(vobj, e2);
				} else if(_code == T_ExorEQ) {
					e2 = expression();
					if(e2 == NULL) {
						delete e;
						delete vobj;
						return NULL;
					}
					return new ExorSubstitution(vobj, e2);
				} else if(_code == T_LShiftEQ) {
					e2 = expression();
					if(e2 == NULL) {
						delete e;
						delete vobj;
						return NULL;
					}
					return new LShiftSubstitution(vobj, e2);
				} else if(_code == T_RShiftEQ) {
					e2 = expression();
					if(e2 == NULL) {
						delete e;
						delete vobj;
						return NULL;
					}
					return new RShiftSubstitution(vobj, e2);
				}
			}
		}

		if(_code == T_Plus2)
			return logic_or(new Increment(vobj, false));
		if(_code == T_Minus2)
			return logic_or(new Decrement(vobj, false));
		putBack();
		return logic_or(vobj);
	}
	putBack();
	return logic_or(NULL);
}

//<論理和> ::= <論理積> | <論理和> '||' <論理積>
Expression *Parser::logic_or(Expression *e) {
	Expression *e1,*e2;
	e1 = logic_and(e);
	if(e1 == NULL) {
		return NULL;
	}
	while(nextToken() == T_LogicOr) {
		e2 = logic_and(NULL);
		if(e2 == NULL) {
			delete e1;
			return NULL;
		}
		e1 = new LogicOR(e1,e2);
	}
	putBack();
	return e1;
}

//<論理積> ::= <ビット和> | <論理積> '&&' <ビット和>
Expression *Parser::logic_and(Expression *e) {
	Expression *e1,*e2;
	e1 = bit_or(e);
	if(e1 == NULL) {
		return NULL;
	}
	while(nextToken() == T_LogicAnd) {
		e2 = bit_or(NULL);
		if(e2 == NULL) {
			delete e1;
			return NULL;
		}
		e1 = new LogicAND(e1,e2);
	}
	putBack();
	return e1;
}

//<ビット和> ::= <排他的ビット和> | <ビット和> '|' <排他的ビット和>
Expression *Parser::bit_or(Expression *e) {
	Expression *e1,*e2;
	e1 = bit_exor(e);
	if(e1 == NULL) {
		return NULL;
	}
	while(nextToken() == T_Or) {
		e2 = bit_exor(NULL);
		if(e2 == NULL) {
			delete e1;
			return NULL;
		}
		e1 = new Or(e1,e2);
	}
	putBack();
	return e1;
}

//<排他的ビット和> ::= <ビット積> | <排他的ビット和> '^' <ビット積>
Expression *Parser::bit_exor(Expression *e) {
	Expression *e1,*e2;
	e1 = bit_and(e);
	if(e1 == NULL) {
		return NULL;
	}
	while(nextToken() == T_Exor) {
		e2 = bit_and(NULL);
		if(e2 == NULL) {
			delete e1;
			return NULL;
		}
		e1 = new Exor(e1,e2);
	}
	putBack();
	return e1;
}

//<ビット積> ::= <等価式> | <ビット積> '&' <等価式>
Expression *Parser::bit_and(Expression *e) {
	Expression *e1,*e2;
	e1 = equal_express(e);
	if(e1 == NULL) {
		return NULL;
	}
	while(nextToken() == T_And) {
		e2 = equal_express(NULL);
		if(e2 == NULL) {
			delete e1;
			return NULL;
		}
		e1 = new And(e1,e2);
	}
	putBack();
	return e1;
}

//<等価式> ::= <関係式> |
//             <等価式> '=='  <関係式>
//             <等価式> '!='  <関係式>
Expression *Parser::equal_express(Expression *e){
	Expression *e1, *e2;
	e1 = relation_express(e);
	if(e1 == NULL)
		return NULL;
	int code = nextToken();
	while(code == T_EQ || code == T_NE){
		e2 = relation_express(NULL);
		if(e2 == NULL){
			delete e1;
			return NULL;
		}
		if(code == T_EQ)
			e1 = new EQ(e1,e2);
		else if(code == T_NE)
			e1 = new NE(e1,e2);
		code = nextToken();
	}
	putBack();
	return e1;
}

//<関係式> ::= <シフト式> |
//             <関係式> '<'  <シフト式>
//             <関係式> '<=' <シフト式>
//             <関係式> '>'  <シフト式>
//             <関係式> '>=' <シフト式>
Expression *Parser::relation_express(Expression *e){
	Expression *e1, *e2;
	e1 = shift_express(e);
	if(e1 == NULL)
		return NULL;
	int code = nextToken();
	while(code == T_GT || code == T_GE || code == T_LE || code == T_LT){
		e2 = shift_express(NULL);
		if(e2 == NULL){
			delete e1;
			return NULL;
		}
		if(code == T_GT)
			e1 = new GT(e1,e2);
		else if(code == T_GE)
			e1 = new GE(e1,e2);
		else if(code == T_LT)
			e1 = new LT(e1,e2);
		else if(code == T_LE)
			e1 = new LE(e1,e2);
		code = nextToken();
	}
	putBack();
	return e1;
}

//<シフト式> ::= <加減式> | <シフト式> ('<<' | '>>') <加減式>
Expression *Parser::shift_express(Expression *e) {
	Expression *e1, *e2;
	e1 = add_express(e);
	if(e1 == NULL)
		return NULL;
	int code = nextToken();
	while(code == T_LShift || code == T_RShift) {
		e2 = add_express(NULL);
		if(e2 == NULL){
			delete e1;
			return NULL;
		}
		if(code == T_LShift)
			e1 = new LShift(e1,e2);
		else if(code == T_RShift)
			e1 = new RShift(e1,e2);
		code = nextToken();
	}
	putBack();
	return e1;
}

//<加減式> ::= <項> | <式> ('+' | '-') <項>
Expression *Parser::add_express(Expression *e){
	Expression *e1, *e2;

	e1 = term(e);
	if (e1 == NULL)
		return NULL;

	int code = nextToken();
	while(code == T_Plus || code == T_Minus) {
		e2 = term(NULL);

		if(e2 == NULL) {
			delete e1;
			return NULL;
		}
		if(code == T_Plus)
			e1 = new Addition(e1,e2);
		else if(code == T_Minus)
			e1 = new Subtraction(e1,e2);
		code = nextToken();
	}
	putBack();
	return e1;
}

//<項> ::= <因子> | <項> ('*' | '/' | '%') <因子>
Expression *Parser::term(Expression *e) {
	Expression *e1, *e2;
	e1 = factor(e);
	if(e1 == NULL)
		return NULL;

	int code = nextToken();
	while(code == T_Aster || code == T_Slash || code == T_Percent){
		e2 = factor(NULL);

		if(e2 == NULL) {
			delete e1;
			return NULL;
		}
		if(code == T_Aster)
			 e1 = new Multiplication(e1,e2);
		else if(code == T_Slash)
			e1 = new Division(e1,e2);
		else if(code == T_Percent)
			e1 = new Remainder(e1,e2);
		code = nextToken();
	}
	putBack();	//戻す
	return e1;
}

//<因子> ::= <粒子> | <粒子> '**' <因子>
Expression *Parser::factor(Expression *e) {
	Expression *e1,*e2;

	if (e)
		e1 = e;
	else
		e1 = particle();
	if(e1 == NULL)
		return NULL;

	if(nextToken() == T_Aster2) {
		e2 = factor(NULL);
		if(e2 == NULL) {
			delete e1;
			return NULL;
		}
		return new Exponentiation(e1,e2);
	}
	putBack();
	return e1;
}

//<粒子> ::= +<原子> | -<原子> | ~<原子> | !<原子>
Expression *Parser::particle() {
	Expression *e;

	int code = nextToken();
	if (code == T_Plus || code == T_Minus || code == T_Not || code == T_LogicNot){
		e = atom();
		if(e == NULL)
			return NULL;
		if (code == T_Minus)
			return new Minus(e);
		if (code == T_Not)
			return new Not(e);
		if (code == T_LogicNot)
			return new LogicNOT(e);
		return e;
	}
	putBack();
	return atom();
}

//<原子> ::= <数値> | <変数> | <変数>++ | <変数>-- |
//           <変数> . <関数> '(' <式>... ')' |
//           <関数> '(' <式>... ')' |
//           '(' <式> ')' |
Expression *Parser::atom() {
	char name[MAX_NAME+1];
	Expression *e = NULL;
	int code = nextToken();
	// <数値>
	if(code == T_Integer)
		return new Integer(_token->getInteger());
	if(code == T_Float)
		return new Float(_token->getFloat());
	if(code == T_Text)
		return new Text(_token->getString());

	// <変数> 
	if(code == T_Name) {
		strcpy(name, _token->getString());
		nextToken();
		// <関数>
		if(_code == T_LPar) {
			FuncName *func = new FuncName(name);
			if(nextToken() == T_RPar)
				return func;
			putBack();
			do {
				if(nextToken() != T_EOL)
					putBack();
				e = expression();
				if(e == NULL) {
					delete func;
					return NULL;
				}
				func->addParam(e);
			} while(nextToken() == T_Comma);

			if(_code == T_RPar) {
				return func;
			}

			printf("SyntaxError : missing token ')' (line = %d)\n",_line_number);
			delete func;
			return NULL;
		} else if(_code == T_Dot) {	// メソッド
			if(nextToken() != T_Name) {
				printf("SyntaxError : Unexpected name (line = %d)\n",_line_number);
				return NULL;
			}
			MethodCall *method = new MethodCall(name, _token->getString());
			if(nextToken() != T_LPar) {
				printf("SyntaxError : missing token '(' (line = %d)\n",_line_number);
				delete method;
				return NULL;
			}
			if(nextToken() != T_RPar) {
				putBack();
				Expression *e = NULL;
				do {
					e = expression();
					if(e == NULL){
						delete method;
						return NULL;
					}
					method->addParam(e);
				} while(nextToken() == T_Comma);

				if(_code != T_RPar) {
					printf("SyntaxError : missing token ')' (line = %d)\n",_line_number);
					delete method;
					return NULL;
				}
			}
			return method;
		}
		// <変数> | <変数>++ | <変数>-- | <変数>[<式>]
		VarObject *vobj = new VarName(name);
		if(_code == T_LBracket) {
			// <変数>[<式>]
			while(_code == T_LBracket) {
				e = expression();
				if(e == NULL) {
					return NULL;
				}
				if(nextToken() != T_RBracket) {
					printf("SyntaxError : missing token ']' (line = %d)\n",_line_number);
					delete e;
					return NULL;
				}
				vobj = new Index(vobj, e);
				nextToken();
			}
		}
	
		// <変数> ++ | --
		if(_code == T_Plus2)
			return new Increment(vobj, false);
		else if(_code == T_Minus2)
			return new Decrement(vobj, false);
		putBack();
		return vobj;	// <変数>
	}

	// (++ | --) <変数>
	if((code == T_Plus2) || (code == T_Minus2)) {
		if(nextToken() != T_Name) {
			printf("SyntaxError : Unexpected name (line = %d)\n",_line_number);
			return e;
		}
		VarObject *vobj = new VarName(_token->getString());
		if(nextToken() == T_LBracket) {
			while(_code == T_LBracket) {
				e = expression();
				if(e == NULL) {
					delete vobj;
					return NULL;
				}
				if(nextToken() != T_RBracket) {
					printf("SyntaxError : missing token ']' (line = %d)\n",_line_number);
					delete e;
					delete vobj;
					return NULL;
				}
				vobj = new Index(vobj, e);
				nextToken();
			}
			putBack();
			if(code == T_Plus2)
				return new Increment(vobj, true);
			return new Decrement(vobj, true);
		}
		putBack();
		if(code == T_Plus2)
			return new Increment(vobj, true);
		return new Decrement(vobj, true);
	}

	//'('<式>')'
	if(_code == T_LPar) {
		e = expression();
		if(nextToken() != T_RPar) {
			printf("SyntaxError : missing token ')' (line = %d)\n",_line_number);
		}
		//エラー
		return e;
	}

	// '[' <リスト> ']'
	if(_code == T_LBracket) {
		ExprList *list = new ExprList();
		if(nextToken() != T_RBracket) {
			putBack();
			do {
				if(nextToken() != T_EOL)
					putBack();
				e = expression();
				if(e == NULL) {
					delete list;
					return NULL;
				}
				list->append(e);
			} while(nextToken() == T_Comma);
		}
		if(_code != T_RBracket) {
			printf("SyntaxError : missing token ']' (line = %d)\n",_line_number);
			delete list;
			return NULL;
		}
		return list;
	}

	if(_code == T_Resource){
		ResMethodCall *method = NULL;
		Token *t = _token;
		_token = NULL;
		int res_id = t->getResID();
		nextToken();
		if(_code != T_Dot){
			printf("SyntaxError : missing '.' (line = %d)\n",_line_number);
			return NULL;
		}
		nextToken();
		if(_code != T_Name){
			printf("SyntaxError : method name must come.\n");
			return NULL;
		}
		method = new ResMethodCall(res_id, _token->getString());
		if(nextToken() != T_LPar) {
			printf("SyntaxError : missing token '(' (line = %d)\n",_line_number);
			delete method;
			return NULL;
		}
		if(nextToken() != T_RPar) {
			putBack();
			Expression *e;
			do {
				e = expression();
				if(e == NULL)
					return NULL;
				method->addParam(e);
			} while(nextToken() == T_Comma);
			if(_code != T_RPar) {
				printf("SyntaxError : missing token ')' (line = %d)\n",_line_number);
				delete method;
				return NULL;
			}
		}
		return method;
	}

	//エラー
	printf("SyntaxError : Invalid expression (line = %d)\n",_line_number);
	return e;	//エラー
}
