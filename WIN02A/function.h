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
 * function.h
 */
#ifndef _FUNCTION_H_
#define _FUNCTION_H_

class Function : public Expression {
public:
	Function() {}

	virtual ~Function() {}

	virtual void setName(char *name) = 0;
	virtual char *getName() = 0;
	virtual Value *evaluate() = 0;
	virtual void setParamValue(int pos, Value *value) = 0;
	virtual void print() = 0;
	void printTree(int depth) {}
	virtual void clearParams() = 0;

	virtual int getParamCount() { return 0; }
	virtual bool isPrint() { return false; }
};

class MathFunction : public Function {
protected:
	char	*_name;
	double	_x;

public:
	MathFunction(char *name) {
		_name = name;
	}

	~MathFunction() {
#ifdef DEBUG
		printf("delete %s()\n", _name);
#endif
	}

	void setName(char *name) {
		_name = name;
	}

	char *getName() {
		return _name;
	}

	void setParamValue(int pos, Value *value);

	virtual int getParamCount() {
		return 1;
	}

	void clearParams(){}

	void print() {
		printf("%s()", _name);
	}
};

class Sin : public MathFunction {
public:
	Sin(char *name) : MathFunction(name) {}

	Value *evaluate();
};

class Cos : public MathFunction {
public:
	Cos(char *name) : MathFunction(name) {}

	Value *evaluate();
};

class Tan : public MathFunction {
public:
	Tan(char *name) : MathFunction(name) {}

	Value *evaluate();
};

class ASin : public MathFunction {
public:
	ASin(char *name) : MathFunction(name) {}

	Value *evaluate();
};

class ACos : public MathFunction {
public:
	ACos(char *name) : MathFunction(name) {}

	Value *evaluate();
};

class ATan : public MathFunction {
public:
	ATan(char *name) : MathFunction(name) {}

	Value *evaluate();
};

class Sinh : public MathFunction {
public:
	Sinh(char *name) : MathFunction(name) {}

	Value *evaluate();
};

class Cosh : public MathFunction {
public:
	Cosh(char *name) : MathFunction(name) {}

	Value *evaluate();
};

class Tanh : public MathFunction {
public:
	Tanh(char *name) : MathFunction(name) {}

	Value *evaluate();
};

class Exp : public MathFunction {
public:
	Exp(char *name) : MathFunction(name) {}

	Value *evaluate();
};

class Log : public MathFunction {
public:
	Log(char *name) : MathFunction(name) {}

	Value *evaluate();
};

class Log10 : public MathFunction {
public:
	Log10(char *name) : MathFunction(name) {}

	Value *evaluate();
};

class Sqrt : public MathFunction {
public:
	Sqrt(char *name) : MathFunction(name) {}

	Value *evaluate();
};

class Cbrt : public MathFunction {
public:
	Cbrt(char *name) : MathFunction(name) {}

	Value *evaluate();
};

class Floor : public MathFunction {
public:
	Floor(char *name) : MathFunction(name) {}

	Value *evaluate();
};

class Ceil : public MathFunction {
public:
	Ceil(char *name) : MathFunction(name) {}

	Value *evaluate();
};

class Round : public MathFunction {
public:
	Round(char *name) : MathFunction(name) {}

	Value *evaluate();
};

class Fabs : public MathFunction {
public:
	Fabs(char *name) : MathFunction(name) {}

	Value *evaluate();
};

class Abs : public MathFunction {
public:
	Abs(char *name) : MathFunction(name) {}

	Value *evaluate();
};

// ─────────────────────────────────
class Pow : public MathFunction {
protected:
	double		_y;

public:
	Pow(char *name) : MathFunction(name){}

	void setParamValue(int pos, Value *value);

	int getParamCount(){ return 2; }

	Value *evaluate();
};

class Hypot : public Pow {
public:
	Hypot(char *name) : Pow(name) {}

	Value *evaluate();
};

class ATan2 : public Pow {
public:
	ATan2(char *name) : Pow(name) {}

	Value *evaluate();
};

class Random : public MathFunction {
public:
	Random(char *name) : MathFunction(name) {}

	Value *evaluate();
};

class SRand : public MathFunction {
public:
	SRand(char *name) :MathFunction(name) {}

	Value *evaluate();
};

// ─────────────────────────────────
class AbstructFunction : public Function{
protected:
	char	*_name;
	List	*_list;

public:
	AbstructFunction(char *name){
		_name = name;
		_list = new List();
	}

	~AbstructFunction(){
#ifdef DEBUG
		printf("delete AbstructFunction\n");
#endif
		delete _list;
	}

	void setName(char *name){
		_name = name;
	}

	void clearParams(){
		_list->clear();
	}

	char *getName(){
		return _name;
	}

	void print();
	void printTree(int depth);

	void setParamValue(int pos, Value *value){
		_list->append( value);
	}

	int getParamCount(){
		return 1;
	}

	virtual bool isPrint() { return false; }
};

class Sum : public AbstructFunction {
public:
	Sum(char *name) : AbstructFunction(name) {}

	Value *evaluate();
};

class Max : public AbstructFunction {
public:
	Max(char *name) : AbstructFunction(name) {}

	Value *evaluate();
};

class Min : public AbstructFunction {
public:
	Min(char *name) : AbstructFunction(name) {}

	Value *evaluate();
};

class Printx : public AbstructFunction {
public:
	Printx(char *name) : AbstructFunction(name) {}

	Value *evaluate();
	bool isPrint() { return true; }
};

class Println : public AbstructFunction {
public:
	Println(char *name) : AbstructFunction(name) {}

	Value *evaluate();
	bool isPrint() { return true; }
};

class Int : public AbstructFunction {
public:
	Int(char *name) : AbstructFunction(name) {}

	Value *evaluate();
};

class chFloat : public AbstructFunction {
public:
	chFloat(char *name) : AbstructFunction(name) {}

	Value *evaluate();
};

class chString : public AbstructFunction {
public:
	chString(char *name) : AbstructFunction(name) {}

	Value *evaluate();
};

class chBool : public AbstructFunction {
public:
	chBool(char *name) : AbstructFunction(name) {}

	Value *evaluate();
};

class Len : public AbstructFunction {
public :
	Len(char *name) : AbstructFunction(name) {}

	Value *evaluate();
};

class Type : public AbstructFunction {
public:
	Type(char *name) : AbstructFunction(name) {}

	Value *evaluate();
};

class Input : public AbstructFunction {
public :
	Input(char *name) : AbstructFunction(name){}

	Value *evaluate();
};

class Time : public AbstructFunction {
public:
	Time(char *name) : AbstructFunction(name) {}

	Value *evaluate();
};

class Delay : public AbstructFunction {
public:
	Delay(char *name) : AbstructFunction(name) {}

	bool isPrint() { return true; }
	Value *evaluate();
};

class Millis : public AbstructFunction {
public:
	Millis(char *name) : AbstructFunction(name) {}

	Value *evaluate();
};

class Micros : public AbstructFunction {
public:
	Micros(char *name) : AbstructFunction(name) {}

	Value *evaluate();
};

class PrintF : public AbstructFunction {
public:
	PrintF(char *name) : AbstructFunction(name) {}

	Value *evaluate();
	bool isPrint() { return true; }
};

class Isalpha : public AbstructFunction {
public:
	Isalpha(char *name) : AbstructFunction(name) {}

	Value *evaluate();
};

class ReadFile : public AbstructFunction {
public:
	ReadFile(char *name) : AbstructFunction(name) {}

	Value *evaluate();
};

class WriteFile : public AbstructFunction {
public:
	WriteFile(char *name) : AbstructFunction(name) {}

	int getParamCount(){
		return 2;
	}

	Value *evaluate();
};

class AppendFile : public AbstructFunction {
public:
	AppendFile(char *name) : AbstructFunction(name) {}

	int getParamCount(){
		return 2;
	}

	Value *evaluate();
};

// ──────────────────────────────────
// ユーザ定義関数関連

class FunctionNode{
private:
	char			*_name;
	Function		*_function;
	FunctionNode	*_next;

public:
	FunctionNode(char *name, Function *f){
		_name = new char[strlen(name)+1];
		strcpy(_name, name);
		_function = f;
		_next = NULL;
	}

	~FunctionNode(){
#ifdef DEBUG
		printf("delete %s()\n", _name);
#endif
		delete _name;
		delete _function;
	}

	char *getName(){
		return _name;
	}

	Function *getFunction(){
		return _function;
	}

	void setFunction(Function *function){
		delete _function;
		_function = function;
	}

	FunctionNode *getNext(){
		return _next;
	}

	void setNext(FunctionNode *next){
		_next = next;
	}
};

// ユーザ定義関数表
class FunctionMap{
private:
	FunctionNode	*_top;
	FunctionNode	*_last;

public:
	FunctionMap(){
		_top = _last = NULL;
	}
	~FunctionMap(){
		clearFunctions();
	}

	void putFunction(char *name, Function *function);
	void addFunction(char *name, Function *function);
	void removeFunction(char *name);
	Function *getFunction(char *name);
	void clearFunctions();
	void printFunctions();
	FunctionNode *searchFunction(char *name);
};

class ReturnException {
public:
	Value	*_value;

	ReturnException(Value *value){
		_value = value;
	}
};

class Param{
private:
	char	*_name;
	Param	*_next;

public:
	 Param(char *name);
	~Param();

	char *getName(){
		return _name;
	}

	void setNext(Param *next){
		_next = next;
	}

	Param *getNext(){
		return _next;
	}
};

class UserFunction : public Function{
private:
	char		*_name;		// Function Name
	Param		*_paramTop;
	Param		*_paramLast;
	Sentence	*_sentence;

public:
	UserFunction(){
		_name = NULL;
		_paramTop = NULL;
		_paramLast = NULL;
		_sentence = NULL;
	}

	~UserFunction();

	void setName(char *name);

	char *getName(){
		return _name;
	}

	void setSentence(Sentence *sentence){
		_sentence = sentence;
	}

	void print();
	void printTree(int depth);
	void addParam(char *name);
	void setParamValue(int pos, Value *value);
	void clearParams() {}
	void deleteParams();
	int  getParamCount();
	Value *evaluate();
};

// ──────────────────────────────────

class FuncName : public Expression {
private:
	char		*_name;
	ExprNode	*_top;
	ExprNode	*_last;

public:
	FuncName(const char *name);
	~FuncName();

	bool isPrint();
	void print();
	void printTree(int s);
	void addParam(Expression *e);
	Value *evaluate();
};

// ──────────────────────────────────
// Global Functions

Function *searchSysFunc(const char *name);
void clearSysFuncs();

#endif
