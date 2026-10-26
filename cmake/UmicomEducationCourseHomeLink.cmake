# Umicom Framework | native source-preserving course navigation | Sammy Hegab | MIT
include_guard(GLOBAL)
get_filename_component(_umicom_home_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
if(NOT TARGET umicom_education_course_home_link)
    add_library(umicom_education_course_home_link STATIC
        "${_umicom_home_root}/src/education/course_home_link.c")
    add_library(Umicom::EducationCourseHomeLink ALIAS umicom_education_course_home_link)
    target_compile_features(umicom_education_course_home_link PUBLIC c_std_23)
    target_include_directories(umicom_education_course_home_link PUBLIC "${_umicom_home_root}/include")
    if(NOT MSVC)
        target_compile_options(umicom_education_course_home_link PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
endif()
if(BUILD_TESTING AND NOT TARGET umicom-education-course-home-link-test)
    add_executable(umicom-education-course-home-link-test
        "${_umicom_home_root}/tests/education/test_course_home_link.c")
    target_link_libraries(umicom-education-course-home-link-test PRIVATE Umicom::EducationCourseHomeLink)
    add_test(NAME framework.education.course_home_link COMMAND umicom-education-course-home-link-test)
endif()
