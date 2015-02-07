cmake_minimum_required(VERSION 2.8.6)
project(Jreen)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include( GNUInstallDirs )

set(CMAKE_JREEN_VERSION_MAJOR 1 CACHE INT "Major Jreen version number" FORCE)
set(CMAKE_JREEN_VERSION_MINOR 2 CACHE INT "Minor Jreen version number" FORCE)
set(CMAKE_JREEN_VERSION_PATCH 1 CACHE INT "Release Jreen version number" FORCE)
set(CMAKE_JREEN_VERSION_STRING "${CMAKE_JREEN_VERSION_MAJOR}.${CMAKE_JREEN_VERSION_MINOR}.${CMAKE_JREEN_VERSION_PATCH}" CACHE STRING "Jreen version string" FORCE)

# Search for source and headers in source directory (non-recursive)
file(GLOB_RECURSE SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

option(JREEN_FORCE_QT4 "Force using of Qt4 in Jreen" off)
set(JREEN_SUFFIX "")
set(JREEN_INCLUDE_PATH "")
set(JREEN_QT_MIDDLE "")

if(NOT JREEN_FORCE_QT4)
	find_package(Qt5Core QUIET)
endif()

if(Qt5Core_DIR)
    find_package(Qt5Network QUIET)
    message(STATUS "Found Qt5!")
    set(JREEN_SUFFIX "-qt5")
    set(JREEN_INCLUDE_PATH "jreen-qt5")
    set(JREEN_QT_MIDDLE "5")

    add_definitions("-DQT_DISABLE_DEPRECATED_BEFORE=0")
    list(REMOVE_ITEM SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/sjdns.cpp")
else()
    message(STATUS "Could not find Qt5, so let's use Qt4...")
    set(JREEN_INCLUDE_PATH "jreen-qt4")

    find_package(Qt4 4.6.0 COMPONENTS QtCore QtNetwork REQUIRED)

    macro(qt5_use_modules)
    endmacro()

    include(${QT_USE_FILE})
endif()

if(JREEN_BUILD_INTERNAL)
        add_definitions("-DJ_BUILD_INTERNAL")
endif()
find_package(PkgConfig)
find_package(GSASL REQUIRED)

pkg_search_module(SPEEX speex)
if(NOT SPEEX_FOUND)
    message(STATUS "Could not find speex library.")
else()
    message(STATUS "Speex library is found.")
    include_directories(${SPEEX_INCLUDE_DIRS})
    add_definitions("-DJREEN_HAVE_SPEEX=1")
endif()

# FindZLIB is broken on Ubuntu, so find the library using pkg-config
if(LINUX)
    find_package(PkgConfig)
    pkg_search_module(ZLIB_PKGCONF REQUIRED zlib)

    find_path(ZLIB_INCLUDE_DIR
        NAMES zlib.h
        PATHS ${ZLIB_PKGCONF_INCLUDE_DIRS}
    )

    find_library(ZLIB_LIBRARIES
        NAMES z
        PATHS ${ZLIB_PKGCONF_LIBRARY_DIRS}
    )

    if(NOT ZLIB_INCLUDE_DIR)
        message(SEND_ERROR "Could not find zlib includes.")
    endif(NOT ZLIB_INCLUDE_DIR)

    if(NOT ZLIB_LIBRARIES)
        message(SEND_ERROR "Could not find zlib library.")
    endif(NOT ZLIB_LIBRARIES)
else()
    find_package(ZLIB REQUIRED)
endif()

# Add include directories
include_directories(.
    ${CMAKE_CURRENT_BINARY_DIR}
    3rdparty
    ${GSASL_INCLUDE_DIRS}
    ${GSASL_INCLUDEDIR}
    ${ZLIB_INCLUDE_DIR}
)

LIST(APPEND SRC ${3RD_PARTY_SRC_C})
LIST(APPEND SRC ${3RD_PARTY_SRC})

# Ice support
option(JREEN_USE_IRISICE "Use ICE from IRIS" OFF)
if (JREEN_USE_IRISICE)
    file(GLOB_RECURSE ICESUPPORT_SRC "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/icesupport/*.cpp")
    file(GLOB_RECURSE ICESUPPORT_SRC_C "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/icesupport/*.c")
    list(APPEND SRC ${ICESUPPORT_SRC})
    list(APPEND SRC ${ICESUPPORT_SRC_C})
    add_definitions("-DHAVE_IRISICE")
endif()

if(NOT Qt5Core_DIR)
    option(JREEN_USE_SYSTEM_JDNS "Use system JDNS" OFF)

    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/jdns")
        set(JREEN_USE_SYSTEM_JDNS ON)
    endif()

    if(JREEN_USE_SYSTEM_JDNS)
        # For Linux it was found already before at line 29
        if(NOT LINUX)
                find_package(PkgConfig)
        endif()
        pkg_search_module(JDNS REQUIRED qjdns)
        include_directories(${JDNS_INCLUDE_DIR})
        list(APPEND EXTRA_LIBS ${JDNS_LIBRARIES})
    else()
        file(GLOB_RECURSE JDNS_SRC "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/jdns/*.cpp")
        file(GLOB_RECURSE JDNS_SRC_C "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/jdns/*.c")
        list(APPEND SRC ${JDNS_SRC})
        list(APPEND SRC ${JDNS_SRC_C})
        include_directories("${CMAKE_CURRENT_SOURCE_DIR}/3rdparty")
    endif()
endif()

# This project will generate library

if(SYMBIAN)
    set(JREEN_BUILD_TYPE STATIC)
else()
set(JREEN_BUILD_TYPE SHARED)
endif()

ADD_LIBRARY(jreen${JREEN_SUFFIX} ${JREEN_BUILD_TYPE} ${SRC} ${MOC_SRC})

#compiler flags
#fixme add macro to clang
if(CMAKE_COMPILER_IS_GNUCXX)
        if(SYMBIAN)
		#disable stupid warnings
		ADD_DEFINITIONS("-w")
        else()
		ADD_DEFINITIONS("-Wall -Wextra" ) #add -Werror
        ENDif()
        if(NOT WIN32)
		ADD_DEFINITIONS("-fvisibility=hidden" )
        endif(NOT WIN32)
elseif(MSVC)
	ADD_DEFINITIONS("/W3" )
endif()

set_target_properties(jreen${JREEN_SUFFIX} PROPERTIES
	VERSION ${CMAKE_JREEN_VERSION_STRING}
	SOVERSION ${CMAKE_JREEN_VERSION_MAJOR}
	DEFINE_SYMBOL J_BUILD_LIBRARY
)

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/include/jreen" )
file(GLOB_RECURSE JREEN_INCLUDE_HEADERS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/src" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")

foreach(header ${JREEN_INCLUDE_HEADERS})
        if(NOT ${header} MATCHES ".*_p\\.h")
                if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/include/jreen/${header}")
			file( WRITE "${CMAKE_CURRENT_BINARY_DIR}/include/jreen/${header}"
		"#include \"${CMAKE_CURRENT_SOURCE_DIR}/src/${header}\"
	"
			)
                endif(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/include/jreen/${header}")
		get_filename_component(headerPath ${header} PATH)
		install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/${header}"
				DESTINATION include/${JREEN_INCLUDE_PATH}/jreen/${headerPath}
				COMPONENT jreenDevel
		)
    endif(NOT ${header} MATCHES ".*_p\\.h")
endforeach(header)

set(JREEN_LIBRARIES jreen${JREEN_SUFFIX} CACHE INTERNAL "" )
set(JREEN_INCLUDE_DIRS "${CMAKE_CURRENT_BINARY_DIR}/include" CACHE INTERNAL "")

if(WIN32)
    list(APPEND EXTRA_LIBS ws2_32)
endif()

if(SPEEX_FOUND)
    list(APPEND EXTRA_LIBS ${SPEEX_LIBRARIES} )
endif()

target_link_libraries(jreen${JREEN_SUFFIX}
    ${QT_LIBRARIES}
    ${GSASL_LIBRARIES}
    ${ZLIB_LIBRARIES}
    ${EXTRA_LIBS}
)

set_target_properties(jreen${JREEN_SUFFIX} PROPERTIES AUTOMOC TRUE)
qt5_use_modules(jreen${JREEN_SUFFIX} Core Network)

# Install package config file
if(NOT WIN32)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/libjreen.pc.cmake
		${CMAKE_CURRENT_BINARY_DIR}/libjreen${JREEN_SUFFIX}.pc
	)
	install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libjreen${JREEN_SUFFIX}.pc
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)
endif()

install( TARGETS jreen${JREEN_SUFFIX}
	RUNTIME DESTINATION bin
		COMPONENT jreen
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
		COMPONENT jreen
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
		COMPONENT jreenDevel
	BUNDLE DESTINATION library
)

