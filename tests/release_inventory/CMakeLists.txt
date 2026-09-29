# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Included in the owner's directory; this also supports deferred attachment.
add_executable(umicom-release-inventory-tests "${CMAKE_CURRENT_LIST_DIR}/test_inventory.c")
target_link_libraries(umicom-release-inventory-tests PRIVATE Umicom::distribution)
set_target_properties(umicom-release-inventory-tests PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom-release-inventory-tests)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom-release-inventory-tests)
endif()
set(_inventory_tests "")
foreach(_case IN ITEMS valid malformed duplicate names context states large bounds)
    add_test(NAME "framework.release_inventory.${_case}" COMMAND umicom-release-inventory-tests "${_case}")
    list(APPEND _inventory_tests "framework.release_inventory.${_case}")
endforeach()
add_test(NAME framework.release_inventory.cli_help COMMAND umicom-release-inventory --help)
list(APPEND _inventory_tests framework.release_inventory.cli_help)
add_test(NAME framework.release_inventory.cli_files COMMAND "${CMAKE_COMMAND}"
    "-DTOOL=$<TARGET_FILE:umicom-release-inventory>"
    "-DDEST=${CMAKE_CURRENT_BINARY_DIR}/inventory-cli-files"
    -P "${CMAKE_CURRENT_LIST_DIR}/check_cli.cmake")
list(APPEND _inventory_tests framework.release_inventory.cli_files)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_executable(umicom-release-inventory-allocation "${CMAKE_CURRENT_LIST_DIR}/test_allocation.c")
    target_link_libraries(umicom-release-inventory-allocation PRIVATE Umicom::distribution)
    target_link_options(umicom-release-inventory-allocation PRIVATE -Wl,--wrap=calloc -Wl,--wrap=malloc)
    set_target_properties(umicom-release-inventory-allocation PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-release-inventory-allocation)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-release-inventory-allocation)
    endif()
    add_test(NAME framework.release_inventory.allocation COMMAND umicom-release-inventory-allocation)
    list(APPEND _inventory_tests framework.release_inventory.allocation)
endif()
set_tests_properties(${_inventory_tests} PROPERTIES TIMEOUT 60 LABELS "framework;release-inventory;FW-01")
if(UMICOM_RELEASE_INVENTORY_COMPOSITION_TESTS)
    foreach(_mode IN ITEMS OFF ON)
        add_test(NAME "framework.release_inventory.composition.${_mode}" COMMAND "${CMAKE_COMMAND}"
            "-DROOT=${CMAKE_CURRENT_LIST_DIR}/../.."
            "-DDEST=${CMAKE_CURRENT_BINARY_DIR}/inventory-composition-${_mode}"
            "-DCOMPILER=${CMAKE_C_COMPILER}" "-DDEFER=${_mode}"
            -P "${CMAKE_CURRENT_LIST_DIR}/check_capture.cmake")
        set_tests_properties("framework.release_inventory.composition.${_mode}" PROPERTIES TIMEOUT 180 LABELS "framework;release-inventory;FW-01")
    endforeach()
endif()
