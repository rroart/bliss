/*====================================================================*
 *     This program is free software; you can redistribute it and/or  *
 *     modify it under the terms of the GNU Lesser General Public     *
 *     License as published by the Free Software Foundation; either   *
 *     version 2 of the License, or (at your option) any later version*
 *                                                                    *
 *     Copyright (c) 2000, 2001  Daniel H.  Ardison  dardison@hotmail.com    *
 *====================================================================.  */

/* BLISS Inspect routine structure & function definitions.  */

/* Details of tallying before/after.  */

struct blir_before_after_details
{
  uchar *before_value;  /* The value to scan for.  */
  uint32 before_value_length;  /* The length of the value to
                                     scan for 0 if not specified.  */
  uchar *after_value;  /* The value to scan for.  */
  uint32 after_value_length;  /* The length of the value to
                                    scan for 0 if not specified.  */
  uint32 before_initial_flag : 1;  /* On if initial specified
                                       with before.  */
  uint32 after_initial_flag : 1;  /* On if initial specified
                                      with after.  */
};

/* Details of one all/leading instance.  */

struct blir_all_leading_details
{
  uchar *identifier3;  /* Identifier 3 or literal-1
                              below, not null terminated.  */
  uint32 identifier3_length;
  struct blir_before_after_details details;  /* Details.  */
};

/* Details of tallying all/leading.  */

struct blir_inspect_all_leading
{
  uint32 all_leading_details_count;  /* Number of before
                                           after details.  */
  struct blir_all_leading_details *details;  /* Array of details.  */
  uint32 all_flag : 1;  /* All if on else leading.  */
};

/* Details of a sub clause tallying characters or tallying
all/leading.  */

union blir_inspect_tallying_details_union
{
  struct blir_inspect_all_leading all_leading;
  struct blir_before_after_details characters;

};

/* Details of an inspect tallying stmt or sub statement.  */

struct blir_inspect_tallying_struct
{
  uint32 *identifier2;  /* Tally into variable.  */
  union blir_inspect_tallying_details_union details;
  uint32 char_flag : 1;  /* Details of characters if on else all/leading.  */
};

/* Different options within inspect replacing.  */
enum blir_replacing_type_enum
{
  blir_replacing_type_characters = 1,  /* Replacing characters.  */
  blir_replacing_type_all,  /* Replacing all.  */
  blir_replacing_type_leading,  /* Replacing leading.  */
  blir_replacing_type_first /* Replacing first.  */
};

/* One replacing details.  */

struct blir_inspect_replacing_struct
{
  enum blir_replacing_type_enum type;
  /* Next two items null/0 for "replacing characters".  */
  uchar *identifier3;  /* Identifier-3 or literal-1 to
                              scan for.  */
  uint32 identifier3_length;  /* Length of above.  */

  uchar *identifier5;  /* Identifier-5 or literal-3 to
                              replace with.  */
  uint32 identifier5_length;  /* Length of above.
                                    Note that this length, if shorter than identifier3_length,
                                    means that the replacing value should be repeated
                                    until identifier3_length is all used up
                                    - this is to support figurative constants.  */
  struct blir_before_after_details details;
};

/* Looks  for the index of string Target in string Source and returns it
   index returned starting at 1
   if not found returns 0.  */

uint32 blir_inspect_indexof (uchar *source,
                                  uint32 source_length, 
                                  uchar *target,  /* Target to look for.  */
                                  uint32 target_length);

/* Returns a substring of string Source, starting at Start_position   
   with length Substring_len, if Start_position+Substring_len is grater than Source_length
   it will truncate to Source_length.  */

uchar *blir_inspect_substring (uchar *source,
                                     uint32 source_length, 
                                     uint32 start_position,  /* Initial source position is 1.  */
                                     uint32 substring_len);  /* Substring lenght of returned string.  */
/* Substring above is null terminated.  */

/* Converting functionality of BLISS inspect clause interface.  */

void
blir_inspect_converting (
                         uchar *identifier1,
                         uint32 identifier1_length,  /* Identifier 1 below, not null terminated.  */
                         uchar *identifier6,  /* Identifier-6 or literal-4 to
                                                         convert from.  */
                         uint32 identifier6_length,  /* Length of above.  */
                         uchar *identifier7,  /* Identifier-7 or literal5 to
                                                         convert to.  */
                         uint32 identifier7_length,  /* Length of above.  */
                         uint32 before_after_count,  /* Number of before after
                                                           details.  */
                         struct blir_before_after_details *details /* Array of details.  */
                         );


/* Replacing functionality of BLISS inspect clause interface.  */

void
blir_inspect_replacing (
                        uchar *identifier1,
                        uint32 identifier1_length,  /* Identifier 1 below, not null terminated.  */
                        uint32 replacing_count,  /* Number of replacing details.  */
                        struct blir_inspect_replacing_struct *replacing_details,  /* Array of replacing details.  */
                        uint32 *replace_status,  /* Vector to store status of replacement tasks.  */
                        /* Values are : 1 --> initial value means no replacements have done.  */
                        /*              2 --> Replacement has been initiated.  */
                        /*              0 --> Replacement is finished.  */
                        uint32 *after_bound,  /* Vector store index of after position for each replacement.  */
                        uint32 *before_bound /* Vector store index of before position for each replacement.  */
                        /* All this three, upper defined vectors occurs replacing_count times.  */);


/* Tallying functionality of BLISS inspect clause interface.  */

void
blir_inspect_tallying (
                       uchar *identifier1,
                       uint32 identifier1_length,  /* Identifier 1 below, not null terminated.  */
                       uint32 tallying_count,  /* Number of tallying details.  */
                       struct blir_inspect_tallying_struct *tallying_details,  /* Array
                                                                                  of tallying details.  */
                       uint32 **tally_status,
                       uint32 **after_bound,
                       uint32 **before_bound
                       );

