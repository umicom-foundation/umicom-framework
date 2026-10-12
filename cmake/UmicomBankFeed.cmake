# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
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
