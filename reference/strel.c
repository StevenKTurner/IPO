
/** 
 * @file strel.c
 * Routines for creation and manipulation of a structuring element
 */

#include <image.h>

/** 
 * @brief Checks whether or not the origin of a 
 *        structuring element coincides with its center
 *
 * @param[in] se Structuring element pointer
 *
 * @return true or false
 * 
 * @author M. Emre Celebi
 * @date 01.20.2008
 */

Bool
se_origin_at_center ( const Strel * se )
{
 SET_FUNC_NAME ( "se_origin_at_center" );

 if ( !IS_VALID_OBJ ( se ) )
  {
   ERROR_RET ( "Invalid structuring element object !", false );
  }

 if ( ( se->origin_row == se->num_rows / 2 ) &&
      ( se->origin_col == se->num_cols / 2 ) )
  {
   return true;
  }

 return false;
}

/** 
 * @brief Sets the origin of a structuring element
 *
 * @param[in] origin_row Row coordinate of the origin
 * @param[in] origin_col Column coordinate of the origin
 * @param[in,out] se Structuring element pointer
 *
 * @return none
 * @note This routine modifies the input structuring element !
 * 
 * @author M. Emre Celebi
 * @date 01.20.2008
 */

void
set_strel_origin ( const int origin_row, const int origin_col, Strel * se )
{
 SET_FUNC_NAME ( "set_strel_origin" );

 if ( !IS_VALID_OBJ ( se ) )
  {
   ERROR ( "Invalid structuring element object !" );
  }

 if ( origin_row < 0 || origin_row >= se->num_rows ||
      origin_col < 0 || origin_col >= se->num_cols )
  {
   ERROR
    ( "Structuring Element origin ( %d, %d ) must be in ( [0..%d], [0..%d] ) !",
      origin_row, origin_col, se->num_rows - 1, se->num_cols - 1 );
  }

 se->origin_row = origin_row;
 se->origin_col = origin_col;
}

/** 
 * @brief Prints a structuring element
 *
 * @param[in] se Structuring element pointer
 *
 * @return none
 * 
 * @author M. Emre Celebi
 * @date 01.20.2008
 */

void
print_strel ( const Strel * se )
{
 SET_FUNC_NAME ( "print_strel" );
 int ir, ic;

 if ( !IS_VALID_OBJ ( se ) )
  {
   ERROR ( "Invalid structuring element object !" );
  }

 for ( ir = 0; ir < se->num_rows; ir++ )
  {
   for ( ic = 0; ic < se->num_cols; ic++ )
    {
     printf ( "%d ", se->data_2d[ir][ic] );
    }
   printf ( "\n" );
  }
}

static void
draw_line_strel ( const int y1, const int x1, const int y2, const int x2,
		  byte ** data )
{
 int d, x, y, ax, ay, sx, sy, dx, dy;

 dx = x2 - x1;
 ax = abs ( dx ) << 1;
 sx = ( dx < 0 ) ? -1 : 1;
 dy = y2 - y1;
 ay = abs ( dy ) << 1;
 sy = ( dy < 0 ) ? -1 : 1;

 x = x1;
 y = y1;

 if ( ax > ay )			/* x dominant */
  {
   d = ay - ( ax >> 1 );
   for ( ;; )
    {
     data[y][x] = OBJECT;
     if ( x == x2 )
      {
       return;
      }
     if ( d >= 0 )
      {
       y += sy;
       d -= ax;
      }
     x += sx;
     d += ay;
    }
  }
 else				/* y dominant */
  {
   d = ax - ( ay >> 1 );
   for ( ;; )
    {
     data[y][x] = OBJECT;
     if ( y == y2 )
      {
       return;
      }
     if ( d >= 0 )
      {
       x += sx;
       d -= ay;
      }
     y += sy;
     d += ax;
    }
  }
}

/** 
 * @brief Creates a flat, linear structuring element
 *
 * @param[in] length Length of the line { positive }
 * @param[in] angle  Angle (in degrees) of the line 
 *
 * @return Pointer to the structuring element or NULL
 * @note 1) LENGTH is approximately the distance between the ends of the line. 
 *       2) ANGLE indicates the orientation of the line as measured in a 
 *          counterclockwise direction from the horizontal axis.
 *       3) The line is symmetric about the origin.
 * 
 * @author M. Emre Celebi
 * @date 01.20.2008
 */

Strel *
make_line_strel ( const int length, const double angle )
{
 SET_FUNC_NAME ( "make_line_strel" );
 double row_offset;
 double col_offset;
 Strel *se;

 if ( length <= 0 )
  {
   ERROR ( "Structuring Element length ( %d ) must be positive !", length );
   return NULL;
  }

 se = MALLOC_STRUCT ( Strel );
 se->type = STRL_LINE;
 row_offset =
  ( int ) ROUND ( 0.5 * ( length - 1 ) * sin ( DEG_TO_RAD ( angle ) ) );
 col_offset =
  ( int ) ROUND ( 0.5 * ( length - 1 ) * cos ( DEG_TO_RAD ( angle ) ) );
 se->num_rows = 2 * fabs ( row_offset ) + 1;
 se->num_cols = 2 * fabs ( col_offset ) + 1;
 se->origin_row = se->num_rows / 2;
 se->origin_col = se->num_cols / 2;

 /* The line is horizontal */
 if ( se->num_rows == 0 )
  {
   se->num_rows = 1;
  }

 /* The line is vertical */
 if ( se->num_cols == 0 )
  {
   se->num_cols = 1;
  }

 se->data_2d = alloc_nd ( sizeof ( byte ), 2, se->num_rows, se->num_cols );
 if ( IS_NULL ( se->data_2d ) )
  {
   ERROR_RET ( "Insufficient memory !", NULL );
  }
 se->data_1d = *( se->data_2d );

 /* Draw lines from the origin to the extreme points */
 draw_line_strel ( se->origin_row, se->origin_col,
		   se->origin_row + row_offset, se->origin_col - col_offset,
		   se->data_2d );

 draw_line_strel ( se->origin_row, se->origin_col,
		   se->origin_row - row_offset, se->origin_col + col_offset,
		   se->data_2d );

 return se;
}

/** 
 * @brief Creates a flat, rectangle-shaped structuring element
 *
 * @param[in] num_rows # rows    { positive-odd }
 * @param[in] num_cols # columns { positive-odd }
 *
 * @return Pointer to the structuring element or NULL
 * @note The origin is set to ( NUM_ROWS / 2, NUM_COLS / 2 )
 * 
 * @author M. Emre Celebi
 * @date 01.20.2008
 */

Strel *
make_rect_strel ( const int num_rows, const int num_cols )
{
 SET_FUNC_NAME ( "make_rect_strel" );
 Strel *se;

 if ( !IS_POS_ODD ( num_rows ) || !IS_POS_ODD ( num_cols ) )
  {
   ERROR
    ( "Structuring element dimensions ( %d, %d ) must be positive and odd !",
      num_rows, num_cols );
   return NULL;
  }

 se = MALLOC_STRUCT ( Strel );
 se->type = STRL_RECT;
 se->num_rows = num_rows;
 se->num_cols = num_cols;
 se->origin_row = num_rows / 2;
 se->origin_col = num_cols / 2;

 se->data_2d = alloc_nd ( sizeof ( byte ), 2, num_rows, num_cols );
 if ( IS_NULL ( se->data_2d ) )
  {
   ERROR_RET ( "Insufficient memory !", NULL );
  }
 se->data_1d = *( se->data_2d );

 /* Set all pixels of the se to 1 */
 memset ( se->data_1d, OBJECT, num_rows * num_cols * sizeof ( byte ) );

 return se;
}

/** 
 * @brief Creates a flat, disk-shaped structuring element
 *
 * @param[in] radius Radius of the disk { positive }
 *
 * @return Pointer to the structuring element or NULL
 * @note 1) The size of the structuring element is: 
 *          ( 2 * RADIUS + 1 ) x ( 2 * RADIUS + 1 )
 *       2) The origin is set to ( RADIUS, RADIUS ).
 * 
 * @author M. Emre Celebi
 * @date 01.20.2008
 */

Strel *
make_disk_strel ( const int radius )
{
 SET_FUNC_NAME ( "make_disk_strel" );
 int ir, ic;
 double radius_sq;
 Strel *se;

 if ( radius <= 0 )
  {
   ERROR ( "Structuring Element radius ( %d ) must be positive !", radius );
   return NULL;
  }

 se = MALLOC_STRUCT ( Strel );
 se->type = STRL_DISK;
 se->num_rows = 2 * radius + 1;
 se->num_cols = 2 * radius + 1;
 se->origin_row = radius;
 se->origin_col = radius;

 se->data_2d = alloc_nd ( sizeof ( byte ), 2, se->num_rows, se->num_cols );
 if ( IS_NULL ( se->data_2d ) )
  {
   ERROR_RET ( "Insufficient memory !", NULL );
  }
 se->data_1d = *( se->data_2d );

 /* Set all pixels of the se to 1 */
 memset ( se->data_1d, OBJECT, se->num_rows * se->num_cols * sizeof ( byte ) );

 /* Set pixels outside the disk to 0 */
 radius_sq = radius * radius;
 for ( ir = -radius; ir <= radius; ir++ )
  {
   for ( ic = -radius; ic <= radius; ic++ )
    {
     if ( radius_sq < ir * ir + ic * ic )
      {
       se->data_2d[ir + radius][ic + radius] = BACKGROUND;
      }
    }
  }

 return se;
}

/** 
 * @brief Creates a flat structuring element
 *
 * @param[in] data Structuring element neighborhood
 * @param[in] num_rows # rows    { positive-odd }
 * @param[in] num_cols # columns { positive-odd }
 * @param[in] origin_row Row coordinate of the origin
 * @param[in] origin_col Column coordinate of the origin
 *
 * @return Pointer to the structuring element or NULL
 * @note 1) DATA elements can only be 0 or 1.
 *       2) The pixel in DATA that corresponds 
 *          to the origin must have the value 1.
 * 
 * @author M. Emre Celebi
 * @date 01.20.2008
 */

Strel *
make_flat_strel ( byte ** data, const int num_rows, const int num_cols,
		  const int origin_row, const int origin_col )
{
 SET_FUNC_NAME ( "make_flat_strel" );
 int ir, ic;
 int value;
 Strel *se;

 if ( IS_NULL ( data ) )
  {
   ERROR_RET ( "Invalid structuring element data !", NULL );
  }

 if ( !IS_POS_ODD ( num_rows ) || !IS_POS_ODD ( num_cols ) )
  {
   ERROR
    ( "Structuring element dimensions ( %d, %d ) must be positive and odd !",
      num_rows, num_cols );
   return NULL;
  }

 if ( origin_row < 0 || origin_row >= num_rows ||
      origin_col < 0 || origin_col >= num_cols )
  {
   ERROR
    ( "Structuring Element origin ( %d, %d ) must be in ( [0..%d], [0..%d] ) !",
      origin_row, origin_col, num_rows - 1, num_cols - 1 );
   return NULL;
  }

 se = MALLOC_STRUCT ( Strel );
 se->type = STRL_FLAT;
 se->num_rows = num_rows;
 se->num_cols = num_cols;
 se->origin_row = origin_row;
 se->origin_col = origin_col;

 se->data_2d = alloc_nd ( sizeof ( byte ), 2, num_rows, num_cols );
 if ( IS_NULL ( se->data_2d ) )
  {
   ERROR_RET ( "Insufficient memory !", NULL );
  }
 se->data_1d = *( se->data_2d );

 for ( ir = 0; ir < num_rows; ir++ )
  {
   for ( ic = 0; ic < num_cols; ic++ )
    {
     value = data[ir][ic];
     if ( value > 1 )		/* No need to check the negative case (byte) */
      {
       ERROR
	( "Structuring element is non-flat ( value = %d ) at position ( %d, %d ) !",
	  value, ir, ic );
       return NULL;
      }
     se->data_2d[ir][ic] = value;
    }
  }

 if ( se->data_2d[origin_row][origin_col] != OBJECT )
  {
   ERROR ( "Structuring element origin ( %d, %d ) must be part of the object !",
	   origin_row, origin_col );
   return NULL;
  }

 return se;
}
