#ifndef __GANSIDECL_H__
#define __GANSIDECL_H__

#include "ansidecl.h"

#if (GCC_VERSION >= 2007)
# ifdef __STDC__
#  undef const
# endif
# undef inline
# define inline __inline__  /* Modern gcc can use `__inline__' freely. */
# ifndef HAVE_LONG_DOUBLE
#  define HAVE_LONG_DOUBLE 1
# endif

# if !defined (HAVE_STRINGIZE) && __STDC__
#  define HAVE_STRINGIZE 1
# endif /* ! HAVE_STRINGIZE && __STDC__ */
#endif /* GCC >= 2.7 */

#ifndef NULL_PTR
#define NULL_PTR ((PTR) 0)
#endif

#endif /* __GANSIDECL_H__ */

