/*  

     BLISS Compiler auto garbage collection interface definitions.
     Copyright (C) 2002 Free Software Foudnation, Inc.

     ---------------------------------------------------------------------------

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

     ---------------------------------------------------------------------------.  */

/* externs for blicgty.c.  */

/* 1. To be GCed.  */
extern GTY(()) tree saved_perform_ptr_array_decl;
extern GTY(()) tree integer8_unsigned_type_node;
extern GTY(()) tree integer8_type_node;
extern GTY(()) tree integer16_unsigned_type_node;
extern GTY(()) tree integer16_type_node;
extern GTY(()) tree integer32_unsigned_type_node;
extern GTY(()) tree integer32_type_node;
extern GTY(()) tree integer64_unsigned_type_node;
extern GTY(()) tree integer64_type_node;
extern GTY(()) tree void_pointer_type_node;
extern GTY(()) tree char_pointer_type_node;
extern GTY(()) tree space_exp;
extern GTY(()) tree saved_perform_sequ_array_decl;
extern GTY(()) tree saved_call_count_decl;
extern uchar * GTY((length ("nonunique_name_length"))) nonunique_name;
extern uchar * GTY((length ("work_string_length"))) work_string;
extern uchar * GTY((length ("fixed_id_length"))) fixed_id;
extern GTY(()) tree limits_trees_mod[20];
extern GTY(()) uint32 limits_trees_sizes[20];

extern GTY(()) tree moveall_fn_decl;
extern GTY(()) tree moveall_fn_type;
extern GTY(()) tree reverse_conversion_fn_decls[4];
extern GTY(()) tree reverse_conversion_fn_types[4];
extern GTY(()) tree conversion_fn_decls[4];
extern GTY(()) tree conversion_fn_types[4];
extern GTY(()) tree cmpfill_fn_type;
extern GTY(()) tree cmpfill_fn_decl;
extern GTY(()) tree cmpall_fn_type;
extern GTY(()) tree cmpall_fn_decl;
extern uchar * GTY((length ("label_name_string_length"))) label_name_string;
extern GTY(()) tree bli_display_fn_decl;
extern GTY(()) tree bli_display_fn_type;
extern GTY(()) tree bli_stop_fn_decl;
extern GTY(()) tree bli_stop_fn_type;
extern GTY(()) tree expon_fndecls[16];
extern GTY(()) tree expon_fntypes[16];  
extern GTY(()) struct bli_token_struct *first_token;
extern GTY(()) struct message_occurrence *message_accumulation = NULL;
extern uint32 * GTY((length ("fixed_id_length"))) work_array = NULL;

extern uchar * GTY(()) blicast_temp_string;
extern struct bli_message_info ** GTY((length ("max_message_id_nbr + 1"))) message_info_ptr;
extern uint32 * GTY((length ("fixed_id_length"))) work_array;
extern struct bli_token_struct * GTY(()) first_token;
extern uchar * GTY((length ("bliu)fbuffer_allocated"))) bliu_fbuffer = NULL;

/* 2. Not to be GCed but used in GC.  */
extern uint32 nonunique_name_length;
extern uint32 work_string_length;
extern uint32 fixed_id_length;
extern uint32 label_name_string_length;
extern uint32 bliu_fbuffer_allocated

