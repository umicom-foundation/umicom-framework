# A blocked Core contract is the expected result, not an accepted release.
execute_process(COMMAND "${TOOL}" check "${CORE}/CORE_REQUIREMENTS.tsv" "${CORE}/PENDING_EVIDENCE.tsv" RESULT_VARIABLE result OUTPUT_VARIABLE report ERROR_VARIABLE error)
if(NOT result STREQUAL "1" OR NOT report MATCHES "RELEASE BLOCKED")
    message(FATAL_ERROR "Expected an explicit refusal, received ${result}: ${report} ${error}")
endif()
message(STATUS "Missing release evidence correctly blocks the proposed Core release")
