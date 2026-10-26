#-----------------------------------------------------------------------------
# Umicom Framework
# File: learning/native-course-platform/CMakeLists.txt
# PURPOSE: Configure native HTML navigation, Markdown/HTML pairing and
#          mathematical stacked-prefix workload planning together.
# AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomNativeEducationAndAiPlatform LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
include(CTest)
include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/UmicomEducationCourseHomeLink.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/UmicomEducationCourseCompanions.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/UmicomAiStackedPrefixCachePlan.cmake")

add_executable(umicom-native-course-navigation
    "${CMAKE_CURRENT_LIST_DIR}/../native-course-navigation/stage_home_navigation.c")
target_link_libraries(umicom-native-course-navigation PRIVATE Umicom::EducationCourseHomeLink)
add_executable(umicom-native-course-integrity
    "${CMAKE_CURRENT_LIST_DIR}/../native-course-integrity/audit_courses.c")
target_link_libraries(umicom-native-course-integrity PRIVATE Umicom::EducationCourseCompanions)
add_executable(umicom-native-stacked-cache-plan-demo
    "${CMAKE_CURRENT_LIST_DIR}/../native-ai-cache-planning/cache_plan_demo.c")
target_link_libraries(umicom-native-stacked-cache-plan-demo PRIVATE Umicom::AiStackedPrefixCachePlan)
