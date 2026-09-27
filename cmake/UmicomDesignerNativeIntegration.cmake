# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
include_guard(GLOBAL)
# Full composition defines declarative and GTK targets after DesktopSystem.
# A minimal host-tool SDK without declarative is not silently enlarged here.
set_property(GLOBAL PROPERTY UMICOM_DESIGNER_NATIVE_MODULE_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}")
function(umicom_complete_designer_native)
    get_property(_directory GLOBAL PROPERTY UMICOM_DESIGNER_NATIVE_MODULE_DIRECTORY)
    if(TARGET Umicom::declarative)
        include("${_directory}/UmicomDesignerNative.cmake")
        if(TARGET umicom_ui_gtk4)
            include("${_directory}/UmicomDesignerNativeGtk4.cmake")
        endif()
    endif()
endfunction()
cmake_language(DEFER CALL umicom_complete_designer_native)
