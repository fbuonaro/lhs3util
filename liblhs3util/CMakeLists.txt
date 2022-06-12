cmake_minimum_required( VERSION 3.10 )

project( liblhs3util )

# general flags for building successfully
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" )

# for debugging
set( LLHC_CXX_FLAGS_DBG "${LLHC_CXX_FLAGS_DBG} -Wall -Wextra -W" )
set( LLHC_CXX_FLAGS_DBG "${LLHC_CXX_FLAGS_DBG} -DDEBUG" )
set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${LLHC_CXX_FLAGS_DBG}" )
set( CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${LLHC_CXX_FLAGS_DBG}" )

##############################################################
# library
##############################################################

set( LIB_SRC_DIR "src" )
set( LIB_INC_DIR "include" )

# pull in gtest
find_package( GTest REQUIRED )
if( NOT GTEST_FOUND )
    message( FATAL_ERROR "gtest could not be found" )
endif( NOT GTEST_FOUND )

# pull in crypto
find_package( OpenSSL REQUIRED )
if( NOT OPENSSL_FOUND )
    message( FATAL_ERROR "openssl/crypto could not be found" )
endif( NOT OPENSSL_FOUND )

# pull in curl
find_package( CURL REQUIRED )
if( NOT CURL_FOUND )
    message( FATAL_ERROR "curl could not be found" )
endif( NOT CURL_FOUND )

add_library( lhs3util SHARED
    "${LIB_SRC_DIR}/is3requester.cxx"
    "${LIB_SRC_DIR}/s3istreamuploader.cxx"
    "${LIB_SRC_DIR}/s3requestcontext.cxx"
    "${LIB_SRC_DIR}/s3requester.cxx" )

target_link_libraries( lhs3util
    PUBLIC
        lhmiscutil
    PRIVATE
        s3
        "${CURL_LIBRARIES}"
        "${OPENSSL_CRYPTO_LIBRARY}" )


target_include_directories( lhs3util
    PUBLIC
        "${LIB_INC_DIR}/lhs3util"
    PRIVATE
        "${LIB_INC_DIR}/lhs3util_impl" )

##############################################################
# unit tests
##############################################################

set( TST_SRC_DIR "./test" )

enable_testing()

add_executable( testlhs3util
    "${TST_SRC_DIR}/testlhs3util.cxx" )

target_link_libraries( testlhs3util
    PRIVATE
        "${GTEST_BOTH_LIBRARIES}"
        lhs3util )

target_include_directories( testlhs3util
    PRIVATE
        "${CMAKE_CURRENT_BINARY_DIR}"
        "${GTEST_INCLUDE_DIRS}"
        "${LIB_INC_DIR}/lhs3util"
        "${LIB_INC_DIR}/lhs3util_impl" )

include( GoogleTest )
gtest_add_tests( TARGET testlhs3util )

##############################################################
# installation
##############################################################

# library
install( TARGETS lhs3util
         LIBRARY
            DESTINATION lib
            COMPONENT lhs3util )

# include directory
install( DIRECTORY "${LIB_INC_DIR}/lhs3util/lhs3util"
         DESTINATION include
         COMPONENT lhs3util
         FILES_MATCHING PATTERN "*.h" )

ADD_CUSTOM_TARGET( install-lhs3util
                   ${CMAKE_COMMAND}
                   -D "CMAKE_INSTALL_COMPONENT=lhs3util"
                   -P "cmake_install.cmake" )
