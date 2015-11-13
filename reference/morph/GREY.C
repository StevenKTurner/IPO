/* Grey level morphological operators */

#define MAX
#include "maxg.h"
FILE *files[20];
char *filenames[64];
int nfiles = 0;

IMAGE Dilate (IMAGE a, IMAGE se)
{
	int i,j,ii,jj,k,n,m;
	IMAGE b;

	b = newimage (a->info->nr, a->info->nc);
	for (i=0; i<a->info->nr; i++)
	  for (j=0; j<a->info->nc; j++)
	  {
	    k = -10000;
	    for (ii=0; ii<se->info->nr; ii++)
	      for (jj=0; jj<se->info->nc; jj++)
	      {
		if (range(a, i+ii-se->info->oi, j+jj-se->info->oj) == 0)
		   continue;
		n = se->data[ii][jj] + 
		     a->data[i+(ii-se->info->oi)][j+(jj-se->info->oj)];
		if (n > k) k = n;
	      }
	    if (k > 255)
	      b->data[i][j] = 255;
	    else if (k<0) b->data[i][j] = 0;
	    else b->data[i][j] = k;
	  }

	return b;
}

IMAGE Erode (IMAGE a, IMAGE se)
{
	int i,j,ii,jj,k,n,m;
	IMAGE b;

	b = newimage (a->info->nr, a->info->nc);
	for (i=0; i<a->info->nr; i++)
	  for (j=0; j<a->info->nc; j++)
	  {
	    k = 10000;
	    for (ii=0; ii<se->info->nr; ii++)
	      for (jj=0; jj<se->info->nc; jj++)
	      {
		if (range(a, i+ii-se->info->oi, j+jj-se->info->oj) == 0)
		   continue;
		n =  a->data[i+(ii-se->info->oi)][j+(jj-se->info->oj)] - 
			se->data[ii][jj];
		if (n < k) k = n;
	      }
	    if (k > 255)
	      b->data[i][j] = k;
	    else if (k<0) b->data[i][j] = 0;
	    else b->data[i][j] = k;
	  }

	return b;
}

IMAGE Gradient (IMAGE a, IMAGE se)
{
	int i, j, k;
	IMAGE c=0;

	CopyVarImage (&c, &a);
	Dilate (a, se);
	Erode (a, se);
	for (i=0; i<a->info->nr; i++)
	  for (j=0; j<a->info->nc; j++)
	  {
	    k = a->data[i][j] - c->data[i][j];
	    if (k<0) k = 0;
	    else if (k>255) k = 255;
	    a->data[i][j] = k;
	  }
	freeimage (c);
	return a;
}

IMAGE TopHat (IMAGE a, IMAGE se)
{
	int i,j,k;
	IMAGE c=0, b=0;

	CopyVarImage (&c, &a);
	c = Erode (a, se);         /* Open ... */
	b = Dilate (c, se);
	CopyVarImage (&c, &a);   
	for (i=0; i<a->info->nr; i++)
	  for (j=0; j<a->info->nc; j++)
	  {
	    k = c->data[i][j] - b->data[i][j];
	    if (k<0) k = 0;
	    else if (k>255) k = 255;
	    a->data[i][j] = k;
	  }
	freeimage (c); freeimage (b);
	return a;
}

/*      Check that a pixel index is in range. Return TRUE(1) if so.     */

int range (IMAGE im, int i, int j)
{
	if ((i<0) || (i>=im->info->nr)) return 0;
	if ((j<0) || (j>=im->info->nc)) return 0;
	return 1;
}

void SubsCmdLine (char *s)
{
	int k, i;
	char *p;

/* Is it a command line arg? */
	if (s[0] == '$')
	{
	  for (i=1; s[i] != '\0'; i++)
	    if (s[i]<'0'|| s[i]>'9') return;
	  p = &(s[1]);
	  sscanf (p, "%d", &k);
	  if (k >= maxargs || k <= 0) return;
	  strcpy (s, arg[k]);
	}
}

/*      PRINT_SE - Print a structuring element to the screen    */

void print_se (IMAGE p)
{
	int i,j;

	printf ("\n=====================================================\n");
	if (p == NULL)
	  printf (" Structuring element is NULL.\n");
	else 
	{
	  printf ("Structuring element: %dx%d origin at (%d,%d)\n",
		p->info->nr, p->info->nc, p->info->oi, p->info->oj);
	  for (i=0; i<p->info->nr; i++)
	  {
	    printf ("	");
	    for (j=0; j<p->info->nc; j++)
	      printf ("%4d ", p->data[i][j]);
	    printf ("\n");
	  }
	}
	printf ("\n=====================================================\n");
}

IMAGE Input_PBM (char *fn)
{
	int i,j,k,n,m,bi, b;
	unsigned char ucval;
	int val;
	char buf1[256];
	FILE *f;
	IMAGE im;

	strcpy (buf1, fn);
	if (fn[0] == '$') SubsCmdLine(buf1);
	f = fopen (buf1, "r");
	if (f==NULL)
	{
	  printf ("Can't open the PBM file named '%s'\n", buf1);
	  return 0;
	}

	pbm_getln (f, buf1);
	if (buf1[0] == 'P')
	{
	  switch (buf1[1])
	  {
case '1':       k=1; break;
case '2':       k=2; break;
case '3':       k=3; break;
case '4':       k=4; break;
case '5':       k=5; break;
case '6':       k=6; break;
default:        printf ("Not a PBM/PGM/PPM file.\n");
		return 0;
	  }
	}
	bi = 2;

	get_num_pbm (f, buf1, &bi, &m);         /* Number of columns */
	get_num_pbm (f, buf1, &bi, &n);         /* Number of rows */
	if (k!=1 && k!=4) get_num_pbm (f, buf1, &bi, &b);       /* Max value */
	else b = 1;

	printf ("\nPBM file class %d size %d columns X %d rows Max=%d\n",
		k, n, m, b);

/* Allocate the image */
	if (k==3 || k==6)       /* Colour */
	  myabort (0, "Colour image.");
	else 
	{
	  im = (IMAGE)newimage (n, m);
	  im->info->oi = PBM_SE_ORIGIN_ROW;
	  im->info->oj = PBM_SE_ORIGIN_COL;
	  PBM_SE_ORIGIN_ROW = 0;
	  PBM_SE_ORIGIN_COL = 0;
	  for (i=0; i<n; i++)
	    for (j=0; j<m; j++)
	      if (k<3)
	      {
		fscanf (f, "%d", &val);
		im->data[i][j] = (unsigned char)val;
	      } else {
		fscanf (f, "%c", &ucval);
		im->data[i][j] = ucval;
	      }
	}
	return im;
}

IMAGE Output_PBM (IMAGE image, char *filename)
{
	FILE *f;
	int i,j,k, perline;
	char buf1[64];

	strcpy (buf1, filename);
	if (buf1[0] == '$') SubsCmdLine(buf1);
	if (image->info->nc > 20) perline = 20;
	 else perline = image->info->nc-1;
	f = fopen (buf1, "w");
	if (f == 0) myabort (0, "Can't open output file.");
	fprintf (f,"P2\n#origin %d %d\n",image->info->oj,image->info->oi);
	fprintf (f, "%d %d %d\n", image->info->nc, image->info->nr, 255);
	k = 0;
	for (i=0; i<image->info->nr; i++)
	  for (j=0; j<image->info->nc; j++)
	  {
		fprintf (f, "%d ", image->data[i][j]);
		k++;
		if (k > perline)
		{
		  fprintf (f, "\n");
		  k = 0;
		}
	  }
	fprintf (f, "\n");
	fclose (f);
	return image;
}

void get_num_pbm (FILE *f, char *b, int *bi, int *res)
{
	int i;
	char str[80];

	while (b[*bi]==' ' || b[*bi]=='\t' || b[*bi]=='\n')
	{
	  if (b[*bi] == '\n') 
	  {
	    pbm_getln (f, b);
	    *bi = 0;
	  } else
	  *bi += 1;
	}

	i = 0;
	while (b[*bi]>='0' && b[*bi]<='9')
	  str[i++] = b[(*bi)++];
	str[i] = '\0';
	sscanf (str, "%d", res);
}

/* Get the next non-comment line from the PBM file f into the
   buffer b. Look for 'pragmas' - commands hidden in the comments */

void pbm_getln (FILE *f, char *b)
{
	int i;
	char c;

/* Read the next significant line (non-comment) from f into buffer b */
	do
	{

/* Read the next line */
	  i = 0;
	  do
	  {
	    fscanf (f, "%c", &c);
	    b[i++] = c;
	    if (c == '\n') b[i] = '\0';
	  } while (c != '\n');

/* If a comment, look for a special parameter */
	  if (b[0] == '#') pbm_param (b);

	} while (b[0]=='\n' || b[0] == '#');
}

/*      Look for a parameter hidden in a comment        */
void pbm_param (char *s)
{
	int i,j;
	char key[24];

/* Extract the key word */
	for (i=0; i<23; i++)
	{
	  j = i;
	  if (s[i+1] == ' ' || s[i+1] == '\n') break;
	  key[i] = s[i+1];
	}
	key[j] = '\0';

/* Convert to lower case */
	for (i=0; i<j; i++)
	  if ( (key[i]>='A') && (key[i]<='Z') )
		key[i] = (char) ( (int)key[i] - (int)'A' + (int)'a' );

/* Which key word is it? */
	if (strcmp(key, "origin") == 0)         /* ORIGIN key word */
	{
	  sscanf (&(s[j+1]), "%d %d", 
	    &PBM_SE_ORIGIN_COL, &PBM_SE_ORIGIN_ROW);
	  return;
	}
}

struct image  *newimage (int nr, int nc)
{
	struct image  *x;                /* New image */
	unsigned char *ptr;             /* new pixel array */
	int i;

	if (nr < 0 || nc < 0) {
		printf ("Error: Bad image size (%d,%d)\n", nr, nc);
		return 0;
	}

/*      Allocate the image structure    */
	x = (struct image  *) malloc( sizeof (struct image) );
	if (!x) {
		printf ("Out of storage in NEWIMAGE.\n");
		return 0;
	}

/*      Allocate and initialize the header      */

	x->info = (struct header *)malloc( sizeof(struct header) );
	if (!(x->info)) {
		printf ("Out of storage in NEWIMAGE.\n");
		return 0;
	}
	x->info->nr = nr;       x->info->nc = nc;
	x->info->oi = x->info->oj = 0;

/*      Allocate the pixel array        */

	x->data = (unsigned char **)malloc(sizeof(unsigned char *)*nr); /* Pointers to rows */
	if (!(x->data)) {
		printf ("Out of storage in NEWIMAGE.\n");
		return 0;
	}
	
	for (i=0; i<nr; i++) {
	  ptr = (unsigned char *)malloc(nc); /* Allocate one row  */
	  if (!ptr) {
		printf ("Out of storage in NEWIMAGE.\n");
		return 0;
	  } else x->data[i] = ptr;
	}
	return x;
}

void freeimage (struct image  *z)
{
/*      Free the storage associated with the image Z    */
	int i;

	if (z != 0) {
	   for (i=0; i<z->info->nr; i++)
	      free (z->data[i]);
	   free (z->info);
	   free (z->data);
	   free (z);
	}
}

IMAGE Complement (IMAGE x)
{
	IMAGE t;
	int i,j;

	t = NewImage (x);
	for (i=0; i<x->info->nr; i++)
	  for (j=0; j<x->info->nc; j++)
	    t->data[i][j] = 255 - x->data[i][j];;
	return t;
}

void myabort (int val, char *mess)
{
	val = val;
	fprintf (stderr, "**** ABORT MAX: %s ****\n", mess);
	exit (2);
}

int Attribute (int which, IMAGE z)
{
	PIXEL p;

	p = (PIXEL)z;
	if (which == 1) return z->info->nc;
	else if (which == 2) return z->info->nr;
	else if (which == 3) return z->info->oi;
	else if (which == 4) return z->info->oj;
	else if (which == 5) return p->row;
	else if (which == 6) return p->col;
	myabort (0, "Illegal attribute.");
}

void CopyVarImage (IMAGE *a, IMAGE *b)
{
	int i,j;

	if (a == b) return;
	if (*a) freeimage (*a);
	*a = newimage ((*b)->info->nr, (*b)->info->nc);
	if (*a == 0) myabort (0, "No more storage.\n");

	for (i=0; i<(*b)->info->nr; i++)
	  for (j=0; j< (*b)->info->nc; j++)
	    (*a)->data[i][j] = (*b)->data[i][j];
	(*a)->info->oi = (*b)->info->oi;
	(*a)->info->oj = (*b)->info->oj;
}

void CopyVarPix (PIXEL *a, PIXEL *b)
{
	int i,j;

	if (a == b) return;
	if (*a) free (*a);
	*a = (PIXEL)malloc (sizeof(struct pixrec));
	if (*a == 0) myabort (0, "No more storage.\n");

	(*a)->row = (*b)->row;
	(*a)->col = (*b)->col;
}

/*      MinMax computes the range for pixel values that will
	contain all pixels in both images.                      */

void MinMax (IMAGE a, IMAGE b, int *rmax, 
	     int *cmax, int *rmin, int *cmin)
{
	int x, y;

/* (0,0) index is which pixel? */
	y = jtop (a, 0);
	x = jtop (b, 0);
	if (x < y) *cmin = x; else *cmin = y;

	y = itop (a, 0);
	x = itop (b, 0);
	if (x < y) *rmin = x; else *rmin = y;

/* Now max indices */
	x = itop (a, a->info->nr);
	y = itop (b, b->info->nr);
	if (x > y) *rmax = x; else *rmax = y;

	x = jtop (a, a->info->nc);
	y = jtop (b, b->info->nc);
	if (x>y) *cmax = x; else *cmax = y;
}

/*      An INDEX into an image is offset by the image's origin.
	These functions convert indices into pixels, or set elements */

int itop (IMAGE a, int i)
{
	return i-a->info->oi;
}

int jtop (IMAGE a, int j)
{
	return j - a->info->oj;
}

/*      These functions convert PIXELS (set elements) into image
	indices, either row or column, using the origin location.       */

int ptoi (IMAGE a, int p)
{
	return p + a->info->oi;
}

int ptoj (IMAGE a, int p)
{
	return p + a->info->oj;
}

/* Get and set pixel values from pixel coordinates (set elements) */

int pget (IMAGE a, int i, int j)
{
	int ii, jj;

	ii = ptoi (a, i);
	jj = ptoj (a, j);
	if (range(a, ii, jj)) return a->data[ii][jj];
	else return 0;
}

void pset (IMAGE a, int i, int j, int val)
{
	int ii,jj;

	ii = ptoi (a, i);
	jj = ptoj (a, j);
	if (range(a, ii, jj))
	  a->data[ii][jj] = val;
	else if (val != 0) myabort (0, "Attempt to set pixel not in image.");
}


IMAGE Union (IMAGE a, IMAGE b)
{
	IMAGE t;
	int i,j, rmin, rmax, cmin, cmax;

	MinMax (a, b, &rmax, &cmax, &rmin, &cmin);
	t = newimage (rmax-rmin, cmax-cmin);
	t->info->oi = -rmin; t->info->oj = -cmin;
	for (i=rmin; i<rmax; i++)
	  for (j=cmin; j< cmax; j++)
	    if (pget(a, i, j)<= pget(b, i, j))
	      pset (t, i, j, pget(b, i, j));
	    else pset (t, i, j, pget(a, i,j));
	return t;
}

IMAGE Intersection (IMAGE a, IMAGE b)
{
	IMAGE t;
	int i,j, rmin,rmax,cmin,cmax;

	MinMax (a, b, &rmax, &cmax, &rmin, &cmin);
	t = newimage (rmax-rmin, cmax-cmin);
	t->info->oi = -rmin; t->info->oj = -cmin;
	for (i=rmin; i<rmax; i++)
	  for (j=cmin; j< cmax; j++)
	    if (pget(a,i,j) > pget(b,i,j))
		pset(t, i,j,pget(a, i,j));
	    else pset (t, i, j, pget(b,i,j));
	return t;
}

IMAGE Difference (IMAGE a, IMAGE b)
{
	IMAGE t;
	int i,j, rmax, rmin, cmax, cmin;

	MinMax (a, b, &rmax, &cmax, &rmin, &cmin);
	t = newimage (rmax-rmin, cmax-cmin);
	t->info->oi = -rmin;    t->info->oj = -cmin;
	for (i=rmin; i<rmax; i++)
	  for (j=cmin; j< cmax; j++)
		pset (t, i, j, pget(a,i,j)-pget(b,i,j));
	return t;
}

int PSubSet (IMAGE a, IMAGE b)
{
	if (ImCompare(a, b)) return 0;
	return SubSet (a, b);
}

int SubSet (IMAGE a, IMAGE b)
{
	int i,j;
	int rmax, rmin, cmax, cmin;

	MinMax (a, b, &rmax, &cmax, &rmin, &cmin);
	for (i=rmin; i<rmax; i++)
	  for (j=cmin; j< cmax; j++)
	    if (pget(a,i,j)==1 && pget(b,i,j)==0)
	      return 0;
	return 1;
}

int ImCompare (IMAGE a, IMAGE b)
{
	int i,j, rmin, rmax, cmin, cmax, x, y;

	MinMax (a, b, &rmax, &cmax, &rmin, &cmin);
 
	for (i=rmin; i<rmax; i++)
	  for (j=cmin; j< cmax; j++)
	  {
	    x = pget (a, i, j);
	    y = pget (b, i, j);
	    if (x != y) return 0;
	  }
	return 1;
}

int ImValue (IMAGE a, int val)
{
	int i,j;

	for (i=0; i<a->info->nr; i++)
	  for (j=0; j< a->info->nc; j++)
	    if (a->data[i][j]!=val) return 0;
	return 1;
}

int PixValue (PIXEL a, PIXEL b)
{
	if (a==0) myabort (0, "NULL pixel.");
	if (b==0) myabort (0, "NULL pixel.");
	if ((a->row == b->row) && (a->col== b->col)) return 1;
	return 0;
}

IMAGE NewImage (IMAGE x)
{
	IMAGE t;
	int i,j;

	if (x == 0) myabort (0, "IMAGE is 0 in NewImage.");
	t = newimage (x->info->nr, x->info->nc);
	if (t == 0) myabort (0, "Out of storage.");
	t->info->oi = x->info->oi;
	t->info->oj = x->info->oj;
	for (i=0; i<x->info->nr; i++)
	  for (j=0; j<x->info->nc; j++)
	    t->data[i][j] = 0;
	return t;
}

IMAGE Translate (IMAGE b, PIXEL p)
{
	IMAGE t;
	int i,j;

	t = newimage (b->info->nr+p->row+b->info->oi,
		 b->info->nc+p->col+b->info->oj);
	t->info->oi = 0; t->info->oj = 0;
	for (i=0; i<t->info->nr; i++)
	  for (j=0; j< t->info->nc; j++)
	     t->data[i][j] = 0;
	for (i=0; i<b->info->nr; i++)
	  for (j=0; j< b->info->nc; j++)
	    if (range(t, i+p->row-b->info->oi, j+p->col-b->info->oj))
	      t->data[i+p->row-b->info->oi][j+p->col-b->info->oj] = b->data[i][j];
	return t;
}

PIXEL Pixel (int i, int j)
{
	PIXEL x;

	x = (PIXEL) malloc (sizeof (struct pixrec));
	if (x == 0) myabort (0, "Out of storage.");
	x->row = i;
	x->col = j;
	return x;
}

int Member (PIXEL p, IMAGE xx)
{
	int i,j;

	if (xx == 0) myabort (0, "Image is NULL.\n");
	i = p->row + xx->info->oi;
	j = p->col + xx->info->oj;
	if (range(xx, i, j))
	{
	  if (xx->data[i][j] >= 1) return 1;
	  else return 0;
	} else return 0;
}

void Outint (int val, char *name)
{
	int i;

	if (name[0] == '\0')            /* Standard output */
	{
	  printf (" %d ", val);
	  return;
	}

	for (i=0; i<nfiles; i++)
	  if (strcmp(filenames[i], name)==0)
	  {
		fprintf (files[i], "%d\n", val);
		return;
	  }

/* New file name */
	strcpy (filenames[nfiles], name);
	files[nfiles] = fopen (name, "w");
	if (files[nfiles] == NULL)
	{
	  fprintf (stderr, "File name '%s': ", name);
	  myabort (0, "Cannot open output file.");
	}
	fprintf (files[nfiles], " %d ", val);
	nfiles++;
}

int inint (int *val, char *name)
{
	int i;

	if (name[0] == '\0')            /* Standard input */
	{
	  scanf ("%d", val);
	  return *val;
	}

	for (i=0; i<nfiles; i++)
	  if (strcmp(filenames[i], name)==0)
	  {
		fscanf (files[i], "%d\n", val);
		return *val;
	  }

/* New input file */
	strcpy (filenames[nfiles], name);
	files[nfiles] = fopen (name, "r");
	if (files[nfiles] == NULL)
	{
	  fprintf (stderr, "File name '%s': ", name);
	  myabort (0, "Cannot open input file.");
	}
	fscanf (files[nfiles], " %d ", val);
	nfiles++;
	return *val;
}

IMAGE ImageGen (PIXEL p1, PIXEL p2, char *s)
{
	int i,j,k;
	IMAGE t;

	t = newimage (p1->row, p1->col);
	t->info->oi = p2->row;
	t->info->oj = p2->col;
	k = 0;
	for (i=0; i<p1->row; i++)
	  for (j=0; j<p1->col; j++)
	  {
	    if (s[k] == '1') t->data[i][j] = 1;
	    else if (s[k] == '2') t->data[i][j] = 2;
	    else if (s[k] == '3') t->data[i][j] = 3;
	    else if (s[k] == '4') t->data[i][j] = 4;
	    else if (s[k] == '5') t->data[i][j] = 5;
	    else if (s[k] == '6') t->data[i][j] = 6;
	    else if (s[k] == '7') t->data[i][j] = 7;
	    else if (s[k] == '8') t->data[i][j] = 8;
	    else if (s[k] == '9') t->data[i][j] = 9;
		else t->data[i][j] = 0;
	    k++;
	  }
	return t;
}

int Isolated (IMAGE a)
{
	int i,j,n,m,k,res;

	k = 0;
	for (i=0; i<a->info->nr; i++)
	  for (j=0; j<a->info->nc; j++)
	    if (a->data[i][j] > 0)
	    {
	      res = 1;
	      for (n= -1; n<=1; n++)
		for (m= -1; m<=1; m++)
		{
		  if (n==0 && m==0) continue;
		  if (range(a,i+n,j+m))
		    if (a->data[i+n][j+m] > 0)
			 res = 0;
		}
	      k += res;
	    }
	return k;
}

IMAGE SetAPixel (IMAGE a, PIXEL p)
{
	int i,j, row, col;
	int rmin, rmax, cmin, cmax;
	IMAGE t;

	row = ptoi(a, p->row);  col = ptoj(a, p->col);
	if (row > a->info->nr) rmax = row; else rmax = a->info->nr;
	if (col > a->info->nc) cmax = col; else cmax = a->info->nc;
	if (row < 0) rmin = row; else rmin = 0;
	if (col < 0) cmin = col; else cmin = 0;
	if (range(a, row, col)==0)
	{
	  t = newimage (rmax-rmin, cmax-cmin);
	  t->info->oi = -rmin; t->info->oj = -cmin;
	  for (i=rmin; i<rmax; i++)
	    for (j=cmin; j<cmax; j++)
	      pset(t, i, j, pget(a, i, j));
	  pset (t, p->row, p->col, 1);
	  return t;
	}
	a->data[row][col] = 1;
	return a;
}

PIXEL PixDif (PIXEL p1, PIXEL p2)
{
	PIXEL p3;

	p3 = (PIXEL)malloc (sizeof(struct pixrec));
	p3->row = p1->row - p2->row;
	p3->col = p1->col - p2->col;
	return p3;
}

PIXEL PixAdd (PIXEL p1, PIXEL p2)
{
	PIXEL p3;

	p3 = (PIXEL)malloc (sizeof(struct pixrec));
	p3->row = p1->row + p2->row;
	p3->col = p1->col + p2->col;
	return p3;
}

void Display (IMAGE x)
{
	Output_PBM (x, ".dis");
	system ("disp .dis");
	system ("del .dis");
}
