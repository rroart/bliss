#define DEFTREECODE(SYM, NAME, TYPE, LENGTH) SYM,
enum bliss_tree_code {
  __DUMMY = LAST_AND_UNUSED_TREE_CODE,
#include "bliss-tree.def"
  LAST_BLISS_TREE_CODE
};
#undef DEFTREECODE
