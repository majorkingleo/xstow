
noinst_LIBRARIES=tools/libtools.a \
	cpputils/cpputilsshared/libcpputilsshared.a \
	cpputils/cpputilsshared/cpputilsformat/libcpputilsformat.a


tools_libtools_a_SOURCES= \
	tools/arg.cpp \
	tools/database.cpp \
	tools/db.cpp \
	tools/dbi.cpp \
	tools/mysql_db.cpp \
	tools/odbc_db.cpp \
	tools/oracle_db.cpp \
	tools/pairs.cpp \
	tools/theme.cpp \
	tools/thread.cpp

cpputils_cpputilsshared_libcpputilsshared_a_SOURCES= \
	cpputils/cpputilsshared/backtrace.cc \
	cpputils/cpputilsshared/cppdir.cc \
	cpputils/cpputilsshared/leoini.cc \
	cpputils/cpputilsshared/logtool2.cc \
	cpputils/cpputilsshared/string_utils.cc \
	cpputils/cpputilsshared/vec_utils.cc \
	cpputils/cpputilsshared/xml.cc


cpputils_cpputilsshared_cpputilsformat_libcpputilsformat_a_SOURCES= \
	cpputils/cpputilsshared/cpputilsformat/format1.cc \
	cpputils/cpputilsshared/cpputilsformat/format2.cc


if ENABLE_MERGE

MERGE_INFO=merge-info

endif

if ENABLE_STATIC

bin_PROGRAMS=xstow xstow-static xstow-stow $(MERGE_INFO)

else

bin_PROGRAMS=xstow $(MERGE_INFO)

endif

xstow_SOURCES=\
	file_option.cpp \
	file_option.h \
	main.cpp \
	setup.h \
	setup.cpp \
	tree.cpp \
	tree.h \
	local_config.h \
	setup_config.h \
	setup_config.cpp \
	nignore.cpp \
	nignore.h \
	copy.cpp \
	debug.h \
	debug.cpp \
	colored_output.h \
	colored_output.cpp \
	printf_format.h

xstow_LDADD= tools/libtools.a \
	cpputils/cpputilsshared/libcpputilsshared.a \
	cpputils/cpputilsshared/cpputilsformat/libcpputilsformat.a \
	$(LDFLAGS_DYNAMIC)

if ENABLE_MERGE

merge_info_SOURCES= \
	merge_info.cpp \
	file_option.cpp \
	file_option.h \
	printf_format.h \
	colored_output.h \
	colored_output.cpp \
	debug.cpp \
	debug.h

merge_info_LDADD= tools/libtools.a \
	cpputils/cpputilsshared/libcpputilsshared.a \
	cpputils/cpputilsshared/cpputilsformat/libcpputilsformat.a \
	$(LDFLAGS_DYNAMIC)

endif

if ENABLE_STATIC

xstow_static_SOURCES= $(xstow_SOURCES)
xstow_static_LDFLAGS= -static
xstow_static_CXXFLAGS= $(CXXFLAGS) -DSTATIC_COMPILE

xstow_stow_SOURCES= $(xstow_SOURCES)
xstow_stow_LDFLAGS= -static
xstow_stow_CXXFLAGS= $(CXXFLAGS) -DSTATIC_COMPILE -DGNU_STOW

endif

AM_CPPFLAGS = \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src/tools \
	-I$(top_srcdir)/src/cpputils/cpputilsshared  \
	-I$(top_srcdir)/src/cpputils/cpputilsshared/cpputilsformat \
	-std=c++11 \
	-DNOWAMAS 

