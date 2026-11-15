#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomSseResponse.cmake
#
# PURPOSE:
# Build reusable event-stream response admission and offline composition regressions.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

include_guard(GLOBAL)
target_sources(umicom_web PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../src/web/sse_response.c")
if(BUILD_TESTING)
    umicom_sse_parser_target(umicom-sse-response-test tests/web/test_sse_response.c)
    foreach(_case IN ITEMS plain case charset quoted-charset http10 informational length-exact length-short length-long chunked identity unknown-field missing-type json-type wrong-charset extra-parameter empty-parameter duplicate-type gzip duplicate-encoding duplicate-length negative-length empty-length comma-length overflow-length length-budget length-chunked chunked-length duplicate-transfer unsupported-transfer http10-chunked redirect busy rate-limited no-content upgrade http2 invalid-status folded-field space-before-colon missing-colon control-value info-length second-status)
        umicom_sse_parser_case("framework.web.sse_response.${_case}" umicom-sse-response-test "${_case}")
    endforeach()
    umicom_sse_parser_target(umicom-sse-response-boundary-test tests/web/test_sse_response_boundaries.c)
    foreach(_case IN ITEMS arguments empty-transport body-before-status bare-lf embedded-cr embedded-nul info-count line-budget line-count total-budget body-before-end incomplete-headers only-informational body-budget plain-trailer header-before-body zero-body trailer-content-type trailer-length trailer-location trailer-status body-after-trailer header-after-trailer-end trailer-ended trailer-unended)
        umicom_sse_parser_case("framework.web.sse_response_boundary.${_case}" umicom-sse-response-boundary-test "${_case}")
    endforeach()
    umicom_local_delta_test_target(umicom-local-chat-http-admission-test test_local_chat_http_admission.c)
    target_link_libraries(umicom-local-chat-http-admission-test PRIVATE Umicom::ai_workspace Umicom::web)
    foreach(_case IN ITEMS success short-body wrong-type busy)
        umicom_local_delta_case("framework.ai_workspace.http_admission.${_case}" umicom-local-chat-http-admission-test "${_case}")
    endforeach()
endif()
