# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Disposable, explicitly generated test inputs. Execute the configured native
# compiler directly, not a shell, and inspect its actual failing transcript.
file(MAKE_DIRECTORY "${OUTPUT}/Umicom Notes")
set(_source "${OUTPUT}/Umicom Notes/main.c")
file(WRITE "${_source}" "#error Umicom Notes diagnostic exercise\nint main(void) { return 0; }\n")
execute_process(COMMAND "${COMPILER}" -std=c2x -fdiagnostics-color=never -c "${_source}" -o "${OUTPUT}/notes.o"
    RESULT_VARIABLE _code OUTPUT_VARIABLE _stdout ERROR_VARIABLE _stderr TIMEOUT 30)
if(_code STREQUAL "0")
    message(FATAL_ERROR "The deliberately invalid source unexpectedly compiled")
endif()
file(WRITE "${OUTPUT}/compiler.log" "${_stdout}${_stderr}")
execute_process(COMMAND "${CLI}" log "${OUTPUT}/compiler.log"
    RESULT_VARIABLE _inspect OUTPUT_VARIABLE _report ERROR_VARIABLE _error TIMEOUT 15)
file(WRITE "${OUTPUT}/review.txt" "${_report}${_error}")
if(NOT _inspect STREQUAL "0" OR NOT _report MATCHES "Unrecorded [(]imported text only[)]" OR
    NOT _report MATCHES "main[.]c:1" OR NOT _report MATCHES "error [|]")
    message(FATAL_ERROR "Actual compiler output was not identified with its original source location: ${_error}")
endif()
file(WRITE "${_source}" "int main(void) { return 0; }\n")
execute_process(COMMAND "${COMPILER}" -std=c2x -fdiagnostics-color=never -c "${_source}" -o "${OUTPUT}/notes.o"
    RESULT_VARIABLE _fixed OUTPUT_VARIABLE _fixed_out ERROR_VARIABLE _fixed_error TIMEOUT 30)
file(WRITE "${OUTPUT}/fixed.log" "${_fixed_out}${_fixed_error}")
if(NOT _fixed STREQUAL "0")
    message(FATAL_ERROR "The corrected practice source did not compile: ${_fixed_error}")
endif()
# This proves compilation and log inspection, not program execution or Studio.
