/* Morphology language - parse and code generation */

#include <stdio.h>
#include <malloc.h>

#define DEBUG 0
#define TURBO 1
#define GCC 2
#define COMPILER GCC

#define BEGINSY 1
#define ENDSY 2
#define EOFSY 0
#define INTCONST 3
#define IDENTSY 4
#define IFSY 5
#define LOOPSY 6
#define EXITSY 7
#define WHENSY 8
#define SEMI 9
#define LPAREN 10
#define RPAREN 11
#define ASGSY 12
#define THENSY 13
#define ELSESY 14
#define COLON 15
#define COMMA 16
#define STRINGCONST 17
#define PIXELCONST 18
#define LBRACK 19
#define RBRACK 20
#define DOTSY 21
#define DOSY 22
#define MESSAGESY 23
#define LBRACE 24
#define RBRACE 25
#define ADDSY 100
#define MINUSSY 101
#define DILSY 102
#define ERODSY 103
#define LTSY 104
#define GTSY 105
#define LESY 106
#define GESY 107
#define EQSY 108
#define MULSY 109
#define INPUTSY 110
#define OUTPUTSY 111
#define TRANSLATESY 112
#define INSY 113
#define DIVSY 114
#define XORSY 115
#define ANDSY 116
#define ORSY  117
#define NESY 118
#define COMPLEMENT 150
#define NEWSY 151
#define ISOSY 152

#define INTTYPE 3
#define PIXELTYPE 1
#define IMAGETYPE 2
#define STRINGTYPE 4

#define STSIZE 200

struct strec {
	char name[64];
	int offset;
	int type;
};

typedef struct strec * SYMBOL;

SYMBOL symtable[STSIZE];
int stnext = 0;

int unop1=150, unop2=152;
int firstop=100, lastop=118;
int loopstack[100];
int lstop = 0;
int ival = 0;
int sy;
char ch = ' ';
char eofch = '\0';
int line_no = 1;
char idval[64];
char strval[128];
FILE *bin;
FILE *infile;
FILE *include;
char incname[64];
int errors = 0;

int exptype, factype, vartype, termtype, genetype, gt1type;

int idchar (char ch);
SYMBOL sym_seek (char *name);
SYMBOL define (char *name);
void declarations ();
int typename (int s);
void program ();
void dostmt ();
void statement ();
void messagestmt();
void loopstmt ();
void ifstmt ();
void exitstmt ();
void asgstmt ();
int genlabel ();
int gena2 (char *op, int k1, int k2, int rest);
int genp2 (char *func, int k1, int k2, int rest);
int gent1 (int k, int k1t,  int op);
void emit_label (int L);
void lpush (int L);
int lpop ();
void emit_if1 (int k, int L);
void emit_goto (int L);
void emit_var (int k);
void gen_asg (int k1, int k1t,  int k2, int k2t);
int factor ();
int expression ();
int term ();
int nexttemp (int t);
void emit_trailer ();
void emit_header();
void gen_exit (int k, int L);
int gene (int k1, int k1t, int k2, int k2t,  int op);
int variable ();
void xerror (int errno);
void nextsy ();

/* ========= Scanner : Input characters, produce tokens ======== */

void nextch()
{
	int k = 0;

	if (ch == '\n') line_no += 1;
	if (ch != eofch)
	{
	  k = fscanf (infile, "%c", &ch);
	  if (k<1) ch = eofch;
	}
}

void skipeol ()
{
	int k;

	while (ch != '\n')
	{
	  k = fscanf (infile, "%c", &ch);
	  if (k < 1)
	  {
	    ch = eofch;
	    break;
	  }
	}
}

int idchar (char ch)
{
	if (ch>='a' && ch <= 'z') return 1;
	if (ch>='A' && ch <= 'Z') return 1;
	if (ch>='0' && ch<='9') return 1;
	if (ch == '_') return 1;
	return 0;
}

/*      Set global SY to the next symbol in the input stream */
void nextsy()
{
	int i=0;

	if (sy == EOFSY) return;

	while ( ch==' ' || ch == '\t' || ch == '\n' || ch ==  '\014')
	  nextch();
	if (ch == eofch)
	{
	  sy = EOFSY;
	  return;
	}

	if (ch >='0' && ch <= '9')
	{
	  sy = INTCONST; ival = 0;
	  while (ch>='0' && ch <= '9')
	  {
	    ival = ival*10 + ch-'0';
	    nextch();
	  }
	  return;
	}
	else if ( (ch>='a' && ch<='z') || (ch>='A' && ch<='Z') || ch == '_' )
	{
	    i = 0;
	    while (idchar(ch))
	    {
	      if (i<64)
		idval[i++] = ch;
	      nextch();
	    }
	    idval[i] = '\0';
	    if (strcmp(idval, "begin")==0) { sy = BEGINSY; return; }
	    if (strcmp(idval, "end")==0) { sy = ENDSY; return; }
	    if (strcmp(idval, "if")==0) { sy = IFSY; return; }
	    if (strcmp(idval, "else")==0) { sy = ELSESY; return; }
	    if (strcmp(idval, "then")==0) { sy = THENSY; return; }
	    if (strcmp(idval, "when")==0) { sy = WHENSY; return; }
	    if (strcmp(idval, "exit")==0) { sy = EXITSY; return; }
	    if (strcmp(idval, "loop")==0) { sy = LOOPSY; return; }
	    if (strcmp(idval, "begin")==0) { sy = BEGINSY; return; }
	    if (strcmp(idval, "do") == 0) { sy = DOSY; return; }
	    if (strcmp(idval, "message") == 0) { sy = MESSAGESY; return; }
	    sy = IDENTSY;
	    return;
	} else if (ch == '"')
	{
	  nextch(); i = 0;
	  while (ch != '"')
	  {
	    if (i<128) strval[i++] = ch;
	    nextch();
	  }
	  nextch();
	  strval[i] = '\0';
	  sy = STRINGCONST;
	  return;
	}
	else {
	  switch (ch)
	  {
case '=':       sy = EQSY; nextch(); break;
case ',':       sy = COMMA; nextch(); break;
case '*':       sy = MULSY; nextch(); break;
case '@':       sy = INSY; nextch(); break;
case '{':       sy = LBRACE; nextch(); break;
case '}':       sy = RBRACE; nextch(); break;
case '[':       sy = LBRACK; nextch(); break;
case ']':       sy = RBRACK; nextch(); break;
case '.':       sy = DOTSY; nextch(); break;
case '#':       sy = ISOSY; nextch(); break;
case '&':       sy = ANDSY; nextch(); break;
case '|':       sy = ORSY; nextch(); break;
case '/':       nextch();
		if (ch == '/')          /* Comment */
		{
		  skipeol();
		  nextsy();
		  return;
		}
		sy = DIVSY;
		break;
case ':':       nextch();
		if (ch == '=')
		{
		  sy = ASGSY;
		  nextch();
		} else sy = COLON;
		break;
case '<':       nextch();
		if (ch == '=')
		{ sy = LESY; nextch(); }
		else if (ch == '<')
		{ sy = INPUTSY; nextch(); }
		else if (ch == '-') { sy = TRANSLATESY; nextch(); break; }
		else if (ch == '>') { sy = NESY; nextch(); break; }
		else sy = LTSY;
		break;
case '>':       nextch();
		if (ch == '=')
		{ sy = GESY; nextch(); }
		else if (ch == '>')
		{ sy = OUTPUTSY; nextch(); }
		else sy = GTSY;
		break;
case '+':       nextch();
		if (ch == '+') { sy = DILSY; nextch(); }
		else sy = ADDSY;
		break;
case '-':       nextch();
		if (ch == '-') { sy = ERODSY; nextch(); }
		else if (ch == '>') { sy = TRANSLATESY; nextch(); break; }
		else sy = MINUSSY;
		break;
case ';':       sy = SEMI;
		nextch();
		break;
case ')':       sy = RPAREN; nextch(); break;
case '(':       sy = LPAREN; nextch(); break;
case '~':       sy = COMPLEMENT; nextch(); break;
case '!':       sy = NEWSY; nextch(); break;
default:        xerror(0);
	  }
	}
}

/* =========== Symbol table: store and retrieve names ============ */

SYMBOL define (char *name)
{
	static int next = 0;
	SYMBOL s;

	if (DEBUG) printf ("Defining '%s': ", name);
	s = sym_seek (idval);
	if (s)
	{
		xerror(12);
		return s;
	}

	if (stnext < STSIZE)
	{
	  symtable[stnext] = (SYMBOL)malloc (sizeof(struct strec));
	  if (symtable[stnext]==0)
	  {
		printf ("PANIC: Out of storage.\n");
		exit (2);
	  }

	  strcpy (symtable[stnext]->name, name);
	  symtable[stnext]->offset = next++;
	  if (DEBUG) printf (" location %d\n", next-1);
	  return symtable[stnext++];
	}
	xerror (10);
	exit(3);
	return 0;
}

SYMBOL sym_seek (char *name)
{
	int i;

	if (DEBUG) printf ("\nLooking for %s ...\n", name);
	for (i=0; i<stnext; i++)
	{
	  if (DEBUG) printf ("'%s' is %d\n", symtable[i]->name,
			symtable[i]->offset);
	  if (strcmp(symtable[i]->name, name) == 0)
		return symtable[i];
	}
	if (DEBUG) printf ("NOT FOUND.\n");
	return 0;
}

void emit_header()
{
	fprintf (bin, "#include <stdio.h>\n#include \"max.h\"\n");
	fprintf (bin, "#include \"%s\"\n", incname);
	fprintf (bin, "main(int argc, char **argv)\n{\n");
}

void emit_trailer ()
{
	fprintf (bin, "\n}\n");
	fclose (bin);
}

void program ()
{
	emit_header ();         /* C prolog */
	declarations();         /* Declare variables */
	fprintf (bin, "arg = argv;\n\tmaxargs = argc;\n\t");
	statement ();           /* Parse a STATEMENT */
	emit_trailer ();        /* C code trailer */
}

int typename (int s)
{
	if (s != IDENTSY) return -1;
	if (strcmp(idval, "pixel")==0) return 1;
	if (strcmp(idval, "image")==0) return 2;
	if (strcmp(idval, "int")==0) return 3;
	return -1;
}

void declarations ()
{
	int i, t;
	SYMBOL s;
	static char *typestring[] = {"void *", "PIXEL", "IMAGE", "int"};

	t = typename(sy);
	if (t<0) return;

	do
	{
	  if (t > 0)
	  {
	    fprintf (bin, "%s ", typestring[t]);
	    nextsy();
	    while (sy == IDENTSY)
	    {
	      s = define (idval);
	      s->type = t;
	      emit_var (s->offset);
	      fprintf (bin, "=0");
	      nextsy();
	      if (sy == COMMA)
	      {
		nextsy();
		fprintf (bin, ", ");
	      }
		else if (sy != SEMI) xerror (9);
	    }
	  }
	  if (sy == SEMI) nextsy();
	   else xerror (9);
	  fprintf (bin, ";\n\t");
	  t = typename(sy);
	} while (t>0);
	fprintf (bin, "\n\t");
}

void statement ()
{
	if (sy == BEGINSY)
	{
	  nextsy();
	  do
	  {
	    statement ();
	    if (sy == SEMI) nextsy();
	      else if (sy != ENDSY) xerror(4);
	  } while ( (sy != ENDSY) && (sy != EOFSY) );
	  if (sy == ENDSY) nextsy();
	} else if (sy == IFSY) ifstmt ();
	else if (sy == EXITSY) exitstmt();
	else if (sy == IDENTSY) asgstmt ();
	else if (sy == LOOPSY) loopstmt ();
	else if (sy == DOSY) dostmt();
	else if (sy == MESSAGESY) messagestmt();
	else xerror (1);
}

void dostmt ()
{
	nextsy();
	expression();
}

void messagestmt()
{
	int k;

	nextsy();
	if ( (sy == ENDSY) || (sy == SEMI) || (sy == ELSESY))
	{
	  fprintf (bin, "printf (\"\\n\");\n\t");
	  return;
	}

	k = expression();
	if (exptype == IMAGETYPE)
	{
	  fprintf (bin, "print_se (");
	  emit_var (k);
	  fprintf (bin, ");\n\t");
	} else if (exptype == INTTYPE)
	{
	  fprintf (bin, "printf (\"%%d \", ", 'd');
	  emit_var (k);
	  fprintf (bin, ");\n\t");
	} else if (exptype == PIXELTYPE)
	{
	  fprintf (bin, "printf (\"[%%d, %%d] \",");
	  emit_var(k);
	  fprintf (bin, "->row, ");
	  emit_var(k);
	  fprintf (bin, "->col);\n\t");
	} else if (exptype == STRINGTYPE)
	{
	  fprintf (bin, "printf (\"%s \");\n\t", strval);
	}
}

void loopstmt ()
{
	int ltop;

	ltop = genlabel ();
	lpush (genlabel());
	emit_label (ltop);
	nextsy();

	while ( (sy != ENDSY) && (sy != EOFSY) )
	{
		statement ();
		if (sy == SEMI) nextsy();
		else if (sy != ENDSY) xerror(4);
	}

	if (sy == ENDSY) nextsy();
	 else xerror(4);
	emit_goto ( ltop );
	emit_label (lpop());
}

void ifstmt ()
{
	int k, l1, l2;

	nextsy();
	if (sy != LPAREN) xerror (2);
	 else nextsy();
	k = expression ();
	if (sy != RPAREN) xerror (3);
	 else nextsy();
	if (sy != THENSY) xerror (8);
	 else nextsy();
	
	l1 = genlabel ();
	emit_if1 (k, l1);
	statement ();

	if (sy == ELSESY)
	{
	  l2 = genlabel ();
	  emit_goto (l2);
	  nextsy();
	  emit_label (l1);
	  statement ();
	  emit_label (l2);
	} else
	  emit_label (l1);
}

void exitstmt ()
{
	int k;

	int level = 1;
	nextsy ();
	if (sy == INTCONST)
	{
	  level = ival;
	  nextsy();
	} 

	if (sy == WHENSY) nextsy ();
	  else xerror (6);
	k = expression ();
	gen_exit (k, loopstack[lstop-level]);
}

void asgstmt ()
{
	int k1, k2;
	int k1t, k2t;

	k1 = variable ();
	k1t = vartype;
	if (sy == ASGSY) nextsy ();
	 else xerror(5);
	k2 = expression ();
	k2t = exptype;
	gen_asg (k1, k1t,  k2, k2t);
}

int variable ()
{
	SYMBOL s;

	s = sym_seek (idval);
	if (s == 0)
	{
	  printf ("IDENTIFIER '%s': ", idval);
	  xerror(11);
	  s = define (idval);
	  s->type = 2;
	}
	nextsy();
	vartype = s->type;
	return s->offset;
}

void gen_exit (int k, int L)
{
	fprintf (bin, "if (");
	emit_var (k);
	fprintf (bin, ") goto L%d;\n\t", L);
}

int genlabel ()
{
	static int next = 0;

	return next++;
}

void emit_label (int L)
{
	fprintf (bin, "\nL%d:\n\t", L);
}

void lpush (int L)
{
	loopstack[lstop++] = L;
}

int lpop ()
{
	return loopstack[--lstop];
}

void emit_goto (int L)
{
	fprintf (bin, "goto L%d;\n\t", L);
}

void emit_if1 (int k, int L)
{
	fprintf (bin, "if ( ");
	emit_var (k);
	fprintf (bin, " == 0 ) goto L%d;\n\t", L);
}

void emit_var (int k)
{
	if (k<0) fprintf (bin, "T%d", -k);
	else fprintf (bin, "V%d", k);
}

void gen_asg (int k1, int k1t,  int k2, int k2t)
{
	if (k1 == k2) return;
	if (k1t == IMAGETYPE && k2t == IMAGETYPE)
	{
	  fprintf (bin, "CopyVarImage ( &");
	  emit_var (k1);
	  fprintf (bin, ", &");
	  emit_var (k2);
	  fprintf (bin, ");\n\t");
	} else if (k1t == PIXELTYPE)
	{
	  fprintf (bin, "CopyVarPix ( &");
	  emit_var (k1);
	  fprintf (bin, ", &");
	  emit_var (k2);
	  fprintf (bin, ");\n\t");
	} else if (k1t == INTTYPE)
	{
	  emit_var (k1);
	  fprintf (bin, " = ");
	  emit_var (k2);
	  fprintf (bin, ";\n\t");
	}
}

int factor ()
{
	int k1, k2, t, attr;
	int k1t, k2t;

/* ( EXPRESSION ) */
	if (sy == LPAREN)
	{
	  nextsy ();
	  k1 = expression ();
	  factype = exptype;
	  if (sy == RPAREN) nextsy();
	    else xerror (7);
	} 

/* [i, j] */
	else if (sy == LBRACK)
	{                               /* Pixel generator */
	  nextsy();
	  k1 = expression ();
	  k1t = exptype;
	  if (sy == COMMA) nextsy();
	   else xerror (19);
	  k2 = expression ();
	  k2t = exptype;
	  if (sy == RBRACK) nextsy();
	   else xerror (19);
	  t = nexttemp(PIXELTYPE);
	  if (k1t == k2t && (k1t == INTTYPE))
	  {
	    emit_var (t);
	    fprintf (bin, " = Pixel (");
	    emit_var (k1);
	    fprintf (bin, ", ");
	    emit_var (k2);
	    fprintf (bin, ");\n\t");
	  } else xerror (20);
	  factype = PIXELTYPE;
	  return t;
	}

/* Identifier */
	else if (sy == IDENTSY)
	{
	   k1 = variable ();
	   factype = vartype;
	  if (sy == DOTSY)              /* Attribute? */
	  {
		nextsy();
		if (sy == IDENTSY)
		{
		  attr = 0;
		  if (strcmp(idval, "cols")==0) attr = 1;
		  else if (strcmp(idval, "rows")==0) attr = 2;
		  else if (strcmp(idval, "origin_r")==0) attr = 3;
		  else if (strcmp(idval, "origin_c")==0) attr = 4;
		  else if (strcmp(idval, "row")==0) attr = 5;
		  else if (strcmp(idval, "column")==0) attr = 6;
		  else if (strcmp(idval, "col")==0) attr = 6;
		  else xerror (23);
		  t = nexttemp (INTTYPE);
		  emit_var (t);
		  fprintf (bin, " = Attribute(%d, (IMAGE)", attr);
		  emit_var (k1);
		  fprintf (bin, ");\n\t");
		  factype = INTTYPE;
		  k1 = t;
		} else xerror (23);
		nextsy();
	  }
	}

/* Integer Constant */
	else if (sy == INTCONST) 
	{
	  k1 = nexttemp (INTTYPE);
	  fprintf (bin, "T%d = %d;\n\t", -k1, ival);
	  nextsy();
	  factype = INTTYPE;

/* String constant: "Hi There" */
	} else if (sy == STRINGCONST)
	{
		factype = STRINGTYPE;
		nextsy();
		k1 = -1;
	}
	return k1;
}

int expression ()
{
	int k1, k2, op;
	int k1t, k2t;

	k1 = term ();
	k1t = termtype;
	while (sy>=firstop && sy <= lastop)
	{
	  op = sy; nextsy();
	  k2 = term();
	  k2t = termtype;
	  k1 = gene (k1, k1t, k2, k2t, op);
	  k1t = genetype;
	}
	exptype = k1t;
	return k1;
}

int term ()
{
	int op, k1, k2, t;
	int k1t;

/* A Unary operator */
	if (sy >= unop1 && sy <= unop2 || sy == MINUSSY)
	{
	  op = sy; nextsy();
	  k1 = factor ();
	  k1t = factype;
	  k1 = gent1 (k1, k1t, op);
	  termtype = gt1type;
	} else if (sy == LBRACE)

/* An image constructor: Two pixels and a string constant */
	{
	  nextsy();
	  k1 = factor ();
	  t = nexttemp (IMAGETYPE);
	  if(factype != PIXELTYPE) xerror (99);
	  if (sy == COMMA) nextsy();
	    else xerror(99);
	  k2 = factor ();
	  if (factype != PIXELTYPE) xerror(99);
	  if (sy == COMMA) nextsy();
	    else xerror(99);
	  factor ();
	  if (factype != STRINGTYPE) xerror(99);
	  emit_var (t);
	  fprintf (bin, " = ImageGen (");
	  emit_var (k1);
	  fprintf (bin, ", ");
	  emit_var (k2);
	  fprintf (bin, ", \"%s\");\n\t", strval);
	  termtype = IMAGETYPE;
	  if (sy == RBRACE) nextsy();
	    else xerror (99);
	  return t;
	} else 

/* A simple factor */
	{
	  k1 = factor ();
	  termtype = factype;
	}
	return k1;
}

int gent1 (int k, int k1t,  int op)
{
	int t;

	switch (op)
	{
case COMPLEMENT:
		if (k1t == IMAGETYPE)
		{
		  t = nexttemp(IMAGETYPE);
		  emit_var (t);
		  fprintf (bin, " = Complement (");
		  emit_var (k);
		  fprintf (bin, ");\n\t");
		  gt1type = IMAGETYPE;
		  return t;
		} else xerror(21);
		break;
case MINUSSY:   if (k1t == INTTYPE)
		{
		  t = nexttemp (INTTYPE);
		  emit_var (t);
		  fprintf (bin, " = -");
		  emit_var (k);
		  fprintf (bin, ";\n\t");
		  gt1type = INTTYPE;
		  return t;
		} else xerror (21);
		break;
case NEWSY:
		if (k1t == IMAGETYPE)
		{
		  t = nexttemp(IMAGETYPE);
		  emit_var (t);
		  fprintf (bin, " = NewImage (");
		  emit_var (k);
		  fprintf (bin, ");\n\t");
		  gt1type = IMAGETYPE;
		  return t;
		} else xerror (21);
		break;
case ISOSY:
		if (k1t == IMAGETYPE)
		{
		  t = nexttemp(INTTYPE);
		  emit_var (t);
		  fprintf (bin, " = Isolated (");
		  emit_var (k);
		  fprintf (bin, ");\n\t");
		  gt1type = INTTYPE;
		  return t;
		} else xerror (21);
		break;
default:        xerror(16);
	}
	gt1type = IMAGETYPE;
	return k;
}

int nexttemp (int t)
{
	static int next = -1;

	if (t == IMAGETYPE) fprintf (include, "IMAGE ");
	else if (t == INTTYPE) fprintf (include, "int ");
	else if (t == PIXELTYPE) fprintf (include, "PIXEL ");
	fprintf (include, "T%d=0;\n", -next);
	return next--;
}

/*      Generate the code for K1 OP K2          */
int gene (int k1, int k1t, int k2, int k2t,  int op)
{
	int t, k;

	switch (op)
	{
case DILSY:     if (k1t == k2t && k1t == IMAGETYPE)
		{
		  t = genp2 ("Dilate", k1, k2, IMAGETYPE);
		  genetype = IMAGETYPE;
		  return t;
		}
		else xerror(13);
		break;

case ERODSY:    if (k1t == k2t && k1t == IMAGETYPE)
		{
		  t = genp2 ("Erode", k1, k2, IMAGETYPE);
		  genetype = IMAGETYPE;
		  return t;
		} else xerror (14);
		break;

case EQSY:      if (k1t == k2t && k1t == IMAGETYPE)
		  t = genp2 ("ImCompare", k1, k2, INTTYPE);
		else if (k1t == k2t &&  k1t == PIXELTYPE)
		  t = genp2 ("PixelValue", k1, k2, INTTYPE);
		else if (k1t == IMAGETYPE && k2t == INTTYPE)
		  t = genp2 ("ImValue", k1, k2, INTTYPE);
		else if (k1t == k2t &&  k1t == INTTYPE)
		  t = gena2 ("==", k1, k2, INTTYPE);
		else xerror (15);
		genetype = INTTYPE;
		return t;

case NESY:      if (k1t == k2t && k1t == IMAGETYPE)
		  t = genp2 ("1-ImCompare", k1, k2, INTTYPE);
		else if (k1t == k2t &&  k1t == PIXELTYPE)
		  t = genp2 ("1-PixelValue", k1, k2, INTTYPE);
		else if (k1t == IMAGETYPE && k2t == INTTYPE)
		  t = genp2 ("1-ImValue", k1, k2, INTTYPE);
		else if (k1t == k2t &&  k1t == INTTYPE)
		  t = gena2 ("!=", k1, k2, INTTYPE);
		else xerror (15);
		genetype = INTTYPE;
		return t;

case LTSY:
		if (k1t==k2t && k1t == IMAGETYPE)
		  t = genp2 ("PSubSet", k2, k1, INTTYPE);
		else if (k1t==k2t && k1t == INTTYPE)
		  t = gena2 ("<", k1, k2, INTTYPE);
		else xerror (15);
		genetype = INTTYPE;
		return t;

case GTSY:
		if (k1t==k2t && k1t == IMAGETYPE)
		  t = genp2 ("PSubSet", k1, k2, INTTYPE);
		else if (k1t==k2t && k1t == INTTYPE)
		  t = gena2 (">", k1, k2, INTTYPE);
		else xerror (15);
		genetype = INTTYPE;
		return t;

case ANDSY:     if (k1t == k2t && k1t == INTTYPE)
		  t = gena2 ("&&", k1, k2, INTTYPE);
		else xerror (15);
		genetype = INTTYPE;
		return t;

case ORSY:      if (k1t==k2t && k1t==INTTYPE)
		  t = gena2 ("||", k1, k2, INTTYPE);
		else xerror (15);
		genetype = INTTYPE;
		return t;

case LESY:      if (k1t == k2t && k1t == IMAGETYPE)
		  t = genp2 ("SubSet", k1, k2, INTTYPE);
		else if (k1t == INTTYPE && k2t == INTTYPE)
		  t = gena2 ("<=", k1, k2, INTTYPE);
		else xerror (15);
		genetype = INTTYPE;
		return t;

case GESY:      if (k1t == k2t && k1t == IMAGETYPE)
		  t = genp2 ("SubSet", k2, k1, INTTYPE);
		else if (k1t==INTTYPE && k2t == INTTYPE)
		  t = gena2 (">=", k1, k2, INTTYPE);
		else xerror (15);
		genetype = INTTYPE;
		return t;

case MINUSSY:   if (k1t == k2t && k1t == IMAGETYPE)
		  t = genp2 ("Difference", k1, k2, IMAGETYPE);
		else if (k1t == k2t && k1t == INTTYPE)
		  t = gena2 ("-", k1, k2, INTTYPE);
		else if (k1t == k2t && k1t == PIXELTYPE)
		  t = genp2 ("PixDif", k1, k2, PIXELTYPE);
		else xerror (15);
		genetype = k2t;
		return t;

case ADDSY:     if (k1t == k2t && k1t == IMAGETYPE)
		  t = genp2 ("Union", k1, k2, IMAGETYPE);
		else if (k1t == k2t && k1t == INTTYPE)
		  t = gena2 ("+", k1, k2, INTTYPE);
		else if (k1t == k2t && k1t == PIXELTYPE)
		  t = genp2 ("PixAdd", k1, k2, PIXELTYPE);
		else if (k1t == IMAGETYPE && k2t == PIXELTYPE)
		{
		  t = genp2 ("SetAPixel", k1, k2, IMAGETYPE);
		  k2t = IMAGETYPE;
		} else if (k1t == PIXELTYPE &&  k2t == IMAGETYPE)
		  t = genp2 ("SetAPixel", k2, k1, IMAGETYPE);
		else xerror (15);
		genetype = k2t;
		return t;

case MULSY:     if (k1t == k2t && k1t == IMAGETYPE)
		  t = genp2 ("Intersection", k1, k2, IMAGETYPE);
		else if (k1t == k2t && k1t == INTTYPE)
		  t = gena2 ("*", k1, k2, INTTYPE);
		else xerror (15);
		genetype = k1t;
		return t;

case INPUTSY:
		if (k1t == IMAGETYPE && k2t == STRINGTYPE)
		{
		  t = nexttemp(IMAGETYPE);
		  emit_var(k1);
		  fprintf (bin, "= Input_PBM (\"%s\");\n\t", strval);
		  fprintf (bin, "if (");
		  emit_var(k1);
		  fprintf (bin, "==0) max_abort(1, \"%s\");\n\t", strval);
		  genetype = IMAGETYPE;
		  return k1;
		} else if (k1t == INTTYPE && k2t == STRINGTYPE)
		{
		  fprintf (bin, "inint (&");
		  emit_var(k1);
		  fprintf (bin, ", \"%s\");\n\t", strval);
		  genetype = INTTYPE;
		  return k1;
		} else if (k1t == PIXELTYPE && k2t == STRINGTYPE)
		{
		  fprintf (bin, "inpix (");
		  emit_var (k1);
		  fprintf (bin, ", \"%s\");\n\t", strval);
		  genetype = PIXELTYPE;
		  return k1;
		} else xerror (15);
		 break;

case OUTPUTSY:
		if (k1t == IMAGETYPE && k2t == STRINGTYPE)
		{
		  fprintf (bin, "Output_PBM (");
		  emit_var (k1);
		  fprintf (bin, ", \"%s\");\n\t", strval);
		  genetype = IMAGETYPE;
		  return k1;
		} else if (k1t == INTTYPE && k2t == STRINGTYPE)
		{
		  fprintf (bin, "Outint (");
		  emit_var(k1);
		  fprintf (bin, ", \"%s\");\n\t", strval);
		  genetype = INTTYPE;
		  return k1;
		} else if (k2t == STRINGTYPE && k1t == PIXELTYPE)
		{
		  fprintf (bin, "Outpix(");
		  emit_var(k1);
		  fprintf (bin, ", \"%s\");\n\t", strval);
		  genetype = PIXELTYPE;
		  return k1;
		} else xerror (15);
		 break;
case INSY:      if (k1t == PIXELTYPE && k2t==IMAGETYPE)
		{
		  t = genp2 ("Member", k1, k2, INTTYPE);
		  genetype = INTTYPE;
		  return t;
		} else xerror (15);
		break;

case TRANSLATESY:
		if (k1t == PIXELTYPE && k2t == IMAGETYPE)
		{
		  k = k1; k1 = k2; k2 = k;
		  k = k1t; k1t = k2t; k2t = k;
		}
		if (k2t == PIXELTYPE && k1t == IMAGETYPE)
		{
		  t = genp2 ("Translate", k1, k2, IMAGETYPE);
		  genetype = IMAGETYPE;
		  return t;
		} else xerror (15);
		break;

	}
	return 0;
}

/* Generate a function call with 2 args: temp = FUNC(k1, k2); */
int genp2 (char *func, int k1, int k2, int rest)
{
	int t;

	t = nexttemp(rest);
	emit_var(t);
	fprintf (bin, "= %s (", func);
	emit_var (k1);
	fprintf (bin, ", ");
	emit_var (k2);
	fprintf (bin, ");\n\t");
	return t;
}

/* Generate an assignment statement: TEMP = VAR OP VAR; */
int gena2 (char *op, int k1, int k2, int rest)
{
	int t;

	t = nexttemp(rest);
	emit_var(t);
	fprintf (bin, " = (");
	emit_var (k1);
	fprintf (bin, " %s ", op);
	emit_var (k2);
	fprintf (bin, ");\n\t");
	return t;
}

void xerror (int errno)
{
	printf ("*** MAX error number %d Line %d ***\n", errno, line_no);
	errors++;
	switch (errno)
	{
case 0:         printf ("Bad character seen: '%c'\n", ch);
		break;
case 1:         printf ("Statement is missing a semicolon.\n");
		break;
case 2:         printf ("Missing or misplaced ')'.\n");
		break;
case 3:         printf ("Missing or misplaced '('.\n");
		break;
case 4:         printf ("Missing or misplaced ';'.\n");
		break;
case 5:         printf ("':=' expected.\n");
		break;
case 6:         printf ("'WHEN' expected.\n");
		break;
case 7:         printf ("')' missing in factor.\n");
		break;
case 8:         printf ("'THEN' expected.\n");
		break;
case 9:         printf ("Error in declaration. Misplaced ','?\n");
		break;
case 10:        printf ("Symbol table out of space.\n");
		break;
case 11:        printf ("Undefined variable.\n");
		break;
case 12:        printf ("Duplicate definition: %s\n", idval);
		break;
case 13:        printf ("Types in DILATE (++) must be IMAGE.\n");
		break;
case 14:        printf ("Types in ERODE (--) must be IMAGE.\n");
		break;
case 15:        printf ("Type conflict in expression.\n");
		break;
case 16:        printf ("No such operator!\n");
		break;
case 19:        printf ("Syntax error in pixel generator.\n");
		break;
case 20:        printf ("Type error in pixel generator: must be ints.\n");
		break;
case 21:        printf ("Type error in unary expression.\n");
		break;
case 23:        printf ("Illegal attribute name.\n");
		break;
default:        printf ("No message.\n");
	}
}

int main(int argc, char *argv[])
{
	char compile [64], binname[64], cname[64];
	char aa[4][64];
	char *p;

	if (argc < 2)
	{
		printf ("Usage: MAX infile  \n");
		exit (1);
	}

	infile = fopen (argv[1], "r");
	if (infile == NULL)
	{
		printf ("Can't open MAX input file '%s'.\n", argv[1]);
		exit(1);
	}
	strcpy (binname, argv[1]);
	p = &(binname[strlen(binname)-4]);
	if (strcmp(p, ".max")==0)
	  p[0] = '\0';
	else
	{
	  printf ("Input file name must end in '.max'\n");
	  exit (0);
	}

	strcpy (incname, binname);
	strcpy (cname, binname);
	strcat (cname, ".c");
	strcat (incname, ".h");
	include = fopen (incname, "w");


	bin = fopen (cname, "w");
	if (bin == NULL)
	{
		printf ("Can't open MAX code file '%s'.\n", argv[2]);
		exit(1);
	}

	printf ("\nC code will be in %s object file will be %s\n", 
		cname, binname);
	if (infile == NULL)
	{
		printf ("Can't open MAX include file '%s'.\n", incname);
		exit(1);
	}

	sy = ELSESY;
	nextsy();
	program();
	if (errors) 
	  fprintf (stderr, "*** There were %d errors! ***\n", errors);
	else fprintf (stderr, "MAX compilation complete.\n");

	fclose (include);
	fclose (bin);
	fclose (infile);

	if (errors == 0)
	{
	  if (COMPILER == TURBO)
	  {
	    sprintf (compile,"tcc  -mh %s maxlib.c", cname);
	    system (compile);
	    fprintf (stderr, "C compilation complete.\n");
	  } else if (COMPILER == GCC)
	  {
	    printf ("When using the GCC compiler, you must compile the\n");
	    printf ("program from the command line.\n");
	    printf ("\n     gcc -o %s %s maxlib.c\n", binname, cname);
	    printf ("followed by:\n");
	    printf ("     coff2exe -s /gnu/bin/go32.exe %s", binname);
	  } else printf ("No C compilation - source is %s\n", cname);
	}
	return 0;
}
