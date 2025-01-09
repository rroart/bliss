#include "../c/c-decl.cc"

// BLISS adds

void
bli_bind (tree name, tree decl, struct c_scope *scope, bool invisible,
      bool nested, location_t locus)
{
  struct c_binding *b, **here;

  if (binding_freelist)
    {
      b = binding_freelist;
      binding_freelist = b->prev;
    }
  else
    b = ggc_alloc<c_binding> ();

  b->shadowed = 0;
  b->decl = decl;
  b->id = name;
  b->depth = scope->depth;
  b->invisible = invisible;
  b->nested = nested;
  b->inner_comp = 0;
  b->in_struct = 0;
  b->locus = locus;

  b->u.type = NULL;

  b->prev = scope->bindings;
  scope->bindings = b;

  if (decl_jump_unsafe (decl))
    scope->has_jump_unsafe_decl = 1;

  if (!name)
    return;

  switch (TREE_CODE (decl))
    {
      // two bliss
    case LINKAGE:
    case MACRO_DEF:
    case LABEL_DECL:     here = &I_LABEL_BINDING (name);   break;
      // two bliss
    case STRUCTURE_TYPE:
    case STRUCTURE_ATTR:
    case ENUMERAL_TYPE:
    case UNION_TYPE:
    case RECORD_TYPE:    here = &I_TAG_BINDING (name);     break;
    case VAR_DECL:
    case FUNCTION_DECL:
    case TYPE_DECL:
    case CONST_DECL:
    case PARM_DECL:
    case ERROR_MARK:     here = &I_SYMBOL_BINDING (name);  break;

    default:
      gcc_unreachable ();
    }

  /* Locate the appropriate place in the chain of shadowed decls
     to insert this binding.  Normally, scope == current_scope and
     this does nothing.  */
  while (*here && (*here)->depth > scope->depth)
    here = &(*here)->shadowed;

  b->shadowed = *here;
  *here = b;
}

tree
bli_pop_scope (void)
{
  struct c_scope *scope = current_scope;
  tree block, context, p;
  struct c_binding *b;

  bool functionbody = scope->function_body;
  bool keep = functionbody || scope->keep || scope->bindings;

  update_label_decls (scope);

  /* If appropriate, create a BLOCK to record the decls for the life
     of this function.  */
  block = NULL_TREE;
  if (keep)
    {
      block = make_node (BLOCK);
      BLOCK_SUBBLOCKS (block) = scope->blocks;
      TREE_USED (block) = 1;

      /* In each subblock, record that this is its superior.  */
      for (p = scope->blocks; p; p = BLOCK_CHAIN (p))
	BLOCK_SUPERCONTEXT (p) = block;

      BLOCK_VARS (block) = NULL_TREE;
    }

  /* The TYPE_CONTEXTs for all of the tagged types belonging to this
     scope must be set so that they point to the appropriate
     construct, i.e.  either to the current FUNCTION_DECL node, or
     else to the BLOCK node we just constructed.

     Note that for tagged types whose scope is just the formal
     parameter list for some function type specification, we can't
     properly set their TYPE_CONTEXTs here, because we don't have a
     pointer to the appropriate FUNCTION_TYPE node readily available
     to us.  For those cases, the TYPE_CONTEXTs of the relevant tagged
     type nodes get set in `grokdeclarator' as soon as we have created
     the FUNCTION_TYPE node which will represent the "scope" for these
     "parameter list local" tagged types.  */
  if (scope->function_body)
    context = current_function_decl;
  else if (scope == file_scope)
    {
      tree file_decl
	= build_translation_unit_decl (get_identifier (main_input_filename));
      context = file_decl;
      debug_hooks->register_main_translation_unit (file_decl);
    }
  else
    context = block;

  /* Clear all bindings in this scope.  */
  for (b = scope->bindings; b; b = free_binding_and_advance (b))
    {
      p = b->decl;
      switch (TREE_CODE (p))
	{
	case LABEL_DECL:
	  /* Warnings for unused labels, errors for undefined labels.  */
	  if (TREE_USED (p) && !DECL_INITIAL (p))
	    {
	      error ("label %q+D used but not defined", p);
	      DECL_INITIAL (p) = error_mark_node;
	    }
	  else
	    warn_for_unused_label (p);

	  /* Labels go in BLOCK_VARS.  */
	  DECL_CHAIN (p) = BLOCK_VARS (block);
	  BLOCK_VARS (block) = p;
	  gcc_assert (I_LABEL_BINDING (b->id) == b);
	  I_LABEL_BINDING (b->id) = b->shadowed;

	  /* Also pop back to the shadowed label_vars.  */
	  release_tree_vector (b->u.label->decls_in_scope);
	  b->u.label = b->u.label->shadowed;
	  break;

	  // bliss
	case STRUCTURE_ATTR:
	case ENUMERAL_TYPE:
	case UNION_TYPE:
	case RECORD_TYPE:

	  /* Types may not have tag-names, in which case the type
	     appears in the bindings list with b->id NULL.  */
	  if (b->id)
	    {
	      gcc_assert (I_TAG_BINDING (b->id) == b);
	      I_TAG_BINDING (b->id) = b->shadowed;
	    }
	  break;

	case FUNCTION_DECL:
	  /* Propagate TREE_ADDRESSABLE from nested functions to their
	     containing functions.  */
	  if (!TREE_ASM_WRITTEN (p)
	      && DECL_INITIAL (p) != NULL_TREE
	      && TREE_ADDRESSABLE (p)
	      && DECL_ABSTRACT_ORIGIN (p) != NULL_TREE
	      && DECL_ABSTRACT_ORIGIN (p) != p)
	    TREE_ADDRESSABLE (DECL_ABSTRACT_ORIGIN (p)) = 1;
	  if (!TREE_PUBLIC (p)
	      && !DECL_INITIAL (p)
	      && !b->nested
	      && scope != file_scope
	      && scope != external_scope)
	    {
	      error ("nested function %q+D declared but never defined", p);
	      undef_nested_function = true;
	    }
	  else if (DECL_DECLARED_INLINE_P (p)
		   && TREE_PUBLIC (p)
		   && !DECL_INITIAL (p))
	    {
	      /* C99 6.7.4p6: "a function with external linkage... declared
		 with an inline function specifier ... shall also be defined
		 in the same translation unit."  */
	      if (!flag_gnu89_inline
		  && !lookup_attribute ("gnu_inline", DECL_ATTRIBUTES (p))
		  && scope == external_scope)
		pedwarn (input_location, 0,
			 "inline function %q+D declared but never defined", p);
	      DECL_EXTERNAL (p) = 1;
	    }

	  goto common_symbol;

	case VAR_DECL:
	  /* Warnings for unused variables.  */
	  if ((!TREE_USED (p) || !DECL_READ_P (p))
	      && !warning_suppressed_p (p, OPT_Wunused_but_set_variable)
	      && !DECL_IN_SYSTEM_HEADER (p)
	      && DECL_NAME (p)
	      && !DECL_ARTIFICIAL (p)
	      && scope != file_scope
	      && scope != external_scope)
	    {
	      if (!TREE_USED (p))
		{
		  warning (OPT_Wunused_variable, "unused variable %q+D", p);
		  suppress_warning (p, OPT_Wunused_variable);
		}
	      else if (DECL_CONTEXT (p) == current_function_decl)
		warning_at (DECL_SOURCE_LOCATION (p),
			    OPT_Wunused_but_set_variable,
			    "variable %qD set but not used", p);
	    }

	  if (b->inner_comp)
	    {
	      error ("type of array %q+D completed incompatibly with"
		     " implicit initialization", p);
	    }

	  /* Fall through.  */
	case TYPE_DECL:
	case CONST_DECL:
	common_symbol:
	  /* All of these go in BLOCK_VARS, but only if this is the
	     binding in the home scope.  */
	  if (!b->nested)
	    {
	      DECL_CHAIN (p) = BLOCK_VARS (block);
	      BLOCK_VARS (block) = p;
	    }
	  else if (VAR_OR_FUNCTION_DECL_P (p) && scope != file_scope)
	    {
	      /* For block local externs add a special
		 DECL_EXTERNAL decl for debug info generation.  */
	      tree extp = copy_node (p);

	      DECL_EXTERNAL (extp) = 1;
	      TREE_STATIC (extp) = 0;
	      TREE_PUBLIC (extp) = 1;
	      DECL_INITIAL (extp) = NULL_TREE;
	      DECL_LANG_SPECIFIC (extp) = NULL;
	      DECL_CONTEXT (extp) = current_function_decl;
	      if (TREE_CODE (p) == FUNCTION_DECL)
		{
		  DECL_RESULT (extp) = NULL_TREE;
		  DECL_SAVED_TREE (extp) = NULL_TREE;
		  DECL_STRUCT_FUNCTION (extp) = NULL;
		}
	      if (b->locus != UNKNOWN_LOCATION)
		DECL_SOURCE_LOCATION (extp) = b->locus;
	      DECL_CHAIN (extp) = BLOCK_VARS (block);
	      BLOCK_VARS (block) = extp;
	    }
	  /* If this is the file scope set DECL_CONTEXT of each decl to
	     the TRANSLATION_UNIT_DECL.  This makes same_translation_unit_p
	     work.  */
	  if (scope == file_scope)
	      DECL_CONTEXT (p) = context;

	  gcc_fallthrough ();
	  /* Parameters go in DECL_ARGUMENTS, not BLOCK_VARS, and have
	     already been put there by store_parm_decls.  Unused-
	     parameter warnings are handled by function.cc.
	     error_mark_node obviously does not go in BLOCK_VARS and
	     does not get unused-variable warnings.  */
	case PARM_DECL:
	case ERROR_MARK:
	  /* It is possible for a decl not to have a name.  We get
	     here with b->id NULL in this case.  */
	  if (b->id)
	    {
	      gcc_assert (I_SYMBOL_BINDING (b->id) == b);
	      I_SYMBOL_BINDING (b->id) = b->shadowed;
	      if (b->shadowed && b->shadowed->u.type)
		TREE_TYPE (b->shadowed->decl) = b->shadowed->u.type;
	    }
	  break;

	  // bliss
	  //    case STRUCTURE_ATTR:
	case STRUCTURE_TYPE:
        case MACRO_DEF:
        case LINKAGE:
          break;

	default:
	  gcc_unreachable ();
	}
    }


  /* Dispose of the block that we just made inside some higher level.  */
  if ((scope->function_body || scope == file_scope) && context)
    {
      DECL_INITIAL (context) = block;
      BLOCK_SUPERCONTEXT (block) = context;
    }
  else if (scope->outer)
    {
      if (block)
	SCOPE_LIST_APPEND (scope->outer, blocks, block);
      /* If we did not make a block for the scope just exited, any
	 blocks made for inner scopes must be carried forward so they
	 will later become subblocks of something else.  */
      else if (scope->blocks)
	SCOPE_LIST_CONCAT (scope->outer, blocks, scope, blocks);
    }

  /* Pop the current scope, and free the structure for reuse.  */
  current_scope = scope->outer;
  if (scope->function_body)
    current_function_scope = scope->outer_function;

  memset (scope, 0, sizeof (struct c_scope));
  scope->outer = scope_freelist;
  scope_freelist = scope;

  return block;
}

void
bli_pushtag (location_t loc, tree name, tree type)
{
  /* Record the identifier as the type's name if it has none.  */
  if (name && !TYPE_NAME (type))
    TYPE_NAME (type) = name;
  bli_bind (name, type, current_scope, /*invisible=*/false, /*nested=*/false, loc);

  /* Create a fake NULL-named TYPE_DECL node whose TREE_TYPE will be the
     tagged type we just added to the current scope.  This fake
     NULL-named TYPE_DECL node helps dwarfout.c to know when it needs
     to output a representation of a tagged type, and it also gives
     us a convenient place to record the "scope start" address for the
     tagged type.  */

  TYPE_STUB_DECL (type) = pushdecl (build_decl (loc,
						TYPE_DECL, NULL_TREE, type));

  /* An approximation for now, so we can tell this is a function-scope tag.
     This will be updated in pop_scope.  */
  TYPE_CONTEXT (type) = DECL_CONTEXT (TYPE_STUB_DECL (type));

  if (warn_cxx_compat && name != NULL_TREE)
    {
      struct c_binding *b = I_SYMBOL_BINDING (name);

      if (b != NULL
	  && b->decl != NULL_TREE
	  && TREE_CODE (b->decl) == TYPE_DECL
	  && (B_IN_CURRENT_SCOPE (b)
	      || (current_scope == file_scope && B_IN_EXTERNAL_SCOPE (b)))
	  && (TYPE_MAIN_VARIANT (TREE_TYPE (b->decl))
	      != TYPE_MAIN_VARIANT (type)))
	{
	  auto_diagnostic_group d;
	  if (warning_at (loc, OPT_Wc___compat,
			  ("using %qD as both a typedef and a tag is "
			   "invalid in C++"), b->decl)
	      && b->locus != UNKNOWN_LOCATION)
	    inform (b->locus, "originally defined here");
	}
    }
}

struct c_arg_info *
bli_get_parm_info (bool ellipsis, tree expr)
{
  struct c_binding *b = current_scope->bindings;
  struct c_arg_info *arg_info = build_arg_info ();

  tree parms = NULL_TREE;
  vec<c_arg_tag, va_gc> *tags = NULL;
  tree types = NULL_TREE;
  tree others = NULL_TREE;

  bool gave_void_only_once_err = false;

  arg_info->had_vla_unspec = current_scope->had_vla_unspec;

  /* The bindings in this scope must not get put into a block.
     We will take care of deleting the binding nodes.  */
  current_scope->bindings = 0;

  /* This function is only called if there was *something* on the
     parameter list.  */
  gcc_assert (b);

  /* A parameter list consisting solely of 'void' indicates that the
     function takes no arguments.  But if the 'void' is qualified
     (by 'const' or 'volatile'), or has a storage class specifier
     ('register'), then the behavior is undefined; issue an error.
     Typedefs for 'void' are OK (see DR#157).  */
  if (b->prev == 0			    /* one binding */
      && TREE_CODE (b->decl) == PARM_DECL   /* which is a parameter */
      && !DECL_NAME (b->decl)               /* anonymous */
      && VOID_TYPE_P (TREE_TYPE (b->decl))) /* of void type */
    {
      if (TYPE_QUALS (TREE_TYPE (b->decl)) != TYPE_UNQUALIFIED
	  || C_DECL_REGISTER (b->decl))
	error_at (b->locus, "%<void%> as only parameter may not be qualified");

      /* There cannot be an ellipsis.  */
      if (ellipsis)
	error_at (b->locus, "%<void%> must be the only parameter");

      arg_info->types = void_list_node;
      return arg_info;
    }

  if (!ellipsis)
    types = void_list_node;

  /* Break up the bindings list into parms, tags, types, and others;
     apply sanity checks; purge the name-to-decl bindings.  */
  while (b)
    {
      tree decl = b->decl;
      tree type = TREE_TYPE (decl);
      c_arg_tag tag;
      const char *keyword;

      switch (TREE_CODE (decl))
	{
	case PARM_DECL:
	  if (b->id)
	    {
	      gcc_assert (I_SYMBOL_BINDING (b->id) == b);
	      I_SYMBOL_BINDING (b->id) = b->shadowed;
	    }

	  /* Check for forward decls that never got their actual decl.  */
	  if (TREE_ASM_WRITTEN (decl))
	    error_at (b->locus,
		      "parameter %q+D has just a forward declaration", decl);
	  /* Check for (..., void, ...) and issue an error.  */
	  else if (VOID_TYPE_P (type) && !DECL_NAME (decl))
	    {
	      if (!gave_void_only_once_err)
		{
		  error_at (b->locus, "%<void%> must be the only parameter");
		  gave_void_only_once_err = true;
		}
	    }
	  else
	    {
	      /* Valid parameter, add it to the list.  */
	      DECL_CHAIN (decl) = parms;
	      parms = decl;

	      /* Since there is a prototype, args are passed in their
		 declared types.  The back end may override this later.  */
	      DECL_ARG_TYPE (decl) = type;
	      types = tree_cons (0, type, types);
	    }
	  break;

	case ENUMERAL_TYPE: keyword = "enum"; goto tag;
	case UNION_TYPE:    keyword = "union"; goto tag;
	case RECORD_TYPE:   keyword = "struct"; goto tag;
	tag:
	  /* Types may not have tag-names, in which case the type
	     appears in the bindings list with b->id NULL.  */
	  if (b->id)
	    {
	      gcc_assert (I_TAG_BINDING (b->id) == b);
	      I_TAG_BINDING (b->id) = b->shadowed;
	    }

	  /* Warn about any struct, union or enum tags defined in a
	     parameter list.  The scope of such types is limited to
	     the parameter list, which is rarely if ever desirable
	     (it's impossible to call such a function with type-
	     correct arguments).  An anonymous union parm type is
	     meaningful as a GNU extension, so don't warn for that.  */
	  if (TREE_CODE (decl) != UNION_TYPE || b->id != NULL_TREE)
	    {
	      if (b->id)
		{
		  /* The %s will be one of 'struct', 'union', or 'enum'.  */
		  if (!flag_isoc23)
		    warning_at (b->locus, 0,
				"%<%s %E%> declared inside parameter list"
				" will not be visible outside of this definition or"
				" declaration", keyword, b->id);
		}
	      else
		/* The %s will be one of 'struct', 'union', or 'enum'.  */
		warning_at (b->locus, 0,
			    "anonymous %s declared inside parameter list"
			    " will not be visible outside of this definition or"
			    " declaration", keyword);
	    }

	  tag.id = b->id;
	  tag.type = decl;
	  vec_safe_push (tags, tag);
	  break;

	case FUNCTION_DECL:
	  /* FUNCTION_DECLs appear when there is an implicit function
	     declaration in the parameter list.  */
	  gcc_assert (b->nested || seen_error ());
	  goto set_shadowed;

	case CONST_DECL:
	case TYPE_DECL:
	  /* CONST_DECLs appear here when we have an embedded enum,
	     and TYPE_DECLs appear here when we have an embedded struct
	     or union.  No warnings for this - we already warned about the
	     type itself.  */

	  /* When we reinsert this decl in the function body, we need
	     to reconstruct whether it was marked as nested.  */
	  gcc_assert (!b->nested);
	  DECL_CHAIN (decl) = others;
	  others = decl;
	  /* fall through */

	case ERROR_MARK:
	set_shadowed:
	  /* error_mark_node appears here when we have an undeclared
	     variable.  Just throw it away.  */
	  if (b->id)
	    {
	      gcc_assert (I_SYMBOL_BINDING (b->id) == b);
	      I_SYMBOL_BINDING (b->id) = b->shadowed;
	    }
	  break;

	  // bliss
	case MACRO_DEF:
	  break; // TODO check

	  /* Other things that might be encountered.  */
	case LABEL_DECL:
	case VAR_DECL:
	default:
	  gcc_unreachable ();
	}

      b = free_binding_and_advance (b);
    }

  arg_info->parms = parms;
  arg_info->tags = tags;
  arg_info->types = types;
  arg_info->others = others;
  arg_info->pending_sizes = expr;
  arg_info->no_named_args_stdarg_p = ellipsis && !types;
  return arg_info;
}

tree
start_structure (
		 enum tree_code code,
		 tree name)
{
  /* If there is already a tag defined at this binding level
     (as a forward reference), just return it.  */

  tree ref = 0;

  if (name != 0)
    ref = 0; // TODO lookup_tag (code, name, 1, &input_location);
  if (ref && TREE_CODE (ref) == code)
    {
      C_TYPE_BEING_DEFINED (ref) = 1;
      TYPE_PACKED (ref) = flag_pack_struct;
      if (TYPE_FIELDS (ref))
        {
	  if (code == STRUCTURE_TYPE)
	    error ("redefinition of `union %s'",
		   IDENTIFIER_POINTER (name));
	}  

      return ref;
    }

  /* Otherwise create a forward-reference just so the tag is in scope.  */

  ref = make_node (code);
  // TODO pushtag (input_location, name, ref);
  C_TYPE_BEING_DEFINED (ref) = 1;
  //  TYPE_PACKED (ref) = flag_pack_struct;
  return ref;
}

tree
finish_structure (
		  tree t,
		  tree fieldlist,
		  tree access_formal,
		  tree allocation_formal,
		  tree structure_size,
		  tree structure_body,
		  tree attributes)
{
  tree x;
  int toplevel = file_scope == current_scope;
  int saw_named_field;

  /* If this type was previously laid out as a forward reference,
     make sure we lay it out again.  */

  TYPE_SIZE (t) = 0;

#if 0
  decl_attributes (&t, attributes, (int) ATTR_FLAG_TYPE_IN_PLACE);
#endif

  /* Install struct as DECL_CONTEXT of each field decl.
     Also process specified field sizes,m which is found in the DECL_INITIAL.
     Store 0 there, except for ": 0" fields (so we can find them
     and delete them, below).  */

  saw_named_field = 0;
  /* Now we have the nearly final fieldlist.  Record it,
     then lay out the structure or union (including the fields).  */

  TYPE_FIELDS (t) = fieldlist;

  /* Now we have the truly final field list.
     Store it in this type and in the variants.  */

  TYPE_FIELDS (t) = fieldlist;

  return t;
}

void
push_parm_decl_init (tree parm, tree init)
{
  tree decl = 0;

  /* Don't attempt to expand sizes while parsing this decl.
     (We can get here with i_s_e 1 somehow from Objective-C.)  */
#if 0
  int save_immediate_size_expand = immediate_size_expand;
  immediate_size_expand = 0;
#endif

  /* TODO
  decl = grokdeclarator ((const struct c_declarator *) TREE_VALUE (TREE_PURPOSE (parm)),
			 (struct c_declspecs *) TREE_PURPOSE (TREE_PURPOSE (parm)),
			 PARM, 0, NULL, NULL, NULL, NULL, DEPRECATED_NORMAL);
  */
#if 0
  // check
  decl_attributes (&decl, TREE_VALUE (parm), 0);
#endif

  decl = pushdecl (decl);

  finish_decl (decl, input_location, NULL_TREE, NULL_TREE, NULL_TREE);
  DECL_INITIAL(decl)=init;
  DECL_RESULT(decl)=init; // borrow this field?
  // TODO DECL_LANG_SPECIFIC(decl)=init; // borrow this field?

#if 0
  immediate_size_expand = save_immediate_size_expand;
#endif
}

tree
my_lookup_tag(tree t)
{
  return 0 ; // TODO lookup_tag(LINKAGE, t, 1, &input_location);
}

void
reset_symbol(tree t)
{
  // TODO I_SYMBOL_BINDING(t) = 0;
}

tree
get_label_binding(tree t)
{
  return 0; // TODOI_LABEL_BINDING(t);
}

void
set_label_binding(tree t)
{
  //I_LABEL_BINDING(t) = 0;
  //c_binding ** b = I_LABEL_BINDING(t);
  // TODO * b = 0;
}


tree
binding_decl(struct c_binding * c) {
  return c->decl;
}
