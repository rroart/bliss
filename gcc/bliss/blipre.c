/*
   
   BLISS Compiler preprocessor (blipre)

   ---------------------------------------------------------------------------------

   Copyright (C) 1986, 87, 89, 92-96, 1997, 1999, 2000, 2001 Free Software Foundation, Inc.
   Written by Paul Rubin, June 1986
   Adapted to ANSI C, Richard Stallman, Jan 1987
   Adapted to BLISS by Tim Josling Mar-Apr 1999
   
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
   
   ---------------------------------------------------------------------------------.  */

#include "blir_temp_config.h"

#include "config.h"
#include "system.h"
/* For asprintf which needs GNU C runtime.  */
#include "ansidecl.h"
#include "diagnostic.h"

/* No garbage collection in the preprocessor.  */
native_int ggc_p = 0;

#include "bli_version.h"

#include "blidebug.h"
#include "bliumem.h"
#include "bliumsc.h"
#include "bliumsg.h" 
#include "blipre.h"
#include "bliplex.h"

/* Needed by diagostic.h.  */
diagnostic_context *global_dc = NULL;

/* Options and parameters - see usage() for definitions.  */
uint32 option_immediate_msg_output = 0;
uint32 option_output_comments = 1; 
uint32 option_fix_eol = 1;
uint32 option_no_seq = 0;
static uint32 copy_max_nesting_level = 10;
uint32 option_output_linenos = 1; 
/* Offsets in lines depending on option_no_seq.  */
uint32 comment_offset = 6;
uint32 area_a_offset = 7;
uint32 area_b_offset = 11;

uint32 option_R_margin = 0;
uchar *option_syslib_directory = NULL; 
struct lib_details { struct lib_details *next; uchar *lib; uchar *directory; }; 
struct lib_details *first_other_directory = NULL;
struct lib_details *last_other_directory = NULL;
uint32 option_dump_lines = 0;
uint32 option_dump_tokens = 0;
uint32 option_dump_copyrepl = 0;
uint32 option_dump_file = 0;
uint32 option_dump_file_after_R_margin = 0;
uint32 option_parser_trace = 0;
uint32 option_stop_before_parse = 0;

uint32 copy_nested_level = 0;
extern uint32 *ignore_tokens;
extern uchar **token_string_ptrs;

/* File structs.  */

dynarray_file_ptr *file_ptrs = NULL;

/* Local prototypes.  */

static void usage (FILE *f);

static void version (void);

/* 
 * Main routine for bliss compiler preprocessor.  */

native_int
main (int argc, char **argv)
{

  struct file_struct *main_file_ptr;

  /* Input options and names.  */

  uchar *in_fname = NULL;  /* Input file name.  */
  uchar *out_fname = NULL;  /* Output file name.  */

  uint32 error_count;

  /* Diagnostic context.  */

  global_dc = BLI_ALLOC ((sizeof (diagnostic_context)));


  resolve_token_string_ptrs ();
  init_ignore_tokens ();
  message_init ();


  main_file_ptr = BLI_ALLOC (sizeof (struct file_struct));

  bliumsc_set_progname ((uchar *)"blipre");

  /*******************************************
   * Options parse
   ******************************************  */

  blipre_options (argc, (uchar **)argv,  & in_fname,  & out_fname);

  /*******************************************
   * Main processing
   ******************************************  */

  main_file_ptr->file_name = in_fname;

  process_file (main_file_ptr);

  if (!option_stop_before_parse)
    {
      process_replace (main_file_ptr);
      if (option_dump_tokens)
	{
	  fprintf (stdout, "Output tokens for overall preprocessing\n");
	  dump_tokens (stdout, main_file_ptr->file_name,
		       main_file_ptr->file_first_token,
		       main_file_ptr->file_token_count);
	}
      output_resulting_file (main_file_ptr, out_fname);
    }

  /*******************************************
   * Output accumulated messages
   ******************************************  */

  output_msgs ();

  /*******************************************
   * Output summary and exit
   ******************************************  */

  error_count = output_summary ();
  
  if (error_count)
      exit (EXIT_FAILURE);
  
  exit (EXIT_SUCCESS);

}

/*******************************************
 * Print program usage instructions to file F
 ******************************************  */

static void
usage (FILE *f)
{
  fprintf (f, "Usage (bliss preprocessor %s):\nblipre "
	   "\n --help "
	   "\n -v (display version number)"
	   "\n -i inputfile (req'd - when used within gcc; "
           "\n    the input files should be just included at the end of  the option list eg \'gcc infile.bli\')"
	   "\n -o outputfile\n"
	   "\n -C (do not output comments)"
	   "\n -P (do not output # \"line number\" lines in output file for debugging)"

	   "\n -ffixeol (add spaces to end lines when needed for syntax - default)"
	   "\n -fnofixeol (don't add spaces to end lines when needed for syntax)"
	   "\n -fnoseq  (input files have no sequence numbers)"
	   "\n -fseq  (input files have sequence numbers- default"
	   "\n -fmax-nesting-level 99...9 max copy depth default 10"
	   "\n -fR-margin=MARGIN (eg -fR-margin=72; default is R-margin at the end of each line, "
           "most compilers set it to after column 72)"

	   "\n -ddump-line (debug - dump lines after initial preprocessing)"
	   "\n -ddump-token (debug - dump tokens after lexical analysis)"
	   "\n -ddump-copyrepl (debug - dump copy/replace verb information)"
	   "\n -ddump-dile (debug - dump file as it is read in)"
	   "\n -dstop-before-parse (debug - stop before doing parse)"
	   "\n -dparser-trace (debug - trace parser actions)"
	   "\n -ddump-R-margin (debug - dump file after R-margin is processed)\n"
	   "\n -dimmediate-msg-output (output errors/warnings as found not in textual order)\n"

	   "\n -ansi (warn about non-ansi 85 compliance)."
	   "\n -pedantic (warn about non-ansi 85 compliance)."
	   "\n -pedantic-errors (errors for non-ansi 85 compliance)."
	   "\n -H (print copy file names as they are opened)."
	   "\n -I directory (specify *the* default copy file directory"
           "\n    - overrides SYSLIB in an environment variable"
 	   "\n -I name=directory (copy library directory name)"
           "\n    - overrides NAME in an environment variable"

           "\n"
           "\n By default fatal, error, warning and unsupported are printed and counted\n"

	   "\n -w          no messages for things normally warned about"
	   "\n -WnoWarning no messages for things normally warned about"
	   "\n -WnoUnsupported no messages for unsupported features"

	   "\n -Wall (warn about everything except NonStandard)"
	   "\n -WXXX      (XXX = FYI Efficiency Obsolete NonStandard) issue messages of these types"
	   "\n -WXXX-error treat (XXX = Warning FYI Efficiency Obsolete NonStandard) as an error"
	   "\n -WXXX-no-error (XXX Unsupported) Do not treat as error"
	   "\n"
           , "blipre " BLISSFORGCC_VERSION);
}

/* Option error convenience macro.  */

#define OPTION_ERROR(print_call) \
{\
  output_msgs();\
  asprintf print_call;\
  MSG(MSGID(25), 0, 0, 0, (uchar *)error_message, NULL);\
  goto usage_error;\
}

/*******************************************
 * Process switches and find file names.  ARGC and ARGV are the values
 * given to main, returns input file name IN_FNAME_PTR and output file
 * name OUT_FNAME_PTR
 ******************************************  */

void
blipre_options (uint32 argc, uchar **argv, uchar **in_fname_ptr,
                       uchar **out_fname_ptr)
{
  int32 max_levels;
  uint32 arg;
  schar *error_message = NULL;
  int dummy_run = 0;

  /*

    Process options - bear in mind I may get options that are really
    meant for someone else (eg the main compiler) so I have to be very
    permissive.  */

  for (arg = 1; arg < argc; arg ++)
    {
      if (argv[arg][0] != '-')
        goto bad_option; 
      switch (argv[arg][1])
        {
        case '-' :
          if (!strcmp ((char *)argv[arg], "--help"))
            {
              usage (stdout);
              dummy_run = 1;
              break;
            }
          break;

        case 'a' :
          if (!strcmp ((char *)argv[arg], "-ansi"))
            {
              output_pedantic ();
              break;
            }
          break;

        case 'd' :
          if (!strcmp ((char *)argv[arg], "-dimmediate-msg-output"))
            {
              option_immediate_msg_output = 1;
              break;
            }
          if (!strcmp ((char *)argv[arg], "-ddump-line"))
            {
              option_dump_lines = 1;
              break;
            }
          if (!strcmp ((char *)argv[arg], "-ddump-token"))
            {
              option_dump_tokens = 1;
              break;
            }
          if (!strcmp ((char *)argv[arg], "-ddump-copyrepl"))
            {
              option_dump_copyrepl = 1;
              break;
            }
          if (!strcmp ((char *)argv[arg], "-ddump-file"))
            {
              option_dump_file = 1;
              break;
            }
          if (!strcmp ((char *)argv[arg], "-ddump-R-margin"))
            {
              option_dump_file_after_R_margin = 1;
              break;
            }
          if (!strcmp ((char *)argv[arg], "-dparser-trace"))
            {
              option_parser_trace = 1;
              break;
            }
          if (!strcmp ((char *)argv[arg], "-dstop-before-parse"))
            {
              option_stop_before_parse = 1;
              break;
            }
          break;

        case 'f' :
          if (!strcmp ((char *)argv[arg], "-ffixeol"))
            {
              option_fix_eol = 1;
              break;
            }
          if (!strcmp ((char *)argv[arg], "-fnofixeol"))
            {
              option_fix_eol = 0;
              break;
            }
          if (!strcmp ((char *)argv[arg], "-fseq"))
            {
              option_no_seq = 0;
              comment_offset = 6;
              area_a_offset = 7;
              area_b_offset = 11;
              break;
            }
          if (!strcmp ((char *)argv[arg], "-fnoseq"))
            {
              option_no_seq = 1;
              comment_offset = 0;
              area_a_offset = 1;
              area_b_offset = 5;
              break;
            }
          if (!strcmp ((char *)argv[arg], "-fmax-nesting-level"))
            {
              if (arg >= (argc -1))
                goto missing_argument;
              max_levels = strtol ((char *)argv[arg +1], NULL, 10);
              if ((max_levels < 1) || (max_levels > 1000000000))
                {
                  OPTION_ERROR (((char **) & error_message, 
                                "%s: Wrong value %s (not 1..1,000,000,000) in option %s\n",
                                         bliumsc_get_progname (), argv[arg +1], argv[arg]));
                }
              copy_max_nesting_level = max_levels;
              arg ++;
              break;
            }
          if (!strcmp ((char *)argv[arg], "-foutput-linenos"))
            {
              option_fix_eol = 0;
              break;
            }
          if (!strncmp ((char *)argv[arg], "-fR-margin=", 11))
            {
              option_R_margin = strtol ((char *)argv[arg]+11, NULL, 10); 
              if (option_R_margin < 1)
                {
                  OPTION_ERROR (((char **) & error_message, 
                                         "R_margin (%d) < 1 \n", option_R_margin));
                }
              if (option_R_margin > 1000)
                {
                  OPTION_ERROR (((char **) & error_message, 
                                         "R_margin (%d) > 1000 \n", option_R_margin));
                }
              break;
            }

        case 'i' :
          if (!strcmp ((char *)argv[arg], "-i"))
            {
              if (arg >= (argc -1))
                goto missing_argument;
              *in_fname_ptr = BLI_ALLOC (strlen ((char *)argv[arg +1])+1);
              memcpy (*in_fname_ptr, (char *)argv[arg +1], strlen ((char *)argv[arg +1])+1);
              arg ++;
            }
          break;

        case 'o' :
          if (!strcmp ((char *)argv[arg], "-o"))
            {
              if (arg >= (argc -1))
                goto missing_argument;
              *out_fname_ptr = BLI_ALLOC (strlen ((char *)argv[arg +1])+1);
              memcpy (*out_fname_ptr, (char *)argv[arg +1], strlen ((char *)argv[arg +1])+1);
              arg ++;
            }
          break;

        case 'p' :
          if (!strcmp ((char *)argv[arg], "-pedantic"))
            output_pedantic ();
          if (!strcmp ((char *)argv[arg], "-pedantic-errors"))
            error_pedantic ();
          break;

        case 'v' :
          if (!strcmp ((char *)argv[arg], "-v"))
            {
              version ();
              dummy_run = 1;
              break;
            }
          break;

        case 'w' :
          if (!strcmp ((char *)argv[arg], "-w"))
            {
              no_output_warning ();
              break;
            }
          break;

        case 'C' :
          if (!strcmp ((char *)argv[arg], "-C"))
            {
              option_output_comments = 0;
              break;
            }
          break;

        case 'H' :
          if (!strcmp ((char *)argv[arg], "-H"))
            {
              output_filenames ();
              break;
            }
          break;

        case 'I' :
          if (!strcmp ((char *)argv[arg], "-I"))
            {
              uchar *whole_argument;
              uchar *library_name;
              uchar *library_directory;
              struct lib_details *lib_det1;

              if (arg >= (argc -1))
                goto missing_argument;
              whole_argument = BLI_ALLOC (strlen ((char *)argv[arg +1])+1);
              memcpy (whole_argument, (char *)argv[arg +1], strlen ((char *)argv[arg +1]));
              /* Eg.  LIB1=/home/tim/bliss/lib1.  */
              library_directory = (uchar *)strchr ((char *)whole_argument, '=');
              if (library_directory)
                {
                  *(library_directory ++) = 0;
                  library_name = whole_argument; 
                  /* 
                  For (library_name_ptr=library_name; *library_name_ptr; library_name_ptr++) 
                    {
                      *library_name_ptr=TOUPPER(*library_name_ptr);
                    }.  */
                  /* Check for dup.  */
                  for (lib_det1 = first_other_directory; lib_det1; lib_det1 = lib_det1->next)
                    {
                      if (!strcmp ((char *)library_name, (char *)lib_det1->lib))
                        goto duplicate_lib;
                    }
                  lib_det1 = BLI_ALLOC (sizeof (struct lib_details));;
                  lib_det1->lib = library_name;
                  lib_det1->directory = library_directory;
                  if (!first_other_directory)
                    first_other_directory = last_other_directory = lib_det1;
                  else
                    {
                      last_other_directory->next = lib_det1;
                      last_other_directory = lib_det1;
                    }
                  arg ++;
                  break; 
                }
              /* Eg.  /home/tim/bliss/mysyslib.  */
              if (option_syslib_directory)
                goto duplicate_option;
              option_syslib_directory = whole_argument;
              arg ++;
            }
          break;

        case 'P' :
          if (!strcmp ((char *)argv[arg], "-P"))
            {
              option_output_linenos = 0;
              break;
            }
          break;
          
        case 'W' :
          if (!strcmp ((char *)argv[arg], "-Wall"))
            {
              output_fyi ();
              output_eff ();
              output_obs ();
              break;
            }
          if (!strcmp ((char *)argv[arg], "-WFYI"))
            {
              output_fyi ();
              break;
            }
          if (!strcmp ((char *)argv[arg], "-WEfficiency"))
            {
              output_eff ();
              break;
            }
          if (!strcmp ((char *)argv[arg], "-WObsolete"))
            {
              output_obs ();
              break;
            }
          if (!strcmp ((char *)argv[arg], "-WNonStandard"))
            {
              output_pedantic ();
              break;
            }
          
          if (!strcmp ((char *)argv[arg], "-WnoWarning"))
            {
              no_output_warning ();
              break;
            }
          if (!strcmp ((char *)argv[arg], "-WnoUnsupported"))
            {
              no_output_unsupported ();
              break;
            }
          if (!strcmp ((char *)argv[arg], "-WWarning-error"))
            {
              error_warning ();
              break;
            }
          if (!strcmp ((char *)argv[arg], "-WFYI-error"))
            {
              error_fyi ();
              break;
            }
          if (!strcmp ((char *)argv[arg], "-WEfficiency-error"))
            {
              error_eff ();
              break;
            }
          if (!strcmp ((char *)argv[arg], "-WObsolete-error"))
            {
              error_obs ();
              break;
            }
          if (!strcmp ((char *)argv[arg], "-WNonStandard-error"))
            {
              error_pedantic ();
              break;
            }
          
          if (!strcmp ((char *)argv[arg], "-WUnsupported-no-error"))
            {
              no_error_unsupported ();
              break;
            }
          break;
          
        default :
          break;
        }
    }
  
  if (dummy_run)
    exit (EXIT_SUCCESS);

  if (!(*in_fname_ptr))
    {
      OPTION_ERROR (((char **) & error_message, 
                             "Input filename not specified"));
    }
  
  if (!option_stop_before_parse)
    {
      if (!(*out_fname_ptr))
	{
          OPTION_ERROR (((char **) & error_message, 
                                 "Output filename not specified"));
	}
    }
  
  if (option_R_margin)
    {
      if (option_R_margin < (area_b_offset + 2))
	{
          OPTION_ERROR (((char **) & error_message, 
                                 "R_margin too low (%d) ", option_R_margin));
	}
    }

  return;

 bad_option :
  OPTION_ERROR (((char **) & error_message, 
                         "%s: unrecognized option %s", bliumsc_get_progname (), (char *)argv[arg]));

 duplicate_lib :
  OPTION_ERROR (((char **) & error_message, 
                 "%s: duplicated libray name in option %s %s, only allowed once", 
                 bliumsc_get_progname (), (char *)argv[arg], (char *)argv[arg +1]));

 duplicate_option :
  OPTION_ERROR (((char **) & error_message, 
                         "%s: duplicated option %s, only allowed once",
                 bliumsc_get_progname (), (char *)argv[arg]));
  
 missing_argument :
  OPTION_ERROR (((char **) & error_message, 
                         "%s: missing argument for %s", bliumsc_get_progname (), (char *)argv[arg]));

 usage_error : 
  usage (stderr);
  output_summary ();
  exit (EXIT_FAILURE);
  return;

}


/* Option error convenience macro.  */

#define DYNAMIC_ERROR(number, fileno, lineno, charno, print_call) \
{\
  asprintf print_call;\
  MSG(number, fileno, lineno, charno, (uchar*)error_message, NULL);\
}

/**************************************************
 * Preprocess a file FILE_PTR including copies and replaces
 *************************************************  */

void
process_file (struct file_struct *file_ptr)
{

  uchar *error_message = NULL;
  uchar *this_file;
  struct file_struct *owner_file;
  int file_scan_ix;

  /* Get file name without directories.  */

  file_ptr->file_short_name = file_ptr->file_name;
  for (file_scan_ix = strlen ((char *)file_ptr->file_name)-1; file_scan_ix >= 0; file_scan_ix --)
    {
      if ((file_ptr->file_name[file_scan_ix] == '\\')
          || (file_ptr->file_name[file_scan_ix] == '/'))
        {
          file_ptr->file_short_name = file_ptr->file_name + file_scan_ix + 1;
          break;
        }
    }
  
  
  file_count ++; 
  file_ptr->file_number = file_count;  /* Start from 1 to allow use of 0 as 'no file' indicator.  */

  owner_file = file_ptr->file_copy_token_ptr ? file_ptr->file_copy_token_ptr->token_file : NULL;
  
  /* Allocate a string for the file name and fill it in, and one for the hierarchy.  */

  {
    uint32 chars_needed = 0;
    uint32 chars_so_far = 0;
    struct file_struct *hier_file = NULL;
    struct file_struct *prev_hier_file = NULL;
    static const char *copied_from = "\n ... copied from ";
    static uint32 copied_from_length = 0;
    
    if (!copied_from_length)
      copied_from_length = strlen (copied_from);
    
    if (!file_names)
      {
        file_names = (dynarray_uchar_ptr *) ALLOC_DYN (10, sizeof (char *));
        file_names_copied = (dynarray_uchar_ptr *) ALLOC_DYN (10, sizeof (char *));
        file_ptrs = (dynarray_file_ptr *) ALLOC_DYN (10, sizeof (void *));
      }
    else
      {
        GROW_DYN (&file_names->dynarray_details, file_count);
        GROW_DYN (&file_names_copied->dynarray_details, file_count);
        GROW_DYN (&file_ptrs->dynarray_details, file_count);
      }
    
    file_names->elem[file_count] = file_ptr->file_short_name;
    file_ptrs->elem[file_count] = file_ptr;

    for (hier_file = file_ptr; hier_file; 
         hier_file = hier_file->file_copy_token_ptr ? hier_file->file_copy_token_ptr->token_file : NULL)
      {
        if (hier_file == file_ptr)
          continue;
        chars_needed += strlen ((char *)hier_file->file_short_name) + copied_from_length
          + SHOULD_BE_ENOUGH_FOR_INT + 1; 
      }
    if (chars_needed)
      chars_needed -= copied_from_length;

    if (chars_needed)
      {
        chars_needed++;
        file_names_copied->elem[file_count] = BLI_ALLOC (chars_needed);
        this_file = (uchar *)file_names_copied->elem[file_count];
        for (hier_file = file_ptr; hier_file; 
             hier_file = hier_file->file_copy_token_ptr ? hier_file->file_copy_token_ptr->token_file : NULL)
          {
            uint32 this_file_name_length;
            uchar lineno_str[SHOULD_BE_ENOUGH_FOR_INT +1];
            uint32 lineno_len;
            if (hier_file != file_ptr)
              {
                if (prev_hier_file != file_ptr)
                  {
                    memcpy (this_file + chars_so_far, copied_from, copied_from_length);
                    chars_so_far += copied_from_length;
                  }

                this_file_name_length = strlen ((char *)hier_file->file_short_name);
                memcpy (this_file + chars_so_far, hier_file->file_short_name, this_file_name_length);
                chars_so_far += this_file_name_length;

                memcpy (this_file + chars_so_far, ":", 1);
                chars_so_far ++;

                lineno_len = snprintf ((char *)lineno_str, SHOULD_BE_ENOUGH_FOR_INT, 
                                    "%d", prev_hier_file->file_copy_token_ptr->token_lineno);
                if ((lineno_len < 1) || (lineno_len > SHOULD_BE_ENOUGH_FOR_INT))
                  abort ();  /* Not enough room for lineno but should be plenty of digits.  */
                memcpy (this_file + chars_so_far, lineno_str, lineno_len);
                chars_so_far += lineno_len;

              }
            prev_hier_file = hier_file;
          }
      }
    else
      {
        file_names_copied->elem[file_count] = NULL;
      }
  }

  /* Check nesting OK.  */

  if (copy_nested_level)
    {
      DYNAMIC_ERROR (MSGID (26), 
                    owner_file->file_number, file_ptr->file_copy_token_ptr->token_lineno, 0, 
                    ((char **) & error_message, 
                     "(fyi) Opening COPY file %s at level %d ",
                     file_ptr->file_name, copy_nested_level));
    }

  if (copy_nested_level > copy_max_nesting_level)
    {
      output_msgs ();
      DYNAMIC_ERROR (MSGID (27), owner_file->file_number,
                    file_ptr->file_copy_token_ptr->token_lineno, 0, 
                    ((char **) & error_message, 
                     "Excessive level of nested files %d>%d, opening %s",
                     copy_nested_level, copy_max_nesting_level,
                     file_ptr->file_short_name));
      output_summary ();
      exit (EXIT_FAILURE);
    }

  if (copy_nested_level > 1)
    {
      MSG (MSGID (28), owner_file->file_number, 
                     file_ptr->file_copy_token_ptr->token_lineno, 0, NULL, NULL);
    }

  copy_nested_level ++;

  /*
   * Read the whole file in.  */

  file_ptr->file_buffer =
    read_file ((file_ptr->file_name ? file_ptr->file_name : (uchar *)""),
	       & (file_ptr->file_length));

  if (!file_ptr->file_buffer)
    {
      MSG (MSGID (1), file_ptr->file_number, 1, 0, NULL, NULL);
      copy_nested_level --;
      return;
    }


  if (option_dump_file)
    dump_file (file_ptr->file_name, file_ptr->file_length,
                 file_ptr->file_buffer);
  
  /* Truncate/fill lines if requested.  */
  
  if (option_R_margin)
    {
      blipre1_R_margin (file_ptr);
      if (option_dump_file_after_R_margin)
        dump_file (file_ptr->file_name, file_ptr->file_length,
                   file_ptr->file_buffer);
    }
  
  /*
   * Clean up the mess of continuations etc.  */
  
  blipre1_fix_lines (file_ptr->file_buffer, 
                     file_ptr->file_length,
                     file_ptr->file_name,
                     file_ptr->file_number,
                     &file_ptr->file_lines,
                     &file_ptr->file_line_count, file_ptr);
  
  if (option_dump_lines)
    dump_lines (file_ptr->file_name, file_ptr->file_line_count,
                file_ptr->file_lines);
  
  /*
   * Break it up into preprocessing tokens.  */
  
  blipre1_tokenize_file (file_ptr);
  
  if (option_dump_tokens)
    dump_tokens (stdout, file_ptr->file_name,
                 file_ptr->file_first_token, file_ptr->file_token_count);
  
  if (option_stop_before_parse)
    {
      copy_nested_level --;
      return;
    }
  
  /* 
     Parser kludge.
     Mark identifiers with special tokens 
     so parser can cope with non-lalr(1) nature of bliss reference 
     modification syntax.  */


  preprocess_copy_identifiers (file_ptr->file_first_token,
			       file_ptr->file_last_token);

  /* Find the copy verbs.  */

  blippr1 ( /* Input parms.  */
	    file_ptr->file_first_token, 1,  /* Find copy verbs.  */
	    /* Output parms.  */
	    & (file_ptr->file_copyrepl_array),
	    & (file_ptr->file_copyrepl_array_count));

  if (option_dump_copyrepl)
    {
      fprintf (stdout, "Copy for %s - %d copy statements",
	       file_ptr->file_name, file_ptr->file_copyrepl_array_count);
      dump_copyrepl (stdout, 
                     file_ptr->file_name,
		     file_ptr->file_copyrepl_array,
		     file_ptr->file_copyrepl_array_count);
    }

  /* Process the copies - read in the files etc.  */

  process_copies (file_ptr);

  copy_nested_level --;

}

/* Clean up token TOKEN_PTR which is a non-numeric literal, process
  "\\" \n '' and "" within string.  Returns cleaned up value
  SANITIZED_VALUE and upper case version SANITIZED_VALUE_UPPER and
  length SANITIZED_VALUE_LENGTH.  */

void
get_string_value (uchar **sanitised_value,
                         uchar **sanitised_value_upper,
                         uint32 *sanitised_length, 
                         struct token_struct *token_ptr)
{
  uchar first_char;
  uchar *from_ptr;
  uchar *to_ptr;
  uchar *end_ptr;

  BLI_ASSERT (token_ptr);
  BLI_ASSERT (sanitised_value);
  BLI_ASSERT (sanitised_length);

  BLI_ASSERT (token_ptr->token_type == STRING);  /* No 'almost' strings here.  */

  first_char = token_ptr->token_chars[0];

  *sanitised_length = token_ptr->token_length - 2;
  *sanitised_value = BLI_ALLOC (*sanitised_length + 1);
  *sanitised_value_upper = BLI_ALLOC (*sanitised_length + 1);

  /* Walk the from string, copying as we go.  */

  for (to_ptr =  *sanitised_value,
       from_ptr = token_ptr->token_chars + 1,
       end_ptr = from_ptr +  *sanitised_length; from_ptr != end_ptr; )
    {
      uchar char1;
      char1 =  *from_ptr;
      if (char1 == first_char)
	{ /* '' Or "".  */
	  BLI_ASSERT (*(from_ptr + 1) == first_char);
	  *to_ptr =  *from_ptr;
	  to_ptr ++;
	  from_ptr += 2;
	  continue;
	}
      *(to_ptr ++) =  *(from_ptr ++);  /* Other.  */
    }

  /* Make upper case copy.  */
  *sanitised_length = to_ptr -  *sanitised_value;
  for (from_ptr =  *sanitised_value, to_ptr =  *sanitised_value_upper;
       from_ptr !=
       (uchar *)((void *)((*sanitised_value) + (*sanitised_length)));
       from_ptr ++, to_ptr ++)
    {
      *to_ptr = TOUPPER (*from_ptr);
    }
  /* Null terminate names.  */
  (*sanitised_value)[ * sanitised_length] = 0;
  (*sanitised_value_upper)[ * sanitised_length] = 0;
}


/*******************************************************
 * Process nested copies for a file FILE_PTR
 ******************************************************  */

void
process_copies (struct file_struct *file_ptr)
{

  uint32 copyrepl_ix;

  BLI_ASSERT (file_ptr);

  for (copyrepl_ix = 0; copyrepl_ix < file_ptr->file_copyrepl_array_count;
       copyrepl_ix ++)
    {
      struct copyrepl_struct *copyrepl_ptr;
      struct token_struct *token_ptr;
      uchar *filename;
      uint32 filename_length;
      uint32 append_cpy_length;

      uchar *pathname;
      uint32 pathname_length = 0;

      uchar *full_filename;
      struct file_struct *copy_file_ptr;
      struct token_struct *last_delete;
      struct token_struct *first_delete;

      /* Remove the copy verb tokens.  */

      copyrepl_ptr = file_ptr->file_copyrepl_array->elem + copyrepl_ix;
      BLI_ASSERT (copyrepl_ptr);

      first_delete = copyrepl_ptr->copyrepl_copyrepl_token_ptr;
      BLI_ASSERT (first_delete != file_ptr->file_first_token);
      last_delete = copyrepl_ptr->copyrepl_fullstop_token_ptr;
      BLI_ASSERT (last_delete);
      if (last_delete->token_type == EOF_symbol)
	  last_delete = last_delete->token_prev;

      BLI_ASSERT (last_delete);

      first_delete->token_prev->token_next = last_delete->token_next;
      last_delete->token_next->token_prev = first_delete->token_prev;;

      /* Ignore broken verbs.  */

      if (!copyrepl_ptr->copyrepl_valid_flag)
	continue;

      /* Get the filename asis or UC (if bliss word).  */

      filename = copyrepl_ptr->copyrepl_filename_token_ptr->token_chars;
      filename_length = copyrepl_ptr->copyrepl_filename_token_ptr->token_length;
      ensure_token_upper (copyrepl_ptr->copyrepl_filename_token_ptr);

      if ((filename[0] == '\'') || (filename[0] == '\"'))
        {
          uchar *trash;
	  get_string_value ( & filename,  & trash,  & filename_length, copyrepl_ptr->copyrepl_filename_token_ptr);
          append_cpy_length = 0;
	}
      else
	{
	  filename = copyrepl_ptr->copyrepl_filename_token_ptr->token_chars_upper;
          append_cpy_length = 4;  /* .CPY.  */
	}

      /* 
         Library name is just a path name sans "/"
         get the path name UC and asis, determine case sensitivity.  */

      pathname = NULL;
      pathname_length = 0;
      if (copyrepl_ptr->copyrepl_library_token_ptr)
        {
          pathname = copyrepl_ptr->copyrepl_library_token_ptr->token_chars;
          pathname_length = copyrepl_ptr->copyrepl_library_token_ptr->token_length;
          ensure_token_upper (copyrepl_ptr->copyrepl_library_token_ptr);
        }

      if (pathname)
	{
	  if ((pathname[0] == '\'') || (pathname[0] == '\"'))
	    {
              uchar *trash;
	      get_string_value ( & pathname,  & trash,  & pathname_length, 
                                copyrepl_ptr->copyrepl_library_token_ptr);
	    }
	  else
	    {
	      pathname = copyrepl_ptr->copyrepl_library_token_ptr->token_chars_upper;
	    }
	}

      /* 
         
         If the pathname is an environment variable, (or if SYSLIB is
         an environment variable in the case of no pathname)
         substitute its value and mark it case sensitive.

         Also if the directory was specified in the -I option, use that value 
         and mark it case sensitive.  */

      if ((pathname) && (strncmp ((char *)pathname, "SYSLIB", 6)))
	{
          uchar *new_pathname = NULL;
          struct lib_details *ldet;
          for (ldet = first_other_directory; ldet; ldet = ldet->next)
            {
              if (!(strncmp ((char *)pathname, (char *)ldet->lib, pathname_length)))
                {
                  new_pathname = ldet->directory;
                  break;
                }
            }
          if (!new_pathname)
            {
              new_pathname =
                (uchar *)getenv ((const char *)pathname);
            }
          pathname = new_pathname ? new_pathname : pathname;
          pathname_length = strlen ((char *)pathname);
	}
      else
	{
          uchar *new_pathname = NULL;
	  new_pathname = 
            option_syslib_directory ?
            (uchar *)option_syslib_directory :
	    (uchar *)getenv ((const char *)"SYSLIB");
          pathname = new_pathname ? new_pathname : pathname;
          pathname_length = pathname ? strlen ((char *)pathname) : 0;
	}
      
      /* At last, build the file name.  */

      full_filename = BLI_ALLOC (2 + filename_length + pathname_length + append_cpy_length);
      /* Bug - filename etc cannot contain a null character.  */
      memcpy (full_filename, pathname, pathname_length);
      if (pathname)
        memcpy (full_filename + pathname_length, "/", 1);  /* Unix dependency.  */
      memcpy (full_filename + pathname_length + !!pathname, filename,
              filename_length);
      memcpy (full_filename + pathname_length + !!pathname + filename_length,
              ".CPY", append_cpy_length);  /* May be 0.  */
      full_filename[pathname_length + filename_length + !!pathname +
                   append_cpy_length] = 0;

      /* Create a file block for the file and process it.  */

      (copyrepl_ptr->copyrepl_copied_file_ptr) = BLI_ALLOC (sizeof (struct file_struct));
      copy_file_ptr = copyrepl_ptr->copyrepl_copied_file_ptr;

      copy_file_ptr->file_name = full_filename;
      copy_file_ptr->file_copyrepl_array_count =
	copyrepl_ptr->copyrepl_replacing_count;
      copy_file_ptr->file_copy_token_ptr =
	copyrepl_ptr->copyrepl_copyrepl_token_ptr;

      process_file (copy_file_ptr);

      /* Process the replacing clauses.  */

      process_replaces (copyrepl_ptr, copy_file_ptr->file_first_token,
			copy_file_ptr->file_last_token,
			SKIP_LEADING_WHITESPACE);

      /* If the copy verb was on a debugging line, all the included 
         data is regarded as debugging.  */

      if (copyrepl_ptr->copyrepl_copyrepl_token_ptr->token_flag_debug_line)
	{
	  for (token_ptr = copy_file_ptr->file_first_token;
	       token_ptr; token_ptr = token_ptr->token_next)
	    {
	      token_ptr->token_flag_debug_line = 1;
	    }
	}

      /* 
         Replaced tokens may paste together with other tokens
         find these cases and fix them up.  */
      redo_tokenising (copy_file_ptr);

      /* Now merge the file's tokens into the owning file's token stream.  */

      if (copy_file_ptr->file_first_token)
	{
	  BLI_ASSERT (copy_file_ptr->file_last_token);
	  BLI_ASSERT (copyrepl_ptr->copyrepl_fullstop_token_ptr->token_next);

	  first_delete->token_prev->token_next
	    = copy_file_ptr->file_first_token;
	  copy_file_ptr->file_first_token->token_prev
	    = first_delete->token_prev;

	  last_delete->token_next->token_prev
	    = copy_file_ptr->file_last_token;
	  copy_file_ptr->file_last_token->token_next
	    = last_delete->token_next;
	}
    }

}

/**************************************************
 * Process replace verbs on a file FILE_PTR
 *************************************************  */
void
process_replace (struct file_struct *file_ptr)
{

  uint32 replace_ix;
  struct token_struct *last_delete;
  struct token_struct *first_delete;

  /*
   * Find the replace verbs.  */

  blippr1 ( /* Input parms.  */
	    file_ptr->file_first_token, 0,  /* Find replaces.  */
	    /* Output parms.  */
	    & (file_ptr->file_copyrepl_array),
	    & (file_ptr->file_copyrepl_array_count));

  if (option_dump_copyrepl)
    {
      fprintf (stdout, "\nReplace statements\n");
      dump_copyrepl (stdout, file_ptr->file_name,
		     file_ptr->file_copyrepl_array,
		     file_ptr->file_copyrepl_array_count);
    }

  /* 
   * Process the replaces.  */

  /*
   * For each replace verb, determine scope
   * then call process_replaces for each scope
   * then move on to the next verb.  */

  for (replace_ix = 0; replace_ix < file_ptr->file_copyrepl_array_count;
       replace_ix ++)
    {

      struct copyrepl_struct *copyrepl_ptr;
      struct token_struct *start_token;
      struct token_struct *end_token;


      copyrepl_ptr = file_ptr->file_copyrepl_array->elem + replace_ix;

      /* Delete the tokens for the replace verb.  */

      first_delete = copyrepl_ptr->copyrepl_copyrepl_token_ptr;
      BLI_ASSERT (first_delete != file_ptr->file_first_token);
      last_delete = copyrepl_ptr->copyrepl_fullstop_token_ptr;
      BLI_ASSERT (last_delete);
      if (last_delete->token_type == EOF_symbol)
	{
	  last_delete = last_delete->token_prev;
	  BLI_ASSERT (last_delete);
	}

      last_delete->token_next->token_prev = first_delete->token_prev;;
      first_delete->token_prev->token_next = last_delete->token_next;

      /* Don't process bad statements.  */

      if (!copyrepl_ptr->copyrepl_valid_flag)
	continue;

      /* Replace off - nothing to do.  */

      if (copyrepl_ptr->copyrepl_replace_off_flag)
	continue;

      BLI_ASSERT (!copyrepl_ptr->copyrepl_is_copy_flag);


      /*
       * Find the scope 
       * basically from the end of the replace
       * to the start of the next replace or EOF.  */

      start_token = copyrepl_ptr->copyrepl_fullstop_token_ptr;
      if (!start_token)
	continue;

      /* Start after the full stop.  */
      start_token = start_token->token_next;
      if (!start_token)
	continue;

      /* If this is the last one, its scope is to EOF.  */
      if (replace_ix == file_ptr->file_copyrepl_array_count - 1)
	{
	  end_token = file_ptr->file_last_token;
	}
      else
	{
	  struct copyrepl_struct *next_copyrepl_ptr;
	  next_copyrepl_ptr = file_ptr->file_copyrepl_array->elem + replace_ix +1;
	  end_token = next_copyrepl_ptr->copyrepl_copyrepl_token_ptr;
	  BLI_ASSERT (end_token);
	}

      /* 
         It's just too easy when you have beautiful modular factored code
         (Your mileage may vary).  */

      process_replaces (copyrepl_ptr, start_token, end_token,
			DONT_SKIP_LEADING_WHITESPACE);
    }
}

/**************************************************
 * Output results of bliss preprocessor for main file FILE_PTR to file
 * named OUT_FNAME
 *************************************************  */

void
output_resulting_file (struct file_struct *file_ptr, uchar *out_fname)
{

  FILE *f;
  struct token_struct *tk1;
  struct file_struct *current_file = NULL;
  uint32 current_line_nbr = 0;
  dynarray_uchar *current_line;
  uint32 current_line_used_flag = 0;
  uint32 current_line_used_count = 0;
  dynarray_uchar *current_debug_info = NULL;
  uint32 current_debug_info_length = 0;
  uint32 current_debug_info_used = 0;
  uint32 blanks_passed = 0;
  uint32 save_blanks_passed = 0;
  uint32 pending_suppress_comment_line = 0;

  f = fopen ((char *)out_fname, "wb");
  if (!f)
      bliumsc_pfatal_with_name (out_fname);

  current_line = (dynarray_uchar *) ALLOC_DYN (80, sizeof(uchar));

  if (option_output_linenos)
    {
      current_debug_info_length = 100;
      current_debug_info = (dynarray_uchar *)
        ALLOC_DYN (current_debug_info_length,
                   sizeof(uchar));
    }

  /* Walk the tokens, printing them out.  */
  for (tk1 = file_ptr->file_first_token; tk1; tk1 = tk1->token_next)
    {

      if (ignore_tokens[tk1->token_type] == IGNORE_TOKENS_BLANK)
	{
	  if (tk1->token_type == NEWLINE)
	    {
              if (pending_suppress_comment_line)
                {
                  pending_suppress_comment_line = 0;
                  continue;
                }
	      /* Ignore new lines within replacing text except in pseudo text.  */
	      if (!(tk1->token_flag_replacement) ||
		  (tk1->token_flag_from_pseudo_text))
		{
		  if (!current_line_used_flag)
		    {
		      init_line (current_line,
				 & current_line_used_flag,
				 & current_line_used_count,
				 current_debug_info,
				 & current_debug_info_length,
				 & current_debug_info_used,
				 & blanks_passed, tk1);
		    }
		  flush_line (f, current_line, &current_line_used_flag,
			      &current_line_used_count,
			      current_debug_info,  &current_debug_info_used,
			      &blanks_passed);
		  continue;
		}
	    }
	  if (tk1->token_type == BLANKS)
	    {
	      blanks_passed += tk1->token_length;
	    }
	  continue;
	}

      if (ignore_tokens[tk1->token_type] == IGNORE_TOKENS_COMMENT)
	{
          if (tk1->token_type == COMMENT)
            {
              if (!option_output_comments)
                {
                  pending_suppress_comment_line = 1;
                  current_line_used_count = 0;
                  current_line_used_flag = 0;
                  continue;
                }
            }
	  else
            /*
              Almost bug 
              - throw away comment-entries 
              - throw away comments in comment-entries
              - throw away comment-entry paragraphs except date-compiled.
              These have no effect on the output code and they
              are obsolete so I could not give a tinker's cuss. Sorry.  */
            continue;

	  /* 
	     It is undefined what happens to comments in identifiers
	     that replace other text.  The standard writers don't seem to 
	     have realised that identifiers can be multi word multi line.
	     Throw them away.
             - I tell a lie - CD1.7 displays awareness of this issue.  */
	  if ((tk1->token_flag_replacement) &&
	      !(tk1->token_flag_from_pseudo_text))
            continue;
	}

      /* 
         Init new line if needed.  */
      if (!current_line_used_flag)
	{
	  init_line (current_line,  
                     &current_line_used_flag,
		     &current_line_used_count,
		     current_debug_info,
		     &current_debug_info_length,
		     &current_debug_info_used,  &blanks_passed, tk1);
	}

      /* 
         Init new line if needed.  */
      if (
	  (
	   (tk1->token_flag_debug_line ==
	    (current_line->elem[option_no_seq ? 0 : 6] == ' '))
	   || ((tk1->token_flag_from_pseudo_text)
	       && ((tk1->token_charno) <= current_line_used_count)))
          && (blanks_passed))
	{
	  if (current_line_used_flag)
	    {
	      save_blanks_passed = blanks_passed;
	      flush_line (f, current_line,  &current_line_used_flag,
			  &current_line_used_count,
			  current_debug_info,  & current_debug_info_used,
			  & blanks_passed);
	    }
	  blanks_passed = save_blanks_passed;
	  init_line (current_line, &current_line_used_flag,
		     & current_line_used_count,
		     current_debug_info,
		     & current_debug_info_length,
		     & current_debug_info_used,  & blanks_passed, tk1);
	}


      /* Add necessary spaces if allowed.  */
      if (blanks_passed)
	{
	  blanks_passed = 0;
	  if ((tk1->token_charno + comment_offset) <= current_line_used_count)
	    {
	      /* Add one space to separate the tokens.  */
	      paste_text (current_line, &current_line_used_flag,
			  &current_line_used_count,
			  (uchar *)" ", 1);
	    }
	  else
	    {
	      int32 gap;
	      int32 gap20;
	      gap = tk1->token_charno - current_line_used_count - 1;
	      while (gap > 0)
		{
		  gap20 = (gap < 20) ? gap : 20;
		  paste_text (current_line, &current_line_used_flag,
                              &current_line_used_count,
                              (uchar *)"                    ", gap20);
		  gap -= gap20;
		}
	    }
	}

      paste_token (current_line,  &current_line_used_flag,
		   &current_line_used_count, tk1);

      current_file = tk1->token_file;
      current_line_nbr = tk1->token_lineno;

    }

  if (current_line_used_flag)
    {
      flush_line (f,
		  current_line,  &current_line_used_flag,
		  &current_line_used_count,
		  current_debug_info,  & current_debug_info_used,
		  &blanks_passed);
    }

  fclose (f);
}

/**************************************************
 * Paste a token TK1 at the end of a line CURRENT_LINE updating
 * use count CURRENT_LINE_USE_COUNT for the current line
 *************************************************  */

void
paste_token (dynarray_uchar *current_line_ptr, uint32 *current_line_used_flag_ptr,
             uint32 *current_line_used_count_ptr,
             struct token_struct *tk1)
{
  
  paste_text (current_line_ptr, current_line_used_flag_ptr,
	      current_line_used_count_ptr,
	      tk1->token_chars, tk1->token_length);

}

/**************************************************
 * Past text S length L at the end of line CURRENT_LINE_PTR updating
 * characters used count CURRENT_LINE_USE_COUNT for the current line
 *************************************************  */

void
paste_text (dynarray_uchar *current_line_ptr, 
            uint32 *current_line_used_flag_ptr ATTRIBUTE_UNUSED,
            uint32 *current_line_used_count_ptr,
            const uchar *s, uint32 l)
{

  GROW_DYN (&current_line_ptr->dynarray_details, 
            *current_line_used_count_ptr + l);

  memcpy (current_line_ptr->elem +  *current_line_used_count_ptr, s, l);
  (*current_line_used_count_ptr) += l;
}

/**************************************************
 * Output a complete line CURRENT_LINE to file F if
 * CURRENT_LINE_USED_FLAG is true - length is
 * CURRENT_LINE_USED_COUNT.  Output trailing blanks per
 * BLANKS_PASSED. Output a debug line if CURRENT_DEBUG_INFO_USED.
 *************************************************  */

void
flush_line (FILE *f,
            dynarray_uchar *current_line_ptr, uint32 *current_line_used_flag,
            uint32 *current_line_used_count,
            dynarray_uchar *current_debug_info, uint32 *current_debug_info_used,
            uint32 *blanks_passed)
{

  uint32 blank_ix = 0;

  BLI_ASSERT ((*current_line_used_flag == 1) || (*current_line_used_flag == 0));

  if (*current_debug_info_used)
    {
      uchar *dbg_char;
      dbg_char = current_debug_info->elem;
      while (*current_debug_info_used)
	{
	  (*current_debug_info_used) --;
	  fputc (*(dbg_char++), f);
	}
      fputc ('\n', f);
    }
  if (*current_line_used_flag)
    {
      uchar *line_char;
      line_char = current_line_ptr->elem;
      while (*current_line_used_count)
	{
	  (*current_line_used_count) --;
	  fputc (*(line_char ++), f);
	}
      for (blank_ix = 0; blank_ix <  *blanks_passed; blank_ix ++)
	{
	  fputc (' ', f);
	}
      fputc ('\n', f);
    }
  *current_line_used_flag = 0;
  BLI_ASSERT (!(*current_line_used_flag));
  *blanks_passed = 0;
}

/**************************************************
 * Init a line CURRENT_LINE including used flag
 * CURRENT_LINE_USED_FLAG, characters used CURRENT_LINE_USED_COUNT_PTR
 * debugging line details CURRENT_DEBUG_INFO_PTR and length
 * CURRENT_DEBUG_INFO_LENGTH_PTR and debug info presence flag
 * CURRENT_DEBUG_INFO_USED_PTR, blanks passed flag
 * BLANKS_PASSED up to first token TK1
 *************************************************  */

void
init_line (dynarray_uchar *current_line_ptr,
           uint32 *current_line_used_flag_ptr,
           uint32 *current_line_used_count_ptr,
           dynarray_uchar *current_debug_info_ptr,
           uint32 *current_debug_info_length_ptr,
           uint32 *current_debug_info_used_ptr,
           uint32 *blanks_passed, 
           struct token_struct *tk1)
{

  struct line_struct *line1;
  uchar comment_char = ' ';
  static struct file_struct *prev_file = NULL;
  static uint32 prev_lineno = -1;


  if (option_output_linenos)
    {
      int printsize;
      uint32 OK = 0;
      struct token_struct *parent_token;

      if ((tk1->token_file == prev_file)
	  && (tk1->token_lineno == (prev_lineno + 1)))
	{
	  prev_lineno = tk1->token_lineno;
	}
      else
	{
	  prev_file = tk1->token_file;
	  prev_lineno = tk1->token_lineno;

	  while (!OK)
	    {
	      printsize =
		snprintf ((native_char *) current_debug_info_ptr->elem,
			  *current_debug_info_length_ptr, "%s#%s:%i",
			  option_no_seq ? "" : "      ",
			  tk1->token_file->file_short_name,
                          tk1->token_lineno);
	      /* Different return value depending on glibc version.  */
	      if ((printsize < 0) || ((uint32)printsize >  *current_debug_info_length_ptr))
                {
		  GROW_DYN (&current_debug_info_ptr->dynarray_details, 
                            (*current_debug_info_length_ptr) * 2);
                }
	      else
		{
		  OK = 1;
		  *current_debug_info_used_ptr += printsize;
		}
	    }
	  for (parent_token = tk1->token_file->file_copy_token_ptr;
	       parent_token;
	       parent_token = parent_token->token_file->file_copy_token_ptr)
	    {
	      OK = 0;
	      while (!OK)
		{
		  uint32 available_size;
		  uchar *current_address =
		    current_debug_info_ptr->elem +  *current_debug_info_used_ptr;
		  available_size =
		    *current_debug_info_length_ptr -
		    *current_debug_info_used_ptr;
		  printsize =
		    snprintf ((char *)current_address, available_size, "<%s:%i",
                              parent_token->token_file->file_short_name,
			      parent_token->token_lineno);
		  /* Different return value depending on glibc version.  */
		  if ((printsize < 0) || ((printsize +  *current_debug_info_used_ptr) >
			  *current_debug_info_length_ptr))
                    {
                      *current_debug_info_length_ptr = (*current_debug_info_length_ptr) * 2;
                      GROW_DYN (&current_debug_info_ptr->dynarray_details, 
                                *current_debug_info_length_ptr);
                    }
		  else
		    {
		      OK = 1;
		      *current_debug_info_used_ptr += printsize;
		    }
		}
	    }
	}
    }

  *current_line_used_count_ptr = 0;
  BLI_ASSERT ((tk1->token_file->file_line_count) >= (tk1->token_lineno));
  BLI_ASSERT (tk1->token_lineno > 0);
  line1 = (tk1->token_file->file_lines->elem) + ((tk1->token_lineno) - 1);
  BLI_ASSERT (line1);

  (*current_line_used_flag_ptr) = 1;

  if (!option_no_seq)
    {
      uint32 line1_length;
      line1_length = line1->line_end_orig - line1->line_start_orig;
      if (line1_length > 5)
	{
	  memcpy (current_line_ptr->elem, (native_char *)(line1->line_start_orig), 6);
	  *current_line_used_count_ptr = 6;
	}
      else
	{
	  memcpy (current_line_ptr->elem, (native_char *)(line1->line_start_orig),
		  line1_length);
	  *current_line_used_count_ptr = line1_length;
	  return;
	}
    }

  if (tk1->token_type == COMMENT)
    {
      current_line_ptr->elem[(*current_line_used_count_ptr)] = '*';
      return;
    }

  if (tk1->token_flag_debug_line)
    {
      comment_char = 'd';  /* Line1->line_ind_char;.  */
    }

  current_line_ptr->elem[(*current_line_used_count_ptr) ++] = comment_char;

  /* Pseudo text at start of line needs extra spaces.  */
  if (tk1->token_flag_from_pseudo_text)
    {
      int blanks_passed_work;
      blanks_passed_work = tk1->token_charno -  *current_line_used_count_ptr - 1;
      BLI_ASSERT (blanks_passed_work >= 0);
      *blanks_passed = (uint32)blanks_passed_work;
    }

}

/*******************************************
 * Print program version
 ******************************************  */

static void
version (void)
{
  fprintf (stdout, "blipre (BlissForGCC) %s"
	   "\nCopyright (C) 1999, 2000, 2001, 2002 Free Software Foundation."
	   "\nBlissForGCC comes with NO WARRANTY "
	   "to the extent permitted by law."
	   "\nYou may redistribute copies of BlissForGCC"
	   "\nunder the terms of the GNU General Public Licence."
	   "\nFor more information about these matters,"
	   "\nsee the files named COPYING.\n", "blipre "BLISSFORGCC_VERSION);
  return;
}

