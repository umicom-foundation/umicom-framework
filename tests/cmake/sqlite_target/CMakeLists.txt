# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Package absence must not make the otherwise headless viewport unusable.
find_package(SQLite3 QUIET)
set(_cases modern legacy both repeated siblings missing)
if(SQLite3_FOUND)
    list(APPEND _cases real)
else()
    message(STATUS "SQLite consumer check not registered: native SQLite development files unavailable")
endif()
foreach(_case IN LISTS _cases)
    add_test(NAME framework.sqlite_target.${_case}
        COMMAND "${CMAKE_COMMAND}" "-DCASE=${_case}"
            "-DSOURCE=${CMAKE_CURRENT_LIST_DIR}/fixture"
            "-DOUTPUT=${CMAKE_CURRENT_BINARY_DIR}/${_case}"
            "-DRESOLVER=${CMAKE_CURRENT_LIST_DIR}/../../../cmake/UmicomSQLiteTarget.cmake"
            "-DCOMPILER=${CMAKE_C_COMPILER}" "-DGENERATOR=${CMAKE_GENERATOR}"
            -P "${CMAKE_CURRENT_LIST_DIR}/run.cmake")
    set_tests_properties(framework.sqlite_target.${_case} PROPERTIES TIMEOUT 60 LABELS "framework;cmake;sqlite")
endforeach()
