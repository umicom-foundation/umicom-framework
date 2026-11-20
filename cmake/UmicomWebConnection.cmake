#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomWebConnection.cmake
#
# PURPOSE:
#   Compose strict request framing, incremental buffering and the existing local HTTP exchange.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
# The byte exchange core uses existing routing; native sockets are a separate adapter.
target_sources(umicom_web PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/web/loopback_access.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/web/request_frame.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/web/request_stream.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/web/closed_response.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/web/connection.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/web/server_connection_native.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/web/fastcgi.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/web/fastcgi_reply.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/web/fastcgi_exchange.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/web/fastcgi_response.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/web/fastcgi_application.c")
target_compile_definitions(umicom_web PUBLIC UMICOM_HAS_FASTCGI_LOOPBACK=1)

# Hosted web builds can bind an existing route to a supervised local worker.
if((WIN32 OR UNIX) AND NOT EMSCRIPTEN)
    target_sources(umicom_web PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../src/web/managed_worker.c")
    target_compile_definitions(umicom_web PUBLIC UMICOM_HAS_MANAGED_WEB_WORKER=1)
    install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/managed-fastcgi-worker.html"
        DESTINATION ${CMAKE_INSTALL_DATADIR}/doc/umicom/learning COMPONENT Development)
endif()

if(BUILD_TESTING)
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../tests/web/fastcgi" fastcgi-tests)
endif()

include("${CMAKE_CURRENT_LIST_DIR}/UmicomWebRequestFrameChecks.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomWebClosedResponseChecks.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomWebExchangeChecks.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomWebNativeExchangeChecks.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomLocalWebService.cmake")

install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/local-web-service.html" DESTINATION ${CMAKE_INSTALL_DATADIR}/doc/umicom/learning COMPONENT Development)

include("${CMAKE_CURRENT_LIST_DIR}/UmicomWebStaticPreviewChecks.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomWebRequestGateChecks.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomWebRequestStreamChecks.cmake")

install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/local-static-preview.html"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/doc/umicom/learning COMPONENT Development)

include("${CMAKE_CURRENT_LIST_DIR}/UmicomWebResponseBodyChecks.cmake")
