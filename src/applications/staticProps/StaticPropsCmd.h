/* StaticPropsCmd.h */

/* File autogenerated by gengetopt version 2.11  */

#ifndef STATICPROPSCMD_H
#define STATICPROPSCMD_H

/* If we use autoconf.  */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef CMDLINE_PARSER_PACKAGE
#define CMDLINE_PARSER_PACKAGE "StaticProps"
#endif

#ifndef CMDLINE_PARSER_VERSION
#define CMDLINE_PARSER_VERSION "1.00"
#endif

struct gengetopt_args_info
{
  char * input_arg;	/* input dump file.  */
  char * output_arg;	/* output file name.  */
  int step_arg;	/* process every n frame (default='1').  */
  int nrbins_arg;	/* number of bins for distance (default='100').  */
  int nanglebins_arg;	/* number of bins for cos(angle) (default='50').  */
  double length_arg;	/* maximum length (Defaults to 1/2 smallest length of first frame).  */
  char * sele1_arg;	/* select first stuntdouble set.  */
  char * sele2_arg;	/* select second stuntdouble set.  */

  int help_given ;	/* Whether help was given.  */
  int version_given ;	/* Whether version was given.  */
  int input_given ;	/* Whether input was given.  */
  int output_given ;	/* Whether output was given.  */
  int step_given ;	/* Whether step was given.  */
  int nrbins_given ;	/* Whether nrbins was given.  */
  int nanglebins_given ;	/* Whether nanglebins was given.  */
  int length_given ;	/* Whether length was given.  */
  int sele1_given ;	/* Whether sele1 was given.  */
  int sele2_given ;	/* Whether sele2 was given.  */
  int gofr_given ;	/* Whether gofr was given.  */
  int r_theta_given ;	/* Whether r_theta was given.  */
  int r_omega_given ;	/* Whether r_omega was given.  */
  int theta_omega_given ;	/* Whether theta_omega was given.  */
  int xyz_given ;	/* Whether xyz was given.  */

} ;

int cmdline_parser (int argc, char * const *argv, struct gengetopt_args_info *args_info);

void cmdline_parser_print_help(void);
void cmdline_parser_print_version(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* STATICPROPSCMD_H */
