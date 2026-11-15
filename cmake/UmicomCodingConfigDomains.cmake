# Umicom Framework
# File: cmake/UmicomCodingConfigDomains.cmake
#
# PURPOSE:
# Register native configuration domain and typed archive admission checks.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

if(BUILD_TESTING AND TARGET Umicom::developer)
    add_executable(umicom-ai-coding-config-domains-test tests/ai_coding_runtime/test_config_domains.c)
    target_link_libraries(umicom-ai-coding-config-domains-test PRIVATE Umicom::developer)
    set_target_properties(umicom-ai-coding-config-domains-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-ai-coding-config-domains-test)
    umicom_apply_sanitizers(umicom-ai-coding-config-domains-test)
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-ai-coding-config-domains-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../src/base/value_archive_internal.h")
    endif()
    foreach(_case IN ITEMS local remote author test zero negative-zero maximum flags provider-zero provider-negative provider-large nan positive-infinity negative-infinity below-range above-range)
        set(_name "framework.ai.coding-config-domain.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-ai-coding-config-domains-test "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;config;archive;admission")
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-config-domains-test)
    endforeach()
endif()
