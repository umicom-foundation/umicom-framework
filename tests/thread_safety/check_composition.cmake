# Umicom Foundation | MIT. No deletion, downloads, source edits or application startup.
cmake_minimum_required(VERSION 3.24)
foreach(_required IN ITEMS FRAMEWORK OUTPUT MODE C_COMPILER CTEST)
    if(NOT DEFINED ${_required})
        message(FATAL_ERROR "Missing ${_required}")
    endif()
endforeach()
file(MAKE_DIRECTORY "${OUTPUT}")
function(run_checked label)
    execute_process(COMMAND ${ARGN} RESULT_VARIABLE _result
        OUTPUT_FILE "${OUTPUT}/${label}.stdout.log"
        ERROR_FILE "${OUTPUT}/${label}.stderr.log" TIMEOUT 90)
    if(NOT _result EQUAL 0)
        message(FATAL_ERROR "${label} failed (${_result}); see ${OUTPUT}")
    endif()
endfunction()
run_checked(configure "${CMAKE_COMMAND}" -S "${FRAMEWORK}/tests/thread_safety/composition"
    -B "${OUTPUT}/build" -G Ninja "-DFRAMEWORK=${FRAMEWORK}" "-DMODE=${MODE}"
    "-DCMAKE_C_COMPILER=${C_COMPILER}" -DCMAKE_BUILD_TYPE=Release)
run_checked(build "${CMAKE_COMMAND}" --build "${OUTPUT}/build" --parallel 2)
if(NOT MODE STREQUAL "minimal")
    run_checked(tests "${CTEST}" --test-dir "${OUTPUT}/build" --parallel 2
        --no-tests=error --output-on-failure)
endif()
