cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(zenoh_pico_publisher)

include(ExternalProject)
include(CMakePrintHelpers)

zephyr_get_system_include_directories_for_lang_as_string(C ext_system_includes)
zephyr_get_include_directories_for_lang_as_string(C ext_includes)
zephyr_get_compile_definitions_for_lang_as_string(C ext_defs)
zephyr_get_compile_options_for_lang_as_String(C ext_opts)
set(ZENOHPICO_CFLAGS
        "${ZENOHPICO_CFLAGS} ${ext_defs} ${ext_system_includes} ${ext_includes} ${ext_opts}"
)

set(ZENOHPICO_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../modules/lib/zenoh-pico)
set(ZENOHPICO_BIN_DIR ${CMAKE_BINARY_DIR}/zenoh-pico)
set(ZENOHPICO_INC_DIR ${ZENOHPICO_BIN_DIR}/include)
set(ZENOHPICO_LIB ${ZENOHPICO_BIN_DIR}/lib/libzenohpico.a)

ExternalProject_Add(zenoh-pico-ext
    SOURCE_DIR ${ZENOHPICO_SRC_DIR}
    BINARY_DIR ${ZENOHPICO_BIN_DIR}
    CMAKE_ARGS
        -DWITH_ZEPHYR=ON
        -DBUILD_TOOLS=OFF
        -DBUILD_TESTING=OFF
        -DBUILD_EXAMPLES=OFF
        -DCMAKE_SYSTEM_NAME=Generic
        -DCMAKE_TRY_COMPILE_TARGET_TYPE=STATIC_LIBRARY
        -DCMAKE_C_FLAGS=${ZENOHPICO_CFLAGS}
        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
        -DCMAKE_INSTALL_PREFIX=${ZENOHPICO_BIN_DIR}
        -DZENOH_DEBUG=0
        -DZ_FEATURE_PUBLICATION=1
    DEPENDS zephyr_interface
    BUILD_BYPRODUCTS ${ZENOHPICO_LIB}
)
add_library(zenohpico_lib STATIC IMPORTED GLOBAL)
add_dependencies(zenohpico_lib zenoh-pico-ext)
set_target_properties(zenohpico_lib PROPERTIES IMPORTED_LOCATION ${ZENOHPICO_LIB})
add_definitions(-DZENOH_ZEPHYR)

target_include_directories(app PRIVATE ${ZENOHPICO_INC_DIR})

target_link_libraries(app PRIVATE zenohpico_lib)

target_sources(app PRIVATE src/main.c)
