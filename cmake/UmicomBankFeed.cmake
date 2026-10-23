# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# This module also supports independent composition; own its SDK registration dependency.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomSdkComponent.cmake")
include_guard(GLOBAL)
include(GNUInstallDirs)
add_library(umicom_bank_feed STATIC
    "${CMAKE_CURRENT_LIST_DIR}/../src/bank_feed/transactions.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/bank_feed/timestamp.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/bank_feed/decode.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/bank_feed/report.c")
add_library(Umicom::bank_feed ALIAS umicom_bank_feed)
set_target_properties(umicom_bank_feed PROPERTIES EXPORT_NAME bank_feed
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_include_directories(umicom_bank_feed PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(umicom_bank_feed PUBLIC Umicom::finance PRIVATE Umicom::developer)
umicom_apply_warnings(umicom_bank_feed)
umicom_apply_sanitizers(umicom_bank_feed)
target_link_libraries(umicom_framework INTERFACE Umicom::bank_feed)
install(TARGETS umicom_bank_feed EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Framework)
# Include this exported library surface in the opt-in SDK without changing full installs.
umicom_register_sdk_libraries(
    umicom_bank_feed
)
install(DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/bank_feed"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom COMPONENT Framework)
# Application guides belong to the shared docs/guides installation in the
# root CMakeLists.txt. Keep this former learning-folder rule disabled for
# source review; new user guides should use the shared installation instead.
if(FALSE)
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/bank-transaction-reports.html"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/learning COMPONENT Learning)
endif()
if(TARGET umicom_ui_gtk4)
    target_sources(umicom_ui_gtk4 PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/bank_feed_gtk4.c")
    target_link_libraries(umicom_ui_gtk4 PUBLIC Umicom::bank_feed)
    install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/ui/gtk4/bank_feed.h"
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/ui/gtk4 COMPONENT Framework)
endif()
if(BUILD_TESTING)
    foreach(kind transactions totals)
        add_executable(umicom-bank-feed-${kind}-test
            "${CMAKE_CURRENT_LIST_DIR}/../tests/bank_feed/test_${kind}.c")
        set_target_properties(umicom-bank-feed-${kind}-test PROPERTIES
            C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        target_link_libraries(umicom-bank-feed-${kind}-test PRIVATE Umicom::bank_feed)
        umicom_apply_warnings(umicom-bank-feed-${kind}-test)
        umicom_apply_sanitizers(umicom-bank-feed-${kind}-test)
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-bank-feed-${kind}-test)
        endif()
    endforeach()
    foreach(case negative exponent fraction integer-bound numeric-kind currency status
        direction duplicate-key escaped-key null-id bad-date bad-offset no-zone control nul
        mutability empty byte-limit wrong-array trailing row-limit duplicate-id overflow
        pending legacy mutable offset missing-id unicode formula pagination discard-extensions
        ownership unchanged values)
        add_test(NAME framework.bank_feed.${case} COMMAND umicom-bank-feed-transactions-test ${case})
        set_tests_properties(framework.bank_feed.${case} PROPERTIES
            TIMEOUT 20 LABELS "framework;banking;import;regression")
    endforeach()
    add_test(NAME framework.bank_feed.totals COMMAND umicom-bank-feed-totals-test)
    set_tests_properties(framework.bank_feed.totals PROPERTIES
        TIMEOUT 20 LABELS "framework;banking;import;regression")
endif()

if(BUILD_TESTING AND TARGET umicom_ui_gtk4)
    add_executable(umicom-bank-feed-native-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/bank_feed/test_native.c")
    set_target_properties(umicom-bank-feed-native-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    target_link_libraries(umicom-bank-feed-native-test PRIVATE Umicom::ui_gtk4)
    umicom_apply_warnings(umicom-bank-feed-native-test)
    umicom_apply_sanitizers(umicom-bank-feed-native-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-bank-feed-native-test)
    endif()
    foreach(case fields retained parent invalid import clear path-change close-loading clear-notify close-notify prepare-close prepare-path)
        add_test(NAME framework.bank_feed.native.${case} COMMAND umicom-bank-feed-native-test ${case})
        set_tests_properties(framework.bank_feed.native.${case} PROPERTIES
            TIMEOUT 20 SKIP_RETURN_CODE 77 LABELS "framework;banking;import;gtk4;regression")
    endforeach()
endif()

# Extend the existing owner so installed and in-tree consumers share these rules.
target_sources(umicom_bank_feed PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/bank_feed/statement.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/bank_feed/statement_report.c")
if(TARGET umicom_ui_gtk4)
    target_sources(umicom_ui_gtk4 PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/bank_statement_gtk4.c")
endif()
# Statement review is a shared account tool, not a product-specific calculation.
if(TARGET umicom_ui_gtk4)
    install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/ui/gtk4/bank_statement.h"
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/ui/gtk4 COMPONENT Framework)
endif()
# Application guides belong to the shared docs/guides installation in the
# root CMakeLists.txt. Keep this former learning-folder rule disabled for
# source review; new user guides should use the shared installation instead.
if(FALSE)
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/statement-reconciliation.html"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/learning COMPONENT Learning)
endif()

if(BUILD_TESTING)
    add_executable(umicom-bank-statement-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/bank_feed/test_statement.c")
    set_target_properties(umicom-bank-statement-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    target_link_libraries(umicom-bank-statement-test PRIVATE Umicom::bank_feed)
    umicom_apply_warnings(umicom-bank-statement-test)
    umicom_apply_sanitizers(umicom-bank-statement-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-bank-statement-test)
    endif()
    foreach(case arithmetic dates values mismatch empty-period range invalid-date currency not-found
        overflow difference-overflow scale negative-opening mixed-scale clone mutable missing-id formula pagination)
        add_test(NAME framework.bank_statement.${case} COMMAND umicom-bank-statement-test ${case})
        set_tests_properties(framework.bank_statement.${case} PROPERTIES
            TIMEOUT 20 LABELS "framework;banking;reconciliation;regression")
    endforeach()
endif()

if(BUILD_TESTING AND TARGET umicom_ui_gtk4)
    add_executable(umicom-bank-statement-native-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/bank_feed/test_statement_native.c")
    set_target_properties(umicom-bank-statement-native-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    target_link_libraries(umicom-bank-statement-native-test PRIVATE Umicom::ui_gtk4)
    umicom_apply_warnings(umicom-bank-statement-native-test)
    umicom_apply_sanitizers(umicom-bank-statement-native-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-bank-statement-native-test)
    endif()
    foreach(case snapshot edit invalid retained parent edit-notify close-notify)
        add_test(NAME framework.bank_statement.native.${case}
            COMMAND umicom-bank-statement-native-test ${case})
        set_tests_properties(framework.bank_statement.native.${case} PROPERTIES
            TIMEOUT 20 SKIP_RETURN_CODE 77 LABELS "framework;banking;reconciliation;gtk4;regression")
    endforeach()
endif()

# Ship invented observations so the workflow can be tried without private data.
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../examples/bank_feed/statement.json"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/examples/bank_feed COMPONENT Learning)

# A second input format shares the existing totals, review and export owners.
target_sources(umicom_bank_feed PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/bank_feed/csv_import.c")
if(BUILD_TESTING)
    add_executable(umicom-bank-feed-csv-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/bank_feed/test_csv_import.c")
    target_link_libraries(umicom-bank-feed-csv-test PRIVATE Umicom::bank_feed)
    umicom_apply_warnings(umicom-bank-feed-csv-test)
    umicom_apply_sanitizers(umicom-bank-feed-csv-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-bank-feed-csv-test)
    endif()
    foreach(case values reconcile duplicate account-scope missing-id header columns date currency
        amount precision status mutability multiline formula late-failure empty row-limit utf8 ownership invalid field-limit)
        add_test(NAME framework.bank_feed.csv.${case} COMMAND umicom-bank-feed-csv-test ${case})
        set_tests_properties(framework.bank_feed.csv.${case} PROPERTIES
            TIMEOUT 20 LABELS "framework;banking;csv;import;regression")
    endforeach()
endif()
if(BUILD_TESTING AND TARGET umicom_ui_gtk4)
    add_executable(umicom-bank-feed-csv-native-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/bank_feed/test_csv_native.c")
    target_link_libraries(umicom-bank-feed-csv-native-test PRIVATE Umicom::ui_gtk4)
    umicom_apply_warnings(umicom-bank-feed-csv-native-test)
    umicom_apply_sanitizers(umicom-bank-feed-csv-native-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-bank-feed-csv-native-test)
    endif()
    foreach(case import format-change loading-change retained invalid reconcile prepare-format)
        add_test(NAME framework.bank_feed.csv.native.${case}
            COMMAND umicom-bank-feed-csv-native-test ${case})
        set_tests_properties(framework.bank_feed.csv.native.${case} PROPERTIES
            TIMEOUT 20 SKIP_RETURN_CODE 77 LABELS "framework;banking;csv;gtk4;regression")
    endforeach()
endif()
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../examples/bank_feed/statement.csv"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/examples/bank_feed COMPONENT Learning)

# Column mappings belong to the bank-feed library so every consumer shares the
# same exact decimal, identity and publication rules.
target_sources(umicom_bank_feed PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/bank_feed/csv_mapping.c")
if(BUILD_TESTING)
    add_executable(umicom-bank-feed-csv-mapping-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/bank_feed/test_csv_mapping.c")
    set_target_properties(umicom-bank-feed-csv-mapping-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    target_link_libraries(umicom-bank-feed-csv-mapping-test PRIVATE Umicom::bank_feed)
    umicom_apply_warnings(umicom-bank-feed-csv-mapping-test)
    umicom_apply_sanitizers(umicom-bank-feed-csv-mapping-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-bank-feed-csv-mapping-test)
    endif()
    foreach(case default-schema reordered ownership pending month-first iso
        mapped-identity mapped-status split split-zero split-both split-empty
        split-negative split-precision duplicate-id late-failure duplicate-header missing-header
        record-width date amount header-text formula empty
        row-limit invalid duplicate-role missing-account conflicting-account currency
        unterminated unknown-format invalid-utf8 account-limit)
        add_test(NAME framework.bank_feed.csv.mapping.${case}
            COMMAND umicom-bank-feed-csv-mapping-test ${case})
        set_tests_properties(framework.bank_feed.csv.mapping.${case} PROPERTIES
            TIMEOUT 20 LABELS "framework;banking;csv;import;regression")
    endforeach()
endif()
if(BUILD_TESTING AND TARGET umicom_ui_gtk4)
    add_executable(umicom-bank-feed-csv-mapping-native-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/bank_feed/test_csv_mapping_native.c")
    set_target_properties(umicom-bank-feed-csv-mapping-native-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    target_link_libraries(umicom-bank-feed-csv-mapping-native-test PRIVATE Umicom::ui_gtk4)
    umicom_apply_warnings(umicom-bank-feed-csv-mapping-native-test)
    umicom_apply_sanitizers(umicom-bank-feed-csv-mapping-native-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-bank-feed-csv-mapping-native-test)
    endif()
    foreach(case import pending separate change-after-import change-loading prepare-change invalidation-change close-loading retained invalid oversized)
        add_test(NAME framework.bank_feed.csv.mapping.native.${case}
            COMMAND umicom-bank-feed-csv-mapping-native-test ${case})
        set_tests_properties(framework.bank_feed.csv.mapping.native.${case} PROPERTIES
            TIMEOUT 20 SKIP_RETURN_CODE 77 LABELS "framework;banking;csv;gtk4;regression")
    endforeach()
endif()
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../examples/bank_feed/mapped-statement.csv"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/examples/bank_feed COMPONENT Learning)

# Daily traces extend the existing bank-feed owner, so applications and installed
# SDK consumers use the same date selection and exact aggregate arithmetic.
target_sources(umicom_bank_feed PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/bank_feed/statement_timeline.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/bank_feed/statement_timeline_report.c")
if(BUILD_TESTING)
    add_executable(umicom-bank-statement-timeline-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/bank_feed/test_statement_timeline.c")
    set_target_properties(umicom-bank-statement-timeline-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    target_link_libraries(umicom-bank-statement-timeline-test PRIVATE Umicom::bank_feed)
    umicom_apply_warnings(umicom-bank-statement-timeline-test)
    umicom_apply_sanitizers(umicom-bank-statement-timeline-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-bank-statement-timeline-test)
    endif()
    foreach(case chronology pending-only empty-period negative-opening earliest-tie same-day precision
        currency-isolation ownership csv-formula pagination invalid overflow intermediate-overflow capacity bounds mismatch row-evidence)
        add_test(NAME framework.bank_statement.timeline.${case}
            COMMAND umicom-bank-statement-timeline-test ${case})
        set_tests_properties(framework.bank_statement.timeline.${case} PROPERTIES
            TIMEOUT 20 LABELS "framework;banking;reconciliation;regression")
    endforeach()
endif()
if(BUILD_TESTING AND TARGET umicom_ui_gtk4)
    add_executable(umicom-bank-statement-timeline-native-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/bank_feed/test_statement_timeline_native.c")
    set_target_properties(umicom-bank-statement-timeline-native-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    target_link_libraries(umicom-bank-statement-timeline-native-test PRIVATE Umicom::ui_gtk4)
    umicom_apply_warnings(umicom-bank-statement-timeline-native-test)
    umicom_apply_sanitizers(umicom-bank-statement-timeline-native-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-bank-statement-timeline-native-test)
    endif()
    foreach(case snapshot edit recompare invalid retained parent buffer-edit buffer-close export-edit export-close aggregate-only)
        add_test(NAME framework.bank_statement.timeline.native.${case}
            COMMAND umicom-bank-statement-timeline-native-test ${case})
        set_tests_properties(framework.bank_statement.timeline.native.${case} PROPERTIES
            TIMEOUT 20 SKIP_RETURN_CODE 77 LABELS "framework;banking;reconciliation;gtk4;regression")
    endforeach()
endif()

install(FILES "${CMAKE_CURRENT_LIST_DIR}/../examples/bank_feed/daily-statement.csv"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/examples/bank_feed COMPONENT Learning)

# Observation archives reuse the bank-feed owner and its bounded JSON services.
# The previous CSV/report interfaces remain unchanged.
target_sources(umicom_bank_feed PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/bank_feed/archive.c")
if(BUILD_TESTING)
    add_executable(umicom-bank-feed-archive-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/bank_feed/test_archive.c")
    set_target_properties(umicom-bank-feed-archive-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    target_link_libraries(umicom-bank-feed-archive-test PRIVATE Umicom::bank_feed)
    umicom_apply_warnings(umicom-bank-feed-archive-test)
    umicom_apply_sanitizers(umicom-bank-feed-archive-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-bank-feed-archive-test)
    endif()
    foreach(case json csv mapped multiline unicode precision missing-id pagination ownership empty capacity
        version metadata duplicate-key unknown-field duplicate-id late-failure date amount control invalid-utf8 truncated invalid-api
        unknown-source mapped-control row-limit)
        add_test(NAME framework.bank_feed.archive.${case} COMMAND umicom-bank-feed-archive-test ${case})
        set_tests_properties(framework.bank_feed.archive.${case} PROPERTIES
            TIMEOUT 20 LABELS "framework;banking;persistence;regression")
    endforeach()
    add_test(NAME framework.bank_feed.csv.control-del COMMAND umicom-bank-feed-csv-test control-del)
    set_tests_properties(framework.bank_feed.csv.control-del PROPERTIES
        TIMEOUT 20 LABELS "framework;banking;csv;import;regression")
endif()
if(BUILD_TESTING AND TARGET umicom_ui_gtk4)
    add_executable(umicom-bank-feed-archive-native-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/bank_feed/test_archive_native.c")
    set_target_properties(umicom-bank-feed-archive-native-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    target_link_libraries(umicom-bank-feed-archive-native-test PRIVATE Umicom::ui_gtk4)
    umicom_apply_warnings(umicom-bank-feed-archive-native-test)
    umicom_apply_sanitizers(umicom-bank-feed-archive-native-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-bank-feed-archive-native-test)
    endif()
    foreach(case roundtrip json-source csv-source exclusive-save invalid loading-change prepare-change
        edit-notify close-notify close-loading retained)
        add_test(NAME framework.bank_feed.archive.native.${case}
            COMMAND umicom-bank-feed-archive-native-test ${case})
        set_tests_properties(framework.bank_feed.archive.native.${case} PROPERTIES
            TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;banking;persistence;gtk4;regression")
    endforeach()
endif()
