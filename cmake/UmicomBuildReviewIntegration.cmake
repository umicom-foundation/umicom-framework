# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
include_guard(GLOBAL)
# DesktopSystem is included before some of the complete Framework targets.
# Defer composition to this directory's end; never require a product-local copy.
set_property(GLOBAL PROPERTY UMICOM_BUILD_REVIEW_MODULE_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}")
function(umicom_complete_build_review)
    get_property(_directory GLOBAL PROPERTY UMICOM_BUILD_REVIEW_MODULE_DIRECTORY)
    if(TARGET Umicom::build AND TARGET Umicom::diagnostics)
        include("${_directory}/UmicomBuildReview.cmake")
        if(TARGET umicom_ui_gtk4)
            include("${_directory}/UmicomBuildReviewGtk4.cmake")
        endif()
    endif()
endfunction()
cmake_language(DEFER CALL umicom_complete_build_review)
