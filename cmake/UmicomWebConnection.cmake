# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
include_guard(GLOBAL)
# The byte exchange core uses existing routing; native sockets are a separate adapter.
target_sources(umicom_web PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/web/loopback_access.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/web/request_frame.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/web/closed_response.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/web/connection.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/web/server_connection_native.c")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomWebRequestFrameChecks.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomWebClosedResponseChecks.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomWebExchangeChecks.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomWebNativeExchangeChecks.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomLocalWebService.cmake")

install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/local-web-service.html" DESTINATION ${CMAKE_INSTALL_DATADIR}/doc/umicom/learning COMPONENT Development)

include("${CMAKE_CURRENT_LIST_DIR}/UmicomWebStaticPreviewChecks.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomWebRequestGateChecks.cmake")
