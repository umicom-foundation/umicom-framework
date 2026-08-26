#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomApplicationExperienceIntegration.cmake
#
# PURPOSE:
#   Attach the Framework-owned application experience catalogue to the existing
#   umicom_application target without creating a second build catalogue.
#
# Created by: Sammy Hegab
# Organisation: Umicom Foundation
# Licence: MIT
#-----------------------------------------------------------------------------

if(NOT TARGET umicom_application)
    message(FATAL_ERROR
        "UmicomApplicationExperienceIntegration requires umicom_application")
endif()

get_target_property(
    _umicom_experience_integrated
    umicom_application
    UMICOM_APPLICATION_EXPERIENCE_INTEGRATED)

if(NOT _umicom_experience_integrated)
    target_sources(umicom_application PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/experience.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/experience_registry.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/experience_status.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/experience_plan.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/experience_catalogue.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/application/experiences/studio.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/application/experiences/trader.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/application/experiences/ai_creator.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/application/experiences/bank.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/application/experiences/tms.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/application/experiences/llm.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/application/experiences/exchange.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/application/experiences/music.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/application/experiences/media.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/application/experiences/accountant.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/application/experiences/rag.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/application/experiences/desktop.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/application/experiences/os.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/application/experiences/games.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/application/experiences/kitchen.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/application/experiences/cad.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/application/experiences/author.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/application/experiences/web_studio.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/application/experiences/mobile_studio.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/application/experiences/database_studio.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/application/experiences/integration_studio.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/application/experiences/operations.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/application/experiences/security_centre.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/application/experiences/marketplace.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/application/experiences/education.c"
    )

    set_property(
        TARGET umicom_application
        PROPERTY UMICOM_APPLICATION_EXPERIENCE_INTEGRATED TRUE)

    if(BUILD_TESTING AND NOT TARGET umicom-application-experience-test)
        add_executable(umicom-application-experience-test
            "${CMAKE_CURRENT_LIST_DIR}/../tests/application_experience/test_main.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/application_experience/test_catalogue.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/application_experience/test_registry.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/application_experience/test_status.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/application_experience/test_priority.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/application_experience/test_profiles.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/application_experience/test_portfolio_alignment.c"
        )
        target_link_libraries(umicom-application-experience-test PRIVATE
            Umicom::application)
        umicom_apply_warnings(umicom-application-experience-test)
        umicom_apply_sanitizers(umicom-application-experience-test)
        add_test(
            NAME application.experience.catalogue
            COMMAND umicom-application-experience-test)
    endif()
endif()
