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
 *	tokenizer.cpp
 */

#include <iostream>
#include <string.h>
#include <ctype.h>
#include "common.h"

#ifdef T_ESP32
#include <FS.h>
#include <SPIFFS.h>
#include "comedit.h"
#include "fsystem.h"
#endif

#include "token.h"
#include "tokenizer.h"

using namespace std;

Symbol _t_eol		= Symbol(T_EOL);		// EndOfLine
Symbol _t_plus		= Symbol(T_Plus);		// +
Symbol _t_minus		= Symbol(T_Minus);		// -
Symbol _t_aster		= Symbol(T_Aster);		// *
Symbol _t_slash		= Symbol(T_Slash);		// /
Symbol _t_percent	= Symbol(T_Percent);	// %
Symbol _t_aster2	= Symbol(T_Aster2);		// **
Symbol _t_plus2		= Symbol(T_Plus2);		// ++
Symbol _t_minus2	= Symbol(T_Minus2);		// --
Symbol _t_equal		= Symbol(T_Equal);		// =
Symbol _t_plusEQ	= Symbol(T_PlusEQ);		// +=
Symbol _t_minusEQ	= Symbol(T_MinusEQ);	// -=
Symbol _t_asterEQ	= Symbol(T_AsterEQ);	// *=
Symbol _t_slashEQ	= Symbol(T_SlashEQ);	// /=
Symbol _t_percentEQ	= Symbol(T_PercentEQ);	// %=
Symbol _t_aster2EQ	= Symbol(T_Aster2EQ);	// **=
Symbol _t_andEQ		= Symbol(T_AndEQ);		// &=
Symbol _t_orEQ		= Symbol(T_OrEQ);		// |=
Symbol _t_exorEQ	= Symbol(T_ExorEQ);		// ^=
Symbol _t_tildeEQ	= Symbol(T_TildeEQ);	// ~=
Symbol _t_lshiftEQ	= Symbol(T_LShiftEQ);	// <<=
Symbol _t_rshiftEQ	= Symbol(T_RShiftEQ);	// >>=
Symbol _t_rshift2EQ	= Symbol(T_RShift2EQ);	// >>>=
Symbol _t_and		= Symbol(T_And);		// &
Symbol _t_or		= Symbol(T_Or);			// |
Symbol _t_exor		= Symbol(T_Exor);		// ^
Symbol _t_not		= Symbol(T_Not);		// ~
Symbol _t_lshift	= Symbol(T_LShift);		// <<
Symbol _t_rshift	= Symbol(T_RShift);		// >>
Symbol _t_rshift2	= Symbol(T_RShift2);	// >>>
Symbol _t_gt		= Symbol(T_GT);			// >
Symbol _t_ge		= Symbol(T_GE);			// >=
Symbol _t_le		= Symbol(T_LE);			// <=
Symbol _t_lt		= Symbol(T_LT);			// <
Symbol _t_eq		= Symbol(T_EQ);			// ==
Symbol _t_ne		= Symbol(T_NE);			// !=
Symbol _t_logicAnd	= Symbol(T_LogicAnd);	// &&
Symbol _t_logicOr	= Symbol(T_LogicOr);	// ||
Symbol _t_logicNot	= Symbol(T_LogicNot);	// !
Symbol _t_question	= Symbol(T_Question);	// ?
Symbol _t_colon		= Symbol(T_Colon);		// :
Symbol _t_semicolon	= Symbol(T_SemiColon);	// ;
Symbol _t_lpar		= Symbol(T_LPar);		// (
Symbol _t_rpar		= Symbol(T_RPar);		// )
Symbol _t_lbracket	= Symbol(T_LBracket);	// [
Symbol _t_rbracket	= Symbol(T_RBracket);	// ]
Symbol _t_lcurly	= Symbol(T_LCurly);		// {
Symbol _t_rcurly	= Symbol(T_RCurly);		// }
Symbol _t_dot		= Symbol(T_Dot);		// .
Symbol _t_comma		= Symbol(T_Comma);		// ,
Symbol _t_undef		= Symbol(T_Undef);		// Undefined Char

KeyWord _k_var		= KeyWord(K_Var);		// var
KeyWord _k_function	= KeyWord(K_Function);	// function
KeyWord _k_return	= KeyWord(K_Return);	// return
KeyWord _k_logic	= KeyWord(K_Logic);		// logic
KeyWord _k_if		= KeyWord(K_If);		// if
KeyWord _k_else		= KeyWord(K_Else);		// else
KeyWord _k_do		= KeyWord(K_Do);		// do
KeyWord _k_while	= KeyWord(K_While);		// while
KeyWord _k_for		= KeyWord(K_For);		// for
KeyWord _k_times	= KeyWord(K_Times);		// times
KeyWord _k_repeat	= KeyWord(K_Repeat);	// repeat
KeyWord _k_break	= KeyWord(K_Break);		// break
KeyWord _k_continue	= KeyWord(K_Continue);	// continue
KeyWord _k_match	= KeyWord(K_Match);		// match
KeyWord _k_case		= KeyWord(K_Case);		// case
KeyWord _k_default	= KeyWord(K_Default);	// default
KeyWord _k_foreach	= KeyWord(K_Foreach);	// foreach

KeyWord _k_tree		= KeyWord(K_Tree);		// tree
KeyWord _k_exit		= KeyWord(K_Exit);		// exit
KeyWord _k_varlist	= KeyWord(K_VarList);	// varlist
KeyWord _k_conlist	= KeyWord(K_ConList);	// conlist
KeyWord _k_funclist	= KeyWord(K_FuncList);	// funclist
KeyWord _k_undef	= KeyWord(K_Undef);		// undef
KeyWord _k_help 	= KeyWord(K_Help);		// help
#ifdef T_ESP32
KeyWord _k_clistESP	= KeyWord(K_ESPCon);	// clist_ESP

//ファイル操作コマンド
KeyWord _k_files	= KeyWord(K_Files);		// files
KeyWord _k_save		= KeyWord(K_Save);		// save
KeyWord _k_load		= KeyWord(K_Load);		// load
KeyWord _k_show		= KeyWord(K_Show);		// show
KeyWord _k_remove	= KeyWord(K_Remove);	// remove
KeyWord _k_rename	= KeyWord(K_Rename);	// rename
KeyWord _k_reset	= KeyWord(K_Reset);		// reset
#endif

TResource _resWiFi	= TResource(R_WiFi);		// WiFi

// キーワード
KeyWords _keyWords[] = {
	{ "var"		, &_k_var 		},
	{ "function", &_k_function 	},
	{ "return"	, &_k_return 	},
	{ "logic"	, &_k_logic		},
	{ "if"		, &_k_if 		},
	{ "else"	, &_k_else 		},
	{ "do"		, &_k_do		},
	{ "while"	, &_k_while 	},
	{ "for"		, &_k_for 		},
	{ "times"	, &_k_times		},
	{ "repeat"	, &_k_repeat 	},
	{ "break"	, &_k_break 	},
	{ "continue", &_k_continue 	},
	{ "match"	, &_k_match 	},
	{ "case"	, &_k_case 		},
	{ "default"	, &_k_default 	},
	{ "foreach"	, &_k_foreach	},
//コマンド
	{ "tree"	, &_k_tree		},
	{ "exit"	, &_k_exit 		},
	{ "vlist"	, &_k_varlist	},
	{ "clist"	, &_k_conlist	},
	{ "flist"	, &_k_funclist	},
	{ "undef"	, &_k_undef		},
	{ "help"	, &_k_help		},
#ifdef T_ESP32
	{ "clist_ESP", &_k_clistESP	},
//ファイル操作コマンド
	{ "files"	, &_k_files		},
	{ "save"	, &_k_save		},
	{ "load"	, &_k_load		},
	{ "show"	, &_k_show		},
	{ "remove"	, &_k_remove	},
	{ "rename"	, &_k_rename	},
	{ "reset"	, &_k_reset		},
#endif
	{ "*", NULL }
};

// リソース名
KeyWords _resource[] = {
	{ "WiFi"	, &_resWiFi		},
	{ "*"		, NULL			}
};

extern int _sentence_depth;
extern int _system_mode;
int _line_number;

#ifdef T_ESP32
CommandEditor	_command; // コマンド入力
#endif

// name と同じキーワードを探す
Token *searchKeyWord(char *name) {
	int i;

	for(i = 0;_keyWords[i]._token != NULL; i++) {
		if(strcmp(name, _keyWords[i]._name) == 0)
			return _keyWords[i]._token;
	}
	return NULL;
}

char *searchKeyName(int KeyCode) {
	int i;
	for(i = 0;_keyWords[i]._token != NULL; i++) {
		if(KeyCode == _keyWords[i]._token->getKeyCode())
			return _keyWords[i]._name;
	}
	return NULL;
}

// name と同じリソースを探す
Token *searchResource(char *name) {
	int i;

	for(i = 0;_resource[i]._token != NULL; i++) {
		if(strcmp(name, _resource[i]._name) == 0)
			return _resource[i]._token;
	}
	return NULL;
}

void Tokenizer::putBack(Token *token) {
	if(_count < TOKEN_STACK_SIZE) {
		_stack[_count++] = token;
	}
}

// 一行入力
#ifdef T_ESP32
void Tokenizer::readLine() {
	if(_system_mode == 0) {	// Dialog Mode
		_command.readLine(_inBuffer, IN_BUFF_SIZE, true);
	} else {	// Script Mode
		int i = 0;
		int c;
		while (_file.available()){
			c = _file.read();
			_inBuffer[i++] = c;
			if (c == 0x0a)
				break;
		}
		_inBuffer[i] = '\0';
		if (i == 0){
			strcpy(_inBuffer, "exit");
		}
	}
	_line_number++;
	_nextChar = _inBuffer;
	_empty = false;
}
#endif

#ifdef T_WINDOWS
void Tokenizer::readLine() {
	if(_system_mode == 0) { // 対話モード
		if (_sentence_depth >= 1){
			printf("  ");
//			for(int i = 1; i < _sentence_depth ; i++){
//				printf("\t");
//			}
		} else {
			printf("$ ");
		}
		fgets(_inBuffer, IN_BUFF_SIZE, stdin);
	} else { // スクリプトモード
		if(fgets(_inBuffer, IN_BUFF_SIZE, _file) == 0)
			strcpy(_inBuffer, "exit");
	}
	_line_number++;
	_nextChar = _inBuffer;
	_empty = false;
}
#endif

#ifdef T_WINDOWS
int Tokenizer::setScript(char *s) {
	_file = fopen(s,"r");
	if(_file == NULL)
		return -1;
	_line_number = 0;
	return 0;
}
#endif

#ifdef T_ESP32
int Tokenizer::setScript(char *s) {
	_file = SPIFFS.open(s);
	if(!_file){
//		printf("Can not open script (%s)\n", s);
		return false;
	}
	_system_mode = 1;	// Script Mode
	_line_number = 1;
	_empty = true;
	return true;
}
#endif

#ifdef T_ESP32
void Tokenizer::closeScript(){
	_file.close();
	_system_mode = 0;	// return to Dialog Mode
}
#endif

//字句を一つ返す
Token *Tokenizer::getToken() {
	char word[MAX_STRING+1];
	char *s,ch;
	int n;

	//putBack された字句があれば、それを返す
	if(_count > 0) {
		return _stack[--_count];
	}

	//空なら一行読む
	if(_empty) {
		readLine();
	}

	//空白
	while(isspace(*_nextChar)) {
		_nextChar++;
	}

	while(*_nextChar == '/') {
		_nextChar++;
		if(*_nextChar == '=') {
			_nextChar++;
			return &_t_slashEQ;
		}
		if(*_nextChar == '/') {
			_empty = true;
			return &_t_eol;
		}
		if(*_nextChar != '*') {
			return &_t_slash;
		}
		//複数行コメント　開始
		_nextChar++;
		ch = ' ';
		while (ch != '*' || *_nextChar != '/'){
			ch = *_nextChar++;
			if (ch == '\0'){
				ch = ' ';
				readLine();
			}
		}
		_nextChar++;
		// 空白を読み飛ばす
		while (isspace(*_nextChar)){
			_nextChar++;
		}
	}

	//行末
	if(*_nextChar=='\0') {
		_empty = true;
		return &_t_eol;
	}

	//識別子
	if(isalpha(*_nextChar) || *_nextChar == '_') {
		s = word;
		n = 0;
		while(isalnum(*_nextChar) || *_nextChar == '_') {
			if(n < MAX_NAME) {
				*s++ = *_nextChar++;
				n++;
			} else {
				_nextChar++;
			}
		}
		*s = '\0';
		Token *t = searchKeyWord(word);
		if(t != NULL)
			return t;					//キーワード
		t = searchResource(word);
		if(t != NULL)
			return t;					//リソース
		return new TIdentifier(word);	//名前
	}

	//文字列
	if(*_nextChar == '"') {
		s = word;
		_nextChar++;
		n = 0;
		while(*_nextChar != '"') {
			if(*_nextChar == '\0') {
				return &_t_undef;
			} else if(*_nextChar == '\\') {
				_nextChar++;
				if(*_nextChar == '"')
					*s++ = '"';
				else if(*_nextChar == '\\')
					*s++ = '\\';
				else if(*_nextChar == 'n')
					*s++ = '\n';
				else if(*_nextChar == 'r')
					*s++ = '\r';
				else if(*_nextChar == 't')
					*s++ = '\t';
				else if(*_nextChar == 'b')
					*s++ = '\b';
				else
					*s++ = *_nextChar;
				_nextChar++;
			} else {
				*s++ = *_nextChar++;
			}
		}
		*s = '\0';
		_nextChar++;
		return new TString(word);
	}

	//文字コード
	if(*_nextChar == '\'') {
		_nextChar++;
		long value = (long)*_nextChar++;
		if(*_nextChar++ != '\'') {
			_empty = true;
			return &_t_undef;	// Error
		}
		return new TInteger(value);
	}

	//数値
	if ((*_nextChar == '.') || isdigit(*_nextChar)){
		int numf = 0, decf = 0, dec = 1;
		long value = 0, value_dec = 0;
		// ビット
		if(*_nextChar++ == '0') {
			// 二進数
			if(*_nextChar == 'b') {
				_nextChar++;
				while(isdigit(*_nextChar)) {
					if(*_nextChar == '1' || *_nextChar == '0')
						value = 2*value + *_nextChar++ - '0';
					else {
						return &_t_undef;	// Error
					}
				}
				return new TInteger(value);
			//十六進数
			} else if(*_nextChar == 'x') {
				int val_alpha;
				_nextChar++;
				while(isdigit(*_nextChar) || isalpha(*_nextChar)) {
					if(isdigit(*_nextChar)) {
						value = 16*value + *_nextChar++ - '0';
					} else if(isalpha(*_nextChar)) {
						if(islower(*_nextChar))
							val_alpha = *_nextChar - 'a';
						else
							val_alpha = *_nextChar - 'A';
						if(val_alpha > 6) {
							return &_t_undef;	// Error
						}
						value = 16*value + 10 + val_alpha;
						_nextChar++;
					}
				}
				return new TInteger(value);
			//八進数
			} else if(*_nextChar != '.') {
				while(isdigit(*_nextChar)) {
					if(*_nextChar == '9' || *_nextChar == '8') {
						return &_t_undef;	// Error
					}
					value = 8*value + *_nextChar++ - '0';
				}
				return new TInteger(value);
			} else {
				_nextChar--;
			}
		} else {
			_nextChar--;
		}

		double v;
		int code = 0;
		// 整数部
		while (isdigit(*_nextChar)){
			value = 10*value + *_nextChar++ - '0';
			numf = 1;
		}
		// 小数部
		if (*_nextChar == '.'){
			_nextChar++;
			while (isdigit(*_nextChar)){
				value_dec = 10*value_dec + *_nextChar++ - '0';
				dec *= 10;
				decf = 1;
			}
			if ((numf == 0) && (decf == 0)) // . のみ
				return &_t_dot;
			code = T_Float;
			v = (double)(value + (double)value_dec/dec);
		}
		if(code == 0) {
			code = T_Integer;
			v = value;
		}
		if(*_nextChar == 'e') {
			_nextChar++;
			bool ch = true;
			int value_e = 0;
			if(*_nextChar == '+')
				ch = true;
			else if(*_nextChar == '-')
				ch = false;
			else if(isdigit(*_nextChar))
				_nextChar--;
			else {
				return &_t_undef;	// Error
			}
			_nextChar++;
			if(!isdigit(*_nextChar)) {
				return &_t_undef;	// Error
			}

			while(isdigit(*_nextChar)) {
				value_e = 10*value_e + *_nextChar++ - '0';
			}
			if(ch)
				v = v*(pow(10,value_e));
			else
				v = v/(pow(10,value_e));
				if(v < 1)
					code = T_Float;
		}
		if(code == T_Integer)
			return new TInteger((long)v);
		return new TFloat(v);
	}


	//記号
	int c = *_nextChar++;
	if(c == '=') {
		if(*_nextChar == '=') {
			_nextChar++;
			return &_t_eq;
		}
		return &_t_equal;
	}
	if(c == '+') {
		if(*_nextChar == '=') {
			_nextChar++;
			return &_t_plusEQ;
		}
		if(*_nextChar == '+') {
			_nextChar++;
			return &_t_plus2;
		}
		return &_t_plus;
	}
	if(c == '-') {
		if(*_nextChar == '=') {
			_nextChar++;
			return &_t_minusEQ;
		}
		if(*_nextChar == '-') {
			_nextChar++;
			return &_t_minus2;
		}
		return &_t_minus;
	}
	if(c == '*') {
		if(*_nextChar == '=') {
			_nextChar++;
			return &_t_asterEQ;
		}
		if(*_nextChar == '*') {
			_nextChar++;
			if(*_nextChar == '=') {
				_nextChar++;
				return &_t_aster2EQ;
			}
			return &_t_aster2;
		}
		return &_t_aster;
	}
	if(c == '%') {
		if(*_nextChar == '=') {
			_nextChar++;
			return &_t_percentEQ;
		}
		return &_t_percent;
	}
	if(c == '&') {
		if(*_nextChar == '=') {
			_nextChar++;
			return &_t_andEQ;
		}
		if(*_nextChar == '&') {
			_nextChar++;
			return &_t_logicAnd;
		}
		return &_t_and;
	}
	if(c == '|') {
		if(*_nextChar == '=') {
			_nextChar++;
			return &_t_orEQ;
		}
		if(*_nextChar == '|') {
			_nextChar++;
			return &_t_logicOr;
		}
		return &_t_or;
	}
	if(c == '^') {
		if(*_nextChar == '=') {
			_nextChar++;
			return &_t_exorEQ;
		}
		return &_t_exor;
	}
	if(c == '~') {
		if(*_nextChar == '=') {
			_nextChar++;
			return &_t_tildeEQ;
		}
		return &_t_not;
	}
	if(c == '<') {
		if(*_nextChar == '=') {
			_nextChar++;
			return &_t_le;
		}
		if(*_nextChar == '<') {
			_nextChar++;
			if(*_nextChar == '=') {
				_nextChar++;
				return &_t_lshiftEQ;
			}
			return &_t_lshift;
		}
		return &_t_lt;
	}
	if(c == '>') {
		if(*_nextChar == '=') {
			_nextChar++;
			return &_t_ge;
		}
		if(*_nextChar == '>') {
			_nextChar++;
			if(*_nextChar == '=') {
				_nextChar++;
				return &_t_rshiftEQ;
			}
			if(*_nextChar == '>') {
				_nextChar++;
				if(*_nextChar == '=') {
					_nextChar++;
					return &_t_rshift2EQ;
				}
				return &_t_rshift2;
			}
			return &_t_rshift;
		}
		return &_t_gt;
	}
	if(c == '!') {
		if(*_nextChar == '=') {
			_nextChar++;
			return &_t_ne;
		}
		return &_t_logicNot;
	}
	if(c == '?')
		return &_t_question;
	if(c == ':')
		return &_t_colon;
	if(c == ';')
		return &_t_semicolon;
	if(c == '(')
		return &_t_lpar;
	if(c == ')')
		return &_t_rpar;
	if(c == '[')
		return &_t_lbracket;
	if(c == ']')
		return &_t_rbracket;
	if(c == '{')
		return &_t_lcurly;
	if(c == '}')
		return &_t_rcurly;
	if(c == ',')
		return &_t_comma;

	//未定義
	return &_t_undef;
}
