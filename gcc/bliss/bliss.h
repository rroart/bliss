#ifndef bliss_h
#define bliss_h

#include <tree.h>

tree find_field(char * s);
tree find_linkage_attr(tree);
tree parser_build_binary_op (enum tree_code, tree, tree);
tree get_value(tree);
tree give_char_array_type_node();
tree start_structure (enum tree_code, tree);
tree finish_structure (tree, tree, tree, tree, tree, tree, tree);
tree parse_this_range (char * this, char * end);
tree parse_this_n (char * this, int n);
tree parse_this (char * this);
tree mylast();
tree build_external(id, attr);

tree my_copy_tree (tree);
void my_substitute (tree, tree, tree);
void my_fold (tree);
tree parm_first_to_last (tree);
void add_macro (char *,int,tree,tree,tree);
char * add_counted_string (tree, int);
char * add_underscore (tree, int);
void * find_macro(struct mymacro * s,char * name);
int make_macro_string(struct dsc$descriptor * dsc, struct mymacro * m, tree r);
tree find_init_attr(tree t);
tree find_structure_attr(tree);
tree find_alloc_attr(tree);
tree find_extension_attr(tree);
tree find_linkage_attr(tree);
tree find_novalue_attr(tree);
tree find_volatile_attr(tree);
tree find_external_name_attr(tree);
tree find_alignment_attr(tree);
tree get_alignment(tree);
void set_external_name(tree,tree);
int is_counted(tree);
int unsigned_attr(tree);
tree sign_convert(tree,tree);
tree conv_unsign(tree);
tree find_tree_code(tree, int);
int my_strcat(struct dsc$descriptor *, int, const char *, int, const char *, int);
int my_strcat_gen(struct dsc$descriptor *, struct dsc$descriptor *, struct dsc$descriptor * , int);
tree handle_preset(tree, tree, tree, tree);
tree handle_initial(tree, tree, tree, tree, int);
tree convert_field_ref_to_decl(tree,tree);
tree handle_structure(tree, tree, int);
tree handle_structure_attribute(tree, tree, int);
tree create_temp_var();
tree set_temp_var(tree, tree);
tree build_our_record(tree);
int print_remain(struct dsc$descriptor *, tree);
tree mytag(tree);
int is_str_tag(tree);
tree strip_literal(tree);
tree find_linkage(char * name);
void add_linkage (char *,tree);
tree convert_string_literal_to_integer(tree);
tree is_symbol(tree);

#endif
