#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/creative_workspace/media_source_excerpt_command.cmake
#
# PURPOSE:
#   Verify the native excerpt command's review receipt and byte-exact handoff.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)

# Redirect native stdout to a file so CMake cannot normalize its line endings.
# The sample's accented filename also exercises the native argument boundary.
string(RANDOM LENGTH 16 ALPHABET 0123456789abcdef run_id)
set(case_dir "${directory}/command-${run_id}")
file(MAKE_DIRECTORY "${case_dir}")
set(source "${case_dir}/command-café-カナ.txt")
set(replacement "${case_dir}/command-revised.txt")
set(output "${case_dir}/command-excerpt.bin")
set(selected "café\r\nlast")
execute_process(COMMAND "${fixture}" seed-command-source "${source}" original
    COMMAND_ERROR_IS_FATAL ANY)
file(SHA256 "${source}" original)
file(SIZE "${source}" original_size)
execute_process(COMMAND "${fixture}" command inspect "${source}" 2 3
    RESULT_VARIABLE reviewed OUTPUT_VARIABLE receipt ERROR_VARIABLE inspection_error)
if(NOT reviewed EQUAL 0 OR NOT inspection_error STREQUAL "")
    message(FATAL_ERROR "Native source inspection failed: ${reviewed} ${inspection_error}")
endif()

string(JSON digest GET "${receipt}" sourceSha256)
string(JSON first GET "${receipt}" firstLine)
string(JSON last GET "${receipt}" lastLine)
string(JSON offset GET "${receipt}" byteOffset)
string(JSON count GET "${receipt}" byteCount)
string(JSON file_bytes GET "${receipt}" fileBytes)
string(JSON fields LENGTH "${receipt}")
string(LENGTH "${selected}" bytes)
if(NOT digest STREQUAL original OR NOT first EQUAL 2 OR NOT last EQUAL 3
   OR NOT offset EQUAL 5 OR NOT count EQUAL bytes OR NOT file_bytes EQUAL original_size OR NOT fields EQUAL 6)
    message(FATAL_ERROR "Inspection returned an incomplete or incorrect receipt")
endif()

execute_process(COMMAND "${fixture}" command excerpt "${source}" 2 3 "${digest}"
    RESULT_VARIABLE extracted OUTPUT_FILE "${output}" ERROR_VARIABLE extract_error)
file(READ "${output}" actual HEX)
string(HEX "${selected}" expected)
file(SHA256 "${source}" retained)
if(NOT extracted EQUAL 0 OR NOT extract_error STREQUAL ""
   OR NOT actual STREQUAL expected OR NOT retained STREQUAL original)
    message(FATAL_ERROR "The approved excerpt or the original source bytes changed")
endif()

# Changing only an unselected line still invalidates the full-source receipt.
# Both leaves belong to this isolated fixture directory.
execute_process(COMMAND "${fixture}" seed-command-source "${replacement}" revised
    COMMAND_ERROR_IS_FATAL ANY)
file(RENAME "${replacement}" "${source}")
file(SHA256 "${source}" revised)
execute_process(COMMAND "${fixture}" command excerpt "${source}" 2 3 "${digest}"
    RESULT_VARIABLE refused OUTPUT_FILE "${output}" ERROR_VARIABLE refusal)
file(SIZE "${output}" after_refusal)
file(SHA256 "${source}" after_stale_read)
if(NOT refused EQUAL 1 OR NOT after_refusal EQUAL 0 OR NOT refusal MATCHES "Source bytes changed"
   OR revised STREQUAL original OR NOT after_stale_read STREQUAL revised)
    message(FATAL_ERROR "A stale receipt was not refused without emitting text")
endif()

# Invalid syntax cannot request a partial excerpt.
execute_process(COMMAND "${fixture}" command excerpt "${source}" +2 3 "${revised}"
    RESULT_VARIABLE invalid OUTPUT_FILE "${output}" ERROR_VARIABLE invalid_error)
file(SIZE "${output}" after_invalid)
if(NOT invalid EQUAL 2 OR NOT after_invalid EQUAL 0 OR NOT invalid_error MATCHES "positive decimal")
    message(FATAL_ERROR "An invalid line number did not preserve the output boundary")
endif()
