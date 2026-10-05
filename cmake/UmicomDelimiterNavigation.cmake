# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
include_guard(GLOBAL)
# Reuse editor text coordinates and document navigation without a dependency
# on a compiler lexer, GTK or an application-owned document service.
target_sources(umicom_editor PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../src/editor/delimiter_navigation.c")
target_sources(umicom_document PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../src/document/delimiter_navigation.c")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomDelimiterChecks.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomDocumentDelimiterChecks.cmake")

install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/delimiter-navigation.html" DESTINATION "${CMAKE_INSTALL_DATADIR}/doc/umicom/learning" COMPONENT Development)

include("${CMAKE_CURRENT_LIST_DIR}/UmicomDelimiterCommands.cmake")
