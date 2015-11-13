/* Include file MAX.H */

/*----------------------------------------------------------------------

	MLIB - General Digital Morphology Package
	Grey level include file

					J. R. Parker
					Laboratory for Computer Vision
					University of Calgary
					Calgary, Alberta, Canada

  ---------------------------------------------------------------------- */

#include <stdio.h>
#include <math.h>
#include <malloc.h>

/* The image header data structure      */
struct header {
	int nr, nc;             /* Rows and columns in the image */
	int oi, oj;             /* Origin */
};

/*      The IMAGE data structure        */
struct image {
		struct header *info;            /* Pointer to header */
		unsigned char **data;           /* Pixel values */
};

#define SQRT2 1.414213562
#ifndef PI
#define PI 3.1415926535
#endif
#define BLACK 0
#define WHITE 1

typedef struct image * IMAGE;

struct pixrec {
	int row, col;
};

typedef struct pixrec * PIXEL;

#if defined (MAX)
int    PBM_SE_ORIGIN_COL=0, PBM_SE_ORIGIN_ROW=0;
char **arg;
int maxargs;
#else
extern int PBM_SE_ORIGIN_COL, PBM_SE_ORIGIN_ROW;
extern char ** arg;
extern int maxargs;
#endif

int range (IMAGE im, int i, int j);
void pset (IMAGE a, int i, int j, int val);
int pget (IMAGE a, int i, int j);
int itop (IMAGE a, int i);
int jtop (IMAGE a, int j);
int ptoi (IMAGE a, int p);
int ptoj (IMAGE a, int p);
void MinMax (IMAGE a, IMAGE b, int *rmax, 
	      int *cmax, int *rmin, int *cmin);
void print_se (IMAGE p);
IMAGE Dilate (IMAGE im, IMAGE p);
IMAGE Erode (IMAGE im, IMAGE p);
IMAGE Input_PBM (char *fn);
IMAGE Output_PBM (IMAGE image, char *filename);
void get_num_pbm (FILE *f, char *b, int *bi, int *res);
void pbm_getln (FILE *f, char *b);
void pbm_param (char *s);
struct image  *newimage (int nr, int nc);
void freeimage (struct image  *z);
IMAGE Complement (IMAGE x);
void myabort (int val, char *mess);
int Attribute (int which, IMAGE z);
void CopyVarImage (IMAGE *a, IMAGE *b);
IMAGE Union (IMAGE a, IMAGE b);
IMAGE Intersection (IMAGE a, IMAGE b);
IMAGE Difference (IMAGE a, IMAGE b);
int SubSet (IMAGE a, IMAGE b);
int ImCompare (IMAGE a, IMAGE b);
int ImValue (IMAGE a, int b);
int PixValue (PIXEL a, PIXEL b);
IMAGE NewImage (IMAGE x);
IMAGE Translate (IMAGE b, PIXEL p);
PIXEL Pixel (int i, int j);
int Member (PIXEL p, IMAGE x);
void Outint (int val, char *name);
void CopyVarPix (PIXEL *a, PIXEL *b);
IMAGE ImageGen (PIXEL p1, PIXEL p2, char *s);
int Isolated (IMAGE a);
IMAGE SetAPixel (IMAGE a, PIXEL p);
PIXEL PixDif (PIXEL p1, PIXEL p2);
int PSubSet (IMAGE a, IMAGE b);
PIXEL PixAdd (PIXEL p1, PIXEL p2);
int inint (int *x, char *f);
void Display (IMAGE x);
