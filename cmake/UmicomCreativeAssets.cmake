# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
include_guard(GLOBAL)
# Captured bytes extend the existing creative owner; products do not copy file I/O.
target_sources(umicom_creative_workspace PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/creative_workspace/asset.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/creative_workspace/asset_file.c")
target_link_libraries(umicom_creative_workspace PUBLIC Umicom::platform)

if(TARGET umicom_creative_workspace_gtk4)
    target_sources(umicom_creative_workspace_gtk4 PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/creative_assets_gtk4.c")
    install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/ui/gtk4/creative_assets.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/ui/gtk4" COMPONENT Framework)
endif()

include("${CMAKE_CURRENT_LIST_DIR}/UmicomCreativeAssetCoreChecks.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomCreativeAssetFileChecks.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomCreativeAssetGtkChecks.cmake")

install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/capture-local-assets.html" DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/docs/learning" COMPONENT Framework)

# Reuse Framework's existing SHA-256 owner. Static linking pulls the digest,
# not the launcher tools; applications carry no copied integrity algorithm.
if(NOT TARGET Umicom::native_launcher)
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomNativeLauncher.cmake")
endif()
target_sources(umicom_creative_workspace PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/creative_workspace/asset_archive.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/creative_workspace/asset_archive_file.c")
target_link_libraries(umicom_creative_workspace PRIVATE Umicom::native_launcher)

include("${CMAKE_CURRENT_LIST_DIR}/UmicomCreativeAssetArchiveChecks.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomCreativeAssetArchiveFileChecks.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomCreativeAssetArchiveGtkChecks.cmake")

if(TARGET umicom_creative_workspace_gtk4)
    # The native page uses the same portable image decoder as other hosts.
    target_link_libraries(umicom_creative_workspace_gtk4 PUBLIC Umicom::png_image)
endif()

include("${CMAKE_CURRENT_LIST_DIR}/UmicomCreativePngPreviewChecks.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomCreativeImageEditChecks.cmake")

# Portable collections reuse both the captured asset owner and its integrity codec.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomCreativeLibrary.cmake")
