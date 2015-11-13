/*----------------------------------------------------------------------

	MLIB - General Digital Morphology Package

					J. R. Parker
					Laboratory for Computer Vision
					University of Calgary
					Calgary, Alberta, Canada

  ---------------------------------------------------------------------- */
#define MLIB
#include "morph.h"

/*      Check that a pixel index is in range. Return TRUE(1) if so.     */

int range (IMAGE im, int i, int j)
{
	if ((i<0) || (i>=im->info->nr)) return 0;
	if ((j<0) || (j>=im->info->nc)) return 0;
	return 1;
}

/*      GET_SE - Read a structuring element from a file         */

int get_se (char *filename, SE *p)
{
	SE q;
	IMAGE seim;

	*p = (SE)0;

/* Read the PBM format structuring element file */
	if (read_pbm (filename, &seim) == 0) return 0;

/* Allocate a structure for the SE */
	q = (SE)malloc (sizeof (struct se_struct));
	if (q == 0)
	{
	  printf ("Can't allocate structuring element in GET_SE.\n");
	  return 0;
	}
	q->nr = seim->info->nr;         q->nc = seim->info->nc;
	q->oi = PBM_SE_ORIGIN_ROW;      q->oj = PBM_SE_ORIGIN_COL;
	q->data = seim->data;
	free (seim->info);
	seim->data = 0;
	free (seim);
	*p = q;
	return 1;
}

/*      PRINT_SE - Print a structuring element to the screen    */

int print_se (SE p)
{
	int i,j;

	printf ("\n=====================================================\n");
	if (p == NULL)
	  printf (" Structuring element is NULL.\n");
	else 
	{
	  printf ("Structuring element: %dx%d origin at (%d,%d)\n",
		p->nr, p->nc, p->oi, p->oj);
	  for (i=0; i<p->nr; i++)
	  {
	    printf ("	");
	    for (j=0; j<p->nc; j++)
	      printf ("%4d ", p->data[i][j]);
	    printf ("\n");
	  }
	}
	printf ("\n=====================================================\n");
}

/*      Apply a dilation step on one pixel of IM, reult to RES  */

void dil_apply (IMAGE im, SE p, int ii, int jj, IMAGE res)
{
	int i,j, is,js, ie, je, k;

/* Find start and end pixel in IM */
	is = ii - p->oi;        js = jj - p->oj;
	ie = is + p->nr;        je = js + p->nc;

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

int bin_dilate (IMAGE im, SE p)
{
	IMAGE tmp;
	int i,j;

/* Source image empty? */
	if (im==0)
	{
	  printf ("Bad image in BIN_DILATE\n");
	  return 0;
	}

/* Create a result image */
	tmp = newimage (im->info->nr, im->info->nc);
	if (tmp == 0)
	{
	  printf ("Out of memory in BIN_DILATE.\n");
	  return 0;
	}
	for (i=0; i<tmp->info->nr; i++)
	  for (j=0; j<tmp->info->nc; j++)
	    tmp->data[i][j] = 0;

/* Apply the SE to each black pixel of the input */
	for (i=0; i<im->info->nr; i++)
	  for (j=0; j<im->info->nc; j++)
	    if (im->data[i][j] == WHITE)
	      dil_apply (im, p, i, j, tmp);

/* Copy result over the input */
	for (i=0; i<im->info->nr; i++)
	  for (j=0; j<im->info->nc; j++)
	    im->data[i][j] = tmp->data[i][j];

/* Free the result image - it was a temp */
	freeimage (tmp);
	return 1;
}

/*      Apply a erosion step on one pixel of IM, reult to RES   */

void erode_apply (IMAGE im, SE p, int ii, int jj, IMAGE res)
{
	int i,j, is,js, ie, je, k, r;

/* Find start and end pixel in IM */
	is = ii - p->oi;        js = jj - p->oj;
	ie = is + p->nr;        je = js + p->nc;
/*  printf ("SE is: Origin (%d,%d) size (%d,%d)\n", p->oi,p->oj,p->nr,p->nc);
    printf ("Start at image pixel (%d,%d)\n", ii, jj);          */

/* Place SE over the image from (is,js) to (ie,je). Set pixels in RES
   if the corresponding pixels in the image agree.      */
	r = 1;
	for (i=is; i<ie; i++)
	{
	  for (j=js; j<je; j++)
	  {
	    if (range(im,i,j))
	    {
	      k = p->data[i-is][j-js];
	      if ((k == 1) && (im->data[i][j]==0)) r = 0;
/*              printf ("%3d ", im->data[i-is][j-js]);             */
	    } else if (p->data[i-is][j-js] != 0) r = 0;
	  }
/*          printf ("\n");         */
	}
	res->data[ii][jj] = r;
}

/*      Erode the image IM using the structuring element P      */

int bin_erode (IMAGE im, SE p)
{
	IMAGE tmp;
	int i,j;

/* Source image empty? */
	if (im==0)
	{
	  printf ("Bad image in BIN_ERODE\n");
	  return 0;
	}

/* Create a result image */
	tmp = newimage (im->info->nr, im->info->nc);
	if (tmp == 0)
	{
	  printf ("Out of memory in BIN_ERODE.\n");
	  return 0;
	}
	for (i=0; i<tmp->info->nr; i++)
	  for (j=0; j<tmp->info->nc; j++)
	    tmp->data[i][j] = 0;

/* Apply the SE to each black pixel of the input */
	for (i=0; i<im->info->nr; i++)
	  for (j=0; j<im->info->nc; j++)
	      erode_apply (im, p, i, j, tmp);

/* Copy result over the input */
	for (i=0; i<im->info->nr; i++)
	  for (j=0; j<im->info->nc; j++)
	    im->data[i][j] = tmp->data[i][j];

/* Free the result image - it was a temp */
	freeimage (tmp);
	return 1;
}

/*      -------------------------------------------------------------------- 
		ALPHA library code - compile seperately for book
      -------------------------------------------------------------------- */

/*  ================================================================= 

	Laboratory for Computer Vision
	Department of Computer Science
	University of Calgary   

	Module PBMSTUFF - Read PBM images, create and free image types

    ================================================================= */


IMAGE red, green, blue;

int read_pbm (char *fn, IMAGE *im)
{
	int i,j,k,n,m,bi, b;
	unsigned char ucval;
	int val;
	char buf1[256];
	FILE *f;

	f = fopen (fn, "r");
	if (f==NULL)
	{
	  printf ("Can't open the PBM file named '%s'\n", fn);
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
	{
		printf ("Colour - later\n\n");
		exit (1);
	} else 
	{
	  *im = (IMAGE)newimage (n, m);
	  for (i=0; i<n; i++)
	    for (j=0; j<m; j++)
	      if (k<3)
	      {
		fscanf (f, "%d", &val);
		(*im)->data[i][j] = (unsigned char)val;
	      } else {
		fscanf (f, "%c", &ucval);
		(*im)->data[i][j] = ucval;
	      }
	}
	return 1;
}

void write_pbm (char *filename, IMAGE image)
{
	FILE *f;
	int i,j,k;

	f = fopen (filename, "w");
	fprintf (f, "P1\n%d %d\n", image->info->nc, image->info->nr);
	for (i=0; i<image->info->nr; i++)
	  for (j=0; j<image->info->nc; j++)
	  {
		fprintf (f, "%d ", image->data[i][j]);
		k++;
		if (k > 10)
		{
		  fprintf (f, "\n");
		  k = 0;
		}
	  }
	fprintf (f, "\n");
	fclose (f);
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

