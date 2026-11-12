#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomWeightLog.cmake
#
# PURPOSE:
#   Attach the reusable observation schema to the existing application service.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#---------------------------------------------------------------------------
include_guard(GLOBAL)
if(NOT TARGET umicom_application)
    return()
endif()
get_filename_component(_weight_log_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
target_sources(umicom_application PRIVATE "${_weight_log_root}/src/healthcare/weight_log.c")
if(COMMAND umicom_release_inventory_own_headers)
    umicom_release_inventory_own_headers(umicom_application include/umicom/healthcare/weight_log.h)
endif()
foreach(_weight_log_component IN ITEMS Framework FrameworkSDK)
    set(_weight_log_exclude "")
    if(_weight_log_component STREQUAL "FrameworkSDK")
        set(_weight_log_exclude EXCLUDE_FROM_ALL)
    endif()
    install(DIRECTORY "${_weight_log_root}/examples/weight_log/"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom-framework/examples/weight_log"
        COMPONENT ${_weight_log_component} ${_weight_log_exclude})
endforeach()
if(BUILD_TESTING)
    add_executable(umicom-weight-log-test "${_weight_log_root}/tests/healthcare/test_weight_log.c")
    add_executable(umicom-weight-log-example "${_weight_log_root}/examples/weight_log/main.c")
    foreach(_weight_log_target IN ITEMS umicom-weight-log-test umicom-weight-log-example)
        target_link_libraries(${_weight_log_target} PRIVATE Umicom::application)
        set_target_properties(${_weight_log_target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings(${_weight_log_target})
        endif()
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers(${_weight_log_target})
        endif()
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${_weight_log_target})
        endif()
    endforeach()
    foreach(_weight_log_case IN ITEMS decimal syntax range entry refusal note)
        set(_weight_log_test framework.weight_log.${_weight_log_case})
        add_test(NAME ${_weight_log_test} COMMAND umicom-weight-log-test ${_weight_log_case})
        set_tests_properties(${_weight_log_test} PROPERTIES TIMEOUT 30 LABELS "framework;forms;wellness;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(${_weight_log_test} TARGETS umicom-weight-log-test)
        endif()
    endforeach()
    add_test(NAME framework.weight_log.example COMMAND umicom-weight-log-example)
    set_tests_properties(framework.weight_log.example PROPERTIES TIMEOUT 30 LABELS "framework;forms;example")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(framework.weight_log.example TARGETS umicom-weight-log-example)
    endif()
endif()
