cmake_minimum_required(VERSION 3.25)
project(UmicomRecordHistoryExample LANGUAGES C)
# This consumer uses only the installed SDK; no Framework source fallback.
find_package(UmicomFramework CONFIG REQUIRED)
add_executable(umicom-record-history-example record_amendments.c)
target_link_libraries(umicom-record-history-example PRIVATE Umicom::data)
set_target_properties(umicom-record-history-example PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)

# The same installed data component provides atomic content and provenance.
add_executable(umicom-record-document-example record_documents.c)
target_link_libraries(umicom-record-document-example PRIVATE Umicom::data)
set_target_properties(umicom-record-document-example PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
include(CTest)
# This consumer also checks the portable draft API through the installed SDK.
add_executable(umicom-record-draft-example record_drafts.c)
target_link_libraries(umicom-record-draft-example PRIVATE Umicom::data)
set_target_properties(umicom-record-draft-example PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(BUILD_TESTING)
    add_test(NAME consumer.record-history COMMAND umicom-record-history-example)
    add_test(NAME consumer.record-documents COMMAND umicom-record-document-example)
    add_test(NAME consumer.record-drafts COMMAND umicom-record-draft-example)
endif()

# Bounded amendment browsing also resolves solely from the installed SDK.
add_executable(umicom-record-history-page-example record_history_pages.c)
target_link_libraries(umicom-record-history-page-example PRIVATE Umicom::data)
set_target_properties(umicom-record-history-page-example PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(BUILD_TESTING)
    add_test(NAME consumer.record-history-pages COMMAND umicom-record-history-page-example)
endif()

# Recovery codecs remain available to installed headless data consumers.
add_executable(umicom-record-draft-recovery-example record_draft_recovery.c)
target_link_libraries(umicom-record-draft-recovery-example PRIVATE Umicom::data)
set_target_properties(umicom-record-draft-recovery-example PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(BUILD_TESTING)
    add_test(NAME consumer.record-draft-recovery COMMAND umicom-record-draft-recovery-example)
endif()
