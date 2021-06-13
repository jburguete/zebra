/**
 * \file config.h
 * \brief header file to define configurations.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021, Javier Burguete Tolosa.
 */
#ifndef CONFIG__H
#define CONFIG__H 1

// debug macros

#define DEBUG_CELL 0            ///< macro to debug the cell functions.
#define DEBUG_INLET 0           ///< macro to debug the inlet functions.
#define DEBUG_JUNCTION 0        ///< macro to debug the junction functions.
#define DEBUG_MAIN 0            ///< macro to debug the main functions.
#define DEBUG_NUTRIENT 0        ///< macro to debug the nutrient functions.
#define DEBUG_NETWORK 0         ///< macro to debug the network functions.
#define DEBUG_PIPE 0            ///< macro to debug the pipe functions.
#define DEBUG_RESULTS 0         ///< macro to debug the results functions.
#define DEBUG_SIMULATION 0      ///< macro to debug the simulation functions.
#define DEBUG_SPECIES 0         ///< macro to debug the species functions.
#define DEBUG_WALL 0            ///< macro to debug the wall functions.

// general macros

#define BUFFER_SIZE 512         ///< size of string buffers.
#define DEFAULT_CFL 0.9         ///< default CFL value.
#define LOCALES_DIR "locales"   ///< locale strings directory.
#define MAX_NUTRIENTS 2         ///< maximum number of nutrients.
#define MAX_SPECIES 1           ///< maximum number of species.
#define PIPE_LINE "  Link                  L/s       m/s    /1000m          "
///< pipe line of Epanet output file.
#define PIPE_LENGTH 57          ///< length of the Epanet output file pipe line.
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

#define XML_CELL_SIZE           (const xmlChar *) "cell-size"
///< cell-size XML label.
#define XML_CFL                 (const xmlChar *) "cfl"
///< cfl XML label.
#define XML_CLING               (const xmlChar *) "cling"
///< cling XML label.
#define XML_CONCENTRATION       (const xmlChar *) "concentration"
///< concentration XML label.
#define XML_COORDINATES         (const xmlChar *) "coordinates"
///< coordinates XML label.
#define XML_DECAY               (const xmlChar *) "decay"
///< decay XML label.
#define XML_DISCHARGES          (const xmlChar *) "discharges"
///< discharges XML label.
#define XML_EAT                 (const xmlChar *) "eat"
///< eat XML label.
#define XML_EPANET              (const xmlChar *) "epanet"
///< epanet XML label.
#define XML_FILE                (const xmlChar *) "file"
///< file XML label.
#define XML_FINAL_TIME          (const xmlChar *) "final-time"
///< final-time XML label.
#define XML_GROW                (const xmlChar *) "grow"
///< grow XML label.
#define XML_INITIAL_TIME        (const xmlChar *) "initial-time"
///< initial-time XML label.
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
#define XML_NODE                (const xmlChar *) "node"
///< node XML label.
#define XML_NUMERICAL_ORDER     (const xmlChar *) "numerical-order"
///< numerical-order XML label.
#define XML_NUTRIENT            (const xmlChar *) "nutrient"
///< nutrient XML label.
#define XML_NUTRIENTS           (const xmlChar *) "nutrients"
///< nutrient XML label.
#define XML_PIPE_LENGTH         (const xmlChar *) "pipe-length"
///< pipe-length XML label.
#define XML_RESULTS             (const xmlChar *) "results"
///< results XML label.
#define XML_SAVING_STEP         (const xmlChar *) "saving-step"
///< saving-step XML label.
#define XML_SIMULATION          (const xmlChar *) "simulation"
///< simulation XML label.
#define XML_SPECIES             (const xmlChar *) "species"
///< species XML label.
#define XML_TIME                (const xmlChar *) "time"
///< time XML label.

#endif
