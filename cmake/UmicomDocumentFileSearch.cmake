# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
include_guard(GLOBAL)
# Decode in Document and inject a reader into Platform. The lower target keeps
# no document-codec dependency, and every caller shares one search engine.
target_sources(umicom_document PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../src/document/file_search.c")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomSearchReaderChecks.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomDocumentFileSearchChecks.cmake")

install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/saved-document-search.html" DESTINATION ${CMAKE_INSTALL_DATADIR}/doc/umicom/learning COMPONENT Development)
