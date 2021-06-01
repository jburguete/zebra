#ifndef CELL__H
#define CELL__H 1

/**
 * \struct Cell
 * \brief struct to define a node cell for calculation.
 */
typedef struct
{
  double position;              ///< position in the pipe.
	double area;                  ///< cross sectional area.
  double velocity;              ///< flow velocity.
  double dispersion;            ///< dispersion coefficient.
	double roughness;             ///< roughness length.
	double friction;              ///< friction factor.
	double density;               ///< density.
	double decay;                 ///< decay coefficient.
	double infestation;           ///< infestation rate.
} Cell;

#endif
