cmake_minimum_required(VERSION 3.14)

project(SugarPPTests LANGUAGES CXX)

# ---- Add root project ----

# Enable warnings from includes
set(SugarPP_INCLUDE_WITHOUT_SYSTEM ON CACHE INTERNAL "")

option(TEST_INSTALLED_VERSION "Use the library installed to the system" OFF)
if(TEST_INSTALLED_VERSION)
  find_package(SugarPP REQUIRED CONFIG)
else()
  include(FetchContent)
  FetchContent_Declare(SugarPP SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/..")
  FetchContent_MakeAvailable(SugarPP)
endif()


# ---- Test ----

enable_testing()

function(add_test)
  cmake_parse_arguments(TEST "" "NAMESPACE;NAME" "" ${ARGN})

  set(target "${TEST_NAMESPACE}_${TEST_NAME}")
  set(test_name "${TEST_NAMESPACE}.${TEST_NAME}")

  add_executable(${target} "source/${TEST_NAMESPACE}/${TEST_NAME}.cpp")

  target_link_libraries(${target} PRIVATE SugarPP::SugarPP)
  target_compile_features(${target} PRIVATE cxx_std_17)
  target_include_directories(${target} PRIVATE "${PROJECT_SOURCE_DIR}/include")

  # Call the original add_test
  _add_test(NAME ${test_name} COMMAND ${target})
endfunction()

# This just reads from stdin, can't test that
# add_test(NAMESPACE io NAME io)

add_test(NAMESPACE range NAME range)
add_test(NAMESPACE when NAME when)
add_test(NAMESPACE types NAME to_string)