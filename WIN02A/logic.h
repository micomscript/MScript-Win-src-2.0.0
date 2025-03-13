////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
Begin license text.

Copyright 2023- MicomScript Project

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and 
associated documentation files (the gSoftwareh), to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial 
portions of the Software.

THE SOFTWARE IS PROVIDED gAS ISh, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
IN THE SOFTWARE.

End license text.
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * logic.h
 */

#ifndef __LOGIC_H__
#define __LOGIC_H__

extern int _line_number;

void printSpace(int s);

class Logic :public Expression{
public:
	Logic() {}
	virtual ~Logic() {}

	virtual char *getName() = 0;

	Value *evaluate();

	virtual void printTree(int depth) {
		printSpace(depth);
		print();
		printf("\n");
	}
};

class Bool : public Logic, public Value {
private:
	bool	_value;

public:
	Bool(bool value) {
		_value = value;
	}
	~Bool() {
#ifdef DEBUG
		printf("delete Bool\n");
#endif
	}

	char	*getName()	{
		if(_value)
			return (char *)"true";
		return (char *)"false";
	}
	bool	eval(){
		return _value;
	}
	long	getInt();
	double	getFloat();
	char	*getText()	{
		if(_value)
			return (char *)"true";
		return (char *)"false";
	}
	Value	*evaluate(){
		return new Bool(_value);
	}
	bool	getBool() { return _value; }
	void	add(Value *val);
	void	sub(Value *val);
	void	mul(Value *val);
	void	div(Value *val);
	void	mod(Value *val);
	void	power(Value *val);
	void	increment(bool ch);
	void	decrement(bool ch);
	void	minus();

	void print();
	void printTree(int depth);

	char	*getType() { return (char *)"bool"; }

	bool	compGT(Value *val);
	bool	compGE(Value *val);
	bool	compLT(Value *val);
	bool	compLE(Value *val);
	bool	compEQ(Value *val);
	bool	compNE(Value *val);
};

class True : public Bool {

public:
	True():Bool(true) {}
};

class False : public Bool {

public:
	False():Bool(false) {}
};

// ˜_—‰‰ŽZŽq------------------------------------------------------
class LogicalOperation : public Logic {
protected:
	Expression	*_left;
	Expression	*_right;
	char	*_logicChar;

public:
	LogicalOperation(Expression *left,Expression *right,char *ch) {
		_left = left;
		_right = right;
		_logicChar = new char[strlen(ch)+1];
		strcpy(_logicChar,ch);
	}
	~LogicalOperation() {
#ifdef DEBUG
		printf("delete LogicalOperation\n");
#endif
		if(_left)
			delete _left;
		if(_right)
			delete _right;
		delete _logicChar;
	}

	char *getName() { return _logicChar; }
	void print() {
		_left->print();
		printf(" %s ",_logicChar);
		_right->print();
	}
	void printTree(int depth);
};

class LogicAND : public LogicalOperation {
public:
	LogicAND(Expression *left,Expression *right):LogicalOperation(left,right,(char *)"&&"){}

	bool eval();
};

class LogicOR : public LogicalOperation {
public:
	LogicOR(Expression *left,Expression *right):LogicalOperation(left,right,(char *)"||"){}

	bool eval();
};

class LogicNOT : public Logic {
private:
	Expression	*_logic;

public:
	LogicNOT(Expression *logic) {
		_logic = logic;
	}
	~LogicNOT() {
#ifdef DEBUG
		printf("delete NOT\n");
#endif
		delete _logic;
	}

	char *getName() { return (char *)"!"; }
	void print() {
		printf("! ");
		_logic->print();
	}
	bool eval();
};

// ”äŠr‰‰ŽZŽq -----------------------------------------------------
class Comparator : public Logic {
protected:
	Expression	*_left;
	Expression	*_right;
	char		*_compChar;

public:
	Comparator(Expression *left,Expression *right,char *ch) {
		_left = left;
		_right = right;
		_compChar = new char[strlen(ch)+1];
		strcpy(_compChar,ch);
	}
	~Comparator() {
#ifdef DEBUG
		printf("delete Comparator\n");
#endif
		if(_left)
			delete _left;
		if(_right)
			delete _right;
		delete _compChar;
	}

	char *getName() { return _compChar; }
	void print();
	void printTree(int depth);
};

// >
class GT : public Comparator {
public:
	GT(Expression *e1,Expression *e2):Comparator(e1,e2,(char *)">") {}

	bool eval();
};

// >=
class GE : public Comparator {
public:
	GE(Expression *e1,Expression *e2):Comparator(e1,e2,(char *)">=") {}

	bool eval();
};

// <=
class LE : public Comparator {
public:
	LE(Expression *e1,Expression *e2):Comparator(e1,e2,(char *)"<=") {}

	bool eval();
};

// <
class LT : public Comparator {
public:
	LT(Expression *e1,Expression *e2):Comparator(e1,e2,(char *)"<") {}

	bool eval();
};

// ==
class EQ : public Comparator {
public:
	EQ(Expression *e1,Expression *e2):Comparator(e1,e2,(char *)"==") {}

	bool eval();
};

// !=
class NE : public Comparator {
public:
	NE(Expression *e1,Expression *e2):Comparator(e1,e2,(char *)"!=") {}

	bool eval();
};

class LogicSubstitution : public Logic {
private:
	VarObject	*_vobj;
	Logic		*_logic;

public:
	LogicSubstitution(VarObject *vobj, Logic *logic) {
		_vobj = vobj;
		_logic = logic;
	}
	~LogicSubstitution() {
#ifdef DEBUG
		printf("delete LogicSubstitution\n");
#endif
		delete _vobj;
		delete _logic;
	}

	char	*getName() { return NULL; }
	void	print();
	void	printTree(int depth);
	bool	 eval();
	Value	*evaluate();
};

#endif