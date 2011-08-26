prefix=${CMAKE_INSTALL_PREFIX}
exec_prefix=${CMAKE_INSTALL_PREFIX}/bin
libdir=${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}
includedir=${CMAKE_INSTALL_PREFIX}/include

Name: libjreen
Description: Qt Jabber/XMPP extensible library
Requires: QtCore QtNetwork
Version: ${CMAKE_JREEN_VERSION_MAJOR}.${CMAKE_JREEN_VERSION_MINOR}.${CMAKE_JREEN_VERSION_PATCH}
Libs: -L${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX} -ljreen
Cflags: -I${CMAKE_INSTALL_PREFIX}/include

