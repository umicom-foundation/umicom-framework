# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Build this directory against a separately installed Framework SDK.
cmake_minimum_required(VERSION 3.24)
project(UmicomSnapshotBatchLesson LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
find_package(UmicomFramework CONFIG REQUIRED)
add_executable(umicom-snapshot-batch-lesson main.c)
target_link_libraries(umicom-snapshot-batch-lesson PRIVATE Umicom::platform)
include(CTest)
if(BUILD_TESTING)
    add_test(NAME snapshot-batch-lesson COMMAND umicom-snapshot-batch-lesson)
endif()

# The second lesson needs the language owner from the same installed SDK.
add_executable(umicom-document-replace-lesson document_replace.c)
target_link_libraries(umicom-document-replace-lesson PRIVATE Umicom::language)
if(BUILD_TESTING)
    add_test(NAME document-replace-lesson COMMAND umicom-document-replace-lesson)
endif()
