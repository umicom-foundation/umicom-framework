#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomSecuritySessionContracts.cmake
#
# PURPOSE:
#   Register identity, credential and session boundary regressions.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
# Session regressions cover trusted clock boundaries and platform entropy failure.
if(NOT TARGET Umicom::security)
    return()
endif()
include_guard(GLOBAL)
include(GNUInstallDirs)
get_filename_component(_session_contract_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
foreach(_session_contract_component IN ITEMS Framework FrameworkSDK)
    set(_session_contract_exclude "")
    if(_session_contract_component STREQUAL "FrameworkSDK")
        set(_session_contract_exclude EXCLUDE_FROM_ALL)
    endif()
    install(FILES "${_session_contract_root}/docs/guides/session-lifetimes.html"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom-framework/docs/guides"
        COMPONENT ${_session_contract_component} ${_session_contract_exclude})
endforeach()
if(BUILD_TESTING)
    add_executable(umicom-session-lifetime-test "${_session_contract_root}/tests/security/test_session_lifetime.c")
    target_link_libraries(umicom-session-lifetime-test PRIVATE Umicom::security)
    # The test provides only the random-source symbol. Registry and zeroing logic
    # compile from production sources; no injectable entropy provider is public.
    add_executable(umicom-session-entropy-test
        "${_session_contract_root}/tests/security/test_session_entropy.c"
        "${_session_contract_root}/src/security/session.c"
        "${_session_contract_root}/src/security/token.c")
    target_link_libraries(umicom-session-entropy-test PRIVATE Umicom::platform Umicom::base)
    set(_session_contract_lifetime_cases basic before-issued rollback equal-now expires after-expiry revoked overflow max-boundary zero-lifetime empty-principal unclosed-principal empty-handle unclosed-handle revoke-bounds missing cleanup outputs null distinct principal-capacity)
    set(_session_contract_entropy_cases unavailable unsupported retry-after-failure collision collision-retry opaque capacity prevalidation)
    foreach(_session_contract_group IN ITEMS lifetime entropy)
        set(_session_contract_target "umicom-session-${_session_contract_group}-test")
        set_target_properties(${_session_contract_target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings(${_session_contract_target})
        endif()
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers(${_session_contract_target})
        endif()
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${_session_contract_target})
        endif()
        foreach(_session_contract_case IN LISTS _session_contract_${_session_contract_group}_cases)
            set(_session_contract_test "framework.security.session.${_session_contract_group}.${_session_contract_case}")
            add_test(NAME ${_session_contract_test} COMMAND ${_session_contract_target} ${_session_contract_case})
            set_tests_properties(${_session_contract_test} PROPERTIES TIMEOUT 30 LABELS "framework;security;session;regression")
            if(COMMAND umicom_test_impact_owners)
                umicom_test_impact_owners(${_session_contract_test} TARGETS ${_session_contract_target})
            endif()
        endforeach()
    endforeach()
endif()
unset(_session_contract_root)
unset(_session_contract_component)
unset(_session_contract_exclude)
unset(_session_contract_lifetime_cases)
unset(_session_contract_entropy_cases)
unset(_session_contract_group)
unset(_session_contract_target)
unset(_session_contract_case)
unset(_session_contract_test)

# Bound identity metadata before composing it into record-access decisions.
if(BUILD_TESTING)
    add_executable(umicom-identity-boundaries-test "${CMAKE_CURRENT_LIST_DIR}/../tests/security/test_identity_boundaries.c")
    target_link_libraries(umicom-identity-boundaries-test PRIVATE Umicom::security)
    set_target_properties(umicom-identity-boundaries-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-identity-boundaries-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-identity-boundaries-test)
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-identity-boundaries-test)
    endif()
    foreach(_identity_boundary_case IN ITEMS unterminated-id unterminated-display max-id max-display empty-id empty-display lookup-unclosed enable-unclosed lookup-empty enable-empty lookup-missing enable-missing enable-revision duplicate outputs null)
        add_test(NAME framework.security.identity.boundaries.${_identity_boundary_case}
            COMMAND umicom-identity-boundaries-test ${_identity_boundary_case})
        set_tests_properties(framework.security.identity.boundaries.${_identity_boundary_case} PROPERTIES
            TIMEOUT 30 LABELS "framework;security;identity;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.security.identity.boundaries.${_identity_boundary_case}
                TARGETS umicom-identity-boundaries-test)
        endif()
    endforeach()
    unset(_identity_boundary_case)
endif()

# Credential records share the same security target; their byte boundaries must
# be checked before clients can treat reference fields as C strings.
if(BUILD_TESTING)
    add_executable(umicom-credential-boundaries-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/security/test_credential_boundaries.c")
    target_link_libraries(umicom-credential-boundaries-test PRIVATE Umicom::security)
    set_target_properties(umicom-credential-boundaries-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-credential-boundaries-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-credential-boundaries-test)
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-credential-boundaries-test)
    endif()
    foreach(_credential_boundary_case IN ITEMS unterminated empty maximum lookup-bounds revoke-bounds missing duplicate capacity revocation null)
        add_test(NAME framework.security.credential.boundaries.${_credential_boundary_case}
            COMMAND umicom-credential-boundaries-test ${_credential_boundary_case})
        set_tests_properties(framework.security.credential.boundaries.${_credential_boundary_case} PROPERTIES
            TIMEOUT 30 LABELS "framework;security;credential;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.security.credential.boundaries.${_credential_boundary_case}
                TARGETS umicom-credential-boundaries-test)
        endif()
    endforeach()
    unset(_credential_boundary_case)
endif()

# Public governance directories have exposed counts, rows and revisions.
if(BUILD_TESTING)
    add_executable(umicom-identity-directory-boundaries-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/security/test_identity_directory_boundaries.c")
    target_link_libraries(umicom-identity-directory-boundaries-test PRIVATE Umicom::security)
    set_target_properties(umicom-identity-directory-boundaries-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-identity-directory-boundaries-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-identity-directory-boundaries-test)
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-identity-directory-boundaries-test)
    endif()
    foreach(_directory_case IN ITEMS fields maximum count corrupt-row lookup revision capacity lifecycle null)
        add_test(NAME framework.security.identity.directory.${_directory_case}
            COMMAND umicom-identity-directory-boundaries-test ${_directory_case})
        set_tests_properties(framework.security.identity.directory.${_directory_case} PROPERTIES
            TIMEOUT 30 LABELS "framework;security;identity;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.security.identity.directory.${_directory_case}
                TARGETS umicom-identity-directory-boundaries-test)
        endif()
    endforeach()
    unset(_directory_case)
endif()
