#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomAiFederatedAuthenticatedFrame.cmake
#
# PURPOSE:
#   Register opt-in authenticated federated frames using the canonical native
#   SHA-256, reusable Framework HMAC and existing federated wire services.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
get_filename_component(_umicom_auth_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
if(NOT TARGET Umicom::AiFederatedWire)
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomAiFederatedWire.cmake")
endif()
if(NOT TARGET Umicom::SecurityHmacSha256)
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomSecurityHmacSha256.cmake")
endif()
if(NOT TARGET umicom_ai_federated_authenticated_frame)
    add_library(umicom_ai_federated_authenticated_frame STATIC
        "${_umicom_auth_root}/src/ai/training/federated_authenticated_frame.c")
    add_library(Umicom::AiFederatedAuthenticatedFrame ALIAS
        umicom_ai_federated_authenticated_frame)
    target_include_directories(umicom_ai_federated_authenticated_frame PUBLIC
        "${_umicom_auth_root}/include")
    target_link_libraries(umicom_ai_federated_authenticated_frame PUBLIC
        Umicom::AiFederatedWire Umicom::SecurityHmacSha256)
    target_compile_features(umicom_ai_federated_authenticated_frame PUBLIC c_std_23)
    if(MSVC)
        target_compile_options(umicom_ai_federated_authenticated_frame PRIVATE /W4)
    else()
        target_compile_options(umicom_ai_federated_authenticated_frame PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
endif()
if(BUILD_TESTING AND NOT TARGET umicom-ai-federated-auth-test)
    add_executable(umicom-ai-federated-auth-test
        "${_umicom_auth_root}/tests/ai_training/test_federated_authenticated_frame.c")
    target_link_libraries(umicom-ai-federated-auth-test PRIVATE
        Umicom::AiFederatedAuthenticatedFrame)
    target_compile_features(umicom-ai-federated-auth-test PRIVATE c_std_23)
    if(MSVC)
        target_compile_options(umicom-ai-federated-auth-test PRIVATE /W4)
    else()
        target_compile_options(umicom-ai-federated-auth-test PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
    foreach(_case IN ITEMS hmac_rfc4231 auth_roundtrip mac_tamper wrong_key
            wrong_epoch invalid_wire short_capacity replay_rejection envelope)
        add_test(NAME "framework.ai.federated_auth.${_case}"
            COMMAND umicom-ai-federated-auth-test "${_case}")
        set_tests_properties("framework.ai.federated_auth.${_case}"
            PROPERTIES LABELS "framework;ai;federated;security;hmac" TIMEOUT 60)
    endforeach()
endif()
if(NOT TARGET umicom-native-federated-auth-demo)
    add_executable(umicom-native-federated-auth-demo
        "${_umicom_auth_root}/learning/native-ai-federated-auth/demo.c")
    target_link_libraries(umicom-native-federated-auth-demo PRIVATE
        Umicom::AiFederatedAuthenticatedFrame)
    target_compile_features(umicom-native-federated-auth-demo PRIVATE c_std_23)
endif()
