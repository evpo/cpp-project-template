/**
 * SyntaxHighlighter
 * http://alexgorbatchev.com/SyntaxHighlighter
 *
 * SyntaxHighlighter is donationware. If you are using it, please donate.
 * http://alexgorbatchev.com/SyntaxHighlighter/donate.html
 *
 * @version
 * 3.0.83 (July 02 2010)
 * 
 * @copyright
 * Copyright (C) 2004-2010 Alex Gorbatchev.
 *
 * @license
 * Dual licensed under the MIT and GPL licenses.
 */
;(function()
  {
    // CommonJS
    typeof(require) != 'undefined' ? SyntaxHighlighter = require('shCore').SyntaxHighlighter : null;

    function Brush()
    {
      // Copyright 2006 Shin, YoungJin
      // Modified to exclude Microsoft types and include STL types, Andy Rushton
      var datatypes =
        'char bool short int long float double ' +
	'clock_t FILE fpos_t lconv jmp_buf mbstate_t size_t _stat time_t tm va_list wchar_t ' + 
        'signed unsigned void ' +
        'string bitset pair deque complex map set multimap multiset vector list shared_ptr ' +
        'iterator const_iterator this_iterator ' +
        'digraph triple foursome hash matrix ntree ' + 
        'simple_ptr_base simple_ptr simple_ptr_clone simple_ptr_nocopy ' +
        'smart_ptr_base smart_ptr smart_ptr_clone smart_ptr_nocopy ' +
        'constructor_copy clone_copy no_copy ' +
        'safe_iterator arc_iterator const_arc_iterator ' + 
        'null_dereference end_dereference wrong_object illegal_copy assert_failed';

      var keywords =
        'break case catch class const ' +
	'const_cast continue private public protected ' +
	'default delete deprecated do dynamic_cast ' +
	'else enum explicit extern if for friend goto inline ' +
	'mutable namespace new noinline noreturn nothrow operator ' +
	'register reinterpret_cast return ' +
	'sizeof static static_cast struct switch template this ' +
	'thread throw true false try typedef typeid typename union ' +
	'using virtual volatile while';
      
      var functions =
        'assert isalnum isalpha iscntrl isdigit isgraph islower isprint' +
	'ispunct isspace isupper isxdigit tolower toupper errno localeconv ' +
	'setlocale acos asin atan atan2 ceil cos cosh exp fabs floor fmod ' +
	'frexp ldexp log log10 modf pow sin sinh sqrt tan tanh jmp_buf ' +
	'longjmp setjmp raise signal sig_atomic_t va_arg va_end va_start ' +
	'clearerr fclose feof ferror fflush fgetc fgetpos fgets fopen ' +
	'fprintf fputc fputs fread freopen fscanf fseek fsetpos ftell ' +
	'fwrite getc getchar gets perror printf putc putchar puts remove ' +
	'rename rewind scanf setbuf setvbuf sprintf sscanf tmpfile tmpnam ' +
	'ungetc vfprintf vprintf vsprintf abort abs atexit atof atoi atol ' +
	'bsearch calloc div exit free getenv labs ldiv malloc mblen mbstowcs ' +
	'mbtowc qsort rand realloc srand strtod strtol strtoul system ' +
	'wcstombs wctomb memchr memcmp memcpy memmove memset strcat strchr ' +
	'strcmp strcoll strcpy strcspn strerror strlen strncat strncmp ' +
	'strncpy strpbrk strrchr strspn strstr strtok strxfrm asctime ' +
	'clock ctime difftime gmtime localtime mktime strftime time';

      this.regexList = [
	{ regex: SyntaxHighlighter.regexLib.singleLineCComments,	css: 'comments' },
	{ regex: SyntaxHighlighter.regexLib.multiLineCComments,	css: 'comments' },
	{ regex: SyntaxHighlighter.regexLib.doubleQuotedString,	css: 'string' },
	{ regex: SyntaxHighlighter.regexLib.singleQuotedString,	css: 'string' },
	{ regex: /^ *#.*/gm,					css: 'preprocessor' },
	{ regex: new RegExp(this.getKeywords(datatypes), 'gm'),	css: 'color1' },
	{ regex: new RegExp(this.getKeywords(functions), 'gm'),	css: 'functions' },
	{ regex: new RegExp(this.getKeywords(keywords), 'gm'),	css: 'keyword' }
      ];
    };

    Brush.prototype	= new SyntaxHighlighter.Highlighter();
    Brush.aliases	= ['cpp', 'c'];

    SyntaxHighlighter.brushes.Cpp = Brush;

    // CommonJS
    typeof(exports) != 'undefined' ? exports.Brush = Brush : null;
  })();
