# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
include_guard(GLOBAL)
target_sources(umicom_document PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../src/document/format_plan.c")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomDocumentFormatPlanChecks.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomDocumentFormatOwnerChecks.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomDocumentFormatPersistenceChecks.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomNativeDocumentFormatChecks.cmake")

install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/document-formats.html" DESTINATION ${CMAKE_INSTALL_DATADIR}/doc/umicom/learning COMPONENT Development)

include("${CMAKE_CURRENT_LIST_DIR}/UmicomDocumentEncodedLimits.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomDocumentFileSearch.cmake")
