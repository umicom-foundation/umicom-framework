# Umicom Framework | Native C23 course quality gate | Sammy Hegab | MIT
include_guard(GLOBAL)
get_filename_component(_umicom_course_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
if(NOT TARGET Umicom::EducationCourseCompanions)
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomEducationCourseCompanions.cmake")
endif()
if(NOT TARGET umicom_education_course_release)
    add_library(umicom_education_course_release STATIC
        "${_umicom_course_root}/src/education/course_release.c")
    add_library(Umicom::EducationCourseRelease ALIAS umicom_education_course_release)
    target_compile_features(umicom_education_course_release PUBLIC c_std_23)
    target_include_directories(umicom_education_course_release PUBLIC
        "${_umicom_course_root}/include")
    target_link_libraries(umicom_education_course_release PUBLIC
        Umicom::EducationCourseCompanions)
    if(MSVC)
        target_compile_options(umicom_education_course_release PRIVATE /W4)
    else()
        target_compile_options(umicom_education_course_release PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
endif()
if(NOT TARGET umicom-native-course-release)
    add_executable(umicom-native-course-release
        "${_umicom_course_root}/learning/native-ai-course-release/main.c")
    target_link_libraries(umicom-native-course-release PRIVATE
        Umicom::EducationCourseRelease)
endif()
if(BUILD_TESTING AND NOT TARGET umicom-education-course-release-test)
    add_executable(umicom-education-course-release-test
        "${_umicom_course_root}/tests/education/test_course_release.c")
    target_link_libraries(umicom-education-course-release-test PRIVATE
        Umicom::EducationCourseRelease)
    add_test(NAME framework.education.course_release
        COMMAND umicom-education-course-release-test
            "${CMAKE_CURRENT_BINARY_DIR}/native-course-release-fixture")
endif()
