/*

   BLISS Compiler code automatic garbage collection interface (blicgty.c)

   ---------------------------------------------------------------------

   Copyright (C) 2002 Free Software Foundation, Inc.

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
   
 */

/* GCC headers.  */

#include "ansidecl.h"
#include "config.h"
#include "system.h"
#include "ansidecl.h"
#include "tree.h"
#include "flags.h"
#include "output.h"
#include <stdio.h>
#include "c-tree.h"
#include "rtl.h"
#include "tm_p.h"
#include "ggc.h"
#include "toplev.h"
#include "target.h"
#include "expr.h"
#include "convert.h"
#include "langhooks-def.h"
#include "langhooks.h"

/* BLI headers.  */

#include "blir_temp_config.h"

#include "blidebug.h"
#include "blicsymn.h"
#include "bliumsc.h"
#include "bliumem.h"
#include "bliumsg.h"
#include "bliclex.h"
#include "blicc1.h"
#include "blicsyt.h"
#include "blicpru.h"
#include "blir_globals.h"

/* New garbage collection regime see gty.texi.  */

#include "gtype-bliss.h"
