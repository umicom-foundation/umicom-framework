# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
cmake_minimum_required(VERSION 3.24)
foreach(_required IN ITEMS ROOT BUILD GENERATOR COMPILER CTEST_PROGRAM)
    if(NOT DEFINED ${_required} OR "${${_required}}" STREQUAL "")
        message(FATAL_ERROR "Installed record-chain qualification needs ${_required}")
    endif()
endforeach()
# New directories avoid accepting stale successful files from an earlier run.
string(RANDOM LENGTH 16 ALPHABET 0123456789abcdef _nonce)
set(_run "${BUILD}/record-chain-installed-${_nonce}")
file(MAKE_DIRECTORY "${_run}")
function(record_chain_step name)
    execute_process(COMMAND ${ARGN} RESULT_VARIABLE _result
        OUTPUT_FILE "${_run}/${name}.log" ERROR_FILE "${_run}/${name}-errors.log")
    if(NOT _result STREQUAL "0")
        message(FATAL_ERROR "Record-chain ${name} failed (${_result}); read ${_run}")
    endif()
endfunction()
record_chain_step(install "${CMAKE_COMMAND}" --install "${BUILD}" --config "${CONFIG}"
    --prefix "${_run}/original" --component FrameworkSDK)
# Relocation tests import paths, not only the original build-tree environment.
file(RENAME "${_run}/original" "${_run}/relocated")
file(COPY "${ROOT}/examples/record_chain/" DESTINATION "${_run}/source")
record_chain_step(configure "${CMAKE_COMMAND}" -S "${_run}/source" -B "${_run}/consumer"
    -G "${GENERATOR}" "-DCMAKE_C_COMPILER=${COMPILER}"
    "-DCMAKE_BUILD_TYPE=${CONFIG}" "-DUmicomFramework_DIR=${_run}/relocated/lib/cmake/UmicomFramework"
    -DCMAKE_FIND_USE_PACKAGE_REGISTRY=OFF -DCMAKE_FIND_USE_SYSTEM_PACKAGE_REGISTRY=OFF)
record_chain_step(build "${CMAKE_COMMAND}" --build "${_run}/consumer" --config "${CONFIG}" --parallel 2)
record_chain_step(test "${CTEST_PROGRAM}" --test-dir "${_run}/consumer" -C "${CONFIG}"
    --output-on-failure --no-tests=error)
