#include "morph.h"

void main (int argc, char *argv[])
{
	SE p;
	IMAGE im;
	int k;

	if (argc < 4)
	{
	  printf ("BinErode <input> <Structuring element> <output>\n");
	  exit(1);
	}

/* Read the structuring element */
	k = get_se (argv[2], &p);
	if (!k)
	{
	  printf ("Bad structuring element, file name '%s'.\n", argv[2]);
	  exit (2);
	}
	printf ("BinErode: Perform a binary erosion on image '%s'.\n", 
		argv[1]);
	printf ("Structuring element is:\n");
	print_se (p);

/* Read the input image */
	if (read_pbm (argv[1], &im) == 0) exit(3);

/* Perform the erosion */
	bin_erode (im, p);

/* Write the result to the specified file */
	write_pbm (argv[3], im);
}

