# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
include_guard(GLOBAL)
target_sources(umicom_document PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../src/document/local_uri.c")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomDocumentSourceNavigationChecks.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomDocumentLocalUriChecks.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomOpenSourceRangeChecks.cmake")
