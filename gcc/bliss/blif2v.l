%{ /* -*- c -*- */
/* 

   BLISS Compiler fixed to variable length utility (blif2v)

   ------------------------------------------------------------------------

   Copyright (C) 1986, 87, 89, 92-96, 1997, 1999, 2000, 2001 Free Software Foundation, Inc.
   Written by Paul Rubin, June 1986
   Adapted to ANSI C, Richard Stallman, Jan 1987
   Converted to fixed to variable length converter by Tim Josling
   
   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
   
   In other words, you are welcome to use, share and improve this program.
   You are forbidden to forbid anyone else to use, share and improve
   what you give them.   Help stamp out software-hoarding!  
   
   
   ------------------------------------------------------------------------.  */

#include <getopt.h>
#include <limits.h>
#include "ansidecl.h"
#include "blir_temp_config.h"
  
int32 yylex (void);

#include "bliutil.h"
#include "blidebug.h"

FILE *out_file;

void usage (FILE *f);
void version (void);

/* Escape option see usage().  */
static int option_escape = 0;
/* Input char count.  */
static int chars = 0;

/* Fixed line length eg 80.  */
static int line_length = 0;
/* Truncate lines to this length eg 72.  */
static int truncation_length = 0;
/* Flag for within nonnumeric literal.  */
static uint32 in_string = 0;

%}

%option nounput
%option noyywrap

%%

.|\n  {
  if ((chars % line_length) < truncation_length)
    {
      uchar uchar1;
      uchar1 = (uchar)yytext[0];
      if (in_string)
        {
          if (uchar1 == in_string)
            {
              in_string = 0;
            }
          goto outchar;
        }
      else
        {
          if (option_escape)
            {
              if ((uchar1 == '\"') || (uchar1 == '\''))
                {
                  in_string = uchar1;
                  goto outchar;
                }
            }
        }
    outchar :
      if (in_string && (uchar1 < 32)) /* Escape char.  */
        {
          fprintf (out_file, "%c & x%c%.2x%c & %c", in_string, in_string, uchar1, in_string, in_string);
        }
      else
        {
          fprintf (out_file, "%c", yytext[0]);
        }
    }
  chars ++;
  if (!(chars % line_length))
    {
      in_string = 0;
      fprintf (out_file, "\n");
    }
}

%%

/* 
 *
 * main routine for bliss compiler utility - convert fixed to variable length
 *
 *
 */

int
main (int argc, char **argv)
{
  
  /* Work vars.  */
  enum
  {
    of_has_arg_no_argument = 0,
    of_has_arg_required_argument = 1,
    of_has_arg_optional_argument = 2
  };

  /* Input options and names.  */

  uchar *in_fname = NULL;  /* Input file name.  */
  uchar *out_fname = NULL;  /* Output file name.  */

  yyin = stdin;
  out_file = stdout;
  
  set_progname ((uchar *)argv[0]);

  /* Process switches.  */

  while (1)
    {
      
      int option_result;
      int option_index = 0;
      
      static struct option int32_options[] = 
      {
        {"version", of_has_arg_no_argument, 0, 'v'},
        {"help", of_has_arg_no_argument, 0, 'h'},
        {"input-file", of_has_arg_required_argument, 0, 0},
        {"output-file", of_has_arg_required_argument, 0, 0},
        {"line-length", of_has_arg_required_argument, 0, 0},
        {"truncation-length", of_has_arg_required_argument, 0, 0},
        {"escape-special", of_has_arg_no_argument, 0, 'e'},
        {0, 0, 0, 0}
      };
      
      option_result = getopt_long (argc, argv, "hvi:o:l:t:e",
                                int32_options,  & option_index);
      
      if (option_result == EOF)break;
      
      /* Merge int32 and short options.  */
      if (option_result == 0)option_result = option_index;
      
      switch (option_result)
        {
          
        case 'h' :
          usage (stdout);
          exit (EXIT_SUCCESS);
          break;
          
        case 'v' :
          version ();
          break;
          
        case 'e' :
          option_escape = 1;
          break;
          
        case 2 :
          if (!optarg)
            {
              fprintf (stderr, "%s: Missing value for --%s\n", get_progname (),
                      int32_options[option_index].name);
              usage (stderr);
              exit (EXIT_FAILURE);
            }
          in_fname = tj_strdup ((uchar *)optarg);
          break;
          
        case 3 :
          if (!optarg)
            {
              fprintf (stderr, "%s: Missing value for --%s\n", get_progname (),
                      int32_options[option_index].name);
              usage (stderr);
              exit (EXIT_FAILURE);
            }
          out_fname = tj_strdup ((uchar *)optarg);
          break;
          
        case 4 :
          if (!optarg)
            {
              fprintf (stderr, "%s: Missing value for --%s\n", get_progname (),
                      int32_options[option_index].name);
              usage (stderr);
              exit (EXIT_FAILURE);
            }
          line_length = atol (optarg);
          if (line_length < 1)
            {
              fprintf (stderr, "%s: Invalid line length %d\n",
                      get_progname (), line_length);
              usage (stderr);
              exit (EXIT_FAILURE);
            }
          break;
          
        case 5 :
          if (!optarg)
            {
              fprintf (stderr, "%s: Missing value for --%s\n", get_progname (),
                      int32_options[option_index].name);
              usage (stderr);
              exit (EXIT_FAILURE);
            }
          truncation_length = atol (optarg);
          if (truncation_length < 1)
            {
              fprintf (stderr, "%s: Invalid truncation length %d\n",
                      get_progname (), truncation_length);
              usage (stderr);
              exit (EXIT_FAILURE);
            }
          break;
          
          
        case 'i' :
          if (!optarg)
            {
              fprintf (stderr, "%s: Missing value for -%c\n", get_progname (),
                      option_result);
              usage (stderr);
              exit (EXIT_FAILURE);
            }
          in_fname = tj_strdup ((uchar *)optarg);
          break;
          
        case 'o' :
          if (!optarg)
            {
              fprintf (stderr, "%s: Missing value for -%c\n", get_progname (),
                      option_result);
              usage (stderr);
              exit (EXIT_FAILURE);
            }
          out_fname = tj_strdup ((uchar *)optarg);
          break;
          
        case 'l' :
          if (!optarg)
            {
              fprintf (stderr, "%s: Missing value for -%c\n", get_progname (),
                      option_result);
              usage (stderr);
              exit (EXIT_FAILURE);
            }
          line_length = atol (optarg);
          if (line_length < 1)
            {
              fprintf (stderr, "%s: Invalid line length %d\n",
                      get_progname (), line_length);
              usage (stderr);
              exit (EXIT_FAILURE);
            }
          break;
          
        case 't' :
          if (!optarg)
            {
              fprintf (stderr, "%s: Missing value for -%c\n", get_progname (),
                      option_result);
              usage (stderr);
              exit (EXIT_FAILURE);
            }
          truncation_length = atol (optarg);
          if (truncation_length < 1)
            {
            fprintf (stderr, "%s: Invalid truncation length %d\n",
                    get_progname (), truncation_length);
            usage (stderr);
            exit (EXIT_FAILURE);
          }
          break;
          
          
        case '?' :
          /* Getopt already put out an error message - 
             doing it yourself doesn't work.  */
          usage (stderr);
          exit (EXIT_FAILURE);
          break;
          
        default :
          fprintf (stderr, "%s: Logic error in option processing #1"
                  " -%c %d\n", get_progname (),
                  option_result, option_result);
          BLI_ASSERT (0 && "Internal error 1");
          exit (EXIT_FAILURE);
          break;  /* Hush.  */
        }
    }
  
  if (optind < argc)
    {
      while (optind < argc)
        {
          fprintf (stderr, "%s: Unexpected option %s\n", get_progname (),
                  argv[optind ++]);
        }
      usage (stderr);
      exit (EXIT_FAILURE);
    }
  
  if (line_length < 1)
    {
      fprintf (stderr, "%s: Missing line length -l --line-length\n",
              get_progname ());
      usage (stderr);
      exit (EXIT_FAILURE);
    }
  
  if (truncation_length < 1)
    {
      truncation_length = INT_MAX;
    }
  
  if (in_fname)
    {
      yyin = fopen ((char *)in_fname, "rb");
      if (!yyin)
        {
          my_pfatal_with_name ((uchar *)in_fname);
        }
    }
  
  if (out_fname)
    {
      out_file = fopen ((char *)out_fname, "wb");
      if (!out_file)
        {
          my_pfatal_with_name ((uchar *)out_fname);
        }
    }
  
  yylex ();

  if (in_fname)
    fclose (yyin);
  if (out_fname)
    fclose (out_file);
  
  exit (EXIT_SUCCESS);
}

/* output usage info to file F */

void 
usage (FILE * f)
{
  fprintf (f, "Usage (convert fixed text file to variable Length):\nblif2v -v --version"
         "\n -h --help \n-i inputfile --input-file inputfile"
         "\n -e --escape-special (output control chars in strings as \" & x\"hh\" & \")"
         "\n -l 99...9 --line-length 99...9 (line length)"
         "\n -t 99...9 --truncation-length 99...9 "
          "(truncate lines to this value)"
         "\n -o outputfile --output-file outputfile"
         "\n inputfile and outputfile default to stdin/stdout\n");
}

/* output version and copyright info */

void 
version (void) {
  fprintf (stdout, "blif2v (BlissForGCC) "BLISSFORGCC_VERSION
          "\nCopyright (C) 1999, 2000 Tim Josling."
          "\nBlissForGCC comes with NO WARRANTY "
          "to the extent permitted by law."
          "\nYou may redistribute copies of BlissForGCC"
          "\nunder the terms of the GNU General Public Licence."
          "\nFor more information about these matters,"
          "\nsee the files named COPYING.\n");
  exit (EXIT_SUCCESS);
}

