#   README
*   extending the original rxMath external library
*   . . .
*   full rewrite :-)
*   NO object oriented, plain simple c/cpp
*   more functions supported
*   . . .
*   `double`, `long double` and `quadmath` support
*
* * *

##  COPYRIGHT
*   Copyright (c) 2019, 2020 Enrico Sorichetti
*   Distributed under the Boost Software License, Version 1.0
*   See accompanying file `LICENSE_1_0.txt` or copy at
*   `[http://www.boost.org/LICENSE_1_0.txt](http://www.boost.org/LICENSE_1_0.txt)`
* * *

##  Repository
*   . . .
*   `https://github.com/00003481/rxMath.git`
*   `git@github.com:00003481/rxMath.git `
* * *

##  prerequisites
*   . . .
*   cmake ( minimum 3.18 )
* * *

##  nice to have
*   . . .
*   ccache to speed up builds
*   available at `https://github.com/ccache/ccache/releases`
* * *

##  How to
*   the `_cmake` script in the directory cmake/Tools will work
*   for a full install of rxmath/rxmath128 when ...
*   the compiler is the GNU gcc/g++
*   and quadmath + quadmath-devel have been installed
*   . . .
*   on apple only the rxmath library will be built,
*   unless a GNU compiler suite has been installed
*   the `_gmake` script will help in doing it
*   naturally the `_gmake` script will have to be modified to point to gcc/g++
*   . . .
*   no need to install, will run nicely from the `build directory`
*   just point the `LD_LIBRARY_PATH` to `<build directory>/lib`
*   . . .
* * *

##  Additional considerations
* * *

##  tested and working on
*   APPLE High Sierra / Mojave
*   Fedora 32/33
*   . . .
*   just rebuilding the Vagrant environment for
*   FreeBSD, NetBSD, OpenBSD
* * *

##  NOTES
*   [Markdown Information](https://bitbucket.org/tutorials/markdowndemo)
* * *


