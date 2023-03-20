cmake_minimum_required( VERSION 3.10 )

project( liblhs3util 
         DESCRIPTION "lhs3util library"
         HOMEPAGE_URL "https://github.com/fbuonaro/lhs3util/liblhs3util"
         LANGUAGES CXX
         VERSION 1.0.0 )

##############################################################
# general
##############################################################

include( GNUInstallDirs )

set( CMAKE_CXX_STANDARD 11 )

add_compile_options( -Wall -Wextra -pedantic -Werror )

##############################################################
# library
##############################################################

# install directories
set( LH_INSTALL_BINDIR "${CMAKE_INSTALL_BINDIR}" )
set( LH_INSTALL_LIBDIR "${CMAKE_INSTALL_LIBDIR}/liblhs3util_${PROJECT_VERSION}" )
set( LH_INSTALL_INCDIR "${CMAKE_INSTALL_INCLUDEDIR}/lhs3util_${PROJECT_VERSION}" )

# pull in crypto
find_package( OpenSSL REQUIRED )
# pull in curl
find_package( CURL REQUIRED )
# pull in liblhmiscutil
find_package( liblhmiscutil REQUIRED )

# library name
set(LH_LIB_NAME lhs3util)

# source files
set( LH_LIB_SRC_FILES 
    "src/is3requester.cxx"
    "src/s3istreamuploader.cxx"
    "src/s3ostreamdownloader.cxx"
    "src/s3requestcontext.cxx"
    "src/s3requester.cxx" )
# library dependencies
set( LH_LIB_PUBLIC_LINKLIBS liblhmiscutil::lhmiscutil )
set( LH_LIB_PRIVATE_LINKLIBS s3 
                             "${CURL_LIBRARIES}" 
                             "${OPENSSL_CRYPTO_LIBRARY}" )
# header dependencies
set( LH_LIB_PUBLIC_INCLUDES 
     $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/lhs3util>        # build
     $<INSTALL_INTERFACE:${LH_INSTALL_INCDIR}> )                               # export
set( LH_LIB_PRIVATE_INCLUDES 
     "${CMAKE_CURRENT_SOURCE_DIR}/include/lhs3util_impl"
     "${CMAKE_CURRENT_BINARY_DIR}" )

lh_add_library()

##############################################################
# unit tests
##############################################################

# pull in gtest
find_package( GTest REQUIRED )

add_executable( testlhs3util
                "test/testlhs3util.cxx" )

target_link_libraries( testlhs3util
                       PRIVATE
                           "${GTEST_BOTH_LIBRARIES}"
                           lhs3util )

target_include_directories( testlhs3util
                            PRIVATE
                                "${LH_LIB_PRIVATE_INCLUDES}"
                                "${CMAKE_CURRENT_BINARY_DIR}"
                                "${GTEST_INCLUDE_DIRS}" )

include( GoogleTest )
gtest_add_tests( TARGET testlhs3util )

##############################################################
# installation
##############################################################

# name of component to install
set(LH_COMPONENT_NAME liblhs3util )
# directories to install
set(LH_LIB_PUBLIC_INCLUDE_DIRS "include/lhs3util/lhs3util")

lh_add_install_library()

##############################################################
# pkg-config
##############################################################
set( PC_INCDIR "${LH_INSTALL_INCDIR}")
set( PC_LIBDIR "${LH_INSTALL_LIBDIR}")
set( PC_PUBREQS )
set( PC_PRIVREQS )
set( PC_CFLAGS )
set( PC_PUBLIBS "-llhs3util" )
set( PC_PRIVLIBS )
set( PC_INSTALL_DIR "${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig" )
set( PC_INSTALL_FILENAME "${PROJECT_NAME}-${CMAKE_PROJECT_VERSION_MAJOR}.pc" )
set( PC_INSTALL_FILE "${PC_INSTALL_DIR}/${PC_INSTALL_FILENAME}" )
set( PC_RELOCATE_SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/rpm_relocate_pc.sh" )

lh_add_pkgconfig()

##############################################################
# cmake packaging
##############################################################

lh_add_install_cmake_config()

##############################################################
# rpm packaging
##############################################################
 
# to relocate the pc file prefix to /somewhere if 'rpm --install --prefix /somewhere'
set( CPACK_RPM_LIBLHS3UTIL_PACKAGE_REQUIRES_POST "sed" PARENT_SCOPE) 
set( CPACK_RPM_LIBLHS3UTIL_POST_INSTALL_SCRIPT_FILE "${PC_RELOCATE_SCRIPT}" PARENT_SCOPE)

set(LH_PACKAGE_DESC "liblhs3util libraries package")

lh_add_cpack_component()
