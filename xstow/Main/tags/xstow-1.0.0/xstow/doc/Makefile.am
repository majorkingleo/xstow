EXTRA_DIST=xstow.s.pod xstow.ini.s.pod xstow.1 xstow.html \
           xstow.ini xstow.ini.html xstow.ini.5 nignore.ini \
	   check-stow.sh ifdef.awk merge-info.1 merge-info.s.pod

man1_MANS=xstow.1

if ENABLE_INI
  man5_MANS=xstow.ini.5
endif

if ENABLE_MERGE
  man1_MANS+= merge-info.1
endif

DEFINES=

if ENABLE_STATIC
  DEFINES+= ENABLE_STATIC
endif

if ENABLE_INI
  DEFINES+= ENABLE_INI
endif

if ENABLE_FNMATCH
  DEFINES+= ENABLE_FNMATCH
endif

if ENABLE_REGEX
  DEFINES+= ENABLE_REGEX
endif

if ENABLE_CURSES
  DEFINES+= ENABLE_CURSES
endif


if ENABLE_INI
if ENABLE_FNMATCH
  DEFINES+= ENABLE_NIGNORE
endif
endif

predef:
	awk -v defines="$(DEFINES)" -f ifdef.awk xstow.s.pod > xstow.pod
	awk -v defines="$(DEFINES)" -f ifdef.awk xstow.ini.s.pod > xstow.ini.pod
	awk -v defines="$(DEFINES)" -f ifdef.awk merge-info.s.pod > merge-info.pod

man: predef	
	version=$(VERSION) ; \
	pod2man -s 1 -c "XStow Reference Guide" -r "XStow $$version" xstow.pod xstow.1 ;\
	pod2man -s 1 -c "XStow Reference Guide" -r "XStow $$version" merge-info.pod merge-info.1 ;\
	pod2man -s 5 -c "XStow Reference Guide" -r "XStow $$version" xstow.ini.pod xstow.ini.5

html: predef
	pod2html xstow.pod > xstow.html
	pod2html xstow.ini.pod > xstow.ini.html
	pod2html merge-info.pod > merge-info.html

docdir = $(datadir)/doc/$(PACKAGE)

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(docdir)
	$(INSTALL_DATA) xstow.html $(DESTDIR)$(docdir)
	$(INSTALL_DATA) xstow.ini.html $(DESTDIR)$(docdir)
	$(INSTALL_DATA) xstow.ini $(DESTDIR)$(docdir)
