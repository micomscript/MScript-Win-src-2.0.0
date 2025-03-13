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
 *	parser.h
 *	構文解析
 */

#ifndef __PARSER_H__
#define __PARSER_H__

class ExitException {
};

class Parser {
private:
	Tokenizer _tokenizer;
	Token     *_token;
	int		  _code;
	int		  _keyCode;

public:
	Parser() {
		_token 	= NULL;
		_code 	= T_EOL;
	}

	int			setScript(char *s);

	int  		nextToken();   					//字句を一つ読み取り、字句種別コードを返す
	void 		putBack();						//字句を戻す

	void		parse();						//構文解析

#ifdef T_ESP32
	Sentence	*file_list();
	Sentence	*save_file();
	Sentence	*show_file();
	Sentence	*load_file();
	Sentence	*remove_file();
	void		 execute_file(char *path);
#endif

	Sentence	*sentence();					// 文
	Sentence	*tree_sentence();				// tree
	Sentence	*return_sentence();				// return
	Sentence	*block_sentence();				// 複文
	Sentence	*var_sentence();				// var文
	Sentence	*func_sentence();				// ユーザ定義関数の作成
	Sentence	*undef_sentence();				// undef
	Sentence	*help_sentence();				// help
	Sentence	*if_sentence();					// if文
	Sentence	*while_sentence(bool ch);		// while文
	Sentence	*for_sentence();				// for文
	Sentence	*foreach_sentence();			// foreach文
	Sentence	*times_sentence();				// times文
	Sentence	*repeat_sentence();				// repeat文
	Sentence	*print_sentence();				// print宣言
	Sentence	*express_sentence();			// 式文

	Expression	*expression();					// 式
	Expression	*logic_or(Expression *e);		// 論理和
	Expression	*logic_and(Expression *e);		// 論理積
	Expression	*bit_or(Expression *e);			// ビット和
	Expression	*bit_exor(Expression *e);		// 排他的ビット和
	Expression	*bit_and(Expression *e);		// ビット積
	Expression	*equal_express(Expression *e);	// 等価式
	Expression	*relation_express(Expression *e);// 関係式
	Expression	*shift_express(Expression *e);	// シフト式
	Expression	*add_express(Expression *e);	// 加減式
	Expression	*term(Expression *e);			// 項
	Expression	*factor(Expression *e);			// 因子
	Expression	*particle();					// 粒子
	Expression	*atom();						// 原子
};

#endif
