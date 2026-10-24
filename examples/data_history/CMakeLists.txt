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

# Discovery and revision-checked content reads resolve from the installed SDK.
add_executable(umicom-record-catalogue-example record_catalogue.c)
target_link_libraries(umicom-record-catalogue-example PRIVATE Umicom::data)
set_target_properties(umicom-record-catalogue-example PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(BUILD_TESTING)
    add_test(NAME consumer.record-catalogue COMMAND umicom-record-catalogue-example)
endif()

# Entity bundles resolve only from the installed portable data component.
add_executable(umicom-record-bundle-example record_bundles.c)
target_link_libraries(umicom-record-bundle-example PRIVATE Umicom::data)
set_target_properties(umicom-record-bundle-example PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(BUILD_TESTING)
    add_test(NAME consumer.record-bundle COMMAND umicom-record-bundle-example)
endif()

# The installed data component owns the receipt transaction and reader too.
add_executable(umicom-record-transfer-example record_transfers.c)
target_link_libraries(umicom-record-transfer-example PRIVATE Umicom::data)
set_target_properties(umicom-record-transfer-example PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(BUILD_TESTING)
    add_test(NAME consumer.record-transfer COMMAND umicom-record-transfer-example)
endif()

# Installed consumers exercise backup ownership and its static dependencies.
get_target_property(_backup_example_definitions Umicom::data INTERFACE_COMPILE_DEFINITIONS)
if("UMICOM_HAS_SQLITE=1" IN_LIST _backup_example_definitions)
    add_executable(umicom-data-backup-example sqlite_backup.c)
    target_link_libraries(umicom-data-backup-example PRIVATE Umicom::data)
    set_target_properties(umicom-data-backup-example PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(BUILD_TESTING)
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/backup-example-output")
        add_test(NAME consumer.data-backup COMMAND umicom-data-backup-example "${CMAKE_CURRENT_BINARY_DIR}/backup-example-output")
    endif()
endif()
unset(_backup_example_definitions)
# Typed search is an optional adapter beyond the portable data component.
if(TARGET Umicom::form_records)
    add_executable(umicom-form-search-example form_search.c)
    target_link_libraries(umicom-form-search-example PRIVATE Umicom::form_records)
    set_target_properties(umicom-form-search-example PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(BUILD_TESTING)
        add_test(NAME consumer.form-search COMMAND umicom-form-search-example)
    endif()
endif()
