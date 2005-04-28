/* nanorodBuilderCmd.h */

/* File autogenerated by gengetopt version 2.13  */

#ifndef NANORODBUILDERCMD_H
#define NANORODBUILDERCMD_H

/* If we use autoconf.  */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef CMDLINE_PARSER_PACKAGE
#define CMDLINE_PARSER_PACKAGE "nanorodBuilder"
#endif

#ifndef CMDLINE_PARSER_VERSION
#define CMDLINE_PARSER_VERSION "1.0"
#endif

struct gengetopt_args_info
{
  char * output_arg;	/* Output file name.  */
  char * latticetype_arg;	/* Lattice type string. Valid types are fcc,hcp,bcc. (default='fcc').  */
  double length_arg;	/* length of nanorod in Angstroms.  */
  double width_arg;	/* diameter  of nanorod in Angstroms.  */
  double latticeCnst_arg;	/* lattice spacing in Angstrons for cubic lattice.  */
  int genGeomview_flag;	/* generate a geomview file with the object geometry (default=off).  */
  
  int help_given ;	/* Whether help was given.  */
  int version_given ;	/* Whether version was given.  */
  int output_given ;	/* Whether output was given.  */
  int latticetype_given ;	/* Whether latticetype was given.  */
  int length_given ;	/* Whether length was given.  */
  int width_given ;	/* Whether width was given.  */
  int latticeCnst_given ;	/* Whether latticeCnst was given.  */
  int genGeomview_given ;	/* Whether genGeomview was given.  */

  char **inputs ; /* unamed options */
  unsigned inputs_num ; /* unamed options number */
} ;

int cmdline_parser (int argc, char * const *argv, struct gengetopt_args_info *args_info);
int cmdline_parser2 (int argc, char * const *argv, struct gengetopt_args_info *args_info, int override, int initialize, int check_required);

void cmdline_parser_print_help(void);
void cmdline_parser_print_version(void);

void cmdline_parser_init (struct gengetopt_args_info *args_info);
void cmdline_parser_free (struct gengetopt_args_info *args_info);

int cmdline_parser_required (struct gengetopt_args_info *args_info, const char *prog_name);


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* NANORODBUILDERCMD_H */
