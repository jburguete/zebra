/**
 * \file config.h
 * \brief header file to define configurations.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021, Javier Burguete Tolosa.
 */
#ifndef CONFIG__H
#define CONFIG__H 1

#define LOCALES_DIR "locales"   ///< locale strings directory.
#define PROGRAM_NAME "zebra"    ///< program name.

// debugs

#define DEBUG_NUTRIENT 1        ///< macro to debug the nutrient functions.
#define DEBUG_SPECIES 1         ///< macro to debug the species functions.
#define DEBUG_INLET 1           ///< macro to debug the inlet functions.
#define DEBUG_MAIN 1            ///< macro to debug the main functions.

// XML labels

#define XML_INLET               (const xmlChar *) "inlet"
///< inlet XML label.
#define XML_NAME                (const xmlChar *) "name"
///< name XML label.
#define XML_NUTRIENT            (const xmlChar *) "nutrient"
///< nutrient XML label.
#define XML_SPECIES             (const xmlChar *) "species"
///< species XML label.

#endif
