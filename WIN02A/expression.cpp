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
 * expression.cpp
 */

#define _USE_MATH_DEFINES

#include<stdio.h>
#include<stdlib.h>
#include<String.h>
#include<math.h>
#include<iostream>
#include<ctype.h>
#include<time.h>
#include<chrono>
#include<thread>
#include<float.h>
#include "common.h"
#ifdef T_ESP32
#include<Arduino.h>
#endif
#include "expression.h"
#include "resource.h"
#include "varmap.h"
#include "logic.h"
#include "sentence.h"
#include "function.h"

using namespace std;

extern VarMapStack _varMap;
extern int _line_number;
extern WiFiRes	_wifi;

Constant *_constants[] = {
	new Constant("pi"    ,new Float(M_PI)),	new Constant("E"	,new Float(exp(1))),
	new Constant("true"  ,new True()),	new Constant("TRUE"	,new True()),
	new Constant("H"     ,new True()),	new Constant("HIGH"	,new True()),		new Constant("High",new True()),
	new Constant("false" ,new False()),	new Constant("FALSE"	,new False()),
	new Constant("L"     ,new False()),	new Constant("LOW"	,new False()),		new Constant("Low", new False()),
	NULL
};

#ifdef T_ESP32
Constant *ESP_constants[] = {
	new Constant("OUTPUT"	 ,new Integer(0)),	new Constant("INPUT"	 ,new Integer(1)),	new Constant("INPUT_PULLUP"	,new Integer(2)),
	new Constant("BLACK"	 ,new Integer(0x0000)), new Constant("NAVY"	 ,new Integer(0x000F)),	new Constant("DARKGREEN"	,new Integer(0x03E0)),
	new Constant("DARKCYAN"	 ,new Integer(0x03EF)),	new Constant("MAROON"	 ,new Integer(0x7800)),	new Constant("PURPLE"		,new Integer(0x780F)),
	new Constant("OLIVE"	 ,new Integer(0x7BE0)),	new Constant("LIGHTGREY" ,new Integer(0xD69A)),	new Constant("DARKGREY"		,new Integer(0x7BEF)),
	new Constant("BLUE"	 ,new Integer(0x001F)),	new Constant("GREEN"	 ,new Integer(0x07E0)),	new Constant("CYAN"		,new Integer(0x07FF)),
	new Constant("RED"	 ,new Integer(0xF800)),	new Constant("MAGENTA"	 ,new Integer(0xF81F)),	new Constant("YELLOW"		,new Integer(0xFFE0)),
	new Constant("WHITE"	 ,new Integer(0xFFFF)),	new Constant("ORANGE"	 ,new Integer(0xFDA0)),	new Constant("GREENYELLOW"	,new Integer(0xB7E0)),
	new Constant("PINK"	 ,new Integer(0xFE19)),	new Constant("BROWN"	 ,new Integer(0x9A60)),	new Constant("GOLD"		,new Integer(0xFEA0)),
	new Constant("SILVER"	 ,new Integer(0xC618)),	new Constant("SKYBLUE"	 ,new Integer(0x867D)),	new Constant("VIOLET"		,new Integer(0x915C)),
	new Constant("LIGHTPINK" ,new Integer(0xFC9F)),
	NULL
};
#endif

void clearConstants() {
	for(int i = 0;_constants[i]; i++) {
		delete _constants[i];
	}
#ifdef T_ESP32
	for(int i = 0;ESP_constants[i];i++) {
		delete ESP_constants[i];
	}
#endif
}

void printConstants() {
	printf("[Constants]\n");
	for(int i = 0;_constants[i];i++) {
		int count = strlen(_constants[i]->getName());
		printf("%s ",_constants[i]->getName());
		while(count < 12) {
			printf(" ");
			count++;
		}
		printf(": ");
		_constants[i]->print();
		printf("\n");
	}
}

#ifdef T_ESP32
void printESPCon() {
	printf("[Constants_ESP]\n");
	for(int i = 0;ESP_constants[i];i++) {
		int count = strlen(ESP_constants[i]->getName());
		printf("%s ",ESP_constants[i]->getName());
		while(count < 12) {
			printf(" ");
			count++;
		}
		printf(": 0x%04X\n",ESP_constants[i]->getInt());
	}
}
#endif

//------------------------------------------------------

bool Expression::eval(){
	Value *v = evaluate();
	if (v == NULL)
		return false;
	bool b = v->getBool();
	delete v;
	return b;
}

Value *Value::methodCall(char *name, List *params){
	printf("Error : Not implemented. (line = %d)\n",_line_number);
	throw ErrorException();
}
//------------------------------------------------------
//Text

Text::Text(char *value) {
	_value = (char *)(new char[strlen(value) + 1]);
	strcpy(_value, value);
}

Text::Text(long value) {
	char s[16];
	sprintf(s, "%d", value);
	_value = new char[strlen(s) +1];
	strcpy(_value, s);
}

Text::~Text() {
#ifdef DEBUG
	printf("delete Text(\"%s\")\n",_value);
#endif
	if(_value)
		delete [] _value;
}

long Text::getInt() {
	return atoi(_value);
}

double Text::getFloat() {
	return atof(_value);
}

//------------------------------------------------------
// getText()

char *Constant::getText() {
	return _value->getText();
}


char *Integer::getText() {
	static char s1[16];
	sprintf(s1, "%d", _value);
	return s1;
}

char*Float::getText() {
	static char s1[16];
	sprintf(s1, "%g", _value);
	return s1;
}

char *Text::getText() {
	return _value;
}

//------------------------------------------------------
// List

void List::clear(){
	ListNode *node, *next;
	for(node = _top; node; node = next) {
		next = node->_next;
		delete node;
	}
	_top = _last = new ListNode(NULL,NULL);
}

long List::getInt() {
	ListNode *node;
	long value = 0;
	for(node = _top; node != _last; node = node->_next) {
		value += node->_value->getInt();
	}
	return value;
}

double List::getFloat() {
	ListNode *node;
	double value = 0;
	for(node = _top; node != _last; node = node->_next) {
		value += node->_value->getFloat();
	}
	return value;
}

char *List::getText() {
	return "List";
}

void List::mod(Value *val) {
	printf("Unimplemented\n");
	throw ErrorException();
}

void List::power(Value *val) {
	printf("Unimplemented\n");
	throw ErrorException();
}

void List::increment(bool ch) {
	printf("Unimplemented\n");
	throw ErrorException();
}

void List::decrement(bool ch) {
	printf("Unimplemented\n");
	throw ErrorException();
}

void List::minus() {
	printf("Unimplemented\n");
	throw ErrorException();
}

int List::size() {
	ListNode *node;
	int n = 0;
	for(node = _top; node != _last; node = node->_next)
		n++;
	return n;
}

Value *List::sum() {
	ListNode *node;
	double sum = 0;
	for(node = _top; node != _last ; node = node->_next){
		sum += node->_value->getFloat();
	}
	return new Float(sum);
}

Value *List::max() {
	ListNode *node;
	double max = DBL_MIN, value;
	for(node = _top; node != _last ; node = node->_next){
		if(node->_value->isList()) {
			List *list = (List *)node->_value;
			value = list->max()->getFloat();
		} else
			value = node->_value->getFloat();
		if (max < value)
			max = value;
	}
	return new Float(max);
}

Value *List::min() {
	ListNode *node;
	double min = DBL_MAX, value;
	for(node = _top; node != _last ; node = node->_next){
		if(node->_value->isList()) {
			List *list = (List *)node->_value;
			value = list->min()->getFloat();
		} else
			value = node->_value->getFloat();
		if (min > value)
			min = value;
	}
	return new Float(min);
}

void List::expression() {
	ListNode *node;
	for(node = _top; node != _last ; node = node->_next) {
		if(node->_value != NULL) {
			if(node->_value->isList()) {
				node->_value->print();
			} else {
				char *text = node ->_value -> getText();
				if(text != NULL)
					printf("%s",text);
			}
		}
	}
}

Value *List::chint() {
	ListNode *node;
	long value;
	for(node = _top; node !=_last ; node = node->_next){
		value = node->_value->getInt();
	}
	return new Integer(value);
}

Value *List::chfloat() {
	ListNode *node;
	double value;
	for(node = _top; node != _last ; node = node->_next){
		value = node->_value->getFloat();
	}
	return new Float(value);
}

Value *List::chstring() {
	ListNode *node;
	char *value;
	for(node = _top; node != _last; node = node -> _next){
		value = node->_value->getText();
	}
	return new Text(value);
}

Value *List::chbool() {
	ListNode *node;
	bool value;
	for(node = _top; node != _last; node = node ->_next){
		value = node->_value->getBool();
	}
	return new Bool(value);
}

Value *List::len() {
	ListNode *node = _top;
	char *value;

	if(node ->_value->isList()){
//		printf("エラー (内部処理の不具合のためList未対応)\n"); //Listの中の計算をしてから処理されるため1が出てくる。
		List *list = (List *)node->_value;
		return new Integer(list->size());
	}
	for(node ; node != _last; node = node ->_next)
		value = node->_value ->getText();
	return new Integer(strlen(value));
}

Value *List::type() {
	char *value = new char[strlen(_top->_value->getType())+1];
	strcpy(value,_top->_value->getType());
	return new Text(value);
}

Value *List::Time(){
	ListNode *node = _top;
	if(node ->_value == NULL)
		return new Integer(time(NULL));
	return new Integer(time((time_t*)node->_value ->getText()));
}

Value *List::Delay(){
#ifdef T_WINDOWS
	ListNode *node = _top;
	std::this_thread::sleep_for(std::chrono::milliseconds(node->_value->getInt()));
#endif
#ifdef T_ESP32
	ListNode *node = _top;
	delay(node->_value->getInt());
#endif
	return new Integer(0);
}

Value *List::Micros(){
#ifdef T_ESP32
	return new Integer(micros());
#endif
#ifdef T_WINDOWS
	printf("not in Windows version. (line = %d)\n",_line_number);
	return 0;
#endif
}


/*******************************************************************************/
void List::append(Value *value) {
	ListNode *node = new ListNode(NULL, NULL);
	_last->setValue(value, node);
	_last = node;
}

void List::insert(int index, Value *value) {
	int pos;
	ListNode *node;

	if(index == 0) {
		_top = new ListNode(value, _top);
		return;
	}
	pos = 1;
	for(node = _top; node != _last; node = node->_next) {
		if(pos == index) {
			node->_next = new ListNode(value, node->_next);
			return;
		}
		pos++;
	}
	printf("IndexError : Index out of range(index = %d, line = %d)\n", index,_line_number);
	throw ErrorException();
}

void List::remove(Value *value) {
	int i = 0;
	ListNode *node, *next, *pre = NULL;
	for(node = _top; node != _last; node = node->_next) {
		if(value->compEQ(node->_value)) {
			next = node->_next;
			if(pre) {
				delete node;
				pre->_next = next;
			} else {
				delete _top;
				_top = next;
			}
			return;
		}
		pre = node;
	}
	printf("IndexError : Not Found %s(%s) (line = %d)\n",value->getType(),value->getText(),_line_number);
	throw ErrorException();
}

Value *List::getElement(int index) {
	int i = 0;
	ListNode *node;
	for(node = _top; node != _last; node = node->_next) {
		if(i == index)
			return node->_value;
		i++;
	}
	printf("IndexError : Index out of range(index = %d, line = %d)\n", index,_line_number);
	throw ErrorException();
}

void List::setElement(int index, Value *value) {
	int i = 0;
	ListNode *node;
	for(node = _top; node != _last; node = node->_next) {
		if(i == index) {
			delete node->_value;
			node->_value = value;
			return;
		}
		i++;
	}
	if(i == index) {
		append(value);
		return;
	}
	printf("IndexError : Index out of range(index = %d, line = %d)\n", index,_line_number);
	throw ErrorException();
}

// VarName--------------------------------------------------

Value *VarName::getValue() {
	Variable *var = _varMap.getVariable(_name);
	if(var == NULL) {
		printf("RuntimeError : Undefined variable(name = %s, line = %d)\n",_name,_line_number);
		throw ErrorException();
	}
	return var->getValue();
}

void VarName::setValue(Value *value) {
	Variable *var = _varMap.getVariable(_name);

	if(var == NULL) {
		printf("RuntimeError : Undefined variable(name = %s, line = %d)\n",_name,_line_number);
		throw ErrorException();
	}
	var->setValue(value);
}

// Index--------------------------------------------------

Value *Index::getValue() {
	Value *val = _vobj->getValue();
	if(val == NULL) {
		return NULL;
	}
	if(!val->isList()) {
		printf("Error : Unexpected Variable(name = %s, line = %d)\n",val->getText(),_line_number);
		throw ErrorException();
	}
	Value *v = _e->evaluate();
	int index = v->getInt();
	delete v;
	List *list = (List *)val;
	return list->getElement(index);
}

void Index::setValue(Value *value) {
	Value *val = _vobj->getValue();
	if(val == NULL) {
		return;
	}
	if(!val->isList()) {
		printf("Error : Unexpected Variable(name = %s, line = %d)\n",val->getText(),_line_number);
		throw ErrorException();
	}
	Value *v = _e->evaluate();
	int index = v->getInt();
	delete v;
	List *list = (List *)val;
	list->setElement(index, value);
}

//--------------------------------------------------------

Variable::Variable(const char *name){
	_name = (char *)(new char[strlen(name) + 1]);
	strcpy(_name, name);
	_value = NULL;
}

void Variable::setData(int value) {
	if(_value != NULL)
		delete _value;
	_value = new Integer(value);
}

void Variable::setData(double value) {
	if(_value != NULL)
		delete _value;
	_value = new Float(value);
}

void Variable::setData(char *value) {
	if(_value != NULL)
		delete _value;
	_value = new Text(value);
}

void Variable::setValue(Value *value) {
	if(_value != NULL)
		delete _value;
	_value = value;
}

Value *Variable::getValue() {
	return _value;
}

// ExprList ExprNode ----------------------------------------

void ExprList::append(Expression *e) {
	if(isEmpty()) {
		_top = _last = new ExprNode(e);
	} else {
		_last->_next = new ExprNode(e);
		_last = _last->_next;
	}
}

Expression *ExprList::getElement(int index) {
	int i = 0;
	ExprNode *node;
	for(node = _top; node; node = node->_next) {
		if(i == index)
			return node->_express;
		i++;
	}
	return NULL;
}

void ExprList::clear(){
	ExprNode *node, *next;
	for(node = _top; node; node = next) {
		next = node->_next;
		delete node;
	}
	_top = _last = _curr = NULL;
}

int ExprList::size(){
	int n = 0;
	ExprNode *node;
	for(node = _top; node ; node = node->_next)
		n++;
	return n;
}

Expression *ExprList::top(){
	_curr = _top;
	if (_curr == NULL)
		return NULL;
	return _curr->_express;
}

Expression *ExprList::next(){
	if (_curr == NULL)
		return NULL;
	_curr = _curr->_next;
	if (_curr == NULL)
		return NULL;
	return _curr->_express;
}

// add------------------------------------------------------

void Integer::add(Value *val) {
	_value += val->getInt();
}

void Float::add(Value *val) {
	_value += val->getFloat();
}

void Text::add(Value *val) {
	char *s1 = _value;
	char *s2 = val->getText();
	_value = new char[strlen(s1) + strlen(s2) +1];
	strcpy(_value, s1);
	strcat(_value, s2);
#ifdef DEBUG
	printf("delete Text \"%s\"\n",s1);
#endif
	delete s1;
}

void List::add(Value *val) {
	ListNode *node;
	int index = 0;
	if(val->isList()) {
		List *list = (List *)val;
		for(node = _top; node != _last; node = node->_next) {
			Value *v = list->getElement(index++);
			if(v) {
				node->_value->add(v);
			}
		}
	} else {
		for(node = _top; node != _last; node = node->_next) {
			node->_value->add(val);
		}
	}
}

// sub------------------------------------------------------

void Integer::sub(Value *val) {
	_value -= val->getInt();
}

void Float::sub(Value *val) {
	_value -= val->getFloat();
}

void Text::sub(Value *val) {
	printf("SyntaxError : Unexpected formula (line = %d)\n",_line_number);
	throw ErrorException();
}

void List::sub(Value *val) {
	ListNode *node;
	int index = 0;
	if(val->isList()) {
		List *list = (List *)val;
		for(node = _top; node != _last; node = node->_next) {
			Value *v = list->getElement(index++);
			if(v)
				node->_value->sub(val);
		}
	} else {
		for(node = _top; node != _last; node = node->_next)
			node->_value->sub(val);
	}
}

// mul------------------------------------------------------
void Integer::mul(Value *val) {
	_value *= val->getInt();
}

void Float::mul(Value *val) {
	_value *= val->getFloat();
}

void Text::mul(Value *val) {
	char *s = _value;
	int n = val->getInt();
	if(n == 0)
		return;
	_value = new char[n * strlen(s) +1];
	_value[0] = '\0';
	for(int i = 0; i < n; i++)
		strcat(_value, s);

#ifdef DEBUG
	printf("delete Text = %s\n", s);
#endif
	delete s;
}

void List::mul(Value *val) {
	ListNode *node;
	int index = 0;
	if(val->isList()) {
		List *list = (List *)val;
		for(node = _top; node != _last; node = node->_next) {
			Value *v = list->getElement(index++);
			if(v)
				node->_value->mul(v);
		}
	} else {
		for(node = _top; node != _last; node = node->_next)
			node->_value->mul(val);
	}
}

// div------------------------------------------------------
void Integer::div(Value *val) {
	int x = val->getInt();
	if (x == 0){
		printf("RuntimeError : Divied by Zero (line = %d)\n",_line_number);
		throw ErrorException();
	} else {
		_value /= x;
	}
}

void Float::div(Value *val) {
	_value /= val->getFloat();
}

void Text::div(Value *val) {
	printf("SyntaxError : Unexpected formula (line = %d)\n",_line_number);
	throw ErrorException();
}

void List::div(Value *val) {
	ListNode *node;
	int index = 0;
	if(val->isList()) {
		List *list = (List *)val;
		for(node = _top; node != _last; node = node->_next) {
			Value *v = list->getElement(index++);
			if(v)
				node->_value->div(val);
		}
	} else {
		for(node = _top; node != _last; node = node->_next)
			node->_value->div(val);
	}
}

// mod------------------------------------------------------
void Integer::mod(Value *val) {
	int x = val->getInt();
	if (x == 0){
		printf("RuntimeError : Divied by Zero (line = %d)\n",_line_number);
		throw ErrorException();
	} else {
		_value %= x;
	}
}

void Float::mod(Value *val) {
	_value = fmod(_value, val->getFloat());
}

void Text::mod(Value *val) {
	printf("SyntaxError : Unexpected formula (line = %d)\n",_line_number);
	throw ErrorException();
}

// pow ------------------------------------------------------
void Integer::power(Value *val) {
	_value = (long)pow(_value, val->getInt());
}

void Float::power(Value *val) {
	_value = pow(_value, val->getFloat());
}

void Text::power(Value *val) {
	printf("SyntaxError : Unexpected formula (line = %d)\n",_line_number);
	throw ErrorException();
}

// increment------------------------------------------------------
void Integer::increment(bool ch) {
	if(ch)
		++_value;
	else
		_value++;
}

void Float::increment(bool ch) {
	if(ch)
		++_value;
	else
		_value++;
}

void Text::increment(bool ch) {
	printf("SyntaxError : Unexpected formula (line = %d)\n",_line_number);
	throw ErrorException();
}

// decrement------------------------------------------------------
void Integer::decrement(bool ch) {
	if(ch)
		--_value;
	else
		_value--;
}

void Float::decrement(bool ch) {
	if(ch)
		--_value;
	else
		_value--;
}

void Text::decrement(bool ch) {
	printf("SyntaxError : Unexpected formula (line = %d)\n",_line_number);
	throw ErrorException();
}

// minus------------------------------------------------------
void Integer::minus() {
	_value = -(_value);
}

void Float::minus() {
	_value = -(_value);
}

void Text::minus() {
	int high = strlen(_value) - 1;
	int low = 0;
	char c;
	while (low < high){
		c = *(_value + low);
		*(_value + low) = *(_value + high);
		*(_value + high) = c;
		low++;
		high--;
	}
}

//-----------------------------------------------------------
Value *Constant::evaluate() {
	return _value->evaluate();
}

Value *Integer::evaluate() {
	return new Integer(_value);
}

Value *Float::evaluate() {
	return new Float(_value);
}

Value *Text::evaluate() {
	return new Text(_value);
}

Value *List::evaluate() {
	List *list = new List();
	ListNode *node;
	for(node = _top; node != _last; node = node->_next) {
		if(node->_value)
			list->append(node->_value->evaluate());
		else
			return NULL;
	}
	return list;
}

Value *Variable::evaluate() {
	if(_value)
		return _value->evaluate();
	return new Integer(0);
}

Value *VarName::evaluate() {
	for(int i = 0; _constants[i]; i++) {
		if(strcmp(_constants[i]->getName(),_name)==0)
			return _constants[i]->evaluate();
	}
#ifdef T_ESP32
	for(int i = 0; ESP_constants[i]; i++) {
		if(strcmp(ESP_constants[i]->getName(),_name)==0)
			return ESP_constants[i]->evaluate();
	}
#endif
	Variable *var = _varMap.getVariable(_name);
	if(var == NULL) {
		printf("RuntimeError : Undefined variable(name = %s, line = %d)\n",_name,_line_number);
		throw ErrorException();
	}
	return var->evaluate();
}

Value *Index::evaluate() {
	Value *v = _vobj->evaluate();
	if(v == NULL)
		return NULL;
	if(!v->isList()) {
		printf("RuntimeError : Unexpected variable(name = %s, line = %d)\n",v->getText(),_line_number);
		throw ErrorException();
	}
	Value *val = _e->evaluate();
	int index = val->getInt();
	delete val;
	List *list = (List *)v;
	v = list->getElement(index);
	if(v == NULL) {
		return NULL;
	}
	val = v->evaluate();
	delete list;
	return val;
}

Value *Addition::evaluate() {
// 多分不要（様子見）
//	if(_e1 == NULL || _e2 == NULL) {
//		printf("Error : Invalid addition (line = %d)\n",_line_number);
//		throw ErrorException();
//	}
	Value *left = _e1->evaluate();
	Value *right = _e2->evaluate();
// 多分不要（様子見）
//	if(left == NULL || right == NULL) {
//		printf("RuntimeError : Invalid addtion (line = %d)\n",_line_number);
//		if(left)
//			delete left;
//		if(right)
//			delete right;
//		return NULL;
//	}
	left->add(right);
	delete right;
	return left;
}

Value *Subtraction::evaluate() {
// 多分不要（様子見）
//	if(_e1 == NULL || _e2 == NULL) {
//		printf("Error : Invalid expression (line = %d)\n",_line_number);
//		throw ErrorException();
//	}
	Value *left = _e1->evaluate();
	Value *right = _e2->evaluate();
// 多分不要（様子見）
//	if(left == NULL || right == NULL) {
//		if(left)
//			delete left;
//		if(right)
//			delete right;
//		return NULL;
//	}
	left->sub(right);
	delete right;
	return left;
}

Value *Multiplication::evaluate() {
// 多分不要（様子見）
//	if(_e1 == NULL || _e2 == NULL) {
//		printf("Error : Invalid expression (line = %d)\n",_line_number);
//		throw ErrorException();
//	}
	Value *left = _e1->evaluate();
	Value *right = _e2->evaluate();
//	if(left == NULL || right == NULL) {
//		if(left)
//			delete left;
//		if(right)
//			delete right;
//		return NULL;
//	}
	left->mul(right);
	delete right;
	return left;
}

Value *Division::evaluate() {
	Value *left = _e1->evaluate();
	Value *right = _e2->evaluate();
	left->div(right);
	delete right;
	return left;
}

Value *Remainder::evaluate() {
	Value *left = _e1->evaluate();
	Value *right = _e2->evaluate();
	left->mod(right);
	delete right;
	return left;
}

Value *Exponentiation::evaluate() {
	Value *left = _e1->evaluate();
	Value *right = _e2->evaluate();
	left->power(right);
	delete right;
	return left;
}

Value *Increment::evaluate() {
	Value *value = _vobj->getValue();
	if(_ch) { // ++前置
		value->increment(_ch);
		return value->evaluate();
	} else {  // 後置++
		Value *v = value->evaluate();
		value->increment(_ch);
		return v;
	}
}

Value *Decrement::evaluate() {
	Value *value = _vobj->getValue();
	if(_ch) {
		value->decrement(_ch);
		return value->evaluate();
	} else {
		Value *v = value->evaluate();
		value->decrement(_ch);
		return v;
	}
}

Value *Minus::evaluate() {
	Value *value = _e->evaluate();
	value->minus();
	return value;
}

Value *EqSubstitution::evaluate() {
	Value *value = _e->evaluate();
	_vobj->setValue(value);
	return value->evaluate();
}

Value *AddSubstitution::evaluate() {
	Value *v = _vobj->getValue();
	Value *value = _e->evaluate();
	v->add(value);
	delete value;
	return v->evaluate();
}

Value *SubSubstitution::evaluate() {
	Value *v = _vobj->getValue();
	Value *value = _e->evaluate();
	v->sub(value);
	delete value;
	return v->evaluate();
}

Value *MulSubstitution::evaluate() {
	Value *v = _vobj->getValue();
	Value *value = _e->evaluate();
	v->mul(value);
	delete value;
	return v->evaluate();
}

Value *DivSubstitution::evaluate() {
	Value *v = _vobj->getValue();
	Value *value = _e->evaluate();
	v->div(value);
	delete value;
	return v->evaluate();
}

Value *ModSubstitution::evaluate() {
	Value *v = _vobj->getValue();
	Value *value = _e->evaluate();
	v->mod(value);
	delete value;
	return v->evaluate();
}

Value *PowSubstitution::evaluate() {
	Value *v = _vobj->getValue();
	Value *value = _e->evaluate();
	v->power(value);
	delete value;
	return v->evaluate();
}

Value *AndSubstitution::evaluate() {
	Value *v = _vobj->getValue();
	int v_num = v->getInt();
	Value *value = _e->evaluate();
	int val_num = value->getInt();
	delete value;
	_vobj->setValue(new Integer(v_num & val_num));
	return _vobj->evaluate();
}

Value *OrSubstitution::evaluate() {
	Value *v = _vobj->getValue();
	int v_num = v->getInt();
	Value *value = _e->evaluate();
	int val_num = value->getInt();
	delete value;
	_vobj->setValue(new Integer(v_num | val_num));
	return _vobj->evaluate();
}

Value *ExorSubstitution::evaluate() {
	Value *v = _vobj->getValue();
	int v_num = v->getInt();
	Value *value = _e->evaluate();
	int val_num = value->getInt();
	delete value;
	_vobj->setValue(new Integer(v_num ^ val_num));
	return _vobj->evaluate();
}

Value *LShiftSubstitution::evaluate() {
	Value *v = _vobj->getValue();
	int v_num = v->getInt();
	Value *value = _e->evaluate();
	int val_num = value->getInt();
	delete value;
	_vobj->setValue(new Integer(v_num << val_num));
	return _vobj->evaluate();
}

Value *RShiftSubstitution::evaluate() {
	Value *v = _vobj->getValue();
	int v_num = v->getInt();
	Value *value = _e->evaluate();
	int val_num = value->getInt();
	delete value;
	_vobj->setValue(new Integer(v_num >> val_num));
	return _vobj->evaluate();
}


Value *ExprList::evaluate() {
	ExprNode *node;
	List *list = new List();
	for(node = _top; node; node = node->_next)
		list->append(node->_express->evaluate());
	return list;
}

Value *And::evaluate() {
	Value *left = _left->evaluate();
	Value *right = _right->evaluate();
	if(left == NULL || right == NULL) {
		if(left)
			delete left;
		if(right)
			delete right;
		return NULL;
	}
	int r_val = right->getInt();
	delete right;
	if(left->isList()) {
		List *list = new List();
		List *left_l = (List *)left;
		for(int i = 0;i < left_l->size(); i++) {
			int l_val = left_l->getElement(i)->getInt();
			list->append(new Integer(l_val & r_val));
		}
		delete left;
		return list;
	}
	int l_val = left->getInt();
	return new Integer(l_val & r_val);
}

Value *Or::evaluate() {
	Value *left = _left->evaluate();
	Value *right = _right->evaluate();
	if(left == NULL || right == NULL) {
		if(left)
			delete left;
		if(right)
			delete right;
		return NULL;
	}
	int r_val = right->getInt();
	delete right;
	if(left->isList()) {
		List *list = new List();
		List *left_l = (List *)left;
		for(int i = 0;i < left_l->size(); i++) {
			int l_val = left_l->getElement(i)->getInt();
			list->append(new Integer(l_val | r_val));
		}
		delete left;
		return list;
	}
	int l_val = left->getInt();
	return new Integer(l_val | r_val);
}

Value *Exor::evaluate() {
	Value *left = _left->evaluate();
	Value *right = _right->evaluate();
	if(left == NULL || right == NULL) {
		if(left)
			delete left;
		if(right)
			delete right;
		return NULL;
	}
	int r_val = right->getInt();
	delete right;
	if(left->isList()) {
		List *list = new List();
		List *left_l = (List *)left;
		for(int i = 0;i < left_l->size(); i++) {
			int l_val = left_l->getElement(i)->getInt();
			list->append(new Integer(l_val ^ r_val));
		}
		delete left;
		return list;
	}
	int l_val = left->getInt();
	return new Integer(l_val ^ r_val);
}

Value *Not::evaluate() {
	Value *value = _e->evaluate();
	if(value == NULL)
		return NULL;
	int v = value->getInt();
	return new Integer(~v);
}

Value *LShift::evaluate() {
	Value *value = _e1->evaluate();
	if(value == NULL)
		return NULL;
	Value *bit	 = _e2->evaluate();
	if(bit == NULL) {
		delete value;
		return NULL;
	}
	int v = value->getInt();
	int b = bit->getInt();
	delete value;
	delete bit;
	return new Integer(v << b);
}

Value *RShift::evaluate() {
	Value *value = _e1->evaluate();
	if(value == NULL)
		return NULL;
	Value *bit	 = _e2->evaluate();
	if(bit == NULL) {
		delete value;
		return NULL;
	}
	int v = value->getInt();
	int b = bit->getInt();
	delete value;
	delete bit;
	return new Integer(v >> b);
}

// print -----------------------------------------------
void Constant::print() {
	_value->print();
}

void Integer::print() {
	printf("%d",_value);
}

void Float::print() {
	printf("%g",_value);
}

void Text::print() {
	if(_value)
		printf("\"%s\"",_value);
	else
		printf("null(Text)");
}

void List::print() {
	ListNode *node;
	int pos = 0;
	printf("[ ");
	for(node = _top; node != _last; node = node->_next) {
		if(pos > 0)
			printf(", ");
		node->_value->print();
		pos++;
	}
	printf(" ]");
}

void Variable::print() {
	printf("%s",_name);
	printf(" = ");
	if(_value)
		_value->print();
	else
		printf("(null)");
}

void VarName::print() {
	printf("%s", _name);
}

void Index::print() {
	_vobj->print();
	printf("[");
	_e->print();
	printf("]");
}

void Operator::print() {
	printf("(");
	_e1->print();
	if(_opeChar == '^')
		printf(")**(");
	else
		printf(") %c(",_opeChar);
	_e2->print();
	printf(")");
}

void Increment::print() {
	if(_ch) {
		printf("++");
		_vobj->print();
	} else {
		_vobj->print();
		printf("++");
	}
}

void Decrement::print() {
	if(_ch) {
		printf("--");
		_vobj->print();
	} else {
		_vobj->print();
		printf("--");
	}
}

void Minus::print() {
	printf("-(");
	_e->print();
	printf(")");
}

void Substitution::print() {
	_vobj->print();
	printf("%s(",_opeChar);
	_e->print();
	printf(")");
}

void ExprList::print() {
	ExprNode *node;
	int pos = 0;
	printf("[");
	for(node = _top; node; node = node->_next) {
		if(pos > 0)
			printf(", ");
		node->_express->print();
		pos++;
	}
	printf("]");
}

void BitOperation::print() {
	_left->print();
	printf(" %s ",_bitChar);
	_right->print();
}

void Not::print() {
	printf("~( ");
	_e->print();
	printf(")");
}

void ShiftOperator::print() {
	_e1->print();
	printf(" %s ",_shiftch);
	_e2->print();
}

// printTree  --------------------------------------------
int box[50];

void printSpace(int s) {
	int i;

	for( i = 0; i <= s - 2; i++) {
		if(box[i] == box[i + 1])
			printf("    ");
		else
			printf(" |  ");
	}

	if(s != 0) {
		if(box[s - 1] == 1)
			printf(" |~~");
		else
			printf(" |__");
	}
}

void Constant::printTree(int s) {
	printSpace(s);
	_value->print();
	printf("(%s)\n",_name);
}

void Integer::printTree(int s) {
	printSpace(s);
	printf("%d(Integer)\n",_value);
}

void Float::printTree(int s) {
	printSpace(s);
	printf("%g(Float)\n",_value);
}

void Text::printTree(int s) {
	printSpace(s);
	printf("%s(Text)\n",_value);
}

void List::printTree(int s) {
	printSpace(s);
	print();
	printf("\n");
}

void VarName::printTree(int s) {
	printSpace(s);
	printf("%s\n", _name);
}

void Index::printTree(int s) {
	printSpace(s);
	_vobj->print();
	printf("[");
	_e->print();
	printf("]\n");
}

void Variable::printTree(int s) {
	printSpace(s);
	printf("Variable:%s:",_name);
	if(_value)
		_value->print();
	else
		printf("null");
	printf("\n");
}

void Operator::printTree(int s) {
	box[s] = 1;
	_e2->printTree(s + 1);
	printSpace(s);
	if(_opeChar == '^')
		printf(" **\n");
	else
		printf(" %c\n",_opeChar);
	box[s] = 0;		//左
	_e1->printTree(s+1);
}

void Increment::printTree(int s) {
	printSpace(s);
	if(_ch) {
		_vobj->print();
	}
	printf("++");
	if(!_ch) {
		_vobj->print();
	}
	printf("\n");
}

void Decrement::printTree(int s) {
	printSpace(s);
	if(_ch) {
		_vobj->print();
	}
	printf("--");
	if(!_ch) {
		_vobj->print();
	}
	printf("\n");
}

void Minus::printTree(int s) {
	box[s] = 1;		//右
	_e->printTree(s + 1);
	printSpace(s);
	printf("-\n");
}

void Substitution::printTree(int s) {
	box[s] = 1;		//右
	_e->printTree(s+1);
	printSpace(s);
	printf(" %s\n",_opeChar);
	box[s] = 0;		//左
	_vobj->printTree(s+1);
}

void ExprList::printTree(int s) {
	printSpace(s);
	print();
	printf("\n");
}

void BitOperation::printTree(int s) {
	printSpace(s);
	print();
	printf("\n");
}

void Not::printTree(int s) {
	printSpace(s);
	print();
	printf("\n");
}

void ShiftOperator::printTree(int s) {
	printSpace(s);
	print();
	printf("\n");
}

// ---------------------------------------------------------
// 比較演算

bool Integer::compGT(Value *val){
	return _value > val->getInt();
}

bool Integer::compGE(Value *val){
	return _value >= val->getInt();
}

bool Integer::compLT(Value *val){
	return _value < val->getInt();
}

bool Integer::compLE(Value *val){
	return _value <= val->getInt();
}

bool Integer::compEQ(Value *val){
	return _value == val->getInt();
}

bool Integer::compNE(Value *val){
	return _value != val->getInt();
}

// --

bool Float::compGT(Value *val){
	return _value > val->getFloat();
}

bool Float::compGE(Value *val){
	return _value >= val->getFloat();
}

bool Float::compLT(Value *val){
	return _value < val->getFloat();
}

bool Float::compLE(Value *val){
	return _value <= val->getFloat();
}

bool Float::compEQ(Value *val){
	return _value == val->getFloat();
}

bool Float::compNE(Value *val){
	return _value != val->getFloat();
}

// --

bool Text::compGT(Value *val){
	return strcmp(_value, val->getText()) > 0;
}

bool Text::compGE(Value *val){
	return strcmp(_value, val->getText()) >= 0;
}

bool Text::compLT(Value *val){
	return strcmp(_value, val->getText()) < 0;
}

bool Text::compLE(Value *val){
	return strcmp(_value, val->getText()) <= 0;
}

bool Text::compEQ(Value *val){
	return strcmp(_value, val->getText()) == 0;
}

bool Text::compNE(Value *val){
	return strcmp(_value, val->getText()) != 0;
}

// --

bool List::compGT(Value *val){
	return getFloat() > val->getFloat();
}

bool List::compGE(Value *val){
	return getFloat() >= val->getFloat();
}

bool List::compLT(Value *val){
	return getFloat() < val->getFloat();
}

bool List::compLE(Value *val){
	return getFloat() <= val->getFloat();
}

bool List::compEQ(Value *val){
	if (!val->isList())
		return false;
	List *right = (List *)val;
	if (size() != right->size())
		return false;
	for(int i = 0; i < size(); i++){
		Value *va = getElement(i);
		Value *vb = right->getElement(i);
		if (va->compEQ(vb) == false)
			return false;
	}
	return true;
}

bool List::compNE(Value *val){
	return !compEQ(val);
}

// ---------------------------------------------------------
// メソッドコール

MethodCall::MethodCall(char *vname, char *mname){
	_varName = new char[strlen(vname)+1];
	strcpy(_varName, vname);
	_methodName = new char[strlen(mname)+1];
	strcpy(_methodName, mname);
}

MethodCall::~MethodCall(){
#ifdef DEBUG
	printf("delete MethodCall...%s.%s()\n", _varName, _methodName);
#endif
	delete [] _varName;
	delete [] _methodName;
	_eList.clear();
}

void MethodCall::addParam(Expression *e) {
	_eList.append(e);
}

void MethodCall::print() {
	printf("%s.%s(", _varName, _methodName);
	_eList.print();
	printf(")");
}

void MethodCall::printTree(int depth) {
	printSpace(depth);
	print();
	printf(")\n");
}

Value *MethodCall::evaluate() {
	Variable *var = _varMap.getVariable(_varName);
	if(var == NULL) {
		printf("RuntimeError : Undefined variable.(name = %s, line = %d)\n",_varName,_line_number);
		throw ErrorException();
	} 
	Value *value = var->getValue();
	if(value == NULL){
		printf("RuntimeError : No value. (line = %d)\n",_line_number);
		throw ErrorException();
	}
	// 引数式→Value
	List *params = new List();
	Expression *e;
	for(e = _eList.top() ; e ; e = _eList.next()){
		params->append(e->evaluate());
	}

	Value *v = value->methodCall(_methodName, params);
	delete params;

	if (v)
		return v;
	return new Integer(0);
}

// ---------------------------------------------------------
// Resource用メソッドコール

ResMethodCall::ResMethodCall(int res_id, char *mname){
	_resID = res_id;
	_methodName = new char[strlen(mname)+1];
	strcpy(_methodName, mname);
}

ResMethodCall::~ResMethodCall(){
#ifdef DEBUG
	printf("delete ResMethodCall...%d.%s()\n", _resID, _methodName);
#endif
	delete [] _methodName;
	_eList.clear();
}

void ResMethodCall::addParam(Expression *e) {
	_eList.append(e);
}

void ResMethodCall::print() {
	if (_resID == R_WiFi){
		printf("WiFi.");
	} else {
		printf("???.");
	}
	printf("%s(", _methodName);
	_eList.print();
	printf(")");
}

void ResMethodCall::printTree(int depth) {
	printSpace(depth);
	print();
	printf(")\n");
}

Value *ResMethodCall::evaluate() {
	Resource *res;

	if(_resID == R_WiFi){
		res = &_wifi;
	} else {
		printf("Unknown Resource\n");
		return new Integer(0);
	}

	// 引数式→Value
	List *params = new List();
	Expression *e;
	for(e = _eList.top() ; e ; e = _eList.next()){
		params->append(e->evaluate());
	}

	Value *v = res->methodCall(_methodName, params);
	delete params;

	if (v)
		return v;
	return new Integer(0);
}

// -----------------------------------------------------------
// methodCall()

Value *Text::methodCall(char *name, List *params){
#ifdef DEBUG
	printf("Text::methodCall(%s)\n", name);
#endif
	if (strcmp(name, "append") == 0){
		int n = params->size();
		for(int i = 0; i < n ; i++){
			Value *v = params->getElement(i);
			add(v);
		}
		return new Integer(strlen(_value));
	} else if (strcmp(name, "substr") == 0){
		int n, offset, len, cnt;
		n = params->size();
		if (n == 0)
			return new Text(_value);
		offset = params->getElement(0)->getInt();
		cnt = strlen(_value);
		if (offset >= cnt - 1)
			return new Text("");
		if (n >= 2)
			len = params->getElement(1)->getInt();
		else
			len = cnt - offset;
		if (len > (cnt - offset))
			len = cnt - offset;
		char *s = new char[len + 1];
		char *p = s;
		char *q = _value + offset;
		for(int i = 0; i < len ; i++){
			*p++ = *q++;
		}
		*p++ = '\0';
		Value *value = new Text(s);
		delete s;
		return value;
	} else if(strcmp(name, "trim") == 0) {
		int n = params->size();
		if(n != 0)
			return new Text(_value);
		char *s = new char[strlen(_value)+1];
		strcpy(s,_value);
		while(isspace(*s))
			s++;
		
		char *end = s + strlen(s) -1;
		while(end > s && isspace(*end))
			end--;
		end[1] = '\0';
		return new Text(s);
	} else if(strcmp(name, "len") == 0) {
		if(params->size() != 0)
			return new False();
		return new Integer(strlen(_value));
	} else if(strcmp(name, "strcmp") == 0) {
		if(params->size() != 1)
			return new False();
		return new Integer(strcmp(_value, params->getElement(0)->getText()));
	} else if(strcmp(name, "split") == 0) {
		int n = params->size();
		char *part;
		if(n != 1){
			part = " ";
		}else {
			part = params->getElement(0)->getText();
		}
		List *list = new List();
		int posx = 0;
		int posy = 0;
		char *crsr;
		char *text = crsr = _value;

		do {
			while(*crsr == *part){
				posx++;
				posy++;
				*crsr++;
			}
			if(*crsr != *part && *crsr != '\0'){
				posy++;
				*crsr++;
				if(*crsr == *part || *crsr == '\0'){
					int XtoY = posy - posx;
					int i=0;
					char *word = new char[XtoY + 1];
					if(XtoY > 0){
						for(; i<XtoY; i++){
							word[i] = text[posx++];
						}
					}
					word[i] = '\0';
					list->append(new Text(word));
					delete word;
					if(*crsr != '\0'){
						posx++;
						posy++;
						*crsr++;
					}
				}
			}
		}while(*crsr != '\0');
		return list;
	} else if(strcmp(name, "find") == 0){
		int n = params->size();
		int pos = 0, i = 0;
		if(n == 2)
			i = params->getElement(1)->getInt();
		else if(n != 1)
			return new False();
		char *text = _value;
		char *word = params->getElement(0)->getText();
		int head = 0;
		bool first = true;
		do {
			if(word[pos] == '\0'){
				return new Integer(head);
			} else if(text[i] != word[pos]){
				pos = 0;
				first = true;
			}
			if(text[i] == word[pos]){
				if(first){
					first = false;
					head = i;
				}
				pos++;
			}
		}while(text[i++]);

		return new Integer(-1);
	} else if(strcmp(name, "r_find") == 0){
		char *text = _value;
		char *word = params->getElement(0)->getText();
		int i = strlen(text) - 1;
		int pos = strlen(word) - 1;
		int hip = pos;
		bool first = true;
		
		do {
			if(pos < 0){
				printf("print pos\n");
				return new Integer(i+1);
			}else if(text[i] != word[pos]){
				pos = hip;
				first = true;
			}
			if(text[i] == word[pos]){
				if(first)
					first = false;
				pos--;
			}
		}while(i-- > -1);
		printf("print -1\n");
		return new Integer(-1);
	} else {
		printf("Error : Not implemented(%s, line = %d)\n", name,_line_number);
		throw ErrorException();
	}
	return NULL;
}

Value *List::methodCall(char *name, List *params){
#ifdef DEBUG
	printf("List::methodCall(%s)\n", name);
#endif
	if (strcmp(name, "append") == 0){
		int n = params->size();
		for(int i = 0; i < n ; i++){
			Value *v = params->getElement(i);
			append(v->evaluate());
		}
		return new Integer(size());
	} else if (strcmp(name, "insert") == 0){
		int n = params->size();
		if (n < 2)
			return new False();
		int pos = params->getElement(0)->getInt();
		Value *v = params->getElement(1);
		insert(pos, v->evaluate());
		return new Integer(size());
	} else if(strcmp(name, "clear") == 0) {
		int n = params->size();
		if(n != 0)
			return new False();
		clear();
		return new Integer(size());
	} else if(strcmp(name, "remove") == 0) {
		int n = params->size();
		if(n != 1)
			return new False();
		remove(params->getElement(0));
		return new Integer(size());
	} else if(strcmp(name, "len") == 0) {
		if(params->size() != 0)
			return new False();
		return new Integer(size());
	} else {
		printf("Error : Not implemented(%s, line = %d)\n", name,_line_number);
		throw ErrorException();
	}
	return NULL;
}

