#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomImageBrandChecks.cmake
#
# PURPOSE:
#   Register owned image composition, canonical SVG export and native PNG regression coverage.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
if(TARGET umicom_ui_gtk4)
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomGtkBrandResources.cmake")
    umicom_embed_gtk_brand(umicom_ui_gtk4)
    target_sources(umicom_ui_gtk4 PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/image_brand_gtk4.c")
    target_link_libraries(umicom_ui_gtk4 PUBLIC Umicom::png_image)
    install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/ui/gtk4/image_brand.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/ui/gtk4" COMPONENT Framework)
endif()
if(BUILD_TESTING)
    add_executable(umicom-image-composite-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/media/test_image_composite.c")
    # This fixture compiles the opaque surface owner for controlled exhaustion
    # and cancellation injection, so it must not link a second Media owner.
    target_link_libraries(umicom-image-composite-test PRIVATE Umicom::platform)
    set_target_properties(umicom-image-composite-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-image-composite-test)
    umicom_apply_sanitizers(umicom-image-composite-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-image-composite-test)
    endif()
    foreach(_case source-over opaque transparent over-clear zero-alpha partial low-alpha rounded
        last-corner origin alias mixed row-preservation null-source null-overlay null-output
        occupied-output x-out y-out width-out height-out x-overflow y-overflow over-limit
        cancel-before cancel-copy cancel-composite cancel-publication revision-clear revision-pixel
        revision-row revision-last-clear revision-last-pixel)
        add_test(NAME framework.media.image_composite.${_case}
            COMMAND umicom-image-composite-test "${_case}")
        set_tests_properties(framework.media.image_composite.${_case}
            PROPERTIES TIMEOUT 30 LABELS "framework;media;ownership;regression;no-filesystem")
    endforeach()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-image-composite-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../src/media/image_surface.c"
            "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/media/image_composite.h")
    endif()
    if(TARGET umicom_ui_gtk4)
        add_executable(umicom-image-brand-test
            "${CMAKE_CURRENT_LIST_DIR}/../tests/media/test_image_brand.c")
        target_link_libraries(umicom-image-brand-test PRIVATE Umicom::ui_gtk4)
        set_target_properties(umicom-image-brand-test PROPERTIES
            C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(umicom-image-brand-test)
        umicom_apply_sanitizers(umicom-image-brand-test)
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-image-brand-test)
        endif()
        foreach(_case apply transparent light portrait minimum png existing cancel small
                      null-source null-output occupied-output relative-output null-receipt)
            add_test(NAME framework.media.image_brand.${_case} COMMAND umicom-image-brand-test "${_case}")
            set_tests_properties(framework.media.image_brand.${_case} PROPERTIES TIMEOUT 30
                SKIP_RETURN_CODE 77 LABELS "framework;media;svg;png;branding;regression")
        endforeach()
        if(COMMAND umicom_target_qualification_inputs)
            umicom_target_qualification_inputs(umicom-image-brand-test FILES
                "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/image_brand_gtk4.c"
                "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/brand_widget.h"
                "${CMAKE_CURRENT_LIST_DIR}/../resources/brand/umicom-logo-on-dark.svg")
        endif()
    endif()
endif()

install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/branded-image-exports.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/docs/learning" COMPONENT Framework)
install(FILES
    "${CMAKE_CURRENT_LIST_DIR}/../examples/image_composite/main.c"
    "${CMAKE_CURRENT_LIST_DIR}/../examples/image_composite/CMakeLists.txt"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/examples/image_composite" COMPONENT Learning)
if(BUILD_TESTING AND TARGET umicom_ui_gtk4)
    add_executable(umicom-image-composite-example
        "${CMAKE_CURRENT_LIST_DIR}/../examples/image_composite/main.c")
    target_link_libraries(umicom-image-composite-example PRIVATE Umicom::ui_gtk4)
    set_target_properties(umicom-image-composite-example PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-image-composite-example)
    umicom_apply_sanitizers(umicom-image-composite-example)
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomApplicationBranding.cmake")
    if(WIN32)
        enable_language(RC)
    endif()
    umicom_apply_application_branding(TARGET umicom-image-composite-example
        PRODUCT_NAME "Umicom Image Composition Example" INTERNAL_NAME "umicom-image-composite-example"
        RESOURCE_ROOT "${CMAKE_CURRENT_LIST_DIR}/../resources")
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-image-composite-example)
    endif()
    foreach(_case self-check cancel reject-position)
        add_test(NAME framework.media.image_composite-example.${_case}
            COMMAND umicom-image-composite-example "${_case}")
        set_tests_properties(framework.media.image_composite-example.${_case}
            PROPERTIES TIMEOUT 30 LABELS "framework;media;learning;no-filesystem")
    endforeach()
endif()

if(BUILD_TESTING AND TARGET umicom_ui_gtk4)
    add_executable(umicom-image-brand-failure-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/media/test_image_brand_failure.c")
    target_link_libraries(umicom-image-brand-failure-test PRIVATE Umicom::ui_gtk4)
    target_include_directories(umicom-image-brand-failure-test PRIVATE
        "${CMAKE_CURRENT_BINARY_DIR}/generated/gtk-brand")
    set_target_properties(umicom-image-brand-failure-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-image-brand-failure-test)
    umicom_apply_sanitizers(umicom-image-brand-failure-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-image-brand-failure-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-image-brand-failure-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/image_brand_gtk4.c"
            "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/brand_widget.h")
    endif()
    foreach(_case decode save-decode width height pixels cancel-after-decode)
        add_test(NAME framework.media.image_brand-failure.${_case}
            COMMAND umicom-image-brand-failure-test "${_case}")
        set_tests_properties(framework.media.image_brand-failure.${_case}
            PROPERTIES TIMEOUT 30 LABELS "framework;media;branding;fault-injection;regression")
    endforeach()
endif()
