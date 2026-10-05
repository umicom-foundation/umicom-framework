# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
include_guard(GLOBAL)
target_sources(umicom_platform PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../src/platform/directory_scan.c")
target_sources(umicom_document PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../src/document/recovery_storage.c")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomRecoveryStorageChecks.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomDirectoryScanChecks.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomRecoveryCoordinatorChecks.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomRecoveryNativeChecks.cmake")

install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/document-recovery.html" DESTINATION ${CMAKE_INSTALL_DATADIR}/doc/umicom/learning COMPONENT Development)

include("${CMAKE_CURRENT_LIST_DIR}/UmicomRecoverySchedule.cmake")
