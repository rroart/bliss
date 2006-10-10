#ifndef bliss_h
#define bliss_h

#include <tree.h>

#define DSC$K_DTYPE_T 14
#define DSC$K_CLASS_S 1

struct dsc$descriptor {
  unsigned short        dsc$w_length;   
  unsigned char dsc$b_dtype;    
  unsigned char dsc$b_class;    
  void          *dsc$a_pointer; 
};
// has not got descrip.h (yet)

struct mymacro {
  struct mymacro * next;
  char * name;
  int type;
  tree param;
  tree param2;
  tree body;
};

tree find_field(char * s);
tree find_linkage_attr(tree);
#if 0
tree parser_build_binary_op (enum tree_code, tree, tree);
#endif
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
void my_fold_p (tree *);
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
int my_strcat_gen(struct dsc$descriptor * dsc, struct dsc$descriptor *dsc1, struct dsc$descriptor * dsc2, int space);
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
struct c_parm * build_parm_decl(tree,tree);
tree bli_start_decl(tree, tree, bool, tree);
tree bli_start_decl_2(tree, tree, bool, tree, int);
struct c_declspecs * build_specs(tree);
int bli_start_function(tree, tree, struct c_arg_info *, tree);
extern tree binding_decl(struct c_binding *);
tree bli_make_pointer_declarator(tree, tree);
struct c_type_name * build_type_name(tree);
tree first_to_last(tree);
tree my_lookup_tag(tree);
tree get_label_binding(tree);
extern tree create_tmp_var (tree, const char *);

#endif
