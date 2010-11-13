CMAKE_MINIMUM_REQUIRED (VERSION 2.6 FATAL_ERROR)
IF(COMMAND cmake_policy)
	cmake_policy (SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)
PROJECT(jdns)

FIND_PACKAGE(Qt4 COMPONENTS QtCore QtNetwork REQUIRED)
INCLUDE(${QT_USE_FILE})

SET(HDR
	qjdns_p.h
)

SET(SRC
	"jdns.c"
	"jdns_mdnsd.c"
	"jdns_packet.c"
	"jdns_sys.c"
	"jdns_util.c"
	"qjdns_sock.cpp"
	"qjdns.cpp"
)

# Generate moc files
QT4_WRAP_CPP(MOC_SRC ${HDR})
QT4_AUTOMOC(MOC_SRC ${SRC})

# This project will generate library
ADD_LIBRARY(jdns STATIC ${SRC} ${MOC_SRC})

IF(UNIX)
	SET_TARGET_PROPERTIES(jdns PROPERTIES COMPILE_FLAGS "${COMPILE_FLAGS} -fPIC")
ENDIF(UNIX)
