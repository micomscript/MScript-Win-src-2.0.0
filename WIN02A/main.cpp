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
 * main 
 */

#include<iostream>
#include<string.h>
#include "common.h"
#include "expression.h"
#include "logic.h"
#include "sentence.h"
#include "token.h"
#include "tokenizer.h"
#include "function.h"
#include "parser.h"
#include "varmap.h"

int _system_mode; // 0:対話モード	1:スクリプトモード

int main(int argc, char **argv) {
	Parser parser;
	_system_mode = 0;			// 対話モード

	if(argc >= 2) {
		if(parser.setScript(argv[1]) != 0) {
			printf("Failed to open file (%s)\n",argv[1]);
			return 1;
		} else {
			_system_mode = 1;	// スクリプトモード
		}
	}

	if (_system_mode == 0){
		printf("MScript 1.0.0-beta\n\n");
	}

	parser.parse();

	return 0;
}
