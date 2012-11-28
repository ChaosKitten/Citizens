cmake_minimum_required(VERSION 2.8.6)
project(Citizens-Client)
enable_testing()

set(CMAKE_MODULE_PATH "." ${CMAKE_MODULE_PATH})
find_package(CryptoPP)

include_directories(
	"/usr/include/irrlicht"
	${PROJECT_SOURCE_DIR}/include
	../common/include
	${CRYPTOPP_INCLUDE_DIRS}
	)


file(GLOB sources src/* ../common/src/*)

add_definitions("-std=c++0x")
add_definitions("-g")
add_definitions("-Wall -Wextra")

add_executable(citizens ${sources})

find_package(Boost 1.4.1 REQUIRED system)
set(CMAKE_MODULE_PATH "/usr/local/share/SFML/cmake/Modules" ${CMAKE_MODULE_PATH})
find_package(SFML 2 REQUIRED network system)


target_link_libraries(citizens 
	${Boost_SYSTEM_LIBRARY}
	"/usr/lib/libIrrlicht.so"
	${SFML_LIBRARIES}
	${CRYPTOPP_LIBRARIES}
	)

