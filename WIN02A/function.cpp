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

#include <iostream>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "common.h"

#ifdef T_ESP32
#include <FS.h>
#include <SPIFFS.h>
#include "comedit.h"
#include "fsystem.h"
#endif

#include "expression.h"
#include "varmap.h"
#include "logic.h"
#include "sentence.h"
#include "function.h"
#include "fobj.h"
#ifdef T_ESP32
#include "function_m.h"
#endif
#include "format.h"

using namespace std;

extern VarMapStack _varMap;
extern bool print_Mode;
extern int _line_number;

#ifdef T_ESP32
extern CommandEditor	_command;
#endif
// ──────────────────────────────────
// 組み込み数学関数表

Function *_sysFuncs[] = {
// 三角関数
	new Sin("sin"), new Cos("cos"), new Tan("tan"),
// 逆三角関数
	new ASin("asin"), new ACos("acos"), new ATan("atan"), new ATan2("atan2"),
// 双曲線関数
	new Sinh("sinh"), new Cosh("cosh"), new Tanh("tanh"),
// 対数・指数
	new Exp("exp"), new Log("log"), new Log10("log10"),
// 平方根・立方根・べき乗
	new Sqrt("sqrt"), new Cbrt("cbrt"), new Pow("pow"),
// 切り上げ・切り捨て・四捨五入
	new Floor("floor"), new Ceil("ceil"), new Round("round"),
// その他
	new Fabs("fabs"), new Abs("abs"), new Hypot("hypot"),
	new Sum("sum"), new Max("max"), new Min("min"),
	new Random("random"), new SRand("srand"), new Printx("print"), new Println("println"),
	new Int("int"), new chFloat("float"), new chString("string"), new chBool("bool"),
	new Len("len"), new Type("type"), new Input("input"),
	new Time("time"), new Delay("delay"), new Millis("millis"), new Micros("micros"),
	new PrintF("printf"),
	new Isalpha("isalpha"),
	new ReadFile("readFile"), new WriteFile("writeFile"), new AppendFile("appendFile"),
	new FileOpen("open"),
//-----------------------------------------------------------------------------------------
#ifdef T_ESP32
	//設定
	new SetColor("setColor"),new TextSize("textSize"),
	//描画
	new ClearScreen("clearScreen"),new FillScreen("fillScreen"),new DrawLine("drawLine"),new DrawPixel("drawPixel"),
	new DrawString("drawString"),new DrawRect("drawRect"),new FillRect("fillRect"),
	new DrawCircle("drawCircle"),new FillCircle("fillCircle"),	new DrawTriangle("drawTriangle"),
	new FillTriangle("fillTriangle"),new DrawFastVLine("drawVline"),new DrawFastHLine("drawHline"),
	new DrawString("drawString"),new DrawEllipse("drawEllipse"),new FillEllipse("fillEllipse"),
	new DrawRoundRect("drawRoundRect"),	new FillRoundRect("fillRoundRect"),
	new DrawCircleHelper("drawCircleHelper"),new FillCircleHelper("fillCircleHelper"),
	//gpio制御
	new PinMode("pinMode"),new DigitalWrite("digitalWrite"),
	new AnalogRead("analogRead"), new DigitalRead("digitalRead"),
	new LedcWrite("ledcWrite"),new LedcSetup("ledcSetup"),new LedcAttachPin("ledcAttachPin"),
	new LedcDetachPin("ledcDetachPin"),new LedcWriteTone("ledcWriteTone"),new LedcRead("ledcRead"),
	new LedcReadFreq("ledcReadFreq"),
#endif
	NULL
};

Function *searchSysFunc(const char *name){
	for(int i = 0; _sysFuncs[i] ; i++){
		if (strcmp(name, _sysFuncs[i]->getName()) == 0)
			return _sysFuncs[i];
	}
	return NULL;
}

void clearSysFuncs(){
	for(int i = 0; _sysFuncs[i] ; i++){
		delete _sysFuncs[i];
	}
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// 組込み数学関数

void MathFunction::setParamValue(int pos, Value *value) {
	if(pos == 0) {
		_x = value->getFloat();
	}
	delete value;
}

void Pow::setParamValue(int pos, Value *value) {
	if(pos == 0)
		_x = value->getFloat();
	else if(pos == 1)
		_y = value->getFloat();
	delete value;
}

// ─────────────────────────────────
// 三角関数

Value *Sin::evaluate(){
	return new Float(sin(_x));
}

Value *Cos::evaluate(){
	return new Float(cos(_x));
}

Value *Tan::evaluate(){
	return new Float(tan(_x));
}

// ─────────────────────────────────
// 逆三角関数
Value *ASin::evaluate(){
	return new Float(asin(_x));
}

Value *ACos::evaluate(){
	return new Float(acos(_x));
}

Value *ATan::evaluate(){
		return new Float(atan(_x));
}

Value *ATan2::evaluate(){
	return new Float(atan2(_y, _x));
}

// ─────────────────────────────────
// 双曲線関数
Value *Sinh::evaluate(){
	return new Float(sinh(_x));
}

Value *Cosh::evaluate(){
	return new Float(cosh(_x));
}

Value *Tanh::evaluate(){
	return new Float(tanh(_x));
}

// ─────────────────────────────────
// 指数・対数

Value *Log::evaluate(){
	return new Float(log(_x));
}

Value *Log10::evaluate(){
	return new Float(log10(_x));
}

Value *Exp::evaluate(){
	return new Float(exp(_x));
}

// ─────────────────────────────────
// その他

Value *Sqrt::evaluate(){
	return new Float(sqrt(_x));
}

Value *Cbrt::evaluate(){
	return new Float(cbrt(_x));
}

Value *Pow::evaluate(){
	return new Float(pow(_x, _y));
}

Value *Floor::evaluate() {
	return new Float(floor(_x));
}

Value *Ceil::evaluate() {
	return new Float(ceil(_x));
}

Value *Round::evaluate() {
	return new Float(round(_x));
}

Value *Fabs::evaluate() {
	return new Float(fabs(_x));
}

Value *Abs::evaluate() {
	return new Integer(abs(_x));
}

Value *Hypot::evaluate() {
	return new Float(hypot(_y, _x));
}

void AbstructFunction::print(){
	printf("%s(", _name);
#ifdef DEBUG
//	if (_value)
//		_value->print();
#endif
	printf(")");
}

void AbstructFunction::printTree(int depth){
	printSpace(depth);
	print();
	printf("\n");
}

/*  sum  */
Value *Sum::evaluate(){
	Value *v;
	if (_list->size() == 0)
		return new Float(0);
	v = _list->sum();
	return v;
}

/*  max  */
Value *Max::evaluate(){
	Value *v;
	if (_list->size() == 0)
		return new Float(0);
	v = _list->max();
	return v;
}

/*  min  */
Value *Min::evaluate(){
	Value *v;
	if (_list->size() == 0)
		return new Float(0);
	v = _list->min();
	return v;
}

/*  random  */
Value *Random::evaluate(){
	return new Float(rand()/(double)RAND_MAX);
}

/*  srand  */
Value *SRand::evaluate(){
	srand(_x);
	return NULL;
} 

/*  print  */
Value *Printx::evaluate(){
	if (_list->size() == 0)
		return NULL;
	_list->expression();
	fflush(stdout);
	return NULL;
}

/*  println  */
Value *Println::evaluate(){
	if(_list->size() == 0) {
		printf("\n");
		return NULL;
	}
	_list->expression();
	printf("\n");
	return NULL;
}

/*  printf  */
Value *PrintF::evaluate(){
	FormatTokenizer ft;
	int n = _list->size();
	if (n == 0)
		return NULL;
	char *format = _list->getElement(0)->getText();
	int i = 0, t;
	char buff[64];

	ft.setFormat(format);
	while ((t = ft.nextToken(buff)) != FTYPE_EOF){
		if (t == FTYPE_TEXT){
			printf(buff);
		} else {
			if (i >= n)
				return NULL;
			if (t == FTYPE_INT){
				printf(buff, _list->getElement(++i)->getInt());
			} else if (t == FTYPE_FLOAT){
				printf(buff, _list->getElement(++i)->getFloat());
			} else if (t == FTYPE_STRING){
				printf(buff, _list->getElement(++i)->getText());
			}
		}
	}
	return NULL;
}

/*   型変換(int)  */
Value *Int::evaluate(){
	Value *v;
	if(_list->size() == 0){
		printf("Error : 'int' : few parameters to call. (line = %d)\n",_line_number);
		throw ErrorException();
	}
	if(_list->size() >= 2){
		printf("Warning : 'int' : too many actual parameters. (line = %d)\n",_line_number);
//		return new Integer(0);
	}
	v = _list->chint();
	return v;
}

/*  型変換(float)  */
Value *chFloat::evaluate(){
	Value *v;
	if(_list->size() == 0){
		printf("Error : 'float' : few parameters to call. (line = %d)\n",_line_number);
		throw ErrorException();
	}
	if(_list->size() >= 2){
		printf("Warning : 'float' : too many actual parameters. (line = %d)\n",_line_number);
//		return new Float(0);
	}
	v = _list->chfloat();
	return v;
}

/*  型変換(string)  */
Value *chString::evaluate(){
	Value *v;
	if(_list->size() == 0){
		printf("Error : 'string' : few parameters to call. (line = %d)\n",_line_number);
		throw ErrorException();
	}
	if(_list->size() >= 2){
		printf("Warning : 'string' : too many actual parameters. (line = %d)\n",_line_number);
//		return NULL;
	}
	v = _list->chstring();
	return v;
}

/*	文字列関数	*/
Value *Len::evaluate(){
	Value *v;
	if(_list ->size() == 0){
		printf("Error : 'len' : few parameters to call. (line = %d)\n",_line_number);
		throw ErrorException();
	}
	if(_list->size() >= 2){
		printf("Warning : 'len' : too many actual parameters. (line = %d)\n",_line_number);
//		return NULL;
	}
	v = _list->len();
	return v;
}

/*	型変換(bool)  */
Value *chBool::evaluate(){
	Value *v;
	if(_list->size() == 0) {
		printf("Error : 'bool' : few parameters to call. (line = %d)\n",_line_number);
		throw ErrorException();
	}
	if(_list->size() >= 2){
		printf("Warning : 'bool' : too many actual parameters. (line = %d)\n",_line_number);
//		return new False();
	}
	v = _list->chbool();
	return v;
}

/*  type関数  */
Value *Type::evaluate() {
	if(_list ->size() == 0){
		printf("Error : 'type' : few parameters to call. (line = %d)\n",_line_number);
		throw ErrorException();
	}
	if(_list ->size() >= 2){
		printf("Warning : 'type' :too many actual parameters. (line = %d)\n",_line_number);
//		return NULL;
	}
//	Value *value = _list->type();
	Value *value = new Text(_list->getElement(0)->getType());	// aikawa
	return value;
}

/*　Input  */
#ifdef T_WINDOWS
Value *Input::evaluate(){
	Value * v;
	char str[IN_BUFF_SIZE];
	if(_list->size() >= 1)
		printf("Warning : 'input' : An parameters exist. (line = %d)\n",_line_number);
	cin.getline(str,IN_BUFF_SIZE);
	v = new Text(str);
	return v;
}
#endif

#ifdef T_ESP32
Value *Input::evaluate(){
	char buff[IN_BUFF_SIZE];
	_command.readLine(buff, IN_BUFF_SIZE, false);
	return new Text(buff);
}
#endif

/*  時間関数  */
Value *Time::evaluate() {
	Value *v;
	v = _list->Time();
	return v;
}

/*  delay  */
Value *Delay::evaluate() {
	Value *v;
	if(_list->size() == 0){
		printf("Error : 'delay' : few parameters to call. (line = %d)\n",_line_number);
		throw ErrorException();
	}
	if(_list->size() >= 2){
		printf("Warning : 'delay' : too many actual parameters. (line = %d)\n",_line_number);
//		return NULL;
	}
	v = _list->Delay();
	return v;	
}

/* millis */
Value *Millis::evaluate() {
	Value *v;
	if(_list->size() != 0){
		printf("Error : 'millis' : An parameters exist. (line = %d)\n",_line_number);
		throw ErrorException();
	}
#ifdef T_ESP32
	return new Integer(millis());
#endif
#ifdef T_WINDOWS
	return new Integer(clock());
#endif
}

/* micros  */
Value *Micros::evaluate() {
	Value *v;
	if(_list->size() != 0){
		printf("Error : 'micros' : An parameters exist. (line = %d)\n",_line_number);
		throw ErrorException();
	}
	v = _list->Micros();
	return v;
}

/*  isalpha  */
Value *Isalpha::evaluate() {
	int v;
	if(_list->size() == 0){
		printf("Error : 'isalpha' :  few parameters to call. (line = %d)\n",_line_number);
		throw ErrorException();
	}
	if(_list->size() >= 2){
		printf("Warning : 'isalpha' : too many actual parameters. (line = %d)\n",_line_number);
//		return NULL;
	}
	
	v = isalpha(_list->getElement(0)->getInt());
	return new Integer(v);
}

/* readFile */
long getFileSize(const char *name){
	struct stat s;
	if (stat(name, &s) == 0)
		return s.st_size;
	return -1L;
}

Value *ReadFile::evaluate(){
	if(_list->size() == 0){
		printf("Error : 'readFile' : few parameters to call. (line = %d)\n",_line_number);
		throw ErrorException();
	}
	char *name = _list->getElement(0)->getText();
	long fsize = getFileSize(name);
	char *str, *next;
	str = (char *)malloc(fsize + 1);
	*str = '\0';
	next = str;
	FILE *fp = fopen(name, "r");
	if (fp == NULL){
		printf("File Open Error (%s)\n", name);
		throw ErrorException();
	}
	char buff[128];
	while ( fgets(buff, 128, fp) != NULL){
		strcpy(next, buff);
		next += strlen(buff);
	}
	fclose(fp);

//	printf("name = %s\n", name);
//	printf("file size = %ld\n", fsize);
	Value *v = new Text(str);
	delete str;
	return v;
}

Value *WriteFile::evaluate(){
	if(_list->size() <= 1){
		printf("Error : 'writeFile' : few parameters to call. (line = %d)\n",_line_number);
		throw ErrorException();
	}
	char *name = _list->getElement(0)->getText();
	char *text = _list->getElement(1)->getText();
	FILE *fp = fopen(name, "w");
	if (fp == NULL){
		printf("File Open Error (%s)\n", name);
		throw ErrorException();
	}
	fputs(text, fp);
	fclose(fp);
	return new Integer(0);
}

Value *AppendFile::evaluate(){
	if(_list->size() <= 1){
		printf("Error : 'appendFile' : few parameters to call. (line = %d)\n",_line_number);
		throw ErrorException();
	}
	char *name = _list->getElement(0)->getText();
	char *text = _list->getElement(1)->getText();
	FILE *fp = fopen(name, "a");
	if (fp == NULL){
		printf("File Open Error (%s)\n", name);
		throw ErrorException();
	}
	fputs(text, fp);
	fclose(fp);
	return new Integer(0);
}

// ─────────────────────────────────
// ユーザ定義関数関連

FunctionMap	_funcMap;	// ユーザ定義関数表

void FunctionMap::putFunction(char *name, Function *function){
	FunctionNode *node = searchFunction(name);
	if (node){
		node->setFunction(function);
	} else {
		addFunction(name, function);
	}
}

void FunctionMap::addFunction(char *name, Function *function){
	FunctionNode *node = new FunctionNode(name, function);
	if (_top){
		_last->setNext(node);
		_last = node;
	} else {
		_top = _last = node;
	}
}

Function *FunctionMap::getFunction(char *name){
	FunctionNode *node = _top;
	for( ; node ; node = node->getNext()){
		if (strcmp(name, node->getName()) == 0)
			return node->getFunction();
	}
	return NULL;
}

void FunctionMap::removeFunction(char *name){
	FunctionNode *node = _top, *pre = NULL;
	for( ; node ; node = node->getNext()){
		if (strcmp(name, node->getName()) == 0){
			if (pre == NULL){
				_top = node->getNext();
			} else {
				pre->setNext(node->getNext());
			}
			delete node;
			return;
		}
		pre = node;
	}
	printf("Error : not found Function : %s (line = %d)\n",name,_line_number);
	throw ErrorException();
}

void FunctionMap::clearFunctions(){
	FunctionNode *node, *next;
	for(node = _top ; node ; node = next){
		next = node->getNext();
		delete node;
	}
	_top = _last = NULL;
}

void FunctionMap::printFunctions(){
	printf("[User Functions]\n");
	FunctionNode *node;
	for(node = _top ; node ; node = node->getNext()){
		node->getFunction()->print();
		printf("\n");
	}
}

FunctionNode *FunctionMap::searchFunction(char *name){
	FunctionNode *node;
	for(node = _top ; node ; node = node->getNext()){
		if (strcmp(node->getName(), name) == 0){
			return node;
		}
	}
	return NULL;
}


Param::Param(char *name){
	_name = new char[strlen(name)+1];
	strcpy(_name, name);
	_next = NULL;
}

Param::~Param(){
#ifdef DEBUG
	printf("delete Param = %s\n", _name);
#endif
	delete _name;
}

UserFunction::~UserFunction(){
#ifdef DEBUG
	if (_name)
		printf("delete %s()\n", _name);
	else
		printf("delete function()\n");
#endif
	delete _name;
	deleteParams();
	delete _sentence;
}

void UserFunction::setName(char *name){
	_name = new char[strlen(name)+1];
	strcpy(_name, name);
}

void UserFunction::addParam(char *name){
	if (_paramTop){
		_paramLast->setNext(new Param(name));
		_paramLast = _paramLast->getNext();
	} else {
		_paramTop = _paramLast = new Param(name);
	}
}

void UserFunction::setParamValue(int pos, Value *value){
	Param *param = _paramTop;
	int i;
	for(i = 0; param && i < pos; i++){
		param = param->getNext();
	}
	if (param){
		_varMap.put(param->getName(), value);
	} else {
		printf("Warning : More than the specified parameters.\n");
		printf("          Check your parameters. (line = %d)\n",_line_number);
	}
}

void UserFunction::deleteParams(){
	Param *param, *next;
	for(param = _paramTop; param ; param = next){
		next = param->getNext();
		delete param;
	}
	_paramTop = _paramLast = NULL;
}

int UserFunction::getParamCount(){
	int n = 0;
	Param *param;
	for(param = _paramTop; param ; param = param->getNext()){
		n++;
	}
	return n;
}

void UserFunction::print(){
	Param *param;
	int n = 0;
	printf("%s(", _name);
	for(param = _paramTop; param; param = param->getNext()){
		if (n > 0)
			printf(",");
		printf("%s", param->getName());
		n++;
	}
	printf(")");
}

void UserFunction::printTree(int depth){
	printSpace(depth);
	print();
	printf("\n");
}

Value *UserFunction::evaluate(){
	Value *value = NULL;
	bool ch = false;
	if(print_Mode == false) {
		ch = true;
		print_Mode = true;
	}
	if(_sentence){
		try{
			_sentence->execute();
		}catch(ReturnException e){
			value = e._value;
		}
	}
	if(ch)
		print_Mode = false;
//	if(value == NULL)
//		value = new Integer(0);
	return value;
}

// ─────────────────────────────────
FuncName::FuncName(const char *name) {
	_name = (char *)(new char[strlen(name)+1]);
	strcpy(_name,name);
	_top = _last = NULL;
}

FuncName::~FuncName() {
#ifdef DEBUG
	printf("delete FuncName(%s)\n", _name);
#endif
	delete []_name;
	ExprNode *node, *next;
	for(node = _top; node; node = next) {
		next = node->_next;
		delete node;
	}
}

void FuncName::addParam(Expression *e) {
	if(_top == NULL) {
		_top = _last = new ExprNode(e);
	} else {
		_last->_next = new ExprNode(e);
		_last = _last->_next;
	}
}

bool FuncName::isPrint() {
	Function *f;
	f = searchSysFunc(_name);
	if (f == NULL){
		return false;
	}
	return f->isPrint();
}

void FuncName::print() {
	printf("%s(", _name);
	ExprNode *node;
	for(node = _top; node; node = node->_next) {
		if(node != _top)
			printf(",");
		node->_express->print();
	}
	printf(")");
}

extern int box[];

void FuncName::printTree(int s) {
	printSpace(s);
	printf("%s()\n", _name);
	box[s] = 0;
	ExprNode *node;
	for(node = _top; node; node = node->_next) {
		node->_express->printTree(s+1);
	}
}

Value *FuncName::evaluate() {
	Function *f;
	f = _funcMap.getFunction(_name);
	if (f == NULL){
		f = searchSysFunc(_name);
	}
	if (f == NULL){
		printf("RuntimeError : Undefined Function : %s (line = %d)\n", _name,_line_number);
		throw ErrorException();
	}
	ExprNode *node;
	ListNode *top = NULL, *last = NULL, *e, *next;
	int paramCnt = f->getParamCount();
	int n = 0;
	for(node = _top; node ; node = node->_next){
		n++;
	}
	if (strcmp(_name, "printf") != 0){
		if (paramCnt != n){
			printf("RuntimeError: Invalid parameter count (line = %d)\n", _line_number);
			throw ErrorException();
		}
	}

	for(node = _top; node ; node = node->_next){
		Value *param = node->_express->evaluate();
		if (param){
			e = new ListNode(param, NULL);
			if (top){
				last->_next = e;
				last = last->_next;
			} else {
				top = last = e;
			}
		} else {
			for(e = top; e ; e = next){
				next = e->_next;
				delete e;
			}
			printf("RuntimeError: It must be bug. (line = %d)\n", _line_number);
			throw ErrorException();
		}
	}
	int i;
	_varMap.stackUp();
	e = top;
	for(i = 0; i < n ; i++){
		f->setParamValue(i, e->_value);
		e = e->_next;
	}
	Value *v = f->evaluate();
	f->clearParams();
	for(e = top; e ; e = next){
		next = e->_next;
		e->_value = NULL;
		delete e;
	}
	_varMap.stackDown();
	if(f->isPrint()) {
		delete v;
		return NULL;
	}
	return v;
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
