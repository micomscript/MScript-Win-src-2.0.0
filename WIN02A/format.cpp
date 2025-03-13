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
 * format.cpp
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "format.h"

FormatTokenizer::FormatTokenizer(){
	_next = NULL;
}

int FormatTokenizer::nextToken(char *s){
	if (_next == NULL){
		return FTYPE_EOF;
	}
	if(_ch == '\0'){
		return FTYPE_EOF;
	}
	if (_ch == '%'){
		*s++ = '%';
		nextCh();
		if (_ch == '%'){
			*s = '\0';
			nextCh();
			return FTYPE_TEXT;
		}
		if (_ch == '-'){
			*s++ = '-';
			nextCh();
		}
		if (isdigit(_ch)){
			while (isdigit(_ch)){
				*s++ = _ch;
				nextCh();
			}
		}
		if (_ch == '.'){
			*s++ = '.';
			nextCh();
			while (isdigit(_ch)){
				*s++ = _ch;
				nextCh();
			}
		}
		if (_ch == 'l'){
			*s++ = 'l';
			nextCh();
		}
		if (_ch == 'c' || _ch == 's' || _ch == 'd' || _ch == 'u' ||
			_ch == 'o' || _ch == 'x' || _ch == 'X' || _ch == 'f' ||
			_ch == 'e' || _ch == 'g'){
			*s++ = _ch;
			*s = '\0';
			int c = _ch;
			nextCh();
			if (c == 'c' || c == 'd' || c == 'u' || c == 'o' || c == 'x' || c == 'X')
				return FTYPE_INT;
			if (c == 'f' || c == 'e' || c == 'g')
				return FTYPE_FLOAT;
			if (c == 's')
				return FTYPE_STRING;
		}
	}
	*s++ = _ch;
	nextCh();
	while ((_ch != '\0') && (_ch != '%')){
		*s++ = _ch;
		nextCh();
	}
	*s = '\0';
	return FTYPE_TEXT;
}
