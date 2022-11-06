cmake_minimum_required( VERSION 3.10 )

project( lhs3util 
         DESCRIPTION "lhs3util library"
         HOMEPAGE_URL "https://github.com/fbuonaro/lhs3util"
         LANGUAGES CXX
         VERSION 1.0.0 )

enable_testing()

add_subdirectory( liblhs3util )