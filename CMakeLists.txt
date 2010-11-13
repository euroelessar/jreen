CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
PROJECT(jreen)

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")


# Search for source and headers in source directory (non-recursive)
FILE(GLOB SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
FILE(GLOB HDR "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")

# Require QT 4.6
SET(QT_MIN_VERSION "4.6.0")

# Search for QT4
FIND_PACKAGE(Qt4 COMPONENTS QtCore QtNetwork QtXml QtGui REQUIRED)
INCLUDE(${QT_USE_FILE})
FIND_PACKAGE(QCA2 REQUIRED)
FIND_PACKAGE(ZLIB REQUIRED)

#add_subdirectory(3rdparty)

# Add JDns sources
LIST(APPEND SRC "3rdparty/jdns/jdns.c"
		"3rdparty/jdns/jdns_mdnsd.c"
		"3rdparty/jdns/jdns_packet.c"
		"3rdparty/jdns/jdns_sys.c"
		"3rdparty/jdns/jdns_util.c"
		"3rdparty/jdns/qjdns_sock.cpp"
		"3rdparty/jdns/qjdns.cpp" )
LIST(APPEND HDR "3rdparty/jdns/qjdns.h"
		"3rdparty/jdns/qjdns_p.h" )

#spike

# Add include directories
INCLUDE_DIRECTORIES(3rdparty
	${QCA2_INCLUDE_DIR}
	${ZLIB_INCLUDE_DIR}
)

# Generate moc files
QT4_WRAP_CPP(MOC_SRC ${HDR})
QT4_AUTOMOC(MOC_SRC ${SRC})

# This project will generate library
ADD_LIBRARY(jreen SHARED ${SRC} ${MOC_SRC} ${HDR} )

#compiler flags
#fixme add macro to clang
if(CMAKE_COMPILER_IS_GNUCXX)
	set_target_properties(jreen PROPERTIES COMPILE_FLAGS "-Wall -Wextra" ) #add -Werror
	if(NOT WIN32)
		set_target_properties(jreen PROPERTIES COMPILE_FLAGS "-fvisibility=hidden" )
	endif(NOT WIN32)
elseif(MSVC)
	set_target_properties(jreen PROPERTIES COMPILE_FLAGS "/W3 /WX" )
endif()
ADD_DEFINITIONS(-DJ_BUILD_LIBRARY)

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/include/jreen" )
file(GLOB JREEN_INCLUDE_HEADERS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/src" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
foreach(header ${JREEN_INCLUDE_HEADERS})
	if(NOT ${header} MATCHES ".*_p\\.h")
		if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/include/jreen/${header}")
			file( WRITE "${CMAKE_CURRENT_BINARY_DIR}/include/jreen/${header}"
		"#include \"${CMAKE_CURRENT_SOURCE_DIR}/src/${header}\"
	"
			)
		endif(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/include/jreen/${header}")
	LIST(APPEND JREEN_INSTALL_HEADERS ${header})
    endif(NOT ${header} MATCHES ".*_p\\.h")
endforeach(header)

set(JREEN_LIBRARIES jreen CACHE INTERNAL "" )
set(JREEN_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/include" CACHE INTERNAL "")

# Link with QT
TARGET_LINK_LIBRARIES(jreen ${QT_LIBRARIES} ${QCA2_LIBRARIES} ${ZLIB_LIBRARIES})

install( TARGETS jreen
	RUNTIME DESTINATION bin
		COMPONENT jreen
	LIBRARY DESTINATION lib
		COMPONENT jreen
	ARCHIVE DESTINATION lib
		COMPONENT jreenDevel
	BUNDLE DESTINATION library
)

INSTALL(FILES ${JREEN_INSTALL_HEADERS}
	DESTINATION include/jreen
		COMPONENT jreenDevel
)
