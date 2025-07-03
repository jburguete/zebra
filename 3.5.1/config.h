/**
 * \file config.h
 * \brief header file to define configurations.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021-2024, Javier Burguete Tolosa.
 */
#ifndef CONFIG__H
#define CONFIG__H 1

// debug macros

#define DEBUG_CELL 0            ///< macro to debug the cell functions.
#define DEBUG_INLET 0           ///< macro to debug the inlet functions.
#define DEBUG_JUNCTION 0        ///< macro to debug the junction functions.
#define DEBUG_MAIN 0            ///< macro to debug the main functions.
#define DEBUG_NETWORK 0         ///< macro to debug the network functions.
#define DEBUG_PIPE 0            ///< macro to debug the pipe functions.
#define DEBUG_RESULTS 0         ///< macro to debug the results functions.
#define DEBUG_SIMULATION 0      ///< macro to debug the simulation functions.
#define DEBUG_SOLUTE 0          ///< macro to debug the solute functions.
#define DEBUG_SPECIES 0         ///< macro to debug the species functions.
#define DEBUG_SPECIMEN 0        ///< macro to debug the specimen functions.
#define DEBUG_TEMPERATURE 0     ///< macro to debug the temperautre functions.
#define DEBUG_WALL 0            ///< macro to debug the wall functions.

// general macros

#define BUFFER_SIZE 512         ///< size of string buffers.
#define DEFAULT_CFL 0.9         ///< default CFL value.
#define DEFAULT_DISPERSION_CFL 10.      ///< default dispersion CFL value.
#define DEFAULT_SEED 7
///< default sed of the pseudo-random numbers generator.
#define DEFAULT_TEMPERATURE 20. ///< default temperature.
#define FLUX_LIMITER 3
///< flux limiter (0:null, 1:upwind, 2:centred, 3:monotonized central)
#define FMTL "%16s"             ///< format of identifier labels.
#define LOCALES_DIR "locales"   ///< locale strings directory.
#define MAX_LABEL_LENGTH 16     ///< maximum identifier label length.
#define MAX_SOLUTES 4           ///< maximum number of solutes.
#define MAX_SPECIES 1           ///< maximum number of species.
#define PIPE_LINE "  Link                  L/s       m/s    /1000m          "
///< pipe line of Epanet output file.
#define PIPE_LINE_RPT "  Link                 m3/h       m/s    /1000m"
///< pipe line of Epanet output file (RPT format).
#define PIPE_LENGTH 57          ///< length of the Epanet output file pipe line.
#define PIPE_LENGTH_RPT 47
///< length of the Epanet output file pipe line (RPT format).
#define PIPE_LENGTHS_SAVE 0
///< macro to save the pipe lengths on a log file.
#define PIPE_LENGTHS_FILE "pipe-lengths.log"
///< name of the pipe lengths log file.
#define PROGRAM_NAME "zebra"    ///< program name.
#define RECIRCULATION_LENGTH 8. ///< length of the recirculation zone.
#define WATER_VISCOSITY 1.0e-3  ///< water viscosity.

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

#define XML_ADULT_DECAY         (const xmlChar *) "adult-decay"
///< adult-decay XML label.
#define XML_ADULT_DECAY_CHLORINE \
  (const xmlChar *) "adult-decay-chlorine"
///< adult-decay-chlorine XML label.
#define XML_ADULT_DECAY_HYDROGEN_PEROXIDE \
  (const xmlChar *) "adult-decay-hydrogen-peroxide"
///< adult-decay-hdrogen-peroxide XML label.
#define XML_ADULT_MAXIMUM_CHLORINE \
  (const xmlChar *) "adult-maximum-chlorine"
///< adult-maximum-chlorine XML label.
#define XML_ADULT_MAXIMUM_HYDROGEN_PEROXIDE \
  (const xmlChar *) "adult_maximum-hydrogen-peroxide"
///< adult-maximum-hydrogen-peroxide XML label.
#define XML_ADULT_MINIMUM_CHLORINE \
  (const xmlChar *) "adult-minimum-chlorine"
///< adult-minimum-chlorine XML label.
#define XML_ADULT_MINIMUM_HYDROGEN_PEROXIDE \
  (const xmlChar *) "adult_minimum-hydrogen-peroxide"
///< adult-minimum-hydrogen-peroxide XML label.
#define XML_BIOLOGICAL_STEP     (const xmlChar *) "biological-step"
///< biological-step XML label.
#define XML_CELL_SIZE           (const xmlChar *) "cell-size"
///< cell-size XML label.
#define XML_CFL                 (const xmlChar *) "cfl"
///< cfl XML label.
#define XML_CHLORINE            (const xmlChar *) "chlorine"
///< chlorine XML label.
#define XML_SETTLEMENT               (const xmlChar *) "settlement"
///< settlement XML label.
#define XML_CONCENTRATION       (const xmlChar *) "concentration"
///< concentration XML label.
#define XML_COORDINATES         (const xmlChar *) "coordinates"
///< coordinates XML label.
#define XML_DECAY_SURFACE       (const xmlChar *) "decay-surface"
///< decay-surface XML label.
#define XML_DECAY_TEMPERATURE   (const xmlChar *) "decay-temperature"
///< decay-temperature XML label.
#define XML_DECAY_TIME          (const xmlChar *) "decay-time"
///< time-decay XML label.
#define XML_DISCHARGES          (const xmlChar *) "discharges"
///< discharges XML label.
#define XML_DISPERSION_CFL      (const xmlChar *) "dispersion-cfl"
///< dispersion-cfl XML label.
#define XML_DISPERSION_MODEL    (const xmlChar *) "dispersion-model"
///< dispersion-model XML label.
#define XML_EAT                 (const xmlChar *) "eat"
///< eat XML label.
#define XML_FILE                (const xmlChar *) "file"
///< file XML label.
#define XML_FINAL_TIME          (const xmlChar *) "final-time"
///< final-time XML label.
#define XML_GROW                (const xmlChar *) "grow"
///< grow XML label.
#define XML_HYDROGEN_PEROXIDE   (const xmlChar *) "hydrogen-peroxide"
///< hydrogen-peroxide XML label.
#define XML_ID                  (const xmlChar *) "id"
///< id XML label.
#define XML_INITIAL_CONDITIONS  (const xmlChar *) "initial-conditions"
///< initial-conditions XML label.
#define XML_INITIAL_TIME        (const xmlChar *) "initial-time"
///< initial-time XML label.
#define XML_INJECT              (const xmlChar *) "inject"
///< inject XML label.
#define XML_INLET               (const xmlChar *) "inlet"
///< inlet XML label.
#define XML_JUVENILE_AGE        (const xmlChar *) "juvenile-age"
///< juvenile-age XML label.
#define XML_LARVA_DECAY         (const xmlChar *) "larva-decay"
///< larva-decay XML label.
#define XML_LARVA_DECAY_CHLORINE \
  (const xmlChar *) "larva-decay-chlorine"
///< larva-decay-chlorine XML label.
#define XML_LARVA_DECAY_HYDROGEN_PEROXIDE \
  (const xmlChar *) "larva-decay-hydrogen-peroxide"
///< larva-decay-hdrogen-peroxide XML label.
#define XML_LARVA_MAXIMUM_CHLORINE \
  (const xmlChar *) "larva-maximum-chlorine"
///< larva-maximum-chlorine XML label.
#define XML_LARVA_MAXIMUM_HYDROGEN_PEROXIDE \
  (const xmlChar *) "larva_maximum-hydrogen-peroxide"
///< larva-maximum-hydrogen-peroxide XML label.
#define XML_LARVA_MINIMUM_CHLORINE \
  (const xmlChar *) "larva-minimum-chlorine"
///< larva-minimum-chlorine XML label.
#define XML_LARVA_MINIMUM_HYDROGEN_PEROXIDE \
  (const xmlChar *) "larva_minimum-hydrogen-peroxide"
///< larva-minimum-hydrogen-peroxide XML label.
#define XML_LENGTH              (const xmlChar *) "length"
///< length XML label.
#define XML_MAP                 (const xmlChar *) "map"
///< map XML label.
#define XML_MAXIMUM_TEMPERATURE (const xmlChar *) "maximum-temperature"
///< maximum-temperature XML label.
#define XML_MAXIMUM_VELOCITY    (const xmlChar *) "maximum-velocity"
///< maximum-velocity XML label.
#define XML_MINIMUM_OXYGEN      (const xmlChar *) "minimum-oxygen"
///< minimum-oxygen XML label.
#define XML_MINIMUM_TEMPERATURE (const xmlChar *) "minimum-temperature"
///< minimum-temperature XML label.
#define XML_NAME                (const xmlChar *) "name"
///< name XML label.
#define XML_NETWORK             (const xmlChar *) "network"
///< network XML label.
#define XML_NODE                (const xmlChar *) "node"
///< node XML label.
#define XML_NUMERICAL_ORDER     (const xmlChar *) "numerical-order"
///< numerical-order XML label.
#define XML_OPTIMAL_TEMPERATURE (const xmlChar *) "optimal-temperature"
///< optimal-temperature XML label.
#define XML_SEED                (const xmlChar *) "seed"
///< seed XML label.
#define XML_SOLUTE              (const xmlChar *) "solute"
///< solute XML label.
#define XML_SOLUTES             (const xmlChar *) "solutes"
///< solute XML label.
#define XML_ORGANIC_MATTER      (const xmlChar *) "organic-matter"
///< organic-matter XML label.
#define XML_OXYGEN              (const xmlChar *) "oxygen"
///< oxygen XML label.
#define XML_PIPE                (const xmlChar *) "pipe"
///< pipe XML label.
#define XML_PIPE_LENGTH         (const xmlChar *) "pipe-length"
///< pipe-length XML label.
#define XML_POINT               (const xmlChar *) "point"
///< point XML label.
#define XML_RESPIRATION         (const xmlChar *) "respiration"
///< respiration XML label.
#define XML_RESULTS             (const xmlChar *) "results"
///< results XML label.
#define XML_RUTHERFORD          (const xmlChar *) "rutherford"
///< rutherford XML label.
#define XML_SAVING_STEP         (const xmlChar *) "saving-step"
///< saving-step XML label.
#define XML_SET                 (const xmlChar *) "set"
///< set XML label.
#define XML_SIMULATION          (const xmlChar *) "simulation"
///< simulation XML label.
#define XML_SOLUBILITY          (const xmlChar *) "solubility"
///< solubility XML label.
#define XML_SPECIES             (const xmlChar *) "species"
///< species XML label.
#define XML_SUMMARY             (const xmlChar *) "summary"
///< summary XML label.
#define XML_TEMPERATURE         (const xmlChar *) "temperature"
///< temperature XML label.
#define XML_TIME                (const xmlChar *) "time"
///< time XML label.
#define XML_TYPE                (const xmlChar *) "type"
///< type XML label.
#define XML_ZEBRA_MUSSEL        (const xmlChar *) "zebra-mussel"
///< zebra-mussel XML label.

#endif
