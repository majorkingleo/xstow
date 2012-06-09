/*
 * $Log: local_config.h,v $
 * Revision 1.2  2005/07/04 21:59:42  martin
 * added logging to all files
 *
 */
#ifndef LOCAL_CONFIG_H
#define LOCAL_CONFIG_H

#include "config.h"

#ifdef STATIC_COMPILE
#undef HAVE_CURSES_H
#endif

#ifdef GNU_STOW

#undef HAVE_CURSES_H
#undef CAN_USE_INI
#undef HAVE_FNMATCH_H
#undef HAVE_REGEX_H

#endif

#ifdef CAN_USE_INI 
#ifdef   HAVE_FNMATCH_H
#define    CAN_USE_NIGNORE
#endif
#endif

#endif
