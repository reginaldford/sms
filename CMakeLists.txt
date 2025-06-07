cmake_minimum_required(VERSION 3.16)
project(sms C)

# Threads
set(THREADS 8)

# Compiler options
set(CMAKE_C_COMPILER clang)
set(CMAKE_C_FLAGS_RELEASE "-O3 -I/usr/local/include")
set(CMAKE_C_FLAGS_DEBUG "-g -I/usr/local/include")
set(CMAKE_C_FLAGS_PROFILING "-fprofile-instr-generate -fcoverage-mapping")
set(CMAKE_EXE_LINKER_FLAGS "-lm -flto -lffi -L/usr/local/lib/")

# Directories (always use source dir as base)
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(SRC_MAIN ${SRC_DIR}/main)
set(SRC_BISON_FLEX ${SRC_DIR}/bison_flex)
set(SRC_TEST ${SRC_DIR}/test)
set(SRC_KERN_TEST ${SRC_DIR}/kernel_test)
set(GEN_PARSER_DIR ${CMAKE_BINARY_DIR}/gen)
file(MAKE_DIRECTORY ${GEN_PARSER_DIR})
set(BUILD_DIR ${CMAKE_BINARY_DIR}/build)
set(BIN_DIR ${CMAKE_BINARY_DIR}/bin)
file(MAKE_DIRECTORY ${BIN_DIR})

# Include paths
include_directories(/usr/local/include)
include_directories(${SRC_MAIN}/parser)
include_directories(${SRC_DIR})

# Sources
file(GLOB_RECURSE SRCS_MAIN CONFIGURE_DEPENDS ${SRC_MAIN}/*.c)
list(FILTER SRCS_MAIN EXCLUDE REGEX ".*/sm_main\\.c$")
list(FILTER SRCS_MAIN EXCLUDE REGEX ".*/example\\.c$")

file(GLOB_RECURSE SRCS_TEST CONFIGURE_DEPENDS ${SRC_TEST}/*.c)
list(FILTER SRCS_TEST EXCLUDE REGEX ".*/sm_test\\.c$")

file(GLOB_RECURSE SRCS_KT CONFIGURE_DEPENDS ${SRC_KERN_TEST}/*.c)
list(FILTER SRCS_KT EXCLUDE REGEX ".*/kernel_test\\.c$")

set(SM_MAIN ${SRC_MAIN}/sm_main.c)
set(SM_TEST_MAIN ${SRC_TEST}/sm_test.c)
set(SM_KERNEL_MAIN ${SRC_KERN_TEST}/kernel_test.c)
set(BISON_SRC ${GEN_PARSER_DIR}/y.tab.c)
set(FLEX_SRC ${GEN_PARSER_DIR}/lex.yy.c)

# Helper to create object libraries
function(make_obj_target name sources flags)
    add_library(${name} OBJECT ${sources})
    set_target_properties(${name} PROPERTIES COMPILE_FLAGS "${flags}")
endfunction()

# Bison/Flex targets
add_custom_command(OUTPUT ${BISON_SRC}
    COMMAND bison -d ${SRC_MAIN}/parser/sms.y -o ${BISON_SRC}
    DEPENDS ${SRC_MAIN}/parser/sms.y)

add_custom_command(OUTPUT ${FLEX_SRC}
    COMMAND flex --yylineno -o ${FLEX_SRC} ${SRC_MAIN}/parser/sms.l
    DEPENDS ${SRC_MAIN}/parser/sms.l ${BISON_SRC})

add_custom_target(parser_gen DEPENDS ${BISON_SRC} ${FLEX_SRC})

# Main binary
make_obj_target(parser_rel "${BISON_SRC};${FLEX_SRC}" "${CMAKE_C_FLAGS_RELEASE}")
make_obj_target(main_rel "${SRCS_MAIN}" "${CMAKE_C_FLAGS_RELEASE}")
make_obj_target(entry_rel "${SM_MAIN}" "${CMAKE_C_FLAGS_RELEASE}")

add_executable(sms $<TARGET_OBJECTS:parser_rel> $<TARGET_OBJECTS:main_rel> $<TARGET_OBJECTS:entry_rel>)
target_link_libraries(sms PRIVATE m ffi)
add_dependencies(sms parser_gen)

# Debug binary
make_obj_target(parser_dbg "${BISON_SRC};${FLEX_SRC}" "${CMAKE_C_FLAGS_DEBUG}")
make_obj_target(main_dbg "${SRCS_MAIN}" "${CMAKE_C_FLAGS_DEBUG}")
make_obj_target(entry_dbg "${SM_MAIN}" "${CMAKE_C_FLAGS_DEBUG}")
add_executable(sms_dbg $<TARGET_OBJECTS:parser_dbg> $<TARGET_OBJECTS:main_dbg> $<TARGET_OBJECTS:entry_dbg>)
target_link_libraries(sms_dbg PRIVATE m ffi)
add_dependencies(sms_dbg parser_gen)

# Test binary
make_obj_target(test_dbg "${SRCS_TEST}" "${CMAKE_C_FLAGS_DEBUG}")
make_obj_target(entry_test_dbg "${SM_TEST_MAIN}" "${CMAKE_C_FLAGS_DEBUG}")
add_executable(sms_test $<TARGET_OBJECTS:parser_dbg> $<TARGET_OBJECTS:main_dbg> $<TARGET_OBJECTS:test_dbg> $<TARGET_OBJECTS:entry_test_dbg>)
target_link_libraries(sms_test PRIVATE m ffi)
add_dependencies(sms_test parser_gen)

# Kernel test binary
make_obj_target(kt_dbg "${SRCS_KT}" "${CMAKE_C_FLAGS_DEBUG}")
make_obj_target(entry_kt_dbg "${SM_KERNEL_MAIN}" "${CMAKE_C_FLAGS_DEBUG}")
add_executable(sms_kernel_test $<TARGET_OBJECTS:parser_rel> $<TARGET_OBJECTS:main_dbg> $<TARGET_OBJECTS:kt_dbg> $<TARGET_OBJECTS:entry_kt_dbg>)
target_link_libraries(sms_kernel_test PRIVATE m ffi)
add_dependencies(sms_kernel_test parser_gen)

# Profiling binary
make_obj_target(parser_prof "${BISON_SRC};${FLEX_SRC}" "${CMAKE_C_FLAGS_PROFILING}")
make_obj_target(main_prof "${SRCS_MAIN}" "${CMAKE_C_FLAGS_PROFILING}")
make_obj_target(entry_prof "${SM_MAIN}" "${CMAKE_C_FLAGS_PROFILING}")
add_executable(sms_prof $<TARGET_OBJECTS:parser_prof> $<TARGET_OBJECTS:main_prof> $<TARGET_OBJECTS:entry_prof>)
target_link_libraries(sms_prof PRIVATE m ffi)
add_dependencies(sms_prof parser_gen)

# Unified binary
add_executable(sms_unified ${BISON_SRC} ${FLEX_SRC} ${SRCS_MAIN} ${SM_MAIN})
set_target_properties(sms_unified PROPERTIES COMPILE_FLAGS "-Oz")
target_link_libraries(sms_unified PRIVATE m ffi)
add_dependencies(sms_unified parser_gen)

# Docs
find_package(Doxygen)
if(DOXYGEN_FOUND)
    add_custom_target(docs
        COMMAND doxygen ${CMAKE_SOURCE_DIR}/docs/docs.conf
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM)
endif()

# Install
install(TARGETS sms RUNTIME DESTINATION bin)
install(TARGETS sms_unified RUNTIME DESTINATION bin)

# Test target
add_custom_target(check
    COMMAND sms_kernel_test && sms_test ${CMAKE_SOURCE_DIR}/test_zone/outline.sms
    COMMENT "Running Kernel and SMS Tests")

