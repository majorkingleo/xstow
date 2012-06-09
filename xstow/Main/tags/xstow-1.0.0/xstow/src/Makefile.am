if ENABLE_MERGE

MERGE_INFO=merge-info

endif

if ENABLE_STATIC

bin_PROGRAMS=xstow xstow-static xstow-stow $(MERGE_INFO)

else

bin_PROGRAMS=xstow $(MERGE_INFO)

endif

xstow_SOURCES=\
	arg.cpp \
	arg.h \
	cppdir.cpp \
	cppdir.h \
	debug.h \
	debug.cpp \
	file_option.cpp \
	file_option.h \
	format.cpp \
	format.h \
	iterator.h \
	main.cpp \
	ref.h \
	setup.h \
	setup.cpp \
	string_utils.cpp \
	string_utils.h \
	tree.cpp \
	tree.h \
	leoini.h \
	leoini.cpp \
	local_config.h \
	setup_config.h \
	setup_config.cpp \
	nignore.cpp \
	nignore.h \
	copy.cpp \
	range.h \
	backtrace.h \
	backtrace.cpp

xstow_LDFLAGS= $(LDFLAGS) $(LDFLAGS_DYNAMIC)

if ENABLE_MERGE

merge_info_SOURCES= merge_info.cpp \
			arg.cpp arg.h \
			file_option.cpp file_option.h \
			cppdir.cpp cppdir.h \
			string_utils.cpp string_utils.h \
			debug.cpp debug.h  \
			local_config.h \
			format.cpp format.h \
			ref.h \
			range.h \
			backtrace.h \
			backtrace.cpp

merge_info_LDFLAGS= $(LDFLAGS) $(LDFLAGS_DYNAMIC)

endif

if ENABLE_STATIC

xstow_static_SOURCES= $(xstow_SOURCES)
xstow_static_LDFLAGS= -static
xstow_static_CXXFLAGS= $(CXXFLAGS) -DSTATIC_COMPILE

xstow_stow_SOURCES= $(xstow_SOURCES)
xstow_stow_LDFLAGS= -static
xstow_stow_CXXFLAGS= $(CXXFLAGS) -DSTATIC_COMPILE -DGNU_STOW

endif

DEFAULT_INCLUDES= -I$(top_builddir)

xstow-mini.o: $(xstow_SOURCES)
	cat $(xstow_SOURCES) > all.cc ; \
	$(CXX) -c -o xstow-mini.o \
	-DSTATIC_COMPILE -DGNU_STOW -DHAVE_CONFIG_H -DNDEBUG \
	$(DEFAULT_INCLUDES) -fno-default-inline -O2 \
	all.cc 


xstow-mini: xstow-mini.o
	$(CXX) -o xstow-mini xstow-mini.o ; \
	strip xstow-mini

xstow-mini-static: xstow-mini.o
	$(CXX) -o xstow-mini-static xstow-mini.o -static ; \
	strip xstow-mini-static
