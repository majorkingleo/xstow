Clone from github:
`git clone --recurse-submodules git@github.com:majorkingleo/xstow.git`

XStow is a replacement of GNU Stow (http://www.gnu.org/software/stow/)
written in C++. It supports all features of Stow with some extensions.

XStow, as GNU Stow, is a program for managing the installation of
software packages, keeping them separate (/usr/local/stow/emacs
vs. /usr/local/stow/perl, for example) while making them appear to be
installed in the same place (/usr/local).

Stow requires Perl. But what about systems where Perl isn't available, or not
yet installed? I tried compiling Stow with perlcc, but it failed.

For compiling XStow a C++ compiler and a system with a couple of POSIX
functions are required. It does not depend on an interpreter. Static
compilation for e.g. rescue disks are possible.

XStow installs packages in exactly the same way as Stow, so you
can even start a _Linux From Scratch_ installation with XStow and when
Perl is running continue installation using GNU Stow.

### Name

The X in the name has nothing to do with X11. It simply stands for
Extended Stow (... I think).

### Status

It works fine.

### Features

  XStow generates Stow compatible symlinks. You can install a package
  with XStow and uninstall it with Stow.

  It also supports the creation of symlinks with absolute path names,
  but this will break compatibilty with Stow.

  * automated stow directory detection
  * automated traversable link detection (/usr/man is a link to /usr/share/man)

### Planned Features

  If you have any other ideas send a mail to kingleo@gmx.at.

### Future

  By default full Stow compatibilty should be obtained. But more
    features will be impemented.

### Why

  * Stow requires Perl. But what about systems where Perl isn't available,
    or not yet installed? I tried compiling Stow with perlcc, but it
    failed. This was the initial reason.

  * Stow lacks some features and I'm not very common with perl... So I
    implemented them in XStow.

### Static versions

  If you enable static compiling 2 binaries 'xstow', 'xstow-static'
  will be created.

  The 'xstow-static' version has no ncurses support
  and the binary is compiled statically.

  If you are using XStow heaviliy, it's recomended to compile these static
  versions, since you are using XStow like a base system tool.
  Upgrading libc with a static xstow is no problem.

### Libraries

  - ncurses

  For better --help screen support, XStow uses the ncurses library.
  With the help of this library XStow uses the available width of the
  terminal it is running on.

  If you don't compile XStow with ncurses support, the terminal width
  will be guessed as 80 characters wide.

  So the ncurses support is absolutely not required.

### git

  When checking out from github you need to get all submodules by command:

  ```
  git clone --recurse-submodules git@github.com:majorkingleo/xstow.git
  ```

  Or if you already checked out:

  ```
  cd xstow && git submodule update --init --recursive
  ```
