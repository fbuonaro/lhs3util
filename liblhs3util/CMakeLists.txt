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
set( INSTALL_BINDIR "${CMAKE_INSTALL_BINDIR}" )
set( INSTALL_LIBDIR "${CMAKE_INSTALL_LIBDIR}/liblhs3util_${PROJECT_VERSION}" )
set( INSTALL_INCDIR "${CMAKE_INSTALL_INCLUDEDIR}/lhs3util_${PROJECT_VERSION}" )

# pull in crypto
find_package( OpenSSL REQUIRED )
# pull in curl
find_package( CURL REQUIRED )
# pull in liblhmiscutil
find_package( liblhmiscutil REQUIRED )

# source files
set( LIBLHS3UTIL_SRC_FILES 
    "src/is3requester.cxx"
    "src/s3istreamuploader.cxx"
    "src/s3ostreamdownloader.cxx"
    "src/s3requestcontext.cxx"
    "src/s3requester.cxx" )
# library dependencies
set( LIBLHS3UTIL_PUBLIC_LINKLIBS liblhmiscutil::lhmiscutil )
set( LIBLHS3UTIL_PRIVATE_LINKLIBS s3 
                                  "${CURL_LIBRARIES}" 
                                  "${OPENSSL_CRYPTO_LIBRARY}" )
# header dependencies
set( LIBLHS3UTIL_PUBLIC_INCLUDES 
     $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/lhs3util>        # build
     $<INSTALL_INTERFACE:${INSTALL_INCDIR}> )                               # export
set( LIBLHS3UTIL_PRIVATE_INCLUDES 
     "include/lhs3util_impl"
     "${CMAKE_CURRENT_BINARY_DIR}" )

# boilerplate ..
# libs
add_library( lhs3util SHARED ${LIBLHS3UTIL_SRC_FILES} )
add_library( lhs3util_static STATIC ${LIBLHS3UTIL_SRC_FILES} )
# lib deps
target_link_libraries( lhs3util
                       PUBLIC "${LIBLHS3UTIL_PUBLIC_LINKLIBS}"
                       PRIVATE "${LIBLHS3UTIL_PRIVATE_LINKLIBS}" )
target_link_libraries( lhs3util_static
                       PUBLIC "${LIBLHS3UTIL_PUBLIC_LINKLIBS}"
                       PRIVATE "${LIBLHS3UTIL_PRIVATE_LINKLIBS}" )
# header deps
target_include_directories( lhs3util
                            PUBLIC "${LIBLHS3UTIL_PUBLIC_INCLUDES}"
                            PRIVATE "${LIBLHS3UTIL_PRIVATE_INCLUDES}" )
target_include_directories( lhs3util_static
                            PUBLIC "${LIBLHS3UTIL_PUBLIC_INCLUDES}"
                            PRIVATE "${LIBLHS3UTIL_PRIVATE_INCLUDES}" )
# properties
set_target_properties( lhs3util
                       PROPERTIES 
                           LIBRARY_OUTPUT_DIRECTORY "${INSTALL_LIBDIR}"
                           SOVERSION ${PROJECT_VERSION}
                           VERSION ${PROJECT_VERSION}
                           CLEAN_DIRECT_OUTPUT 1)
set_target_properties( lhs3util_static 
                       PROPERTIES
                           ARCHIVE_OUTPUT_DIRECTORY "${INSTALL_LIBDIR}"
                           OUTPUT_NAME lhs3util )
# .. boilerplate

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
                                "${LIBLHS3UTIL_PRIVATE_INCLUDES}"
                                "${CMAKE_CURRENT_BINARY_DIR}"
                                "${GTEST_INCLUDE_DIRS}" )

include( GoogleTest )
gtest_add_tests( TARGET testlhs3util )

##############################################################
# installation
##############################################################

# boilerplate ..
# install lib/bin
install( TARGETS lhs3util lhs3util_static
         EXPORT liblhs3util-targets 
         COMPONENT liblhs3util
         RUNTIME DESTINATION "${INSTALL_BINDIR}"
         COMPONENT liblhs3util
         LIBRARY DESTINATION "${INSTALL_LIBDIR}"
         COMPONENT liblhs3util
         ARCHIVE DESTINATION "${INSTALL_LIBDIR}"
         COMPONENT liblhs3util
         INCLUDES DESTINATION "${LIBLHS3UTIL_PUBLIC_INCLUDES}" )
# install header dir
install( DIRECTORY "include/lhs3util/lhs3util"
         DESTINATION "${INSTALL_INCDIR}"
         COMPONENT liblhs3util
         FILES_MATCHING PATTERN "*.h" )
# install export for things outside of build
install( EXPORT liblhs3util-targets 
         FILE liblhs3utilTargets.cmake 
         NAMESPACE liblhs3util:: 
         DESTINATION "${INSTALL_LIBDIR}/cmake" 
         COMPONENT liblhs3util )
# export export for things in build
export( EXPORT liblhs3util-targets
    FILE ${CMAKE_BINARY_DIR}/cmake/liblhs3utilTargets.cmake 
    NAMESPACE liblhs3util:: )
ADD_CUSTOM_TARGET( install-lhs3util
                   ${CMAKE_COMMAND}
                   -D "CMAKE_INSTALL_COMPONENT=liblhs3util"
                   -P "cmake_install.cmake" )
# .. boilerplate

##############################################################
# pkg-config
##############################################################
set( PC_INCDIR "${INSTALL_INCDIR}")
set( PC_LIBDIR "${INSTALL_LIBDIR}")
set( PC_PUBREQS )
set( PC_PRIVREQS )
set( PC_CFLAGS )
set( PC_PUBLIBS "-llhs3util" )
set( PC_PRIVLIBS )
set( PC_INSTALL_DIR "${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig" )
set( PC_INSTALL_FILENAME "${PROJECT_NAME}-${CMAKE_PROJECT_VERSION_MAJOR}.pc" )
set( PC_INSTALL_FILE "${PC_INSTALL_DIR}/${PC_INSTALL_FILENAME}" )
set( PC_RELOCATE_SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/rpm_relocate_pc.sh" )

# boilerplate ..
# configure pc file
configure_file( "${CMAKE_SOURCE_DIR}/modules/lhscriptutil/cmake/pkgconfig/component.pc.in"
                "${CMAKE_CURRENT_BINARY_DIR}/${PC_INSTALL_FILENAME}"
                @ONLY )
# install pc file
install( FILES "${CMAKE_CURRENT_BINARY_DIR}/${PC_INSTALL_FILENAME}"
         DESTINATION "${PC_INSTALL_DIR}"
         COMPONENT liblhs3util )
# configure pc relocation script
configure_file( "${CMAKE_SOURCE_DIR}/modules/lhscriptutil/cmake/pkgconfig/rpm_relocate_pc.sh.in"
                "${CMAKE_CURRENT_BINARY_DIR}/rpm_relocate_pc.sh"
                @ONLY )
# .. boilerplate

##############################################################
# cmake packaging
##############################################################

# boilerplate ..
include(CMakePackageConfigHelpers)

configure_package_config_file( "${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
                               "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
                               INSTALL_DESTINATION "${INSTALL_LIBDIR}/cmake" )

write_basic_package_version_file( "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
                                  VERSION "${PROJECT_VERSION}"
                                  COMPATIBILITY SameMajorVersion )

install( FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake" "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
         DESTINATION "${INSTALL_LIBDIR}/cmake"
         COMPONENT liblhs3util )
# .. boilerplate

##############################################################
# rpm packaging
##############################################################
 
# boilerplate ..
set( CPACK_PROJECT_NAME ${PROJECT_NAME} )
set( CPACK_PROJECT_VERSION ${PROJECT_VERSION} )
set( CPACK_COMPONENTS_GROUPING ONE_PER_GROUP )
set( CPACK_GENERATOR "RPM" )

# set( CPACK_GENERATOR "RPM" "TGZ" )
# CPACK_RPM_BUILD_REQUIRES
# CPACK_RPM_liblhs3util_REQUIRES # component requires

set( CPACK_RPM_PACKAGE_RELOCATABLE ON)
set( CPACK_RPM_COMPONENT_INSTALL ON )

# to relocate the pc file prefix to /somewhere if 'rpm --install --prefix /somewhere'
set( CPACK_RPM_PACKAGE_REQUIRES_POST "sed" ) 
set( CPACK_RPM_POST_INSTALL_SCRIPT_FILE "${PC_RELOCATE_SCRIPT}" )

include( CPack )

# component, group can be different; group will appended to package
cpack_add_component(liblhs3util
    DISPLAY_NAME  "lhs3util library"
    DESCRIPTION   "lhs3util library"
    GROUP liblhs3util)
cpack_add_component_group(liblhs3util)
# .. boilerplate