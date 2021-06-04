/**
 * \file config.h
 * \brief header file to define configurations.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021, Javier Burguete Tolosa.
 */
#ifndef CONFIG__H
#define CONFIG__H 1

// debug macros

#define DEBUG_INLET 1           ///< macro to debug the inlet functions.
#define DEBUG_MAIN 1            ///< macro to debug the main functions.
#define DEBUG_NUTRIENT 1        ///< macro to debug the nutrient functions.
#define DEBUG_NETWORK 1         ///< macro to debug the network functions.
#define DEBUG_OUTLET 1          ///< macro to debug the outlet functions.
#define DEBUG_SPECIES 1         ///< macro to debug the species functions.

// general macros

#define BUFFER_SIZE 512         ///< size of string buffers.
#define LOCALES_DIR "locales"   ///< locale strings directory.
#define PIPE_LENGTHS_SAVE 1
///< macro to save the pipe lengths on a log file.
#define PIPE_LENGTHS_FILE "pipe-lengths.log"
///< name of the pipe lengths log file.
#define PROGRAM_NAME "zebra"    ///< program name.

// Epanet labels
#define EPANET_COORDINATES      "[COORDINATES]"
///< [COORDINATES] Epanet label.
#define EPANET_END              "[END]"
///< [END] Epanet label.
#define EPANET_JUNCTIONS        "[JUNCTIONS]"
///< [JUNCTIONS] Epanet label.
#define EPANET_PIPES            "[PIPES]"
///< [PIPES] Epanet label.
#define EPANET_RESERVOIRS       "[RESERVOIRS]"
///< [RESERVOIRS] Epanet label.
#define EPANET_TITLE            "[TITLE]"
///< [TITLE] Epanet label.

// XML labels

#define XML_CLING               (const xmlChar *) "cling"
///< cling XML label.
#define XML_CONCENTRATION       (const xmlChar *) "concentration"
///< concentration XML label.
#define XML_COORDINATES         (const xmlChar *) "coordinates"
///< coordinates XML label.
#define XML_DECAY               (const xmlChar *) "decay"
///< decay XML label.
#define XML_DISCHARGE           (const xmlChar *) "discharge"
///< discharge XML label.
#define XML_EAT                 (const xmlChar *) "eat"
///< eat XML label.
#define XML_EPANET              (const xmlChar *) "epanet"
///< epanet XML label.
#define XML_GROW                (const xmlChar *) "grow"
///< grow XML label.
#define XML_INLET               (const xmlChar *) "inlet"
///< inlet XML label.
#define XML_LENGTH              (const xmlChar *) "length"
///< length XML label.
#define XML_MAXIMUM_VELOCITY    (const xmlChar *) "maximum-velocity"
///< maximum-velocity XML label.
#define XML_NAME                (const xmlChar *) "name"
///< name XML label.
#define XML_NETWORK             (const xmlChar *) "network"
///< network XML label.
#define XML_NUTRIENT            (const xmlChar *) "nutrient"
///< nutrient XML label.
#define XML_OUTLET              (const xmlChar *) "outlet"
///< outlet XML label.
#define XML_PIPE_LENGTH         (const xmlChar *) "pipe-length"
///< pipe-length XML label.
#define XML_SPECIES             (const xmlChar *) "species"
///< species XML label.
#define XML_TIME                (const xmlChar *) "time"
///< time XML label.

#endif
