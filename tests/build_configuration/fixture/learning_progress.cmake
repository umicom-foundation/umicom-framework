#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/build_configuration/fixture/learning_progress.cmake
#
# PURPOSE:
#   Check real learning module composition in either order and without optional peers.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
add_library(Umicom::data INTERFACE IMPORTED)
add_library(Umicom::browser INTERFACE IMPORTED)
if(NOT FIXTURE_ORDER MATCHES "^(progress-first|catalogue-first|progress-only|catalogue-only)$")
    message(FATAL_ERROR "Unknown learning module order: ${FIXTURE_ORDER}")
endif()
if(FIXTURE_ORDER STREQUAL "progress-first" OR FIXTURE_ORDER STREQUAL "progress-only")
    include("${ROOT}/cmake/UmicomLearningProgress.cmake")
endif()
if(NOT FIXTURE_ORDER STREQUAL "progress-only")
    include("${ROOT}/cmake/UmicomLearningCatalogueGtk4.cmake")
endif()
if(FIXTURE_ORDER STREQUAL "catalogue-first")
    include("${ROOT}/cmake/UmicomLearningProgress.cmake")
endif()
# Repeated calls must not duplicate either link edges or test ownership.
include("${ROOT}/cmake/UmicomLearningProgressGtk4.cmake")
include("${ROOT}/cmake/UmicomLearningProgressGtk4.cmake")
if(FIXTURE_ORDER MATCHES "first$")
    foreach(_target IN ITEMS umicom_learning_catalogue_gtk4 umicom_learning_progress)
        if(NOT TARGET "${_target}")
            message(FATAL_ERROR "Missing learning owner: ${_target}")
        endif()
    endforeach()
    get_target_property(_links umicom_learning_catalogue_gtk4 LINK_LIBRARIES)
    get_target_property(_definitions umicom_learning_catalogue_gtk4 COMPILE_DEFINITIONS)
    list(FILTER _links INCLUDE REGEX "^Umicom::learning_progress$")
    list(FILTER _definitions INCLUDE REGEX "^UMICOM_LEARNING_PROGRESS_AVAILABLE=1$")
    list(LENGTH _links _link_count)
    list(LENGTH _definitions _definition_count)
    if(NOT _link_count EQUAL 1 OR NOT _definition_count EQUAL 1)
        message(FATAL_ERROR "Optional progress binding is missing or repeated")
    endif()
    if(BUILD_TESTING)
        require_policies(umicom-learning-catalogue-progress-test)
        get_property(_tests DIRECTORY PROPERTY TESTS)
        list(FILTER _tests INCLUDE REGEX "^framework[.]learning_pack[.]native_progress[.]")
        list(LENGTH _tests _count)
        if(NOT _count EQUAL 19)
            message(FATAL_ERROR "Native progress registration changed unexpectedly: ${_tests}")
        endif()
    elseif(TARGET umicom-learning-catalogue-progress-test)
        message(FATAL_ERROR "Native progress regression target leaked into a non-testing consumer")
    endif()
elseif(FIXTURE_ORDER STREQUAL "catalogue-only")
    get_target_property(_definitions umicom_learning_catalogue_gtk4 COMPILE_DEFINITIONS)
    if("UMICOM_LEARNING_PROGRESS_AVAILABLE=1" IN_LIST _definitions OR
        TARGET umicom_learning_progress OR TARGET umicom-learning-catalogue-progress-test)
        message(FATAL_ERROR "Catalogue unexpectedly gained an absent persistence provider")
    endif()
elseif(TARGET umicom_learning_catalogue_gtk4 OR TARGET umicom-learning-catalogue-progress-test)
    message(FATAL_ERROR "Headless progress unexpectedly created a native catalogue")
endif()
