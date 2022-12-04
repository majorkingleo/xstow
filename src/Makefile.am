
noinst_LIBRARIES=tools/libtools.a \
	cpputils/cpputilsshared/libcpputilsshared.a \
	cpputils/cpputilsshared/cpputilsformat/libcpputilsformat.a


tools_libtools_a_SOURCES= \
	tools/arg.cpp \
	tools/arg.h \
	tools/pairs.cpp \
	tools/pairs.h 

cpputils_cpputilsshared_libcpputilsshared_a_SOURCES= \
	cpputils/cpputilsshared/backtrace.cc \
	cpputils/cpputilsshared/backtrace.h \
	cpputils/cpputilsshared/cppdir.cc \
	cpputils/cpputilsshared/cppdir.h \
	cpputils/cpputilsshared/leoini.cc \
	cpputils/cpputilsshared/leoini.h \
	cpputils/cpputilsshared/string_utils.cc \
	cpputils/cpputilsshared/string_utils.h \
	cpputils/cpputilsshared/ref.h \
	cpputils/cpputilsshared/range.h \
	cpputils/cpputilsshared/iterator.h \
	cpputils/cpputilsshared/xml.cc \
	cpputils/cpputilsshared/xml.h \
	cpputils/cpputilsshared/stderr_exception.h \
	cpputils/cpputilsshared/TupleReverse.h
	cpputils/cpputilsshared/utf8.h \
	cpputils/cpputilsshared/utf8/core.h \
	cpputils/cpputilsshared/utf8/checked.h \
	cpputils/cpputilsshared/utf8/unchecked.h \
	cpputils/cpputilsshared/utf8/cpp11.h \
	cpputils/cpputilsshared/utf8/cpp17.h \
	cpputils/cpputilsshared/utf8_util.h \
	cpputils/cpputilsshared/utf8_util.cc


cpputils_cpputilsshared_cpputilsformat_libcpputilsformat_a_SOURCES= \
	cpputils/cpputilsshared/cpputilsformat/format1.cc \
	cpputils/cpputilsshared/cpputilsformat/format1.h \
	cpputils/cpputilsshared/cpputilsformat/format2.cc \
	cpputils/cpputilsshared/cpputilsformat/format2.h \
	cpputils/cpputilsshared/cpputilsformat/format.h \
	cpputils/cpputilsshared/cpputilsformat/cformat.h \
	cpputils/cpputilsshared/cpputilsformat/cwformat.h \
	cpputils/cpputilsshared/cpputilsformat/cwformat.cc \
	cpputils/cpputilsshared/cpputilsformat/wformat2.h \
	cpputils/cpputilsshared/cpputilsformat/wformat2.cc


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
	printf_format.h \
	tools_config.h

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

