#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomSseParser.cmake
#
# PURPOSE:
# Integrate bounded native SSE receiving and its independent protocol regressions.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

include_guard(GLOBAL)
target_sources(umicom_web PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../src/web/sse_parser.c")
if(BUILD_TESTING)
    function(umicom_sse_parser_target target source)
        add_executable(${target} "${CMAKE_CURRENT_LIST_DIR}/../${source}")
        target_link_libraries(${target} PRIVATE Umicom::web)
        target_compile_features(${target} PRIVATE c_std_23)
        umicom_apply_warnings(${target})
        umicom_apply_sanitizers(${target})
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${target})
        endif()
    endfunction()
    function(umicom_sse_parser_case name target)
        add_test(NAME "${name}" COMMAND ${target} ${ARGN})
        set_tests_properties("${name}" PROPERTIES TIMEOUT 60 LABELS "framework;web;sse;protocol;ownership")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("${name}" TARGETS ${target})
        endif()
    endfunction()
    umicom_sse_parser_target(umicom-sse-parser-test tests/web/test_sse_parser.c)
    foreach(_case IN ITEMS basic empty-data multiline trailing-data comments unknown case-sensitive no-colon one-space tab-value event-name empty-name metadata-block id-persistence id-reset retry retry-invalid retry-overflow retry-maximum cr crlf mixed-newlines leading-bom later-bom utf8 unfinished-line unfinished-event metadata-only coalesced)
        umicom_sse_parser_case("framework.web.sse_parser.${_case}" umicom-sse-parser-test "${_case}")
    endforeach()
    umicom_sse_parser_target(umicom-sse-parser-boundary-test tests/web/test_sse_parser_boundaries.c)
    foreach(_case IN ITEMS arguments line-limit data-limit event-limit id-limit input-limit exact-limits invalid-leading invalid-continuation overlong surrogate out-of-range nul-data nul-comment eof-utf8 empty-feed sticky-failure)
        umicom_sse_parser_case("framework.web.sse_boundary.${_case}" umicom-sse-parser-boundary-test "${_case}")
    endforeach()
    umicom_sse_parser_target(umicom-sse-roundtrip-test tests/web/test_sse_roundtrip.c)
    foreach(_case IN ITEMS roundtrip successor)
        umicom_sse_parser_case("framework.web.sse_integration.${_case}" umicom-sse-roundtrip-test "${_case}")
    endforeach()
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang" AND NOT APPLE AND NOT MSVC)
        umicom_sse_parser_target(umicom-sse-parser-fault-test tests/web/test_sse_parser_faults.c)
        target_link_options(umicom-sse-parser-fault-test PRIVATE
            -Wl,--wrap=calloc -Wl,--wrap=malloc -Wl,--wrap=free)
        foreach(_case IN ITEMS owner message)
            umicom_sse_parser_case("framework.web.sse_fault.${_case}" umicom-sse-parser-fault-test "${_case}")
        endforeach()
    endif()
endif()
