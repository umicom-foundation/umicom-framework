# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Compose optional PNG decoding behind a stable C interface.
include_guard(GLOBAL)
find_package(PNG QUIET)
add_library(umicom_png_image STATIC "${CMAKE_CURRENT_LIST_DIR}/../src/media/png_image.c")
add_library(Umicom::png_image ALIAS umicom_png_image)
set_target_properties(umicom_png_image PROPERTIES EXPORT_NAME png_image C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_link_libraries(umicom_png_image PUBLIC Umicom::media_engine Umicom::platform)
if(PNG_FOUND)
    target_compile_definitions(umicom_png_image PRIVATE UMICOM_MEDIA_PNG_ENABLED=1)
    target_link_libraries(umicom_png_image PRIVATE PNG::PNG)
    set(UMICOM_FRAMEWORK_FIND_PNG "find_dependency(PNG)")
else()
    set(UMICOM_FRAMEWORK_FIND_PNG "")
    message(STATUS "PNG image decoder unavailable: libpng was not found")
endif()
umicom_apply_warnings(umicom_png_image)
umicom_apply_sanitizers(umicom_png_image)
install(TARGETS umicom_png_image EXPORT UmicomFrameworkTargets ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Framework)
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/media/png_image.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/media COMPONENT Framework)

include("${CMAKE_CURRENT_LIST_DIR}/UmicomPngImageChecks.cmake")

# Reading and writing share one private native codec memory/error boundary.
target_sources(umicom_png_image PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/media/png_memory.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/media/png_encode.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/media/png_file.c")
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/media/image_edit.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/media COMPONENT Framework)

include("${CMAKE_CURRENT_LIST_DIR}/UmicomImageEditChecks.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomPngEncodeChecks.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomPngFileChecks.cmake")
