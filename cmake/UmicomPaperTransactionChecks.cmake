#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomPaperTransactionChecks.cmake
#
# PURPOSE:
#   Register paper state, session and audit admission regression coverage and the installed SDK exercise.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
if(BUILD_TESTING)
    foreach(_fixture audit_admission session_admission paper_transaction)
        add_executable(umicom-broker-${_fixture}-test
            "${CMAKE_CURRENT_LIST_DIR}/../tests/broker_connectivity/test_${_fixture}.c")
        target_link_libraries(umicom-broker-${_fixture}-test PRIVATE Umicom::trading)
        set_target_properties(umicom-broker-${_fixture}-test PROPERTIES
            C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(umicom-broker-${_fixture}-test)
        umicom_apply_sanitizers(umicom-broker-${_fixture}-test)
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-broker-${_fixture}-test)
        endif()
    endforeach()
    foreach(_case append gap last-sequence sequence-exhausted sequence-zero sequence-repeat
        text-eventId text-causationId text-source text-kind text-entityId text-detail empty-kind
        negative-time bad-status bad-persisted count-over count-read count-mark revision-zero
        revision-max gap-corrupt retained-text retained-sequence full mark null-owner null-record)
        add_test(NAME framework.broker.audit_admission.${_case}
            COMMAND umicom-broker-audit_admission-test "${_case}")
        set_tests_properties(framework.broker.audit_admission.${_case}
            PROPERTIES TIMEOUT 30 LABELS "framework;broker;audit;regression")
    endforeach()
    foreach(_operation connect authenticate heartbeat disconnect check)
        foreach(_case normal revision invalid-state zero-revision)
            add_test(NAME framework.broker.session_admission.${_operation}-${_case}
                COMMAND umicom-broker-session_admission-test "${_operation}" "${_case}")
            set_tests_properties(framework.broker.session_admission.${_operation}-${_case}
                PROPERTIES TIMEOUT 30 LABELS "framework;broker;session;regression")
        endforeach()
        if(NOT _operation STREQUAL "disconnect")
            add_test(NAME framework.broker.session_admission.${_operation}-clock
                COMMAND umicom-broker-session_admission-test "${_operation}" clock)
            set_tests_properties(framework.broker.session_admission.${_operation}-clock
                PROPERTIES TIMEOUT 30 LABELS "framework;broker;session;regression")
        endif()
    endforeach()
    foreach(_pair disconnect:attempts check:repeated-timeout check:healthy-at-limit check:zero-timeout
                  check:disconnected heartbeat:recover)
        string(REPLACE ":" ";" _parts "${_pair}")
        list(GET _parts 0 _operation)
        list(GET _parts 1 _case)
        add_test(NAME framework.broker.session_admission.${_operation}-${_case}
            COMMAND umicom-broker-session_admission-test "${_operation}" "${_case}")
        set_tests_properties(framework.broker.session_admission.${_operation}-${_case}
            PROPERTIES TIMEOUT 30 LABELS "framework;broker;session;regression")
    endforeach()
    foreach(_operation connect submit cancel)
        foreach(_case normal audit-full audit-revision audit-sequence audit-zero audit-count audit-gap
                      time-range clock callback copied-owner order-count storage-count)
            add_test(NAME framework.broker.paper_transaction.${_operation}-${_case}
                COMMAND umicom-broker-paper_transaction-test "${_operation}" "${_case}")
            set_tests_properties(framework.broker.paper_transaction.${_operation}-${_case}
                PROPERTIES TIMEOUT 30 LABELS "framework;broker;paper;transaction;regression")
        endforeach()
    endforeach()
    foreach(_pair connect:session-last submit:order-revision submit:invalid-request submit:live
                  submit:simulation submit:duplicate cancel:order-revision cancel:invalid-id cancel:unknown
                  cancel:record-sequence cancel:record-status cancel:record-request cancel:repeat)
        string(REPLACE ":" ";" _parts "${_pair}")
        list(GET _parts 0 _operation)
        list(GET _parts 1 _case)
        add_test(NAME framework.broker.paper_transaction.${_operation}-${_case}
            COMMAND umicom-broker-paper_transaction-test "${_operation}" "${_case}")
        set_tests_properties(framework.broker.paper_transaction.${_operation}-${_case}
            PROPERTIES TIMEOUT 30 LABELS "framework;broker;paper;transaction;regression")
    endforeach()
endif()
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/paper-trading-audit.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/docs/learning" COMPONENT Framework)
install(FILES
    "${CMAKE_CURRENT_LIST_DIR}/../examples/paper_audit/main.c"
    "${CMAKE_CURRENT_LIST_DIR}/../examples/paper_audit/CMakeLists.txt"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/examples/paper_audit" COMPONENT Learning)
if(BUILD_TESTING)
    add_executable(umicom-paper-audit-example "${CMAKE_CURRENT_LIST_DIR}/../examples/paper_audit/main.c")
    target_link_libraries(umicom-paper-audit-example PRIVATE Umicom::trading)
    set_target_properties(umicom-paper-audit-example PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-paper-audit-example)
    umicom_apply_sanitizers(umicom-paper-audit-example)
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomApplicationBranding.cmake")
    if(WIN32)
        enable_language(RC)
    endif()
    umicom_apply_application_branding(TARGET umicom-paper-audit-example
        PRODUCT_NAME "Umicom Paper Audit Example" INTERNAL_NAME "umicom-paper-audit-example"
        RESOURCE_ROOT "${CMAKE_CURRENT_LIST_DIR}/../resources")
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-paper-audit-example)
    endif()
    foreach(_case self-check duplicate malformed)
        add_test(NAME framework.broker.paper_audit-example.${_case}
            COMMAND umicom-paper-audit-example "${_case}")
        set_tests_properties(framework.broker.paper_audit-example.${_case}
            PROPERTIES TIMEOUT 30 LABELS "framework;broker;paper;learning;no-network;no-filesystem")
    endforeach()
endif()

if(BUILD_TESTING)
    add_executable(umicom-paper-transaction-failure-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/broker_connectivity/test_paper_transaction_failure.c")
    target_link_libraries(umicom-paper-transaction-failure-test PRIVATE Umicom::trading)
    set_target_properties(umicom-paper-transaction-failure-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-paper-transaction-failure-test)
    umicom_apply_sanitizers(umicom-paper-transaction-failure-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-paper-transaction-failure-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-paper-transaction-failure-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../src/broker_connectivity/paper_runtime.c")
    endif()
    foreach(_operation connect submit cancel)
        foreach(_failure allocation audit)
            add_test(NAME framework.broker.paper_transaction-failure.${_operation}-${_failure}
                COMMAND umicom-paper-transaction-failure-test "${_operation}" "${_failure}")
            set_tests_properties(framework.broker.paper_transaction-failure.${_operation}-${_failure}
                PROPERTIES TIMEOUT 30 LABELS "framework;broker;paper;transaction;fault-injection;regression")
        endforeach()
    endforeach()
endif()
