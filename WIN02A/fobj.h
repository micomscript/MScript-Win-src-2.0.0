// fobj.h

class AbstructObject : public Value{
public:
	void	add(Value *val) {}
	void	sub(Value *val) {}
	void	mul(Value *val) {}
	void	div(Value *val) {}
	void	mod(Value *val) {}
	void	power(Value *val) {}
	void	increment(bool ch) {}
	void	decrement(bool ch) {}
	void	minus() {}
	void	printTree(int s);
	// Implement next method:
	// getInt(), getFloat(), getText(), getBool(), getType()
	// evaluate(), print()
};

class FileObject : public AbstructObject{
private:
	FILE			*_fp;
	char			*_fname;
	char			 _mode;
	unsigned char	 _state;	// 0:close 1:open

public:
	FileObject(char *name, char *mode);
	~FileObject();

	long getInt(){
		return _state;
	}

	double getFloat(){
		return (double)_state;
	}

	bool getBool(){
		return _state != 0;
	}

	char	*getText();
	char	*getType();
	Value	*evaluate();
	Value	*methodCall(char *name, List *params);
	void	 print();

	unsigned char status(){
		return _state;
	}
};

class FileOpen : public AbstructFunction{
public:
	FileOpen(char *name) : AbstructFunction(name){}

	int getParamCount(){
		return 2;
	}

	Value *evaluate();
};
