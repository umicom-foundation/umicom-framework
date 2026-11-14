# -----------------------------------------------------------------------------
# Umicom Framework
# File: tests/pe_imports/CMakeLists.txt
# PURPOSE: Register individual executable byte-layout regression fixtures.
# EDUCATIONAL OVERVIEW: Each fixture declares its expected acceptance and import
# names independently of the parser. CTest reports every corpus member by name.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
# -----------------------------------------------------------------------------
add_executable(umicom-pe-import-test test_pe_imports.c)
target_link_libraries(umicom-pe-import-test PRIVATE Umicom::pe_imports)
set_target_properties(umicom-pe-import-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(umicom-pe-import-test PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
endif()
file(GLOB _umi_pe_cases CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/cases/*.case")
foreach(_case IN LISTS _umi_pe_cases)
    get_filename_component(_name "${_case}" NAME_WE)
    add_test(NAME "framework.pe_imports.${_name}" COMMAND umicom-pe-import-test "${_case}")
endforeach()
add_test(NAME framework.pe_imports.argument_contract COMMAND umicom-pe-import-test --arguments)
# Link the two private helper namespaces as independent translation units.
add_executable(umicom-setup-helper-symbols-test
    ../setup_centre_symbols/component.c ../setup_centre_symbols/foreign.c ../setup_centre_symbols/main.c)
set_target_properties(umicom-setup-helper-symbols-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(umicom-setup-helper-symbols-test PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
endif()
add_test(NAME framework.setup_centre.private_symbols COMMAND umicom-setup-helper-symbols-test)
