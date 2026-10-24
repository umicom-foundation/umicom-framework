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
if(BUILD_TESTING)
    add_test(NAME consumer.record-history COMMAND umicom-record-history-example)
    add_test(NAME consumer.record-documents COMMAND umicom-record-document-example)
endif()
