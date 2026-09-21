# -----------------------------------------------------------------------------
# Umicom Framework
# File: tests/ctest_execution/fixture/CMakeLists.txt
# Author: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
# Purpose: Define a disposable CTest catalogue, never the application's tests.
# -----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.21)
project(UmicomCtestFixture NONE)
include(CTest)
if(NOT EXISTS "${UMICOM_CTEST_FIXTURE_HELPER}")
    message(FATAL_ERROR "Build the native fixture helper before configuring this catalogue.")
endif()
set(helper "${UMICOM_CTEST_FIXTURE_HELPER}")
add_test(NAME notes.pass COMMAND "${helper}" pass)
add_test(NAME notes.fail COMMAND "${helper}" fail)
add_test(NAME notes.skip COMMAND "${helper}" skip)
set_tests_properties(notes.skip PROPERTIES SKIP_RETURN_CODE 77)
add_test(NAME notes.regex-skip COMMAND "${helper}" regex-skip)
set_tests_properties(notes.regex-skip PROPERTIES SKIP_REGULAR_EXPRESSION "UMICOM_SKIP_THIS_FIXTURE")
add_test(NAME notes.disabled COMMAND "${helper}" pass)
set_tests_properties(notes.disabled PROPERTIES DISABLED TRUE)
add_test(NAME notes.timeout COMMAND "${CMAKE_COMMAND}" -E sleep 0.3)
set_tests_properties(notes.timeout PROPERTIES TIMEOUT 0.05)
add_test(NAME notes.process-timeout COMMAND "${CMAKE_COMMAND}" -E sleep 0.3)
add_test(NAME notes.missing COMMAND "${CMAKE_CURRENT_BINARY_DIR}/umicom-executable-that-does-not-exist")
add_test(NAME notes.large-output COMMAND "${helper}" large)

# Both neighbour names would match the former unescaped expressions. They must
# never be executed when their similarly spelled literal test is selected.
add_test(NAME "notes.save[1]+" COMMAND "${helper}" pass)
add_test(NAME notesXsave1 COMMAND "${helper}" sentinel "${CMAKE_CURRENT_BINARY_DIR}/unexpected.marker")
add_test(NAME notes.save COMMAND "${helper}" pass)
add_test(NAME notesXsave COMMAND "${helper}" sentinel "${CMAKE_CURRENT_BINARY_DIR}/unexpected.marker")
add_test(NAME [=[notes."quoted" & value]=] COMMAND "${helper}" pass)
add_test(NAME [=[notes.back\slash]=] COMMAND "${helper}" pass)
add_test(NAME [=[notes;semicolon]=] COMMAND "${helper}" pass)
add_test(NAME "notes.é" COMMAND "${helper}" pass)
string(REPEAT "x" 249 name_tail)
add_test(NAME "notes.${name_tail}" COMMAND "${helper}" pass)

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/test-working-directory")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/test-working-directory/umicom-working-directory.marker" "Umicom fixture\n")
add_test(NAME notes.working-directory COMMAND "${helper}" cwd)
set_tests_properties(notes.working-directory PROPERTIES WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/test-working-directory")
add_test(NAME notes.configuration COMMAND "${helper}" pass CONFIGURATIONS Debug)

add_test(NAME notes.setup COMMAND "${helper}" create "${CMAKE_CURRENT_BINARY_DIR}/fixture.flag")
set_tests_properties(notes.setup PROPERTIES FIXTURES_SETUP notes_environment)
add_test(NAME notes.fixture-body COMMAND "${helper}" require "${CMAKE_CURRENT_BINARY_DIR}/fixture.flag")
set_tests_properties(notes.fixture-body PROPERTIES FIXTURES_REQUIRED notes_environment)
add_test(NAME notes.cleanup COMMAND "${helper}" remove "${CMAKE_CURRENT_BINARY_DIR}/fixture.flag")
set_tests_properties(notes.cleanup PROPERTIES FIXTURES_CLEANUP notes_environment)

add_test(NAME notes.bad-setup COMMAND "${helper}" fail)
set_tests_properties(notes.bad-setup PROPERTIES FIXTURES_SETUP notes_unavailable)
add_test(NAME notes.blocked-body COMMAND "${helper}" pass)
set_tests_properties(notes.blocked-body PROPERTIES FIXTURES_REQUIRED notes_unavailable)
add_test(NAME notes.cleanup-body COMMAND "${helper}" pass)
set_tests_properties(notes.cleanup-body PROPERTIES FIXTURES_REQUIRED notes_bad_cleanup)
add_test(NAME notes.bad-cleanup COMMAND "${helper}" fail)
set_tests_properties(notes.bad-cleanup PROPERTIES FIXTURES_CLEANUP notes_bad_cleanup)
