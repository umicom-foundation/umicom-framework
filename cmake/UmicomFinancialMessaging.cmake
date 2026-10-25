# Umicom Framework
# File: cmake/UmicomFinancialMessaging.cmake
# PURPOSE: Attach reusable logical financial messaging to the existing finance SDK.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
if(NOT TARGET umicom_finance)
    return()
endif()
include_guard(GLOBAL)
include(GNUInstallDirs)
get_filename_component(_ufmp_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
target_sources(umicom_finance PRIVATE
    "${_ufmp_root}/src/financial_messaging/types.c"
    "${_ufmp_root}/src/financial_messaging/envelope.c"
    "${_ufmp_root}/src/financial_messaging/binding.c"
    "${_ufmp_root}/src/financial_messaging/replay.c"
    "${_ufmp_root}/src/financial_messaging/progress.c"
    "${_ufmp_root}/src/financial_messaging/simulation.c"
    "${_ufmp_root}/src/financial_messaging/clearing_preview.c"
    "${_ufmp_root}/src/financial_messaging/clearing_example.c")
# Thin hosts can detect this additive API while retaining their legacy entry points.
target_compile_definitions(umicom_finance PUBLIC UMICOM_HAS_UFMP_FOUNDATION=1 UMICOM_HAS_UFMP_CLEARING=1)
foreach(_ufmp_header IN ITEMS types envelope replay progress simulation clearing_preview)
    install(FILES "${_ufmp_root}/include/umicom/financial_messaging/${_ufmp_header}.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/financial_messaging" COMPONENT Framework)
    if(COMMAND umicom_release_inventory_own_headers)
        umicom_release_inventory_own_headers(umicom_finance
            "include/umicom/financial_messaging/${_ufmp_header}.h")
    endif()
endforeach()
if(COMMAND umicom_target_qualification_inputs)
    umicom_target_qualification_inputs(umicom_finance FILES
        "${_ufmp_root}/src/financial_messaging/internal.h")
endif()
install(FILES "${_ufmp_root}/docs/guides/financial-messaging-simulation.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom-framework/docs/guides" COMPONENT Framework)
if(BUILD_TESTING)
    set(_ufmp_envelope_cases valid revision expired expiry-boundary future skew-boundary lifetime overflow-time payload-limit missing-key sender unterminated unknown-type schema body tampered empty-body null policy optional-fields)
    set(_ufmp_replay_cases identical new-delivery changed-payload changed-recipient changed-type changed-schema changed-format changed-revision changed-sender new-operation message-conflict time-retry conversation-retry corrupt-record invalid-candidate output-preserved padding domain-separation)
    set(_ufmp_progress_cases created delivery acceptance settlement prefunded-credit complete wrong-issuer wrong-payment stale no-acceptance rejected late-rejection duplicate contradiction revision-overflow corrupt-state invalid-evidence null)
    set(_ufmp_catalogue_cases roundtrip unknown bounded-name identifier requirements)
    set(_ufmp_simulation_cases scenario format short arguments)
    set(_ufmp_clearing_cases balanced pay receive empty duplicate currency scale account scheme day owner counterparty negative zero overflow extremes capacity malformed format amount-currency)
    set(_ufmp_legacy_netting_cases normal pay-max receive-max negative-pay negative-receive malformed-id null zero)
    set(_ufmp_binding_vector_cases envelope operation)
    foreach(_ufmp_group IN ITEMS envelope replay progress catalogue simulation clearing legacy_netting binding_vector)
        set(_ufmp_target "umicom-ufmp-${_ufmp_group}-test")
        add_executable(${_ufmp_target} "${_ufmp_root}/tests/financial_messaging/test_${_ufmp_group}.c")
        target_link_libraries(${_ufmp_target} PRIVATE Umicom::finance)
        set_target_properties(${_ufmp_target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(${_ufmp_target})
        umicom_apply_sanitizers(${_ufmp_target})
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${_ufmp_target})
        endif()
        if(COMMAND umicom_target_qualification_inputs)
            umicom_target_qualification_inputs(${_ufmp_target} FILES
                "${_ufmp_root}/tests/financial_messaging/fixture.h")
        endif()
        foreach(_ufmp_case IN LISTS _ufmp_${_ufmp_group}_cases)
            set(_ufmp_test "framework.ufmp.${_ufmp_group}.${_ufmp_case}")
            add_test(NAME ${_ufmp_test} COMMAND ${_ufmp_target} ${_ufmp_case})
            set_tests_properties(${_ufmp_test} PROPERTIES TIMEOUT 30
                LABELS "framework;finance;ufmp;synthetic;regression")
            if(COMMAND umicom_test_impact_owners)
                umicom_test_impact_owners(${_ufmp_test} TARGETS ${_ufmp_target})
            endif()
        endforeach()
    endforeach()
endif()
foreach(_ufmp_variable IN ITEMS root header group target case test envelope_cases replay_cases
        progress_cases catalogue_cases simulation_cases clearing_cases legacy_netting_cases binding_vector_cases)
    unset(_ufmp_${_ufmp_variable})
endforeach()
unset(_ufmp_variable)
