# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
cmake_minimum_required(VERSION 3.24)
project(UmicomInventoryComposition LANGUAGES C)
include(CTest)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
add_subdirectory("${ROOT}/tools/release_baseline" baseline)

# A utility target models large generated sources/flags without passing a huge
# command line to the compiler. Inventory must retain the exact property bytes.
add_custom_target(inventory-fixture-large)
string(REPEAT "release-metadata" 18000 _large)
set_property(TARGET inventory-fixture-large PROPERTY COMPILE_OPTIONS "${_large}")

add_custom_target(inventory-fixture-aggregate)
string(REPEAT "input-" 17 _leaf_prefix)
foreach(_number RANGE 1 1350)
    set(_leaf "inventory-${_leaf_prefix}${_number}")
    add_custom_target("${_leaf}")
    add_dependencies(inventory-fixture-aggregate "${_leaf}")
endforeach()

# Register after collection was requested, including another deferred callback.
function(register_final_test)
    add_test(NAME "inventory fixture [space]" COMMAND "${CMAKE_COMMAND}" -E true)
endfunction()
function(register_late_tests)
    add_test(NAME inventory.fixture.late COMMAND "${CMAKE_COMMAND}" -E true)
    cmake_language(DEFER CALL register_final_test)
endfunction()
cmake_language(DEFER CALL register_late_tests)
