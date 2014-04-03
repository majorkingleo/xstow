AC_INIT(xstow, 1.0.1)
AC_CONFIG_SRCDIR([src/main.cpp])
AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE

AC_SEARCH_LIBS([strerror],[cposix])
AC_PROG_CXX
AC_LANG(C++)
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AM_PROG_CC_STDC
AC_HEADER_STDC
AC_C_CONST
AC_C_INLINE

AC_CHECK_HEADER(string, 
                echo -n,  
                [AC_MSG_ERROR([this header file is required])])

AC_CHECK_HEADER([vector],      , [AC_MSG_ERROR([Sorry we cannot do anything for you])])
AC_CHECK_HEADER([list],        , [AC_MSG_ERROR([Sorry we can't do anything for you])])
AC_CHECK_HEADER([dirent.h],    , [AC_MSG_ERROR([Sorry we can't do anything for you])])
AC_CHECK_HEADER([iostream],    , [AC_MSG_ERROR([Sorry we can't do anything for you])])
AC_CHECK_HEADER([iomanip],     , [AC_MSG_ERROR([Sorry we can't do anything for you])])
AC_CHECK_HEADER([iterator],    , [AC_MSG_ERROR([Sorry we can't do anything for you])])
AC_CHECK_HEADER([fstream],     , [AC_MSG_ERROR([Sorry we can't do anything for you])])
AC_CHECK_HEADER([exception],   , [AC_MSG_ERROR([Sorry we can't do anything for you])])
AC_CHECK_HEADER([sys/stat.h],  , [AC_MSG_ERROR([Sorry we can't do anything for you])])
AC_CHECK_HEADER([unistd.h],    , [AC_MSG_ERROR([Sorry we can't do anything for you])])
AC_CHECK_HEADER([errno.h],     , [AC_MSG_ERROR([Sorry we can't do anything for you])])
AC_CHECK_HEADER([ctype.h],     , [AC_MSG_ERROR([Sorry we can't do anything for you])])
AC_CHECK_HEADER([cstdio],      , [AC_MSG_ERROR([Sorry we can't do anything for you])])
AC_CHECK_HEADER([cstring],     , [AC_MSG_ERROR([Sorry we can't do anything for you])])
AC_CHECK_HEADER([string],      , [AC_MSG_ERROR([Sorry we can't do anything for you])])
AC_CHECK_HEADER([sys/types.h], , [AC_MSG_ERROR([Sorry we can't do anything for you])])
AC_CHECK_HEADER([sys/stat.h],  , [AC_MSG_ERROR([Sorry we can't do anything for you])])
AC_CHECK_HEADER([algorithm],   , [AC_MSG_ERROR([Sorry we can't do anything for you])])

AC_MSG_CHECKING(for shell pattern support)
AC_ARG_WITH(fnmatch,
	[  --with-fnmatch      Enable shell pattern matching],
	[
		if test "x$withval" = "xno" ; then
			AC_MSG_RESULT(disabled)
			support_fnmatch=0
		else
			support_fnmatch=1
		fi
	],
	[
		
		AC_MSG_RESULT(enabled)
		AC_CHECK_HEADER([fnmatch.h],   
				[
				  AC_DEFINE(HAVE_FNMATCH_H, 1, [fnmatch is present])
				  support_fnmatch=1
				], 
				[ 
				  support_fnmatch=0
				])
	])
		   
AC_MSG_CHECKING(for regular expressions support)
AC_ARG_WITH(regex,
	[  --with-regex        Enable regular expressions support],
	[
		if test "x$withval" = "xno" ; then
			AC_MSG_RESULT(disabled)
			support_regex=0
		else
			support_regex=1
		fi
	],
	[
			AC_MSG_RESULT(enabled)
			AC_CHECK_HEADER([regex.h],
					[
					  AC_DEFINE(HAVE_REGEX_H, 1, [POSIX regex is present])
					  support_regex=1	
					], 
					[ 
					  support_regex=0 
					])
	])

AC_MSG_CHECKING(for curses support)
AC_ARG_WITH(curses,
	[  --with-curses       Enable using curses library],
	[
		if test "x$withval" = "xno" ; then
			AC_MSG_RESULT(disabled)
			support_curses=0
		else
			support_curses=1
		fi
	],
	[
		AC_MSG_RESULT(enabled)
		AC_CHECK_HEADER([curses.h],
				[
				  # test which libraray to use
				  AC_CHECK_LIB([curses],[initscr],
						[
						  support_curses=1
						  curseslib="curses"						  
						],
						[ support_curses=0 ])
			

				  if test $support_curses -eq 0; then

				     AC_CHECK_LIB([ncurses],[initscr],
						[
						  support_curses=1
						  curseslib="ncurses"
						],
						[ support_curses=0 ])			
				  fi	

				  if test $support_curses -eq 1; then

				     AC_MSG_CHECKING([for ominous macros in curses])
				     AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
					#include <curses.h>
					#include <list>
					struct Foo { Foo() { std::list<int> il; il.erase(il.begin()); } };
					]], [[ Foo foo ]])],[
						AC_MSG_RESULT(no)
						support_curses=1
					],[
						AC_MSG_RESULT(yes => curses support disabled)
						support_curses=0
					]) 

				  fi

				  if test $support_curses -eq 1; then

				     AC_CHECK_LIB([tinfo],[COLS],
						[
						  need_tinfo=1
						  tinfolib="-ltinfo"
						],
						[ need_tinfo=0 ])

				  fi

				  if test $support_curses -eq 1; then
				  	AC_SUBST(LDFLAGS_DYNAMIC, "$LDFLAGS -l$curseslib $tinfolib")
				        AC_DEFINE(HAVE_CURSES_H, 1, [curses library can be used])
				  fi

				],
				[
				  support_curses=0
				])
	])


NO_STL_SSTREAM=0
NO_STL_STRSTREAM=0

AC_CHECK_HEADER([sstream],     [AC_DEFINE(HAVE_STL_SSTREAM, 1, [sstream is present])] , [ NO_STL_SSTREAM=1 ])
AC_CHECK_HEADER([strstream],   [AC_DEFINE(HAVE_STL_STRSTREAM, 1, [old strstream can be used])], [ NO_STL_STRSTREAM=1 ])

if test $NO_STL_SSTREAM -gt 0 ; then
  if test $NO_STL_STRSTREAM -gt 0 ; then

     AC_MSG_ERROR([sstream, or strstream header is required])

  fi
fi

AC_MSG_CHECKING(for ini file support)
AC_ARG_ENABLE(ini,
	[  --disable-ini              Enable ini file support],
	[
	   if test "x$enableval" = "xno" ; then
		AC_MSG_RESULT(disabled)
		no_ini=1
		support_ini=0
	   fi
        ],
	[ 
          no_ini=0 
	  AC_MSG_RESULT(enabled)
        ] )

AC_MSG_CHECKING([for GNU like STL containers])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[ 
#include <list> 
struct Foo { typedef std::list<Foo>::iterator it; }; 
]], [[ Foo foo ]])],[
			AC_MSG_RESULT(yes)

			if test $no_ini -eq 0; then
			   AC_DEFINE(CAN_USE_INI, 1, [the ini class should compile clean])
			   support_ini=1
			fi
		],[
			AC_MSG_RESULT(no)
			AC_MSG_WARN([cannot compile with configuration file support])
			support_ini=0
		
	])

AC_MSG_CHECKING([for debugging support])
AC_ARG_ENABLE(debug,
	[  --enable-debug             Enable extra debugging code],
	[ 
	   if test "x$enableval" = "xno" ; then
	      AC_MSG_RESULT(disabled)
	      support_debug=0
              CXXFLAGS="$CXXFLAGS -DNDEBUG"
           else
	      AC_MSG_RESULT(enabled)
	      support_debug=1
	   fi   
	],
	[
           CXXFLAGS="$CXXFLAGS -DNDEBUG"
	   AC_MSG_RESULT(disabled)
	   support_debug=0
	] )

AC_MSG_CHECKING([for new std::ios_base])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <fstream>
]], [[ std::ifstream in( "foo", std::ios_base::in ); ]])],[
		AC_MSG_RESULT(yes)
		AC_DEFINE(HAVE_IOS_BASE, 1, [ios_base can be used])
	],[
		AC_MSG_RESULT(no)
		AC_DEFINE(ios_base, ios, [use old ios instead of ios_base])
	
])

CXXFLAGS="$CXXFLAGS -DSYSCONFDIR='\"\$(sysconfdir)\"'"

AC_MSG_CHECKING([for static support])
AC_ARG_ENABLE(static,
	[  --disable-static           Disable xstow-static and xstow-stow targets],
	[ 
	   if test "x$enableval" = "xno" ; then
		AC_MSG_RESULT(disabled)
		support_static=0
	   else
	     AC_MSG_RESULT(enabled)
	     support_static=1
           fi
	],
	[
	   AC_MSG_RESULT(enabled)
	   support_static=0
	] )

AC_MSG_CHECKING([create merge-info?])
AC_ARG_ENABLE(merge-info,
	[  --disable-merge-info       Disable creating of the merge-info tool],
	[
	   if test "x$enableval" = "xno" ; then
	      AC_MSG_RESULT(disabled)
	      support_merge=0
	   else
	      AC_MSG_RESULT(enable)
	      support_merge=1
	   fi
	 ],
	 [
	   AC_MSG_RESULT(enabled)
	   support_merge=1
	 ])


AC_CONFIG_FILES([
Makefile
src/Makefile
doc/Makefile])

# report features:

if test $support_ini -gt 0 ; then
  AM_CONDITIONAL(ENABLE_INI, true )
  support_ini=yes
else
  AM_CONDITIONAL(ENABLE_INI, false )
  support_ini=no
fi

if test $support_debug -gt 0 ; then
  support_debug=yes
else
  support_debug=no
fi


if test $support_fnmatch -gt 0 ; then
  AM_CONDITIONAL(ENABLE_FNMATCH, true )
  support_fnmatch=yes
else
  AM_CONDITIONAL(ENABLE_FNMATCH, false )
  support_fnmatch=no
fi

if test $support_regex -gt 0 ; then
  AM_CONDITIONAL(ENABLE_REGEX, true )
  support_regex=yes
else
  AM_CONDITIONAL(ENABLE_REGEX, false )
  support_regex=no
fi

if test $support_curses -gt 0 ; then
  AM_CONDITIONAL(ENABLE_CURSES, true )
  support_curses=yes
else
  AM_CONDITIONAL(ENABLE_CURSES, false )
  support_curses=no
fi

if test $support_static -gt 0 ; then
  AM_CONDITIONAL(ENABLE_STATIC, true )
  support_static=yes
else
  AM_CONDITIONAL(ENABLE_STATIC, false )
  support_static=no
fi

if test $support_merge -gt 0 ; then
  AM_CONDITIONAL(ENABLE_MERGE, true )
  support_merge=yes
else
  AM_CONDITIONAL(ENABLE_MERGE, false )
  support_merge=no
fi

AC_OUTPUT

echo
echo

echo "supported features:"


echo "    debugging:                              $support_debug"
echo "    configuration files:                    $support_ini"
echo "    shell pattern matching:                 $support_fnmatch"
echo "    regex pattern matching:                 $support_regex"
echo "    using curses library:                   $support_curses"
echo "    create static versions:                 $support_static"
echo "    create merge-info tool:                 $support_merge"

echo
echo

echo "make file targets:"
echo "    make all"
echo "    make mini"
echo "    make docu"

echo
echo

echo "If you wanna have a documentation specific to the options you"
echo "enabled or disabled, run 'make docu' before installing xstow."
echo "By default a pregenerated docu with all options turned on is used."
echo "Requirements: pod2man, pod2html (shipped with perl) and awk."

echo
echo 

