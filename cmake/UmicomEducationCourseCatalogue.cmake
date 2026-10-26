#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomEducationCourseCatalogue.cmake
# PURPOSE: Compile the Framework-owned native HTML course catalogue renderer.
# AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
get_filename_component(_umicom_course_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

if(NOT TARGET umicom_education_course_catalogue)
    add_library(umicom_education_course_catalogue STATIC
        "${_umicom_course_root}/src/education/course_catalogue.c")
    add_library(Umicom::EducationCourseCatalogue ALIAS
        umicom_education_course_catalogue)
    target_include_directories(umicom_education_course_catalogue PUBLIC
        "${_umicom_course_root}/include")
    target_compile_features(umicom_education_course_catalogue PUBLIC c_std_23)
    if(MSVC)
        target_compile_options(umicom_education_course_catalogue PRIVATE /W4)
    else()
        target_compile_options(umicom_education_course_catalogue PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    endif()
endif()
if(BUILD_TESTING AND NOT TARGET umicom-education-course-catalogue-test)
    add_executable(umicom-education-course-catalogue-test
        "${_umicom_course_root}/tests/education/test_course_catalogue.c")
    target_link_libraries(umicom-education-course-catalogue-test PRIVATE
        Umicom::EducationCourseCatalogue)
    add_test(NAME framework.education.course_catalogue
        COMMAND umicom-education-course-catalogue-test)
    set_tests_properties(framework.education.course_catalogue PROPERTIES
        LABELS "framework;education;html;security")
endif()
if(NOT TARGET umicom-education-course-catalogue-demo)
    add_executable(umicom-education-course-catalogue-demo
        "${_umicom_course_root}/learning/native-education-catalogue/catalogue_demo.c")
    target_link_libraries(umicom-education-course-catalogue-demo PRIVATE
        Umicom::EducationCourseCatalogue)
endif()
