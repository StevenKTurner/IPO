/*----------------------------------------------------------------------

	MLIB - General Digital Morphology Package

					J. R. Parker
					Laboratory for Computer Vision
					University of Calgary
					Calgary, Alberta, Canada

  ---------------------------------------------------------------------- */
#define MORPHOLOGY_INCLUDE
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

struct se_struct {
	unsigned char **data;
	int nr, nc, oi, oj;
};

typedef struct image * IMAGE;
typedef struct se_struct * SE;

#if defined(MLIB)
int    PBM_SE_ORIGIN_COL=0, PBM_SE_ORIGIN_ROW=0;
#else 
extern int PBM_SE_ORIGIN_COL, PBM_SE_ORIGIN_ROW;
#endif

int get_se (char *filename, SE *p);
int print_se (SE p);
void dil_apply (IMAGE im, SE p, int ii, int jj, IMAGE res);
int bin_dilate (IMAGE im, SE p);
void erode_apply (IMAGE im, SE p, int ii, int jj, IMAGE res);
int bin_erode (IMAGE im, SE p);
IMAGE newimage (int nr, int nc);
void freeimage (IMAGE im);
int read_pbm (char *fn, IMAGE *im);
void pbm_param (char *s);
void write_pbm (char *filename, IMAGE image);
void pbm_getln (FILE *f, char *b);
void get_num_pbm (FILE *f, char *b, int *bi, int *res);
int range (IMAGE im, int i, int j);
