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
 * expression.h
 */
#ifndef _EXPRESSION_H_
#define _EXPRESSION_H_

extern int _line_number;

class Value;
class Function;
class True;
class False;
class Logic;
class ErrorException;

void printSpace(int s);
void clearConstants();
void printConstants();
#ifdef T_ESP32
void printESPCon();
#endif

class Expression {

public:
	virtual 		~Expression() { }

	virtual Value 	*evaluate() = 0;
	virtual bool	eval();

	virtual void 	print() = 0;
	virtual void 	printTree(int s) = 0;

	virtual bool	isVariable() { return false; }

	virtual int		getParamCount() { return -1; }
	virtual bool	isPrint() { return false; }
};

class List;

class Value : public Expression {
private:

public:
	virtual ~Value() {}

	virtual long	getInt() = 0;
	virtual double	getFloat() = 0;
	virtual char	*getText() = 0;
	virtual bool	getBool() { return false; }

	virtual void	add(Value *val) = 0;
	virtual void	sub(Value *val) = 0;
	virtual void	mul(Value *val) = 0;
	virtual void	div(Value *val) = 0;
	virtual void	mod(Value *val) = 0;
	virtual void	power(Value *val) = 0;
	virtual void	increment(bool ch) = 0;
	virtual void	decrement(bool ch) = 0;
	virtual void	minus() = 0;

	virtual bool	isList() { return false; }
	virtual char	*getType() = 0;

	virtual bool	compGT(Value *val){
		return false;
	}
	virtual bool	compGE(Value *val){
		return false;
	}
	virtual bool	compLT(Value *val){
		return false;
	}
	virtual bool	compLE(Value *val){
		return false;
	}
	virtual bool	compEQ(Value *val){
		return false;
	}
	virtual bool	compNE(Value *val){
		return false;
	}
	virtual Value *methodCall(char *name, List *params);
};

class Constant : public Value {
private:
	char	*_name;
	Value	*_value;

public:
	Constant(char *name,Value *value) {
		_name = new char[strlen(name)+1];
		strcpy(_name,name);
		_value = value;
	}
	~Constant() {
#ifdef DEBUG
		printf("delete constant(%s)\n",_name);
#endif
	}

	char *getName() { return _name; }
	void print();
	void printTree(int s);
	long getInt() {
		return _value->getInt();
	}
	double getFloat() {
		return _value->getFloat();
	}
	bool getBool(){
		return _value->getBool();
	}
	char *getText();

	void	add(Value *val)		{}
	void	sub(Value *val)		{}
	void	mul(Value *val)		{}
	void	div(Value *val)		{}
	void	mod(Value *val)		{}
	void	power(Value *val)	{}
	void	increment(bool ch)	{}
	void	decrement(bool ch)	{}
	void	minus()				{}

	Value *evaluate();

	char *getType() { return _value->getType(); }
};

class Integer : public Value {
private:
	long	_value;

public:
	Integer() {
		_value = 0;
	}
	Integer(long value) {
		_value = value;
	}
	~Integer() {
#ifdef DEBUG
		printf("delete Integer(%d)\n",_value);
#endif
	}

	void print();
	void printTree(int s);
	long getInt() {
		return _value;
	}
	double getFloat() {
		return (double)_value;
	}
	bool getBool(){
		return _value != 0;
	}
	char *getText();

	void	add(Value *val);
	void	sub(Value *val);
	void	mul(Value *val);
	void	div(Value *val);
	void	mod(Value *val);
	void	power(Value *val);
	void	increment(bool ch);
	void	decrement(bool ch);
	void	minus();

	Value *evaluate();

	char	*getType() { return (char *)"integer"; }

	bool	compGT(Value *val);
	bool	compGE(Value *val);
	bool	compLT(Value *val);
	bool	compLE(Value *val);
	bool	compEQ(Value *val);
	bool	compNE(Value *val);
};

class Float : public Value {
private:
	double	_value;

public:
	Float() {
		_value = 0;
	}
	Float(double value) {
		_value = value;
	}
	~Float() {
#ifdef DEBUG
		printf("delete Float(%f)\n",_value);
#endif
	}

	void print();
	void printTree(int s);

	long getInt() {
		return (long)_value;
	}
	double getFloat() {
		return _value;
	}
	bool getBool(){
		return _value != 0;
	}
	char *getText();

	void	add(Value *val);
	void	sub(Value *val);
	void	mul(Value *val);
	void	div(Value *val);
	void	mod(Value *val);
	void	power(Value *val);
	void	increment(bool ch);
	void	decrement(bool ch);
	void	minus();

	Value *evaluate();

	char	*getType() { return (char *)"float"; }

	bool	compGT(Value *val);
	bool	compGE(Value *val);
	bool	compLT(Value *val);
	bool	compLE(Value *val);
	bool	compEQ(Value *val);
	bool	compNE(Value *val);
};

class Text : public Value {
private:
	char 	*_value;

public:
	Text() {
		_value = NULL;
	}
	Text(char *value);
	Text(long value);
	~Text();

	void print();
	void printTree(int s);

	long getInt();
	double getFloat();
	char *getText();
	bool getBool(){
		return _value != NULL;
	}

	void 	add(Value *val);
	void	sub(Value *val);
	void	mul(Value *val);
	void	div(Value *val);
	void	mod(Value *val);
	void	power(Value *val);
	void	increment(bool ch);
	void	decrement(bool ch);
	void	minus();

	Value *evaluate();

	char	*getType() { return (char *)"string"; }

	bool	compGT(Value *val);
	bool	compGE(Value *val);
	bool	compLT(Value *val);
	bool	compLE(Value *val);
	bool	compEQ(Value *val);
	bool	compNE(Value *val);

	Value	*methodCall(char *name, List *params);
};

class ListNode {
public:
	Value		*_value;
	ListNode	*_next;

	ListNode(Value *value, ListNode *next) {
		_value = value;
		_next = next;
	}
	~ListNode() {
#ifdef DEBUG
		if(_value) {
			printf("delete list element ");
			_value->print();
			printf("\n");
		} else {
			printf("delete list element(dummy)\n");
		}
#endif
		if(_value)
			delete _value;
	}
	void setValue(Value *value, ListNode *next) {
		_value = value;
		_next = next;
	}
};

class List : public Value {
private:
	ListNode	*_top;
	ListNode	*_last;

public:
	List() {
		_top = _last = new ListNode(NULL,NULL);
	}
	~List() {
		clear();
		delete _top;
	}

	void print();
	void printTree(int s);
	void clear();

	long getInt();
	double getFloat();
	char *getText();
	bool getBool(){
		if(size() == 0)
			return false;
		else
			return true;
	}

	void 	add(Value *val);
	void	sub(Value *val);
	void	mul(Value *val);
	void	div(Value *val);
	void	mod(Value *val);
	void	power(Value *val);
	void	increment(bool ch);
	void	decrement(bool ch);
	void	minus();

	int size();
	Value *sum();
	Value *max();
	Value *min();
	void  expression();
	Value *chint();
	Value *chfloat();
	Value *chstring();
	Value *chbool();
	Value *len();
	Value *type();
	Value *Time();
	Value *Delay();
	Value *Micros();


	void	append(Value *value);
	void	insert(int index, Value *value);
	void	remove(Value *value);

	Value	*getElement(int index);
	void	setElement(int index, Value *value);

	bool	isEmpty() { return _top == _last; }
	bool	isList() { return true; }

	Value *evaluate();

	char	*getType() { return (char *)"list"; }

	bool	compGT(Value *val);
	bool	compGE(Value *val);
	bool	compLT(Value *val);
	bool	compLE(Value *val);
	bool	compEQ(Value *val);
	bool	compNE(Value *val);

	Value	*methodCall(char *name, List *params);
};


class Variable : public Expression {
private:
	char	*_name;
	Value	*_value;

public:
	Variable(const char *name);
	~Variable() {
#ifdef DEBUG
		printf("delete Variable %s\n",_name);
#endif
		delete _name;
		if(_value)
			delete _value;
	}

	void print();
	void printTree(int s);

	void setData(int value);
	void setData(double value);
	void setData(char *value);
	void setValue(Value *value);

	Value *getValue();
	char *getName() {
		return _name;
	}
	bool isVariable() { return true; }
	Value *evaluate();
};

class VarObject : public Expression {

public:
	VarObject() {}

	virtual bool	isList() { return false; }
	virtual Value	*getValue() = 0;
	virtual void	setValue(Value *value) = 0;
};

class VarName : public VarObject {
protected:
	char	*_name;

public:
	VarName(const char *name) {
		_name = (char *)(new char[strlen(name)+1]);
		strcpy(_name, name);
	}
	~VarName() {
#ifdef DEBUG
		printf("delete VarName(%s)\n", _name);
#endif
		delete _name;
	}

	void print();
	void printTree(int s);

	bool isVariable() { return true; }

	Value *getValue();
	void setValue(Value *value);

	Value *evaluate();
};

class Index : public VarObject {
private:
	VarObject	*_vobj;
	Expression	*_e;

public:
	Index(VarObject *vobj, Expression *e) {
		_vobj = vobj;
		_e = e;
	}

	void print();
	void printTree(int s);

	Value	*getValue();
	void	setValue(Value *value);
	bool	isList() { return true; }

	Value	*evaluate();
};

class Operator : public Expression {
protected:
	Expression *_e1;
	Expression *_e2;
	char	   _opeChar;

public:
	Operator(Expression *e1, Expression *e2, char ch) {
		_e1 = e1;
		_e2 = e2;
		_opeChar = ch;
	}

	~Operator() {
#ifdef DEBUG
		printf("delete Operator %c\n", _opeChar);
#endif
		delete _e1;
		delete _e2;
	}

	void print();
	void printTree(int s);
};


//加算
class Addition : public Operator {

public:
	Addition(Expression *e1, Expression *e2):Operator(e1,e2,'+') { }

	Value *evaluate();
};

//減算
class Subtraction : public Operator {

public:
	Subtraction(Expression *e1,Expression *e2):Operator(e1,e2,'-') { }

	Value *evaluate();
};

//乗算
class Multiplication : public Operator {

public:
	Multiplication(Expression *e1, Expression *e2):Operator(e1,e2,'*') { }

	Value *evaluate();
};

//除算
class Division : public Operator {

public:
	Division(Expression *e1, Expression *e2):Operator(e1,e2,'/') { }

	Value *evaluate();
};

//剰余
class Remainder : public Operator {

public:
	Remainder(Expression *e1, Expression *e2):Operator(e1,e2,'%') { }

	Value *evaluate();
};

//べき乗
class Exponentiation : public Operator {

public:
	Exponentiation(Expression *e1, Expression *e2):Operator(e1,e2,'^') { }

	Value *evaluate();
};

//インクリメント(++)
class Increment : public Expression {
private:
	VarObject	*_vobj;
	bool		_ch;	// true:前置  false:後置
public:
	Increment(VarObject *vobj, bool ch) {
		_vobj = vobj;
		_ch = ch;
	}

	~Increment() {
#ifdef DEBUG
		printf("delete ++\n");
#endif
	}

	void print();
	void printTree(int s);

	Value *evaluate();
};

//デクリメント(--)
class Decrement : public Expression {
private:
	VarObject	*_vobj;
	bool		_ch;
public:
	Decrement(VarObject *vobj, bool ch) {
		_vobj = vobj;
		_ch = ch;
	}

	~Decrement(){
#ifdef DEBUG
		printf("delete --\n");
#endif
	}

	void print();
	void printTree(int s);

	Value *evaluate();
};

//符号
class Minus : public Expression {
private:
	Expression *_e;

public:
	Minus(Expression *e) {
		_e = e;
	}

	~Minus() {
#ifdef DEBUG
		printf("delete Minus\n");
#endif
		delete _e;
	}

	Value *evaluate();
	void print();
	void printTree(int s);

};

// 代入
class Substitution : public Expression  {
protected:
	VarObject		*_vobj;
	Expression		*_e;
	char			*_opeChar;

public:
	Substitution(VarObject *vobj, Expression *e,char *ch) {
		_vobj = vobj;
		_e = e;
		_opeChar = ch;
	}

	~Substitution() {
#ifdef DEBUG
		printf("delete Operator =\n");
#endif
		delete _vobj;
		delete _e;
	}

	void print();
	void printTree(int s);
};

// =
class EqSubstitution : public Substitution {

public:
	EqSubstitution(VarObject *vobj, Expression *e):
		Substitution(vobj,e,(char *)"=") {}

	Value *evaluate();
};

// +=
class AddSubstitution : public Substitution {

public:
	AddSubstitution(VarObject *vobj, Expression *e):
		Substitution(vobj,e,(char *)"+=") {}

	Value *evaluate();
};

// -=
class SubSubstitution : public Substitution {

public:
	SubSubstitution(VarObject *vobj, Expression *e):
		Substitution(vobj,e,(char *)"-=") {}

	Value *evaluate();
};

// *=
class MulSubstitution : public Substitution {

public:
	MulSubstitution(VarObject *vobj, Expression *e):
		Substitution(vobj,e,(char *)"*=") {}

	Value *evaluate();
};

// /=
class DivSubstitution : public Substitution {

public:
	DivSubstitution(VarObject *vobj, Expression *e):
		Substitution(vobj,e,(char *)"/=") {}

	Value *evaluate();
};

// %=
class ModSubstitution : public Substitution {

public:
	ModSubstitution(VarObject *vobj, Expression *e):
		Substitution(vobj,e,(char *)"%=") {}

	Value *evaluate();
};

// **=
class PowSubstitution : public Substitution {

public:
	PowSubstitution(VarObject *vobj, Expression *e):
		Substitution(vobj,e,(char *)"**=") {}

	Value *evaluate();
};

// &=
class AndSubstitution : public Substitution {

public:
	AndSubstitution(VarObject *vobj, Expression *e):
		Substitution(vobj,e,(char *)"&=") {}

	Value *evaluate();
};

// |=
class OrSubstitution : public Substitution {

public:
	OrSubstitution(VarObject *vobj, Expression *e):
		Substitution(vobj,e,(char *)"|=") {}

	Value *evaluate();
};

// ^=
class ExorSubstitution : public Substitution {

public:
	ExorSubstitution(VarObject *vobj, Expression *e):
		Substitution(vobj,e,(char *)"^=") {}

	Value *evaluate();
};

// <<=
class LShiftSubstitution : public Substitution {

public:
	LShiftSubstitution(VarObject *vobj, Expression *e):
		Substitution(vobj,e,(char *)"<<=") {}

	Value *evaluate();
};

// >>=
class RShiftSubstitution : public Substitution {

public:
	RShiftSubstitution(VarObject *vobj, Expression *e):
		Substitution(vobj,e,(char *)">>=") {}

	Value *evaluate();
};

class ExprNode {
public:
	Expression	*_express;
	ExprNode	*_next;

	ExprNode(Expression *e) {
		_express = e;
		_next = NULL;
	}
	~ExprNode() {
#ifdef DEBUG
		if(_express) {
			printf("delete expr-list element ");
			_express->print();
			printf("\n");
		} else {
			printf("delete expr-list element(dummy)\n");
		}
#endif
		if(_express) {
			delete _express;
		}
	}
};

class ExprList : public Expression {
private:
	ExprNode	*_top;
	ExprNode	*_last;
	ExprNode	*_curr;

public:
	ExprList() {
		_top = _last = _curr = NULL;
	}
	~ExprList() {
		ExprNode *node, *next;

		for(node = _top; node; node = next) {
			next = node->_next;
			delete node;
		}
	}

	void append(Expression *e);

	Expression *getElement(int pos);

	bool isEmpty() { return _top == NULL; }

	void print();
	void printTree(int s);
	void clear();
	int  size();

	Expression *top();
	Expression *next();

	Value *evaluate();
};

// ビット演算子 ----------------------------------------------------
class BitOperation : public Expression {
protected:
	Expression	*_left;
	Expression	*_right;
	char		*_bitChar;

public:
	BitOperation(Expression *left, Expression *right, char *ch) {
		_left = left;
		_right = right;
		_bitChar = new char[strlen(ch)+1];
		strcpy(_bitChar,ch);
	}
	~BitOperation() {
#ifdef DEBUG
		printf("delete BitOperation\n");
#endif
		if(_left)
			delete _left;
		if(_right)
			delete _right;
		delete _bitChar;
	}

	void print();
	void printTree(int s);
};

class And : public BitOperation {

public:
	And(Expression *left, Expression *right):BitOperation(left,right,"&") {}

	Value *evaluate();
};

class Or : public BitOperation {

public:
	Or(Expression *left, Expression *right):BitOperation(left,right,"|") {}

	Value *evaluate();
};

class Exor : public BitOperation {

public:
	Exor(Expression *left, Expression *right):BitOperation(left,right,"^") {}

	Value *evaluate();
};

class Not : public Expression {
private:
	Expression	*_e;

public:
	Not(Expression *e) {
		_e = e;
	}
	~Not() {
#ifdef DEBUG
		printf("delete not\n");
#endif
	}

	void print();
	void printTree(int s);
	Value *evaluate();
};

class ShiftOperator : public Expression {
protected:
	Expression	*_e1;
	Expression	*_e2;
	char		*_shiftch;

public:
	ShiftOperator(Expression *e1,Expression *e2,char *ch) {
		_e1 = e1;
		_e2 = e2;
		_shiftch = new char[strlen(ch)+1];
		strcpy(_shiftch,ch);
	}
	~ShiftOperator() {
#ifdef DEBUG
		printf("delete ShiftOperator\n");
#endif
		delete _e1;
		delete _e2;
		delete _shiftch;
	}

	void print();
	void printTree(int s);
};

class LShift : public ShiftOperator {
public:
	LShift(Expression *e1,Expression *e2):ShiftOperator(e1,e2,(char *)"<<"){}

	Value *evaluate();
};

class RShift : public ShiftOperator {
public:
	RShift(Expression *e1,Expression *e2):ShiftOperator(e1,e2,(char *)">>"){}

	Value *evaluate();
};

// メソッドコール --------------------------------------------------

class MethodCall : public Expression{
private:
	char		*_varName;
	char		*_methodName;
	ExprList	 _eList;

public:
	 MethodCall(char *vname, char *mname);
	~MethodCall();

	void print();
	void printTree(int depth);
	void addParam(Expression *e);
	Value *evaluate();
};

class ResMethodCall : public Expression{
private:
	int			 _resID;
	char		*_methodName;
	ExprList	 _eList;

public:
	 ResMethodCall(int res_id, char *mname);
	~ResMethodCall();

	void print();
	void printTree(int depth);
	void addParam(Expression *e);
	Value *evaluate();
};

#endif
