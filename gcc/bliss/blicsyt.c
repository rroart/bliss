/* 
   
   BLISS Compiler parser symbol table and parse tree (blicsyt.c)

   -------------------------------------------------------------------
   
   Copyright (C) 1999, 2000, 2001 Free Software Foundation, Inc.
   
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
   
   -------------------------------------------------------------------.  */

#include "blir_temp_config.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "system.h"
#include "ansidecl.h"
#include "ggc.h"

#include "blidebug.h"
#include "bliumsc.h"
#include "bliumem.h"
#include "bliumsg.h"
#include "bliclex.h"
#include "blicc1.h"
#include "blicsymn.h"
#include "blicsyt.h"
#include "blicpru.h"

static uint32 program_count = 0;

#define MAGIC 000777000777u

/* Symbol table hash table.  */
static struct symtab_entry * *  GTY((length ("work_string_length"))) symbol_table_hash = NULL;

/* Hash table size.  */
static uint32 symbol_table_hash_entries = 0;

struct bli_tree_struct_program * GTY(()) top_level_program = NULL;

/* Create the symbol table.  TOKEN_COUNT is a hint how big it needs to be to avoid too many clashes.  */
void
create_symbol_table (uint32 token_count)
{
  BLI_ASSERT (!symbol_table_hash);
  symbol_table_hash_entries = (token_count < 10) ? 10 : token_count;
  symbol_table_hash = BLI_ALLOC (symbol_table_hash_entries * sizeof (void *));
 }

/* Cleanup the symbol table.  Any non global items will be chopped.  */
void
cleanup_symbol_table ()
{
  uint32 symbol_hash_ix;
  void **store_ptr_here;
  struct symtab_entry *current_item;

  /* Link the still used items; unlink the others.  ggc will free them if it needs to.  */
  for (symbol_hash_ix = 0; symbol_hash_ix < symbol_table_hash_entries; symbol_hash_ix ++)
    {
      store_ptr_here = (void **)symbol_table_hash + symbol_hash_ix;
      for (current_item = symbol_table_hash[symbol_hash_ix];
           current_item; current_item = current_item->symtab_hash_next)
          if (current_item->symtab_entry_flags & SYMBOL_TABLE_FLAG_GLOBAL)
            {
              * store_ptr_here = current_item;
              store_ptr_here = (void **) & (current_item->symtab_hash_next);
            }
    }
}

/* Tell symbol table that we have a new program, so old symbols are obsolete.  */

void symbol_table_new_pgm ()
{
  program_count ++;
  cleanup_symbol_table ();
}

/* Add entry type TYPE, with token or data_item or label ITEM , to the symbol table.  */

void *
create_symtab_entry (enum symtab_entry_type_enum type, void *item)
{

  struct symtab_entry *st; 
  uint32 index; 
  struct bli_token_struct *tk;
  struct bli_tree_struct_data_item *di;
  struct bli_tree_struct_label *lb;
  uchar *my_name;
  uint32 my_name_length;

  BLI_ASSERT (symbol_table_hash); 

  st = BLI_ALLOC (sizeof (struct symtab_entry));

  st->magic = MAGIC;
  st->symtab_entry_type = type;
  st->symtab_entry_item = item;

  BLI_ASSERT (item);

  tk = item;
  
  if (tk->pfx.cat != type_token)
    {
      BLI_ASSERT (tk->pfx.cat == type_production);
      if (tk->pfx.type.tok_type == BLI_PROD_TYPE_DATA_ITEM)
        {
          di = item;
          tk = di->data_name_token;
        }
      else
        {
          lb = item;
          BLI_ASSERT ((tk->pfx.type.tok_type == BLI_PROD_TYPE_PARAGRAPH)
                 || (tk->pfx.type.tok_type == BLI_PROD_TYPE_SECTION));
          tk = lb->pfx.reference_token;
        }
    }

  /* Do not add fillers to the hash table.  */
  if ((!tk) || (tk->pfx.type.tok_type == FILLER))
    return st;
  
  my_name = tk->string_details->string_upper->string;
  my_name_length = tk->string_details->length;

  st->symtab_entry_name = my_name;
  st->symtab_entry_name_length = my_name_length;
  
  index = bliumsc_hash_string (my_name, my_name_length) % symbol_table_hash_entries;
  
  st->symtab_hash_next = symbol_table_hash[index];
  symbol_table_hash[index] = st;

  return st;
}

/* Overwrite attributes FLAGS_MASK with new values FLAGS of symbol
   table entry ENTRY.  */

void
put_symtab_entry_attribute_flags (void *entry, uint32 flags,
				  uint32 flags_mask)
{

  struct symtab_entry *st;

  st = entry;
  BLI_ASSERT (st);
  BLI_ASSERT (st->magic == MAGIC);

  st->symtab_entry_flags &= ~flags_mask;
  st->symtab_entry_flags |= (flags & flags_mask);

}

/* Obtain the attributes FLAGS_MASK of symbol table entry ENTRY.  */

uint32
get_symtab_entry_attribute_flags (void *entry, uint32 flags_mask)
{

  struct symtab_entry *st;

  st = entry;
  BLI_ASSERT (st);
  BLI_ASSERT (st->magic == MAGIC);

  return (st->symtab_entry_flags & flags_mask);

}

/* Get the payload from symbol table entry ENTRY.  */

void *
get_symtab_entry_item (void *entry)
{

  struct symtab_entry *st;

  st = entry;
  BLI_ASSERT (st);
  BLI_ASSERT (st->magic == MAGIC);
  BLI_ASSERT (st->symtab_entry_item);
  return st->symtab_entry_item;  /* Null for builtin function.  */
}

/* Get type of sumbol table entry ENTRY.  */

enum symtab_entry_type_enum
get_symtab_entry_type (void *entry)
{

  struct symtab_entry *st;

  st = entry;
  BLI_ASSERT (st);
  BLI_ASSERT (st->magic == MAGIC);

  return st->symtab_entry_type;

}

/* Get next symbol table entry up in hierarchy from ENTRY.  */

void *
get_symtab_entry_owner (void *entry)
{

  struct symtab_entry *st;

  st = entry;
  BLI_ASSERT (st);
  BLI_ASSERT (st->magic == MAGIC);

  return st->symtab_entry_owner;

}

/* Set hierarchical owner of symbol table entry ENTRY to OWNER.  */

void
set_symtab_entry_owner (void *entry, void *owner)
{

  struct symtab_entry *st;
  struct symtab_entry *st2;

  st = entry;
  BLI_ASSERT (st);
  BLI_ASSERT (st->magic == MAGIC);

  st2 = owner;
  BLI_ASSERT (st2);
  BLI_ASSERT (st2->magic == MAGIC);

  st->symtab_entry_owner = owner;

}

/* Set object of symbol table entry ENTRY to OBJECT.  */

void
set_symtab_entry_item (void *entry, void *item)
{

  struct symtab_entry *st;

  st = entry;
  BLI_ASSERT (st);
  BLI_ASSERT (st->magic == MAGIC);

  st->symtab_entry_item = item;

}

/* Find the (first synonym) symbol table entry for NAME of length NAME_LENGTH.  */

void *
find_symtab_entry (uchar *name, uint32 name_length)
{

  struct symtab_entry *st;
  uint32 index;

  BLI_ASSERT (symbol_table_hash);

  index = bliumsc_hash_string (name, name_length) % symbol_table_hash_entries;

  for (st = symbol_table_hash[index]; st; st = st->symtab_hash_next)
    {
      BLI_ASSERT (st->magic == MAGIC);
      if (name_length != st->symtab_entry_name_length)
	continue;
      if (memcmp (name, st->symtab_entry_name, name_length))
	continue;
      return st;
    }

  return NULL;

}

/* Find the next synonym symbol table entry for NAME of length NAME_LENGTH.  */

void *
find_next_symtab_entry (void *entry, uchar *name, uint32 name_length)
{

  struct symtab_entry *st;

  st = entry;
  BLI_ASSERT (st);
  BLI_ASSERT (st->magic == MAGIC);

  st = st->symtab_hash_next;

  for (; st; st = st->symtab_hash_next)
    {
      BLI_ASSERT (st->magic == MAGIC);
      if (name_length != st->symtab_entry_name_length)
	continue;
      if (memcmp (name, st->symtab_entry_name, name_length))
	continue;
      return st;
    }
  return NULL;

}

/* Find an entry in the symbol table which matches not just one name
   but the whole name hierarchy.  NBR_NAMES is the number of names in
   the hierarchy NAMES, and NAME_LENGTHS are the lengths of each NAMES
   entry, returns 0,1,2 depending on 0 1 or more matches and returns
   the individual entry in *ENTRY. Entry must be of type TYPE to
   match.  In the arrays the most subordinate entry is the [0] entry
   (ie the highest BLISS level number). The caller needs to make sure
   that the lowest entry is not a filler.  */

uint32 /* 0==Not found 1=found unique 2=found not unique.  */
find_symtab_entry_hierarchy (uint32 nbr_names, uchar **names, uint32 *name_lengths,
			     /* Return.  */void **entry, enum symtab_entry_type_enum type)
{
  struct symtab_entry *st;
  struct symtab_entry *st2;
  uint32 match_count = 0;
  uint32 name_ix;

  BLI_ASSERT (nbr_names > 0);
  * entry = NULL;

  BLI_ASSERT (name_lengths[0] != 0);  /* Should not be filler.  */

  /* For each possible match.  */
  for (st = find_symtab_entry (names[0], name_lengths[0]);  /* Per matching lowest entry.  */
       st; st = find_next_symtab_entry (st, names[0], name_lengths[0]))
    {
      if (type)
        if (st->symtab_entry_type != type)
          continue;
      /* For each element in the names array.  */
      for (st2 = st->symtab_entry_owner, name_ix = 1;  /* Per element in list.  */
	   name_ix < nbr_names; name_ix ++)
	{
	  uint32 success = 0;
          if (name_lengths[name_ix] == 0) /* Filler.  */
            continue;
          /* Scanning up in the data item hierarchy.  */
	  for (; st2 && (!success); st2 = st2->symtab_entry_owner)
            if (st2->symtab_entry_name_length &&  /* Case of filler.  */
                (st2->symtab_entry_name_length == name_lengths[name_ix]) &&
                (!memcmp (st2->symtab_entry_name, names[name_ix],
                          name_lengths[name_ix])))
              success = 1;
	  if (!success)
            break; 
	}
      if (name_ix == nbr_names)
	{
          if (!match_count)
            * entry = st;  /* First match found.  */
	  match_count ++;
	}
    }
  return match_count > 2 ? 2 : match_count;
}

/* Find next entry in the symbol table after ENTRY which matches not
   just one name but the whole name hierarchy.  NBR_NAMES is the number
   of names in the hierarchy NAMES, and NAME_LENGTHS are the lengths
   of each NAMES entry, returns 0 fi no match found, 1 if a match was
   found and returns the individual entry in *ENTRY. Entry must be of
   type TYPE to match if TYPE is non zero.  In the arrays the most
   subordinate entry is the [0] entry (ie the highest BLISS level
   number). The caller needs to make sure that the lowest entry is not
   a filler.  */

uint32 /* 0==Not found 1=found one or more.  */
find_next_symtab_entry_hierarchy (uint32 nbr_names, 
                                  uchar **names, uint32 *name_lengths,
                                  /* Input and return.  */void **entry, 
                                  enum symtab_entry_type_enum type)
{
  struct symtab_entry *st;
  struct symtab_entry *st2;
  uint32 name_ix;

  BLI_ASSERT (nbr_names > 0);
  * entry = NULL;

  BLI_ASSERT (name_lengths[0] != 0);  /* Should not be filler.  */

  for (st = find_next_symtab_entry (entry, names[0], name_lengths[0]);  /* Per matching lowest entry.  */
       st; st = find_next_symtab_entry (st, names[0], name_lengths[0]))
    {
      if (type)
        if (st->symtab_entry_type != type)
          continue;
      for (st2 = st->symtab_entry_owner, name_ix = 1;  /* Per element in list.  */
	   name_ix < nbr_names; name_ix ++)
	{
	  uint32 success = 0;
          if (name_lengths[name_ix] == 0) /* Filler.  */
            continue;
	  for (; st2 && (!success); st2 = st2->symtab_entry_owner)
            if (st2->symtab_entry_name_length &&  /* Case of filler.  */
                (st2->symtab_entry_name_length == name_lengths[name_ix]) &&
                (!memcmp (st2->symtab_entry_name, names[name_ix],
                          name_lengths[name_ix])))
              success = 1;
	  if (!success)
            break;
	}
      if (name_ix == nbr_names)
	{
	  * entry = st;  /* First match in file.  */
          break;
	}
    }
  return * entry != NULL;
}

/* Issue an error and return 1 if a symbol for token TOKEN is used in some
   other namespace than TYPE.  */

uint32
check_symtab_entry_namespace (struct bli_token_struct *token, enum symtab_entry_type_enum type)
{
  void *ste;
  enum symtab_entry_type_enum tab_type;
  for (ste = find_symtab_entry (token->string_details->string_upper->string, token->string_details->length);
      ste;
      ste = find_next_symtab_entry
        (ste, token->string_details->string_upper->string, token->string_details->length))
    {
      tab_type = get_symtab_entry_type (ste);
      if (tab_type != type)
        {
          /* Used in >1 name space.  */
          MSG_TOKEN (MSGID (165), token, NULL);
          return 1;
        }
    }
  return 0;
}
