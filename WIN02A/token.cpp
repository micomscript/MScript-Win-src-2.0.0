////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
Begin license text.

Copyright 2023- MicomScript Project

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and 
associated documentation files (the ÅgSoftwareÅh), to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial 
portions of the Software.

THE SOFTWARE IS PROVIDED ÅgAS ISÅh, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
IN THE SOFTWARE.

End license text.
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * tokenn.cpp
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "common.h"
#include "token.h"

Token::~Token() {
#ifdef DEBUG
	printf("delete Token...");
	print();
#endif
}

// éØï éq
TIdentifier::TIdentifier(const char *name){
	_code = T_Name;
	_string = new char[strlen(name) + 1];
	strcpy(_string, name);
}

TIdentifier::~TIdentifier(){
#ifdef DEBUG
	printf("delete Token...");
	print();
#endif
	delete [] _string;
}

// ï∂éöóÒ
TString::TString(const char *string){
	_code = T_Text;
	_string = new char[strlen(string) + 1];
	strcpy(_string, string);
}

TString::~TString(){
#ifdef DEBUG
	printf("delete Token...");
	print();
#endif
	delete [] _string;
}

//ï\é¶
void Token::print() {
	printf("Token\n");
}

void TIdentifier::print(){
	printf("Identifier(%s)\n", _string);
}

void TString::print(){
	printf("Text(%s)\n", _string);
}

void TInteger::print(){
	printf("Integer(%d)\n", _value);
}

void TFloat::print(){
	printf("Float(%f)\n", _value);
}

void KeyWord::print(){
	printf("KeyWord(code = %d)\n", _keyCode);
}

void TResource::print(){
	printf("TResource(id = %d)\n", _resID);
}

void Symbol::print(){
	if(_code == T_Plus)				printf("+");
	else if(_code == T_Minus)		printf("-");
	else if(_code == T_Aster)		printf("*");
	else if(_code == T_Slash)		printf("/");
	else if(_code == T_Percent)		printf("%");
	else if(_code == T_Aster2)		printf("**");
	else if(_code == T_Plus2)		printf("++");
	else if(_code == T_Minus2)		printf("--");
	else if(_code == T_Equal)		printf("=");
	else if(_code == T_PlusEQ)		printf("+=");
	else if(_code == T_MinusEQ)		printf("-=");
	else if(_code == T_AsterEQ)		printf("*=");
	else if(_code == T_SlashEQ)		printf("/=");
	else if(_code == T_PercentEQ)	printf("%=");
	else if(_code == T_Aster2EQ)	printf("**=");
	else if(_code == T_AndEQ)		printf("&=");
	else if(_code == T_OrEQ)		printf("|=");
	else if(_code == T_ExorEQ)		printf("^=");
	else if(_code == T_TildeEQ)		printf("~=");
	else if(_code == T_LShiftEQ)	printf("<<=");
	else if(_code == T_RShiftEQ)	printf(">>=");
	else if(_code == T_RShift2EQ)	printf(">>>=");
	else if(_code == T_And)			printf("&");
	else if(_code == T_Or)			printf("|");
	else if(_code == T_Exor)		printf("^");
	else if(_code == T_Not)			printf("~");
	else if(_code == T_LShift)		printf("<<");
	else if(_code == T_RShift)		printf(">>");
	else if(_code == T_RShift2)		printf(">>>");
	else if(_code == T_GT)			printf(">");
	else if(_code == T_GE)			printf(">=");
	else if(_code == T_LE)			printf("<=");
	else if(_code == T_LT)			printf("<");
	else if(_code == T_EQ)			printf("==");
	else if(_code == T_NE)			printf("!=");
	else if(_code == T_LogicAnd)	printf("&&");
	else if(_code == T_LogicOr)		printf("||");
	else if(_code == T_LogicNot)	printf("!");
	else if(_code == T_Question)	printf("?");
	else if(_code == T_Colon)		printf(":");
	else if(_code == T_SemiColon)	printf(";");
	else if(_code == T_LPar)		printf("(");
	else if(_code == T_RPar)		printf(")");
	else if(_code == T_LBracket)	printf("[");
	else if(_code == T_RBracket)	printf("]");
	else if(_code == T_LCurly)		printf("{");
	else if(_code == T_RCurly)		printf("}");
	else if(_code == T_Dot)			printf(".");
	else if(_code == T_Comma)		printf(",");
	else if(_code == T_Undef)		printf("Undefined character");
	else							printf("Ñ™");
	printf("\n");
}

