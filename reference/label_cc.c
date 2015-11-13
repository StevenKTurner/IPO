
/** 
 * @file label_cc.c
 * Routines for labeling the connected components in a binary image
 */

#include <image.h>

static void find_union ( int cur_idx, int neigh_idx, int *parent );

/** @cond INTERNAL_FUNCTION */

/* Update either the parent of the current pixel or that of the neighbor */

static void
find_union ( int cur_idx, int neigh_idx, int *parent )
{
 /* find parent */
 while ( cur_idx != parent[cur_idx] )
  {
   cur_idx = parent[cur_idx];
  }

 /* find parent */
 while ( neigh_idx != parent[neigh_idx] )
  {
   neigh_idx = parent[neigh_idx];
  }

 /* merge sets */
 if ( cur_idx > neigh_idx )
  {
   parent[cur_idx] = neigh_idx;
  }
 else
  {
   parent[neigh_idx] = cur_idx;
  }
}

/** @endcond INTERNAL_FUNCTION */

/** 
 * @brief Labels the connected components in a binary image using
 *        the Union-Find data structure
 *
 * @param[in] in_img Image pointer { binary }
 * @param[in] connectivity Connectivity { 4 or 8 }
 *
 * @return Label image or NULL
 *
 * @note 1) This algorithm uses auxiliary storage of size (# pixels in the image).
 *       2) Background is labeled as 0. Connected component labels range from 1 to NUM_CC.
 *       3) 8-connective labeling is significantly slower when compared to 4-connective labeling.
 * @ref Shapiro L. and Stockman G. (2001) "Computer Vision" Prentice-Hall
 *
 * @author M. Emre Celebi
 * @date 06.19.2007
 */

Image *
label_cc ( const Image * in_img, const int connectivity )
{
 SET_FUNC_NAME ( "label_cc" );
 byte *in_data;
 int ir, ic;
 int ik;
 int cur_label;
 int num_rows, num_cols;
 int num_pixels;
 int index, index_north;
 int last_col_idx;
 int value;
 int *out_data;
 int *parent;
 Image *out_img;

 if ( !is_bin_img ( in_img ) )
  {
   ERROR_RET ( "Not a binary image !", NULL );
  }

 if ( connectivity != 4 && connectivity != 8 )
  {
   ERROR ( "Connectivity ( %d ) must be either 4 or 8 !", connectivity );
   return NULL;
  }

 num_rows = get_num_rows ( in_img );
 num_cols = get_num_cols ( in_img );
 num_pixels = num_rows * num_cols;

 in_data = get_img_data_1d ( in_img );

 /* Label image is of INT_1B type */
 out_img = alloc_img ( PIX_INT_1B, num_rows, num_cols );
 if ( IS_NULL ( out_img ) )
  {
   ERROR_RET ( "Insufficient memory !", NULL );
  }

 out_data = get_img_data_1d ( out_img );

 parent = ( int * ) calloc ( num_pixels, sizeof ( int ) );
 if ( IS_NULL ( parent ) )
  {
   ERROR_RET ( "Insufficient memory !", NULL );
  }

 /* Linear index of the current pixel */
 index = 0;
 if ( connectivity == 4 )
  {
   for ( ir = 0; ir < num_rows; ir++ )
    {
     for ( ic = 0; ic < num_cols; ic++ )
      {
       if ( in_data[index] == OBJECT )
	{
	 /* Initialize the parent */
	 parent[index] = index;

	 /* North of current pixel */
	 if ( ( ir != 0 ) && ( in_data[index - num_cols] == OBJECT ) )
	  {
	   /* Update one of the parents */
	   find_union ( index, index - num_cols, parent );
	  }

	 /* West of current pixel */
	 if ( ( ic != 0 ) && ( in_data[index - 1] == OBJECT ) )
	  {
	   /* Update one of the parents */
	   find_union ( index, index - 1, parent );
	  }
	}

       index++;
      }
    }
  }
 else
  {
   /* Index of the last column */
   last_col_idx = num_cols - 1;
   for ( ir = 0; ir < num_rows; ir++ )
    {
     for ( ic = 0; ic < num_cols; ic++ )
      {
       if ( in_data[index] == OBJECT )
	{
	 /* Initialize the parent */
	 parent[index] = index;

	 if ( ir != 0 )
	  {
	   index_north = index - num_cols;

	   /* North of current pixel */
	   if ( in_data[index_north] == OBJECT )
	    {
	     /* Update one of the parents */
	     find_union ( index, index_north, parent );
	    }

	   /* North-West of current pixel */
	   if ( ( ic != 0 ) && ( in_data[index_north - 1] == OBJECT ) )
	    {
	     /* Update one of the parents */
	     find_union ( index, index_north - 1, parent );
	    }

	   /* North-East of current pixel */
	   if ( ( ic != last_col_idx ) &&
		( in_data[index_north + 1] == OBJECT ) )
	    {
	     /* Update one of the parents */
	     find_union ( index, index_north + 1, parent );
	    }
	  }

	 /* West of current pixel */
	 if ( ( ic != 0 ) && ( in_data[index - 1] == OBJECT ) )
	  {
	   /* Update one of the parents */
	   find_union ( index, index - 1, parent );
	  }
	}

       index++;
      }
    }
  }

 /* Start from label 0 */
 cur_label = 0;

 /* Traverse the labels and assign each output pixel a label */
 for ( ik = 0; ik < num_pixels; ik++ )
  {
   value = parent[ik];
   if ( value == ik )
    {
     out_data[ik] = cur_label;
     parent[ik] = cur_label++;
    }
   else
    {
     out_data[ik] = parent[value];
     parent[ik] = parent[value];
    }
  }

 /* 
    Pathological case: There is an object at the origin. This results in this 
    object and the background having the label 0. The remedy is to increment 
    all the object labels by 1. Also CUR_LABEL needs to be incremented.
  */
 if ( in_data[0] == OBJECT )
  {
   for ( ik = 0; ik < num_pixels; ik++ )
    {
     if ( in_data[ik] == OBJECT )
      {
       /* Increment each non-zero label by 1 */
       out_data[ik]++;
      }
    }
   /* Increment the last label ( results in an increment in NUM_CC ) */
   cur_label++;
  }

 /* Determine the number of connected components. Discount the background. */
 out_img->num_cc = cur_label - 1;

 free ( parent );

 return out_img;
}

/** 
 * @brief Retrieves the areas of the connected components in a label image
 *
 * @param[in] lab_img Label image pointer
 *
 * @return An array that contains CC areas or NULL
 *
 * @author M. Emre Celebi
 * @date 06.19.2007
 */

int *
get_cc_areas ( const Image * lab_img )
{
 SET_FUNC_NAME ( "get_cc_areas" );
 int i;
 int num_pixels;
 int *lab_data;
 int *cc_area;

 if ( !is_label_img ( lab_img ) )
  {
   ERROR_RET ( "Not a label image !", NULL );
  }

 num_pixels = get_num_rows ( lab_img ) * get_num_cols ( lab_img );
 lab_data = get_img_data_1d ( lab_img );

 cc_area = ( int * ) calloc ( get_num_cc ( lab_img ) + 1, sizeof ( int ) );
 if ( IS_NULL ( cc_area ) )
  {
   ERROR_RET ( "Insufficient memory !", NULL );
  }

 for ( i = 0; i < num_pixels; i++ )
  {
   cc_area[lab_data[i]]++;
  }

 return cc_area;
}

/** 
 * @brief Removes the connected components that are smaller than a threshold
 *
 * @param[in] lab_img Label image pointer
 * @param[in] area_thresh Area threshold
 *
 * @return Pointer to the new label image or NULL
 *
 * @author M. Emre Celebi
 * @date 06.19.2007
 */

Image *
remove_small_cc ( const Image * lab_img, const int area_thresh )
{
 SET_FUNC_NAME ( "remove_small_cc" );
 int i;
 int num_rows, num_cols;
 int num_pixels;
 int num_cc;
 int label;
 int *cc_area;
 int *new_label;
 int *lab_data;
 int *out_data;
 Bool *remove_cc;
 Image *out_img;

 if ( !is_label_img ( lab_img ) )
  {
   ERROR_RET ( "Not a label image !", NULL );
  }

 if ( area_thresh <= 0 )
  {
   ERROR ( "Area threshold ( %d ) must be positive !", area_thresh );
   return NULL;
  }

 num_rows = get_num_rows ( lab_img );
 num_cols = get_num_cols ( lab_img );
 num_pixels = num_rows * num_cols;
 num_cc = get_num_cc ( lab_img );

 lab_data = get_img_data_1d ( lab_img );

 out_img = alloc_img ( PIX_INT_1B, num_rows, num_cols );
 if ( IS_NULL ( out_img ) )
  {
   ERROR_RET ( "Insufficient memory !", NULL );
  }

 out_data = get_img_data_1d ( out_img );

 /* Get the CC areas */
 cc_area = get_cc_areas ( lab_img );

 remove_cc = ( Bool * ) malloc ( ( num_cc + 1 ) * sizeof ( Bool ) );
 new_label = ( int * ) malloc ( ( num_cc + 1 ) * sizeof ( int ) );
 if ( IS_NULL ( remove_cc ) || IS_NULL ( new_label ) )
  {
   ERROR_RET ( "Insufficient memory !", NULL );
  }

 remove_cc[0] = true;
 label = 1;			/* New labels start from 1 */

 /* Decide on which CC to remove. Relabel the components to be retained. */
 for ( i = 1; i <= num_cc; i++ )
  {
   if ( cc_area[i] < area_thresh )
    {
     remove_cc[i] = true;
    }
   else
    {
     remove_cc[i] = false;
     new_label[i] = label;
     label++;
    }
  }

 out_img->num_cc = label - 1;

 for ( i = 0; i < num_pixels; i++ )
  {
   /* If the CC is to be retained */
   if ( !remove_cc[lab_data[i]] )
    {
     /* Copy it to the output */
     out_data[i] = new_label[lab_data[i]];
    }
  }

 free ( remove_cc );
 free ( new_label );

 return out_img;
}

/** 
 * @brief Retains the largest connected component and removes the remaining
 *
 * @param[in] lab_img Label image pointer
 *
 * @return Pointer to the new label image or NULL
 *
 * @author M. Emre Celebi
 * @date 06.19.2007
 */

Image *
retain_largest_cc ( const Image * lab_img )
{
 SET_FUNC_NAME ( "retain_largest_cc" );
 int i;
 int num_cc;
 int max_area;
 int *cc_area;

 if ( !is_label_img ( lab_img ) )
  {
   ERROR_RET ( "Not a label image !", NULL );
  }

 num_cc = get_num_cc ( lab_img );

 /* Get the CC areas */
 cc_area = get_cc_areas ( lab_img );

 /* Find the maximum CC area. Note that the largest CC might not be unique. */
 max_area = INT_MIN;
 for ( i = 1; i <= num_cc; i++ )
  {
   if ( max_area < cc_area[i] )
    {
     max_area = cc_area[i];
    }
  }

 /* Remove the components smaller than the largest component */
 return remove_small_cc ( lab_img, max_area );
}

/** 
 * @brief Converts a label image to a binary image
 *
 * @param[in] lab_img Label image pointer
 *
 * @return Pointer to the binary image or NULL
 *
 * @author M. Emre Celebi
 * @date 06.19.2007
 */

Image *
label_to_bin ( const Image * lab_img )
{
 SET_FUNC_NAME ( "label_to_bin" );
 byte *bin_data;
 int i;
 int num_rows, num_cols;
 int num_pixels;
 int *lab_data;
 Image *bin_img;

 if ( !is_label_img ( lab_img ) )
  {
   ERROR_RET ( "Not a label image !", NULL );
  }

 num_rows = get_num_rows ( lab_img );
 num_cols = get_num_cols ( lab_img );
 num_pixels = num_rows * num_cols;

 lab_data = get_img_data_1d ( lab_img );

 bin_img = alloc_img ( PIX_BIN, num_rows, num_cols );
 if ( IS_NULL ( bin_img ) )
  {
   ERROR_RET ( "Insufficient memory !", NULL );
  }

 bin_data = get_img_data_1d ( bin_img );

 for ( i = 0; i < num_pixels; i++ )
  {
   /* Object pixels will have positive labels */
   bin_data[i] = ( lab_data[i] > 0 ) ? OBJECT : BACKGROUND;
  }

 return bin_img;
}

/** @cond INTERNAL_FUNCTION */

/* Returns true if there's any object touching the image walls */
Bool
obj_touch_walls ( const Image * img )
{
 byte **data;
 int ir, ic;
 int num_rows, num_cols;

 num_rows = get_num_rows ( img );
 num_cols = get_num_cols ( img );
 data = get_img_data_nd ( img );

 /* For all the columns */
 for ( ic = 0; ic < num_cols; ic++ )
  {
   /* Check whether there is any object touching the top and bottom walls */
   if ( data[0][ic] == OBJECT || data[num_rows - 1][ic] == OBJECT )
    {
     return true;
    }
  }

 /* For all the rows */
 for ( ir = 1; ir < num_rows - 1; ir++ )
  {
   /* Check whether there is any object touching the left and right walls */
   if ( data[ir][0] == OBJECT || data[ir][num_cols - 1] == OBJECT )
    {
     return true;
    }
  }

 return false;
}

/** @endcond INTERNAL_FUNCTION */
