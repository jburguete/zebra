/**
 * \file specimen.c
 * \brief source file to define the specimen.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021-2024, Javier Burguete Tolosa.
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <libintl.h>
#include <libxml/parser.h>
#include <glib.h>
#include <gsl/gsl_rng.h>
#include "config.h"
#include "tools.h"
#include "temperature.h"
#include "solute.h"
#include "species.h"
#include "specimen.h"
