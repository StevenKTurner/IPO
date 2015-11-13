/* Library code for MAX compiler : the runtime system */

/*----------------------------------------------------------------------

	MLIB - General Digital Morphology Package

					J. R. Parker
					Laboratory for Computer Vision
					University of Calgary
					Calgary, Alberta, Canada

  ---------------------------------------------------------------------- */
#define MAX
#include "max.h"

FILE *files[20];
char *filenames[64];
int nfiles = 0;
char NEXT;

/*      Check that a pixel index is in range. Return TRUE(1) if so.     */

int range (IMAGE im, int i, int j)
{
	if ((i<0) || (i>=im->info->nr)) return 0;
	if ((j<0) || (j>=im->info->nc)) return 0;
	return 1;
}

void SubsCmdLine (char *s)
{
	int k=0, i=0;
	char *p=0;

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
	int i=0, j=0;

	printf ("\n=====================================================\n");
	if (p == NULL)
	  printf (" Structuring element is NULL.\n");
	else 
	{
	  printf ("Structuring element: %d x %d, origin at (%d,%d)\n",
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

/*      Apply a dilation step on one pixel of IM, reult to RES  */

void dil_apply (IMAGE im, IMAGE p, int ii, int jj, IMAGE res)
{
	int i=0,j=0, is=0,js=0, ie=0, je=0, k=0;

/* Find start and end pixel in IM */
	is = ii - p->info->oi;  js = jj - p->info->oj;
	ie = is + p->info->nr;  je = js + p->info->nc;

/* Place SE over the image from (is,js) to (ie,je). Set pixels in RES
   if the corresponding SE pixel is 1; else do nothing.         */
	for (i=is; i<ie; i++)
	  for (j=js; j<je; j++)
	  {
	    if (range(im,i,j))
	    {
	      k = p->data[i-is][j-js];
	      if (k>=0) res->data[i][j] |= k;
	    }
	  }
}

/* BIN_DILATE - Dilate the given image using the given structuring element */

IMAGE Dilate (IMAGE im, IMAGE p)
{
	IMAGE tmp=0;
	int i=0,j=0;

/* Source image empty? */
	if (im==0)
	  max_abort (0, "NULL Image in Dilate.");

/* Create a result image */
	tmp = newimage (im->info->nr, im->info->nc);
	if (tmp == 0)
	  max_abort (0, "Out of memory in Dilate.");
	for (i=0; i<tmp->info->nr; i++)
	  for (j=0; j<tmp->info->nc; j++)
	    tmp->data[i][j] = 0;

/* Apply the SE to each black pixel of the input */
	for (i=0; i<im->info->nr; i++)
	  for (j=0; j<im->info->nc; j++)
	    if (im->data[i][j] == WHITE)
	      dil_apply (im, p, i, j, tmp);

/* Free the result image - it was a temp */
	return tmp;
}

/*      Apply a erosion step on one pixel of IM, reult to RES   */

void erode_apply (IMAGE im, IMAGE p, int ii, int jj, IMAGE res)
{
	int i=0,j=0, is=0,js=0, ie=0, je=0, k=0, r=0;

/* Find start and end pixel in IM */
	is = ii - p->info->oi;  js = jj - p->info->oj;
	ie = is + p->info->nr;  je = js + p->info->nc;

/* Place SE over the image from (is,js) to (ie,je). Set pixels in RES
   if the corresponding 1 pixels in the image agree.    */
	r = 1;
	for (i=is; i<ie; i++)
	  for (j=js; j<je; j++)
	  {
	    if (range(im,i,j))
	    {
	      k = p->data[i-is][j-js];
	      if ((k==1) && (im->data[i][j]==0)) r = 0;
	    } else if (p->data[i-is][j-js] != 0)  r = 0;
	  }
	res->data[ii][jj] = r;
}

/*      Erode the image IM using the structuring element P      */

IMAGE Erode (IMAGE im, IMAGE p)
{
	IMAGE tmp=0;
	int i=0, j=0;

/* Source image empty? */
	if (im==0)
	  max_abort (0, "NULL image in ERODE.");

/* Create a result image */
	tmp = newimage (im->info->nr, im->info->nc);
	if (tmp == 0)
	  max_abort (0, "Out of memory in Erode.");
	for (i=0; i<tmp->info->nr; i++)
	  for (j=0; j<tmp->info->nc; j++)
	    tmp->data[i][j] = 0;

/* Apply the SE to each black pixel of the input */
	for (i=0; i<im->info->nr; i++)
	  for (j=0; j<im->info->nc; j++)
	      erode_apply (im, p, i, j, tmp);

	return tmp;
}

IMAGE Input_PBM (char *fn)
{
	int i=0,j=0,k=0,n=0,m=0,bi=0, b=0;
	unsigned char ucval='\0';
	int val=0;
	char buf1[256];
	FILE *f=NULL;
	IMAGE im=0;

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
		k, m, n, b);

/* Allocate the image */
	if (k==3 || k==6)       /* Colour */
	  max_abort (0, "Colour image.");
	else 
	{
	  im = (IMAGE) newimage (n, m);
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
	fclose (f);
	return im;
}

IMAGE Output_PBM (IMAGE image, char *filename)
{
	FILE *f=NULL;
	int i=0,j=0,k=0, perline=0;
	char buf1[64];

	strcpy (buf1, filename);
	if (buf1[0] == '$') SubsCmdLine(buf1);
	if (image->info->nc > 35) perline = 32;
	 else perline = image->info->nc-1;
	f = fopen (buf1, "w");
	if (f == 0) max_abort (0, "Can't open output file.");
	fprintf (f,"P1\n#origin %d %d\n",image->info->oj,image->info->oi);
	fprintf (f, "%d %d\n", image->info->nc, image->info->nr);
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
	int i=0;
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
	int i=0;
	char c=' ';

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
	int i=0, j=0;
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

IMAGE newimage (int nr, int nc)
{
	IMAGE x=0;                /* New image */
	unsigned char *ptr=0;             /* new pixel array */
	int i=0;

	if (nr < 0 || nc < 0) 
	{
		printf ("Error: Bad image size (%d,%d)\n", nr, nc);
		return 0;
	}

/*      Allocate the image structure    */
	x = (struct image  *) malloc( sizeof (struct image) );
	if (!x) 
	{
		printf ("Out of storage in NEWIMAGE.\n");
		return 0;
	}

/*      Allocate and initialize the header      */

	x->info = (struct header *)malloc( sizeof(struct header) );
	if (!(x->info))
	{
		printf ("Out of storage in NEWIMAGE.\n");
		return 0;
	}

	x->info->nr = nr;       x->info->nc = nc;
	x->info->oi = 0;        x->info->oj = 0;

/*      Allocate the pixel array        */

	x->data = (unsigned char **)
		 malloc(sizeof(unsigned char *)*nr); /* Pointers to rows */
	if (!(x->data)) 
	{
		printf ("Out of storage in NEWIMAGE.\n");
		return 0;
	}

	for (i=0; i<nr; i++)
	{
	  ptr = (unsigned char *)malloc(nc); /* Allocate one row  */
	  if (!ptr) 
	  {
		printf ("Out of storage in NEWIMAGE.\n");
		return 0;
	  } else 
	  {
	    x->data[i] = ptr;
	  }
	}
	return x;
}

void freeimage (struct image  *z)
{
/*      Free the storage associated with the image Z    */
	int i=0;

	if (z != 0) 
	{
	   for (i=0; i<z->info->nr; i++)
	      free (z->data[i]);
	   free (z->info);
	   free (z->data);
	   free (z);
	}
}

IMAGE Complement (IMAGE x)
{
	IMAGE t=0;
	int i=0,j=0;

	t = NewImage (x);
	for (i=0; i<x->info->nr; i++)
	  for (j=0; j<x->info->nc; j++)
	    if (x->data[i][j] == 1) 
		t->data[i][j] = 0;
	    else t->data[i][j] = 1;
	return t;
}

void max_abort (int val, char *mess)
{
	fprintf (stderr, "**** ABORT MAX: [%d]:%s ****\n", val, mess);
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
	max_abort (0, "Illegal attribute.");
	return 0;
}

void CopyVarImage (IMAGE *a, IMAGE *b)
{
	int i=0, j=0;

	if (a == b) return;
	if (*a) freeimage (*a);
	*a = newimage ((*b)->info->nr, (*b)->info->nc);
	if (*a == 0) max_abort (0, "No more storage.\n");

	for (i=0; i<(*b)->info->nr; i++)
	  for (j=0; j< (*b)->info->nc; j++)
	    (*a)->data[i][j] = (*b)->data[i][j];
	(*a)->info->oi = (*b)->info->oi;
	(*a)->info->oj = (*b)->info->oj;
	(*a)->info->nr = (*b)->info->nr;
	(*a)->info->nc = (*b)->info->nc;

}

void CopyVarPix (PIXEL *a, PIXEL *b)
{
	if (a == b) return;
	if (*a) free (*a);
	*a = (PIXEL)malloc (sizeof(struct pixrec));
	if (*a == 0) max_abort (0, "No more storage.\n");

	(*a)->row = (*b)->row;
	(*a)->col = (*b)->col;
}

/*      MinMax computes the range for pixel values that will
	contain all pixels in both images.                      */

void MinMax (IMAGE a, IMAGE b, int *rmax, 
	     int *cmax, int *rmin, int *cmin)
{
	int x=0, y=0;

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
	int ii=0, jj=0;

	ii = ptoi (a, i);
	jj = ptoj (a, j);
	if (range(a, ii, jj)) return a->data[ii][jj];
	else return 0;
}

void pset (IMAGE a, int i, int j, int val)
{
	int ii=0, jj=0;

	ii = ptoi (a, i);
	jj = ptoj (a, j);
	if (range(a, ii, jj))
	  a->data[ii][jj] = val;
	else if (val != 0) max_abort (0, "Attempt to set pixel not in image.");
}


IMAGE Union (IMAGE a, IMAGE b)
{
	IMAGE t=0;
	int i=0, j=0, rmin=0, rmax=0, cmin=0, cmax=0;

	MinMax (a, b, &rmax, &cmax, &rmin, &cmin);
	t = newimage (rmax-rmin, cmax-cmin);
	t->info->oi = -rmin; t->info->oj = -cmin;
	for (i=rmin; i<rmax; i++)
	  for (j=cmin; j< cmax; j++)
	    if (pget(a, i, j)== 1 || pget(b, i, j)==1)
	      pset (t, i, j, 1);
	  else pset (t, i, j, 0);
	return t;
}

IMAGE Intersection (IMAGE a, IMAGE b)
{
	IMAGE t=0;
	int i=0, j=0, rmin=0, rmax=0, cmin=0, cmax=0;

	MinMax (a, b, &rmax, &cmax, &rmin, &cmin);
	t = newimage (rmax-rmin, cmax-cmin);
	t->info->oi = -rmin; t->info->oj = -cmin;
	for (i=rmin; i<rmax; i++)
	  for (j=cmin; j< cmax; j++)
	    if (pget(a,i,j)==1 && pget(b,i,j)==1)
		pset(t, i,j,1);
	    else pset (t, i, j, 0);
	return t;
}

IMAGE Difference (IMAGE a, IMAGE b)
{
	IMAGE t=0;
	int i=0, j=0, rmax=0, rmin=0, cmax=0, cmin=0;

	MinMax (a, b, &rmax, &cmax, &rmin, &cmin);
	t = newimage (rmax-rmin+1, cmax-cmin+1);   
	t->info->oi = -rmin;    t->info->oj = -cmin;
	if (t == 0)
	{
		printf ("Out of storage in Difference!\n");
		exit (1);
	}

	for (i=rmin; i<rmax; i++)
	{
	  for (j=cmin; j< cmax; j++)
	  {
	    if (pget(a,i,j)==1 && pget(b,i,j)!=1)
		  pset (t, i, j, 1);
	    else  pset (t, i, j, 0); 
	  }
	}
	return t;
}

int PSubSet (IMAGE a, IMAGE b)
{
	if (ImCompare(a, b)) return 0;
	return SubSet (a, b);
}

int SubSet (IMAGE a, IMAGE b)
{
	int i=0, j=0;
	int rmax=0, rmin=0, cmax=0, cmin=0;

	MinMax (a, b, &rmax, &cmax, &rmin, &cmin);
	for (i=rmin; i<rmax; i++)
	  for (j=cmin; j< cmax; j++)
	    if (pget(a,i,j)==1 && pget(b,i,j)==0)
	      return 0;
	return 1;
}

int ImCompare (IMAGE a, IMAGE b)
{
	int i=0, j=0, rmin=0, rmax=0, cmin=0, cmax=0, x=0, y=0;

	MinMax (a, b, &rmax, &cmax, &rmin, &cmin);
 
	for (i=rmin; i<rmax; i++)
	  for (j=cmin; j< cmax; j++)
	  {
	    x = pget (a, i, j);
	    y = pget (b, i, j);
	    if (x==1 && y == 0) return 0;
	    if (x==0 && y == 1) return 0;
	  }
	return 1;
}

int ImValue (IMAGE a, int val)
{
	int i=0, j=0;

	for (i=0; i<a->info->nr; i++)
	  for (j=0; j< a->info->nc; j++)
	    if (a->data[i][j]!=val) return 0;
	return 1;
}

int PixValue (PIXEL a, PIXEL b)
{
	if (a==0) max_abort (0, "NULL pixel.");
	if (b==0) max_abort (0, "NULL pixel.");
	if ((a->row == b->row) && (a->col== b->col)) return 1;
	return 0;
}

IMAGE NewImage (IMAGE x)
{
	IMAGE t=0;
	int i=0, j=0;
		
	if (x == 0) max_abort (0, "IMAGE is 0 in NewImage.");
	t = newimage (x->info->nr, x->info->nc);
	if (t == 0) max_abort (0, "Out of storage.");
	t->info->oi = x->info->oi;
	t->info->oj = x->info->oj;
	for (i=0; i<x->info->nr; i++)
	  for (j=0; j<x->info->nc; j++)
	    t->data[i][j] = 0;
	return t;
}

IMAGE Translate (IMAGE b, PIXEL p)
{
	IMAGE t=0;
	int i=0, j=0;

	t = newimage (b->info->nr+p->row+b->info->oi,
		 b->info->nc+p->col+b->info->oj);
	t->info->oi = 0; t->info->oj = 0;
	for (i=0; i<t->info->nr; i++)
	  for (j=0; j< t->info->nc; j++)
	     t->data[i][j] = 0;
	for (i=0; i<b->info->nr; i++)
	  for (j=0; j< b->info->nc; j++)
	    if (b->data[i][j] == 1)
	      if (range(t, i+p->row-b->info->oi, j+p->col-b->info->oj))
		t->data[i+p->row-b->info->oi][j+p->col-b->info->oj] = 1;
	return t;
}

PIXEL Pixel (int i, int j)
{
	PIXEL x;

	x = (PIXEL) malloc (sizeof (struct pixrec));
	if (x == 0) max_abort (0, "Out of storage.");
	x->row = i;
	x->col = j;
	return x;
}

int Member (PIXEL p, IMAGE xx)
{
	int i=0, j=0;

	if (xx == 0) max_abort (0, "Image is NULL.\n");
	i = p->row + xx->info->oi;
	j = p->col + xx->info->oj;
	if (range(xx, i, j))
	{
	  if (xx->data[i][j] == 1) return 1;
	  else return 0;
	} else return 0;
}

void Outpix (PIXEL val, char *name)
{
	Outint (val->row, name);
	Outint (val->col, name);
}

void Outint (int val, char *name)
{
	int i=0;

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
	  max_abort (0, "Cannot open output file.");
	}
	fprintf (files[nfiles], " %d ", val);
	nfiles++;
}

PIXEL inpix (PIXEL p, char *name)
{
	inint (&(p->row), name);
	inint (&(p->col), name);
	return p;
}

int inint (int *val, char *name)
{
	int i=0;

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
	  max_abort (0, "Cannot open input file.");
	}
	fscanf (files[nfiles], " %d ", val);
	nfiles++;
	return *val;
}

IMAGE ImageGen (PIXEL p1, PIXEL p2, char *s)
{
	int i=0, j=0, k=0;
	IMAGE t=0;

	t = newimage (p1->row, p1->col);
	t->info->oi = p2->row;
	t->info->oj = p2->col;
	k = 0;
	for (i=0; i<p1->row; i++)
	  for (j=0; j<p1->col; j++)
	  {
	    if (s[k] == '1') t->data[i][j] = 1;
	    else if (s[k] == '2') t->data[i][j] = 2;
		else t->data[i][j] = 0;
	    k++;
	  }
	return t;
}

int Isolated (IMAGE a)
{
	int i=0, j=0, n=0, m=0, k=0, res=0;

	k = 0;
	for (i=0; i<a->info->nr; i++)
	  for (j=0; j<a->info->nc; j++)
	    if (a->data[i][j] == 1)
	    {
	      res = 1;
	      for (n= -1; n<=1; n++)
		for (m= -1; m<=1; m++)
		{
		  if (n==0 && m==0) continue;
		  if (range(a,i+n,j+m))
		    if (a->data[i+n][j+m] == 1)
			 res = 0;
		}
	      k += res;
	    }
	return k;
}

IMAGE SetAPixel (IMAGE a, PIXEL p)
{
	int i=0, j=0, row=0, col=0;
	int rmin=0, rmax=0, cmin=0, cmax=0;
	IMAGE t=0;

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

