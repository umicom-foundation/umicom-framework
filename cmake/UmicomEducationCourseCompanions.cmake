# Umicom Framework | Markdown/HTML course companion audit | Sammy Hegab | MIT
include_guard(GLOBAL)
get_filename_component(_umicom_companions_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
if(NOT TARGET umicom_education_course_companions)
    add_library(umicom_education_course_companions STATIC
        "${_umicom_companions_root}/src/education/course_companions.c")
    add_library(Umicom::EducationCourseCompanions ALIAS umicom_education_course_companions)
    target_include_directories(umicom_education_course_companions PUBLIC "${_umicom_companions_root}/include")
    target_compile_features(umicom_education_course_companions PUBLIC c_std_23)
    if(NOT MSVC)
        target_compile_options(umicom_education_course_companions PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
endif()
if(BUILD_TESTING AND NOT TARGET umicom-education-course-companions-test)
    add_executable(umicom-education-course-companions-test
        "${_umicom_companions_root}/tests/education/test_course_companions.c")
    target_link_libraries(umicom-education-course-companions-test PRIVATE Umicom::EducationCourseCompanions)
    add_test(NAME framework.education.course_companions COMMAND umicom-education-course-companions-test)
endif()
