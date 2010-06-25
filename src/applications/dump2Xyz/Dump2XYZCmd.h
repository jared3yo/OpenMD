/** @file Dump2XYZCmd.h
 *  @brief The header file for the command line option parser
 *  generated by GNU Gengetopt version 2.22
 *  http://www.gnu.org/software/gengetopt.
 *  DO NOT modify this file, since it can be overwritten
 *  @author GNU Gengetopt by Lorenzo Bettini */

#ifndef DUMP2XYZCMD_H
#define DUMP2XYZCMD_H

/* If we use autoconf.  */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h> /* for FILE */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef CMDLINE_PARSER_PACKAGE
/** @brief the program name */
#define CMDLINE_PARSER_PACKAGE "Dump2XYZ"
#endif

#ifndef CMDLINE_PARSER_VERSION
/** @brief the program version */
#define CMDLINE_PARSER_VERSION "0.99"
#endif

/** @brief Where the command line options are stored */
struct gengetopt_args_info
{
  const char *help_help; /**< @brief Print help and exit help description.  */
  const char *version_help; /**< @brief Print version and exit help description.  */
  char * input_arg;	/**< @brief input dump file.  */
  char * input_orig;	/**< @brief input dump file original value given at command line.  */
  const char *input_help; /**< @brief input dump file help description.  */
  char * output_arg;	/**< @brief output file name.  */
  char * output_orig;	/**< @brief output file name original value given at command line.  */
  const char *output_help; /**< @brief output file name help description.  */
  int frame_arg;	/**< @brief print every n frame (default='1').  */
  char * frame_orig;	/**< @brief print every n frame original value given at command line.  */
  const char *frame_help; /**< @brief print every n frame help description.  */
  int water_flag;	/**< @brief skip the the waters (default=off).  */
  const char *water_help; /**< @brief skip the the waters help description.  */
  int periodicBox_flag;	/**< @brief map to the periodic box (default=off).  */
  const char *periodicBox_help; /**< @brief map to the periodic box help description.  */
  int zconstraint_flag;	/**< @brief replace the atom types of zconstraint molecules (default=off).  */
  const char *zconstraint_help; /**< @brief replace the atom types of zconstraint molecules help description.  */
  int rigidbody_flag;	/**< @brief add a pseudo COM atom to rigidbody (default=off).  */
  const char *rigidbody_help; /**< @brief add a pseudo COM atom to rigidbody help description.  */
  int watertype_flag;	/**< @brief replace the atom type of water model (default=on).  */
  const char *watertype_help; /**< @brief replace the atom type of water model help description.  */
  char * selection_arg;	/**< @brief general selection syntax.  */
  char * selection_orig;	/**< @brief general selection syntax original value given at command line.  */
  const char *selection_help; /**< @brief general selection syntax help description.  */
  char * originsele_arg;	/**< @brief select origin.  */
  char * originsele_orig;	/**< @brief select origin original value given at command line.  */
  const char *originsele_help; /**< @brief select origin help description.  */
  char * refsele_arg;	/**< @brief select reference.  */
  char * refsele_orig;	/**< @brief select reference original value given at command line.  */
  const char *refsele_help; /**< @brief select reference help description.  */
  int repeatX_arg;	/**< @brief The number of images to repeat in the x direction (default='0').  */
  char * repeatX_orig;	/**< @brief The number of images to repeat in the x direction original value given at command line.  */
  const char *repeatX_help; /**< @brief The number of images to repeat in the x direction help description.  */
  int repeatY_arg;	/**< @brief The number of images to repeat in the y direction (default='0').  */
  char * repeatY_orig;	/**< @brief The number of images to repeat in the y direction original value given at command line.  */
  const char *repeatY_help; /**< @brief The number of images to repeat in the y direction help description.  */
  int repeatZ_arg;	/**< @brief The number of images to repeat in the z direction (default='0').  */
  char * repeatZ_orig;	/**< @brief The number of images to repeat in the z direction original value given at command line.  */
  const char *repeatZ_help; /**< @brief The number of images to repeat in the z direction help description.  */
  int basetype_flag;	/**< @brief Convert to base atom type (default=off).  */
  const char *basetype_help; /**< @brief Convert to base atom type help description.  */
  int velocities_flag;	/**< @brief Print velocities in xyz file (default=off).  */
  const char *velocities_help; /**< @brief Print velocities in xyz file help description.  */
  int forces_flag;	/**< @brief Print forces xyz file (default=off).  */
  const char *forces_help; /**< @brief Print forces xyz file help description.  */
  int vectors_flag;	/**< @brief Print vectors (dipoles, etc) in xyz file (default=off).  */
  const char *vectors_help; /**< @brief Print vectors (dipoles, etc) in xyz file help description.  */
  int charges_flag;	/**< @brief Print charges in xyz file (default=off).  */
  const char *charges_help; /**< @brief Print charges in xyz file help description.  */
  
  unsigned int help_given ;	/**< @brief Whether help was given.  */
  unsigned int version_given ;	/**< @brief Whether version was given.  */
  unsigned int input_given ;	/**< @brief Whether input was given.  */
  unsigned int output_given ;	/**< @brief Whether output was given.  */
  unsigned int frame_given ;	/**< @brief Whether frame was given.  */
  unsigned int water_given ;	/**< @brief Whether water was given.  */
  unsigned int periodicBox_given ;	/**< @brief Whether periodicBox was given.  */
  unsigned int zconstraint_given ;	/**< @brief Whether zconstraint was given.  */
  unsigned int rigidbody_given ;	/**< @brief Whether rigidbody was given.  */
  unsigned int watertype_given ;	/**< @brief Whether watertype was given.  */
  unsigned int selection_given ;	/**< @brief Whether selection was given.  */
  unsigned int originsele_given ;	/**< @brief Whether originsele was given.  */
  unsigned int refsele_given ;	/**< @brief Whether refsele was given.  */
  unsigned int repeatX_given ;	/**< @brief Whether repeatX was given.  */
  unsigned int repeatY_given ;	/**< @brief Whether repeatY was given.  */
  unsigned int repeatZ_given ;	/**< @brief Whether repeatZ was given.  */
  unsigned int basetype_given ;	/**< @brief Whether basetype was given.  */
  unsigned int velocities_given ;	/**< @brief Whether velocities was given.  */
  unsigned int forces_given ;	/**< @brief Whether forces was given.  */
  unsigned int vectors_given ;	/**< @brief Whether vectors was given.  */
  unsigned int charges_given ;	/**< @brief Whether charges was given.  */

  char **inputs ; /**< @brief unamed options (options without names) */
  unsigned inputs_num ; /**< @brief unamed options number */
} ;

/** @brief The additional parameters to pass to parser functions */
struct cmdline_parser_params
{
  int override; /**< @brief whether to override possibly already present options (default 0) */
  int initialize; /**< @brief whether to initialize the option structure gengetopt_args_info (default 1) */
  int check_required; /**< @brief whether to check that all required options were provided (default 1) */
  int check_ambiguity; /**< @brief whether to check for options already specified in the option structure gengetopt_args_info (default 0) */
  int print_errors; /**< @brief whether getopt_long should print an error message for a bad option (default 1) */
} ;

/** @brief the purpose string of the program */
extern const char *gengetopt_args_info_purpose;
/** @brief the usage string of the program */
extern const char *gengetopt_args_info_usage;
/** @brief all the lines making the help output */
extern const char *gengetopt_args_info_help[];

/**
 * The command line parser
 * @param argc the number of command line options
 * @param argv the command line options
 * @param args_info the structure where option information will be stored
 * @return 0 if everything went fine, NON 0 if an error took place
 */
int cmdline_parser (int argc, char * const *argv,
  struct gengetopt_args_info *args_info);

/**
 * The command line parser (version with additional parameters - deprecated)
 * @param argc the number of command line options
 * @param argv the command line options
 * @param args_info the structure where option information will be stored
 * @param override whether to override possibly already present options
 * @param initialize whether to initialize the option structure my_args_info
 * @param check_required whether to check that all required options were provided
 * @return 0 if everything went fine, NON 0 if an error took place
 * @deprecated use cmdline_parser_ext() instead
 */
int cmdline_parser2 (int argc, char * const *argv,
  struct gengetopt_args_info *args_info,
  int override, int initialize, int check_required);

/**
 * The command line parser (version with additional parameters)
 * @param argc the number of command line options
 * @param argv the command line options
 * @param args_info the structure where option information will be stored
 * @param params additional parameters for the parser
 * @return 0 if everything went fine, NON 0 if an error took place
 */
int cmdline_parser_ext (int argc, char * const *argv,
  struct gengetopt_args_info *args_info,
  struct cmdline_parser_params *params);

/**
 * Save the contents of the option struct into an already open FILE stream.
 * @param outfile the stream where to dump options
 * @param args_info the option struct to dump
 * @return 0 if everything went fine, NON 0 if an error took place
 */
int cmdline_parser_dump(FILE *outfile,
  struct gengetopt_args_info *args_info);

/**
 * Save the contents of the option struct into a (text) file.
 * This file can be read by the config file parser (if generated by gengetopt)
 * @param filename the file where to save
 * @param args_info the option struct to save
 * @return 0 if everything went fine, NON 0 if an error took place
 */
int cmdline_parser_file_save(const char *filename,
  struct gengetopt_args_info *args_info);

/**
 * Print the help
 */
void cmdline_parser_print_help(void);
/**
 * Print the version
 */
void cmdline_parser_print_version(void);

/**
 * Initializes all the fields a cmdline_parser_params structure 
 * to their default values
 * @param params the structure to initialize
 */
void cmdline_parser_params_init(struct cmdline_parser_params *params);

/**
 * Allocates dynamically a cmdline_parser_params structure and initializes
 * all its fields to their default values
 * @return the created and initialized cmdline_parser_params structure
 */
struct cmdline_parser_params *cmdline_parser_params_create(void);

/**
 * Initializes the passed gengetopt_args_info structure's fields
 * (also set default values for options that have a default)
 * @param args_info the structure to initialize
 */
void cmdline_parser_init (struct gengetopt_args_info *args_info);
/**
 * Deallocates the string fields of the gengetopt_args_info structure
 * (but does not deallocate the structure itself)
 * @param args_info the structure to deallocate
 */
void cmdline_parser_free (struct gengetopt_args_info *args_info);

/**
 * Checks that all the required options were specified
 * @param args_info the structure to check
 * @param prog_name the name of the program that will be used to print
 *   possible errors
 * @return
 */
int cmdline_parser_required (struct gengetopt_args_info *args_info,
  const char *prog_name);


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* DUMP2XYZCMD_H */
