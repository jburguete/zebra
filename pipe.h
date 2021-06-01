#ifndef PIPE__H
#define PIPE__H 1

/**
 * \struct Pipe
 * \brief struct to define an irrigation pipe.
 */
typedef struct
{
  Cell *cell;                   ///< array of node cells.
	Point *point;                 ///< array of points.
	double length;                ///< length.
	double area;                  ///< cross sectional area.
	double discharge;             ///< discharge.
	double velocity;              ///< flow velocity.
	unsigned int ncells;          ///< number of node cells.
	unsigned int npoints;         ///< number of points.
} Pipe;

#endif
