#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomBankOperations.cmake
# PURPOSE: Compose an optional, reusable C23 banking operations capability.
# AUTHOR: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
include(GNUInstallDirs)
set(_umicom_bank_operations_root "${CMAKE_CURRENT_LIST_DIR}/..")
foreach(dependency IN ITEMS Umicom::finance Umicom::data)
    if(NOT TARGET ${dependency})
        message(FATAL_ERROR "Umicom banking operations require ${dependency}")
    endif()
endforeach()

# Applications select this capability; all implementation and dependency policy
# remain Framework-owned. No second copy of finance or Data Server is compiled.
add_library(umicom_bank_operations STATIC
    "${_umicom_bank_operations_root}/src/bank_operations/runtime.c"
    "${_umicom_bank_operations_root}/src/bank_operations/validation.c"
    "${_umicom_bank_operations_root}/src/bank_operations/codec.c"
    "${_umicom_bank_operations_root}/src/bank_operations/repository.c"
    "${_umicom_bank_operations_root}/src/bank_operations/ledger.c"
    "${_umicom_bank_operations_root}/src/bank_operations/charges.c"
    "${_umicom_bank_operations_root}/src/bank_operations/accounts.c"
    "${_umicom_bank_operations_root}/src/bank_operations/transfers.c"
    "${_umicom_bank_operations_root}/src/bank_operations/cards.c")
add_library(Umicom::bank_operations ALIAS umicom_bank_operations)
set_target_properties(umicom_bank_operations PROPERTIES EXPORT_NAME bank_operations
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_include_directories(umicom_bank_operations PUBLIC
    $<BUILD_INTERFACE:${_umicom_bank_operations_root}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(umicom_bank_operations PUBLIC Umicom::finance Umicom::data)
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom_bank_operations)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom_bank_operations)
endif()
install(TARGETS umicom_bank_operations EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Framework)
install(DIRECTORY "${_umicom_bank_operations_root}/include/umicom/bank_operations"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom" COMPONENT Framework)

# GTK remains an adapter. Headless applications do not acquire a GTK dependency.
if(TARGET Umicom::ui_gtk4)
    add_library(umicom_bank_operations_gtk4 STATIC
        "${_umicom_bank_operations_root}/adapters/gtk4/bank_operations_gtk4.c")
    add_library(Umicom::bank_operations_gtk4 ALIAS umicom_bank_operations_gtk4)
    set_target_properties(umicom_bank_operations_gtk4 PROPERTIES EXPORT_NAME bank_operations_gtk4
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    target_link_libraries(umicom_bank_operations_gtk4 PUBLIC Umicom::bank_operations Umicom::ui_gtk4)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom_bank_operations_gtk4)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom_bank_operations_gtk4)
    endif()
    install(TARGETS umicom_bank_operations_gtk4 EXPORT UmicomFrameworkTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Framework)
    install(FILES "${_umicom_bank_operations_root}/include/umicom/ui/gtk4/bank_operations.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/ui/gtk4" COMPONENT Framework)
endif()

if(BUILD_TESTING)
    add_executable(umicom-bank-operations-test
        "${_umicom_bank_operations_root}/tests/bank_operations/test_operations.c")
    target_link_libraries(umicom-bank-operations-test PRIVATE Umicom::bank_operations)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-bank-operations-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-bank-operations-test)
    endif()
    if(TARGET Umicom::bank_operations_gtk4)
        add_executable(umicom-bank-operations-gtk-test
            "${_umicom_bank_operations_root}/tests/bank_operations/test_gtk_launcher.c")
        target_link_libraries(umicom-bank-operations-gtk-test PRIVATE Umicom::bank_operations_gtk4)
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings(umicom-bank-operations-gtk-test)
        endif()
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers(umicom-bank-operations-gtk-test)
        endif()
        add_test(NAME framework.bank_operations.gtk_launcher COMMAND umicom-bank-operations-gtk-test)
        set_tests_properties(framework.bank_operations.gtk_launcher PROPERTIES TIMEOUT 30
            SKIP_RETURN_CODE 77 LABELS "framework;bank;bank-operations;gtk4;regression")
    endif()
    include("${_umicom_bank_operations_root}/tests/bank_operations/cases.cmake")
    foreach(case IN LISTS UMICOM_BANK_OPERATIONS_TEST_CASES)
        add_test(NAME framework.bank_operations.${case}
            COMMAND umicom-bank-operations-test "${case}" "${CMAKE_CURRENT_BINARY_DIR}/bank-operations-${case}.sqlite")
        set_tests_properties(framework.bank_operations.${case} PROPERTIES
            TIMEOUT 40 SKIP_RETURN_CODE 77 LABELS "framework;bank;bank-operations;regression")
    endforeach()
endif()
# Install the public acceptance and merge guide beside the Framework capability.
install(FILES "${_umicom_bank_operations_root}/docs/BANK_OPERATIONS.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/docs" COMPONENT Framework)
unset(_umicom_bank_operations_root)

# Money presentation is reusable Framework behaviour, not GTK-only arithmetic.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomMoneyText.cmake")
target_link_libraries(umicom_bank_operations PUBLIC Umicom::money_text)

# Reviewed forms and commands use the same banking transition and repository.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomBankReview.cmake")

# Interest uses the existing portable banking arithmetic and the same event log.
target_sources(umicom_bank_operations PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../src/bank_operations/interest.c")
# The calculator already belongs to Umicom::finance, linked above. No second
# finance library or duplicate source compilation is needed.

include("${CMAKE_CURRENT_LIST_DIR}/UmicomBankInterest.cmake")

target_sources(umicom_bank_operations PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../src/bank_operations/statement_text.c")

install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/practice-interest.md"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/learning COMPONENT Learning)

# Statement exports share the base CSV owner and the canonical bank ledger.
target_sources(umicom_bank_operations PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../src/bank_operations/statement_csv.c")

if(BUILD_TESTING)
    add_executable(umicom-report-export-statement-test "${CMAKE_CURRENT_LIST_DIR}/../tests/report_export/test_statement_csv.c")
    target_link_libraries(umicom-report-export-statement-test PRIVATE Umicom::bank_operations)
    umicom_apply_warnings(umicom-report-export-statement-test)
    umicom_apply_sanitizers(umicom-report-export-statement-test)
    foreach(case IN ITEMS range empty reversal invalid ownership)
        add_test(NAME "framework.report_export.statement.${case}" COMMAND umicom-report-export-statement-test ${case})
        set_tests_properties("framework.report_export.statement.${case}" PROPERTIES TIMEOUT 30 LABELS "framework;bank;csv;regression")
    endforeach()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-report-export-statement-test)
    endif()
endif()

# Reviewed account charges share the already configured bank and native targets.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomBankCharges.cmake")


if(BUILD_TESTING AND TARGET Umicom::bank_operations_gtk4)
    add_executable(umicom-payment-quote-native-test "${CMAKE_CURRENT_LIST_DIR}/../tests/payment_quotes/test_native.c")
    # The test includes the real banking adapter once, and links the shared calculator.
    target_link_libraries(umicom-payment-quote-native-test PRIVATE Umicom::bank_operations Umicom::ui_gtk4)
    umicom_bank_review_target(umicom-payment-quote-native-test)
    foreach(case IN ITEMS calculate edit rounding invalid overflow detach owner-close retained-button)
        add_test(NAME framework.payment_quotes.native.${case} COMMAND umicom-payment-quote-native-test ${case})
        set_tests_properties(framework.payment_quotes.native.${case} PROPERTIES TIMEOUT 45 SKIP_RETURN_CODE 77 LABELS "finance;payments;quote;gtk4;regression")
    endforeach()
endif()

# Retained open requests share reviewed execution and exact report ownership.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomBankWorkQueue.cmake")

# Investigation changes dispositions while retaining original comparisons.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomBankReconciliation.cmake")
