cmake_minimum_required(VERSION 2.8.9)

project(Citizen-Server)

enable_testing()

set(CMAKE_MODULE_PATH "." ${CMAKE_MODULE_PATH})
find_package(CryptoPP)

include_directories(include ../common/include ${CRYPTOPP_INCLUDE_DIRS})

file(GLOB all_source src/* ../common/src/*)

add_definitions("-std=c++0x")
add_definitions("-g")
add_definitions("-Wall -Wextra -Wno-switch")
add_definitions("-DWITH_MYSQL")

# add support for mysql++
FIND_PATH(MYSQLPP_INCLUDE_PATH mysql++.h /usr/include/mysql++/)

FIND_PATH(MYSQL_INCLUDE_PATH mysql.h /usr/include/mysql/)
IF(MYSQL_INCLUDE_PATH AND MYSQLPP_INCLUDE_PATH)
  SET(MYSQLPP_INCLUDE_DIR
    ${MYSQLPP_INCLUDE_PATH}
    ${MYSQL_INCLUDE_PATH}
  )
ENDIF(MYSQL_INCLUDE_PATH AND MYSQLPP_INCLUDE_PATH)

FIND_LIBRARY(MYSQLPP_LIBRARIES mysqlpp)

IF (MYSQLPP_INCLUDE_DIR AND MYSQLPP_LIBRARIES)
   SET(MYSQLPP_FOUND TRUE)
ELSE (MYSQLPP_INCLUDE_DIR AND MYSQLPP_LIBRARIES)
   SET(MYSQLPP_FOUND FALSE)
ENDIF (MYSQLPP_INCLUDE_DIR AND MYSQLPP_LIBRARIES)

IF(MYSQLPP_FIND_REQUIRED)
  IF(MYSQLPP_FOUND)
  ELSE(MYSQLPP_FOUND)
    MESSAGE(FATAL_ERROR "Could not find mysqlpp")
  ENDIF(MYSQLPP_FOUND)
ENDIF(MYSQLPP_FIND_REQUIRED)


MARK_AS_ADVANCED(MYSQLPP_LIBRARIES MYSQLPP_INCLUDE_DIR)

include_directories(${MYSQLPP_INCLUDE_DIR})

# enable threading support in C++
find_package(Threads)

set(CMAKE_MODULE_PATH "/usr/local/share/SFML/cmake/Modules" ${CMAKE_MODULE_PATH})
find_package(SFML 2 REQUIRED Network)

add_executable(citizend ${all_source})

# link everything together
target_link_libraries(citizend 
    ${CMAKE_THREAD_LIBS_INIT} 
    ${MYSQLPP_LIBRARIES}
    ${SFML_LIBRARIES}
    ${CRYPTOPP_LIBRARIES}
    )

# add a target to generate API documentation with Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
add_custom_target(doc
${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
COMMENT "Generating API documentation with Doxygen" VERBATIM
)
endif(DOXYGEN_FOUND)

# runs ./citizend -c test.conf -b test_suite.txt
add_test(test test.sh)
