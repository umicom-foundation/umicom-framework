cmake_minimum_required(VERSION 3.24)
# Umicom Foundation | Sammy Hegab | MIT
# Runs only private build directories. Never edits/deletes the supplied source.
if(NOT DEFINED FRAMEWORK OR NOT DEFINED OUTPUT OR NOT DEFINED MODE)
    message(FATAL_ERROR "Missing composition-test arguments")
endif()
file(MAKE_DIRECTORY "${OUTPUT}")
execute_process(COMMAND "${CMAKE_COMMAND}"
    -S "${CMAKE_CURRENT_LIST_DIR}/composition" -B "${OUTPUT}" -G Ninja
    "-DFRAMEWORK=${FRAMEWORK}" "-DMODE=${MODE}" -DCMAKE_BUILD_TYPE=Release
    "-DCMAKE_C_COMPILER=${COMPILER}"
    RESULT_VARIABLE result OUTPUT_FILE "${OUTPUT}/configure.log" ERROR_FILE "${OUTPUT}/configure-errors.log")
if(NOT result EQUAL 0)
    message(FATAL_ERROR "Composition configuration failed; see ${OUTPUT}/configure-errors.log")
endif()
execute_process(COMMAND "${CMAKE_COMMAND}" --build "${OUTPUT}" --parallel 2
    RESULT_VARIABLE result OUTPUT_FILE "${OUTPUT}/build.log" ERROR_FILE "${OUTPUT}/build-errors.log")
if(NOT result EQUAL 0)
    message(FATAL_ERROR "Composition build failed; see ${OUTPUT}/build-errors.log")
endif()
execute_process(COMMAND "${CTEST}" --test-dir "${OUTPUT}" --show-only=json-v1
    RESULT_VARIABLE result OUTPUT_VARIABLE inventory ERROR_VARIABLE diagnostic)
if(NOT result EQUAL 0)
    message(FATAL_ERROR "Cannot inspect tests: ${diagnostic}")
endif()
file(WRITE "${OUTPUT}/inventory.json" "${inventory}")
string(JSON count LENGTH "${inventory}" tests)
if(MODE STREQUAL "minimal")
    if(NOT count EQUAL 0)
        message(FATAL_ERROR "Minimal host gained unwanted tests")
    endif()
    return()
endif()
if(HOST_SYSTEM STREQUAL "Linux")
    set(expected 31)
else()
    set(expected 23)
endif()
if(NOT count EQUAL expected)
    message(FATAL_ERROR "Expected ${expected} native cases, found ${count}")
endif()
set(expectedNames null thread_running thread_self thread_rejoin empty empty_repeat
    drain cancel_pending cancel_running ignore_cancel escalate post_stop caller_release
    legacy_finish mixed_results worker_self other_queue cancel_all stress competing_join
    notes_drain notes_cancel unavailable_contract)
if(HOST_SYSTEM STREQUAL "Linux")
    list(APPEND expectedNames no_result tls_thread tls_queue
        join_failure busy_progress release_failure legacy_retry no_blocking_join)
endif()
list(TRANSFORM expectedNames PREPEND "framework.queue_shutdown.")
set(names)
math(EXPR last "${count} - 1")
foreach(index RANGE 0 ${last})
    string(JSON name GET "${inventory}" tests ${index} name)
    if(name IN_LIST names)
        message(FATAL_ERROR "Duplicate test: ${name}")
    endif()
    list(APPEND names "${name}")
endforeach()
list(SORT names)
list(SORT expectedNames)
if(NOT names STREQUAL expectedNames)
    message(FATAL_ERROR "Registered native test names differ from the expected inventory")
endif()
execute_process(COMMAND "${CTEST}" --test-dir "${OUTPUT}" --parallel 2
    --no-tests=error --output-on-failure --output-junit "${OUTPUT}/tests.xml"
    RESULT_VARIABLE result OUTPUT_FILE "${OUTPUT}/tests.log" ERROR_FILE "${OUTPUT}/test-errors.log")
if(NOT result EQUAL 0)
    message(FATAL_ERROR "Composition tests failed; see ${OUTPUT}/tests.log")
endif()
