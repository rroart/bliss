%{ /* -*- c -*- */
/* 

   BLISS Compiler expand tabs utility (bliutab)

   -------------------------------------------------------------------------

   Copyright (C) 1986, 87, 89, 92-96, 1997, 1999, 2000, 2001 Free Software Foundation, Inc.
   Written by Paul Rubin, June 1986
   Adapted to ANSI C, Richard Stallman, Jan 1987
   Converted to expand tabs utility by Tim Josling
   
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
   
   -------------------------------------------------------------------------.  */

#include <getopt.h>
#include "ansidecl.h"
#include "blir_temp_config.h"
  
int32 yylex (void);

#include "bliutil.h"
#include "blidebug.h"

FILE *out_file;

void usage (FILE *f);
void version (void);

static int charpos;
static int list_position; 
static int tab_regular = 0;
static int tab_column_entry[500];
static int tab_column_count = 0; 
static int i; 

%}

%option nounput
%option noyywrap

%%


.*\n|.*  {
  charpos = 0;
  list_position = 0;
  for (i = 0; i < yyleng; i ++)
    {
      if (yytext[i] == '\t')
        {
          if (tab_regular > 0)
            {
              while ((!charpos) || ((charpos +1) % tab_regular))
                {
                  fprintf (out_file, " ");
                  charpos ++;
                }
            }
          else
            {
              while ((list_position < tab_column_count)
                     && (charpos >= tab_column_entry[list_position]))
                {
                  list_position ++;
                }
              if (list_position < tab_column_count)
                {
                  while ((charpos +1) < tab_column_entry[list_position])
                    {
                      fprintf (out_file, " ");
                      charpos ++;
                    }
                  list_position ++;
                }
              else
                {
                  /* Tab beyond list - output nothing.  */
                }
            }
        }
      else
        {
          fprintf (out_file, "%c", yytext[i]);
          charpos ++;
        }
    }
}

%%

/* 
 *
 * main routine for bliss compiler utility - expand tabs
 *
 *
 */

int
main (int argc, char **argv) 
{
  
  int tab_column_value;
  
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
        {"tab-regular", of_has_arg_required_argument, 0, 0},
        {"tab-column", of_has_arg_required_argument, 0, 0},
        {0, 0, 0, 0}
      };
      
      option_result = getopt_long (argc, argv, "hvi:o:l:r:c:",
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
        tab_regular = (int)strtol (optarg, NULL, 10);
        if (tab_regular < 1)
          {
            fprintf (stderr, "%s: Invalid tab interval %d < 1\n",
                    get_progname (), tab_regular);
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
        tab_column_value = (int)strtol (optarg, NULL, 10);
        if (tab_column_value < 1)
          {
            fprintf (stderr, "%s: Invalid value for --%s is %d\n", get_progname (),
                    int32_options[option_index].name, tab_column_value);
            usage (stderr);
            exit (EXIT_FAILURE);
          }
      if (tab_column_count > 0)
        {
          if (tab_column_value < tab_column_entry[tab_column_count -1])
            {
              fprintf (stderr, "%s: Tab column numbers not ascending --%s is %d\n",
                      get_progname (),
                      int32_options[option_index].name, tab_column_value);
              usage (stderr);
              exit (EXIT_FAILURE);
            }
        }
      if (tab_column_count >= 500)
        {
          fprintf (stderr, "%s: You are a goose (to have 500 tabs)\n",
                  get_progname ());
          usage (stderr);
          exit (EXIT_FAILURE);
        }
      tab_column_entry[tab_column_count] = tab_column_value;
      tab_column_count ++;
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
      
      case 'r' :
        if (!optarg)
          {
            fprintf (stderr, "%s: Missing value for -%c\n", get_progname (),
                    option_result);
            usage (stderr);
            exit (EXIT_FAILURE);
          }
        tab_regular = (int)strtol (optarg, NULL, 10);
        if (tab_regular < 1)
          {
            fprintf (stderr, "%s: Invalid tab interval %d < 1\n",
                    get_progname (), tab_regular);
            usage (stderr);
            exit (EXIT_FAILURE);
          }
        break;
        
      case 'c' :
        if (!optarg)
          {
            fprintf (stderr, "%s: Missing value for -%c\n", get_progname (),
                    option_result);
            usage (stderr);
            exit (EXIT_FAILURE);
          }
        tab_column_value = (int)strtol (optarg, NULL, 10);
        if (tab_column_value < 1)
          {
            fprintf (stderr, "%s: Invalid value for -%c is %d\n", get_progname (),
                    option_result, tab_column_value);
            usage (stderr);
            exit (EXIT_FAILURE);
          }
        if (tab_column_count > 0)
          {
            if (tab_column_value < tab_column_entry[tab_column_count -1])
              {
                fprintf (stderr, "%s: Tab column numbers not ascending -%c is %d\n",
                        get_progname (),
                        option_result, tab_column_value);
                usage (stderr);
                exit (EXIT_FAILURE);
              }
          }
        if (tab_column_count >= 500)
          {
            fprintf (stderr, "%s: You are a goose (to have 500 tabs)\n",
                    get_progname ());
            usage (stderr);
            exit (EXIT_FAILURE);
          }
        tab_column_entry[tab_column_count] = tab_column_value;
        tab_column_count ++;
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
  
  if (tab_regular && tab_column_count)
    {
      fprintf (stderr, "%s: Both tab options specified\n", get_progname ());
      usage (stderr);
      exit (EXIT_FAILURE);
    }
  if (!tab_regular && !tab_column_count)
    {
      fprintf (stderr, "%s: No tab option specified\n", get_progname ());
      usage (stderr);
      exit (EXIT_FAILURE);
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

/* output usage info for this program */

void usage (FILE * f)
{
  fprintf (f, "Usage (expand tabs in file):\nbliutab -v --version"
          "\n -h --help \n -i inputfile --input-file inputfile"
          "\n -o outputfile --output-file outputfile"
          "\n -r 99...9 --tab-regular 99...9 (tab every 99...9 columns)"
          "\n -c 99...9 --tab-column 99...9 (tab at this column"
          "\n          may specify multiple columns)"
          "\n inputfile and outputfile default to stdin/stdout\n");
}

/* output version and copyright info for this program */

void version (void) {
  fprintf (stdout, "bliutab (BlissForGCC) "BLISSFORGCC_VERSION
          "\nCopyright (C) 1999 Tim Josling."
          "\nBlissForGCC comes with NO WARRANTY "
          "to the extent permitted by law."
          "\nYou may redistribute copies of BlissForGCC"
          "\nunder the terms of the GNU General Public Licence."
          "\nFor more information about these matters,"
          "\nsee the files named COPYING.\n");
  exit (EXIT_SUCCESS);
}




