# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
include_guard(GLOBAL)
if(NOT BUILD_TESTING)
    return()
endif()

# Use the shipped CMake modules in a small consumer. This checks target
# ownership and dependency discovery; it does not substitute for a full build.
foreach(_testing IN ITEMS ON OFF)
    foreach(_native IN ITEMS ON OFF)
        foreach(_order IN ITEMS core-first language-first)
            set(_case "diagnostic.${_testing}.${_native}.${_order}")
            add_test(NAME framework.build_configuration.${_case}
                COMMAND "${CMAKE_COMMAND}"
                    "-DROOT=${CMAKE_CURRENT_LIST_DIR}/.."
                    "-DDEST=${CMAKE_CURRENT_BINARY_DIR}/configuration-${_case}"
                    "-DGENERATOR=${CMAKE_GENERATOR}"
                    "-DCOMPILER=${CMAKE_C_COMPILER}"
                    "-DTESTING=${_testing}" "-DNATIVE=${_native}"
                    "-DORDER=${_order}" "-DKIND=diagnostic"
                    -P "${CMAKE_CURRENT_LIST_DIR}/../tests/build_configuration/check.cmake")
            set_tests_properties(framework.build_configuration.${_case}
                PROPERTIES TIMEOUT 90 LABELS "framework;cmake;composition;regression")
        endforeach()
    endforeach()
endforeach()

foreach(_testing IN ITEMS ON OFF)
    foreach(_gtk IN ITEMS ON OFF)
        foreach(_glib IN ITEMS discovered existing)
            set(_case "quick_open.${_testing}.${_gtk}.${_glib}")
            add_test(NAME framework.build_configuration.${_case}
                COMMAND "${CMAKE_COMMAND}"
                    "-DROOT=${CMAKE_CURRENT_LIST_DIR}/.."
                    "-DDEST=${CMAKE_CURRENT_BINARY_DIR}/configuration-${_case}"
                    "-DGENERATOR=${CMAKE_GENERATOR}"
                    "-DCOMPILER=${CMAKE_C_COMPILER}"
                    "-DTESTING=${_testing}" "-DGTK=${_gtk}"
                    "-DGLIB=${_glib}" "-DKIND=quick-open"
                    -P "${CMAKE_CURRENT_LIST_DIR}/../tests/build_configuration/check.cmake")
            set_tests_properties(framework.build_configuration.${_case}
                PROPERTIES TIMEOUT 90 LABELS "framework;cmake;composition;regression")
        endforeach()
    endforeach()
endforeach()

# Exercise both real headless source layouts, including negative fixtures.
foreach(_surface IN ITEMS portfolio os missing undeclared alternate-declared)
    add_test(NAME framework.build_configuration.surface.${_surface}
        COMMAND "${CMAKE_COMMAND}"
            "-DROOT=${CMAKE_CURRENT_LIST_DIR}/.."
            "-DDEST=${CMAKE_CURRENT_BINARY_DIR}/surface-audit-${_surface}"
            "-DCASE=${_surface}"
            -P "${CMAKE_CURRENT_LIST_DIR}/../tests/build_configuration/test_surface_audit.cmake")
    set_tests_properties(framework.build_configuration.surface.${_surface}
        PROPERTIES TIMEOUT 30 LABELS "framework;cmake;surface;regression")
endforeach()
