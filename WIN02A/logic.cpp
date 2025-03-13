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
 * logic.cpp
 */

#include<stdio.h>
#include<string.h>
#include<math.h>
#include "common.h"
#include "expression.h"
#include "sentence.h"
#include "logic.h"

extern int box[100];
extern int _line_number;

Value *Logic::evaluate(){
	bool b = eval();
	if (b)
		return new True();
	return new False();
}

// Bool -----------------------------------------------------

long Bool::getInt(){
	return _value ? 1 : 0;
}

double Bool::getFloat(){
	return _value ? 1.0 : 0.0;
}

void Bool::add(Value *val) {
	_value = _value || val->getBool();
}

void Bool::sub(Value *val) {
	if (_value){
		if (val->getBool())
			_value = false;
	}
}

void Bool::mul(Value *val) {
	_value = _value && val->getBool();
}

void Bool::div(Value *val) {
	printf("Unimplemented (line=  %d)\n",_line_number);
	throw ErrorException();
}

void Bool::mod(Value *val) {
	printf("Unimplemented (line=  %d)\n",_line_number);
	throw ErrorException();
}

void Bool::power(Value *val) {
	printf("Unimplemented (line=  %d)\n",_line_number);
	throw ErrorException();
}

void Bool::increment(bool ch) {
	printf("Unimplemented (line=  %d)\n",_line_number);
	throw ErrorException();
}

void Bool::decrement(bool ch) {
	printf("Unimplemented (line=  %d)\n",_line_number);
	throw ErrorException();
}

void Bool::minus() {
	printf("Unimplemented (line=  %d)\n",_line_number);
	throw ErrorException();
}

bool Bool::compGT(Value *val){
	bool b = val->getBool();
	return _value && !b;
}

bool Bool::compGE(Value *val){
	bool b = val->getBool();
	return _value || !b;
}

bool Bool::compLT(Value *val){
	bool b = val->getBool();
	return !_value && b;
}

bool Bool::compLE(Value *val){
	bool b = val->getBool();
	return !_value || b;
}

bool Bool::compEQ(Value *val){
	bool b = val->getBool();
	return _value == b;
}

bool Bool::compNE(Value *val){
	bool b = val->getBool();
	return _value != b;
}

// evaluate(), eval() ---------------------------------------------

bool LogicAND::eval(){
	Value *v1 = _left->evaluate();
	bool b1 = v1->getBool();
	delete v1;
	if (b1 == false){
		return false;
	}
	Value *v2 = _right->evaluate();
	bool b2 = v2->getBool();
	delete v2;
	return b2;
}

bool LogicOR::eval(){
	Value *v1 = _left->evaluate();
	bool b1 = v1->getBool();
	delete v1;
	if (b1){
		return true;
	}
	Value *v2 = _right->evaluate();
	bool b2 = v2->getBool();
	delete v2;
	return b2;
}

bool LogicNOT::eval(){
	Value *v = _logic->evaluate();
	bool b = v->getBool();
	delete v;
	return !b;
}

bool GT::eval(){
	Value *left = _left->evaluate();
	Value *right = _right->evaluate();
	bool b = false;
	if (left && right)
		b = left->compGT(right);
	delete left;
	delete right;
	return b;
}

bool GE::eval(){
	Value *left = _left->evaluate();
	Value *right = _right->evaluate();
	bool b = false;
	if (left && right)
		b = left->compGE(right);
	delete left;
	delete right;
	return b;
}

bool LE::eval(){
	Value *left = _left->evaluate();
	Value *right = _right->evaluate();
	bool b = false;
	if (left && right)
		b = left->compLE(right);
	delete left;
	delete right;
	return b;
}

bool LT::eval(){
	Value *left = _left->evaluate();
	Value *right = _right->evaluate();
	bool b = false;
	if (left && right)
		b = left->compLT(right);
	delete left;
	delete right;
	return b;
}

bool EQ::eval(){
	Value *left = _left->evaluate();
	Value *right = _right->evaluate();
	bool b = false;
	if (left && right)
		b = left->compEQ(right);
	delete left;
	delete right;
	return b;
}

bool NE::eval(){
	Value *left = _left->evaluate();
	Value *right = _right->evaluate();
	bool b = false;
	if (left && right)
		b = left->compNE(right);
	delete left;
	delete right;
	return b;
}

// print ---------------------------------------------------------
void Bool::print() {
	if(_value)
		printf("true");
	else
		printf("false");
}

void Bool::printTree(int depth) {
	printSpace(depth);
	print();
	printf("\n");
}

void LogicalOperation::printTree(int depth) {
	box[depth] = 1;
	_right->printTree(depth + 1);
	printSpace(depth);
	printf(" %s\n",_logicChar);
	box[depth] = 0;
	_left->printTree(depth+1);
}

void Comparator::print() {
	_left->print();
	printf(" %s ",_compChar);
	_right->print();
}

void Comparator::printTree(int depth) {
	box[depth] = 1;
	_right->printTree(depth + 1);
	printSpace(depth);
	printf(" %s\n",_compChar);
	box[depth] = 0;
	_left->printTree(depth+1);
}

void LogicSubstitution::print() {
	_vobj->print();
	printf("=(");
	_logic->print();
}

bool LogicSubstitution::eval() {
	bool b = _logic->eval();
	if(b)
		_vobj->setValue(new True());
	else
		_vobj->setValue(new False());
	return b;
}

Value *LogicSubstitution::evaluate() {
	Value *e;
	bool b;
	b = _logic->eval();
	if(b)
		e = new True();
	else
		e = new False();
	_vobj->setValue(e);
	return e->evaluate();
}

void LogicSubstitution::printTree(int depth) {
	box[depth] = 1;		//âE
	_logic->printTree(depth+1);
	printSpace(depth);
	printf(" =\n");
	box[depth] = 0;		//ç∂
	_vobj->printTree(depth+1);
}
