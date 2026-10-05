# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Model checks reuse canonical settings, security and complete JSON parsing.
# Keep optional transport linkage explicit for both build and installed users.
include_guard(GLOBAL)
include(GNUInstallDirs)
set(_umicom_connection_check_root "${CMAKE_CURRENT_LIST_DIR}/..")
set(UMICOM_PROVIDER_CONNECTION_CHECK_HTTP "AUTO" CACHE STRING "Provider model check HTTP: AUTO, ON or OFF")
set_property(CACHE UMICOM_PROVIDER_CONNECTION_CHECK_HTTP PROPERTY STRINGS AUTO ON OFF)
string(TOUPPER "${UMICOM_PROVIDER_CONNECTION_CHECK_HTTP}" _umicom_check_mode)
if(NOT _umicom_check_mode MATCHES "^(AUTO|ON|OFF)$")
    message(FATAL_ERROR "UMICOM_PROVIDER_CONNECTION_CHECK_HTTP must be AUTO, ON or OFF")
endif()
set(UMICOM_PROVIDER_CONNECTION_CHECK_HTTP_BUILT OFF)
if(NOT _umicom_check_mode STREQUAL "OFF")
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(UMICOM_CONNECTION_CHECK_CURL QUIET IMPORTED_TARGET GLOBAL libcurl>=7.85.0)
    endif()
    if(TARGET PkgConfig::UMICOM_CONNECTION_CHECK_CURL)
        set(UMICOM_PROVIDER_CONNECTION_CHECK_HTTP_BUILT ON)
    elseif(_umicom_check_mode STREQUAL "ON")
        message(FATAL_ERROR "Provider model checks require pkg-config and libcurl >= 7.85. Use OFF for a build without this transport.")
    endif()
endif()
add_library(umicom_provider_connection_checks STATIC
    "${_umicom_connection_check_root}/src/provider_connection_checks/check.c"
    "${_umicom_connection_check_root}/src/provider_connection_checks/catalogue.c"
    "${_umicom_connection_check_root}/src/provider_connection_checks/http.c")
add_library(Umicom::provider_connection_checks ALIAS umicom_provider_connection_checks)
set_target_properties(umicom_provider_connection_checks PROPERTIES EXPORT_NAME provider_connection_checks
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_include_directories(umicom_provider_connection_checks PUBLIC
    $<BUILD_INTERFACE:${_umicom_connection_check_root}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(umicom_provider_connection_checks PUBLIC Umicom::provider_connections
    PRIVATE Umicom::developer)
if(UMICOM_PROVIDER_CONNECTION_CHECK_HTTP_BUILT)
    target_compile_definitions(umicom_provider_connection_checks PRIVATE UMICOM_CONNECTION_CHECK_HAS_HTTP=1)
    target_link_libraries(umicom_provider_connection_checks PRIVATE PkgConfig::UMICOM_CONNECTION_CHECK_CURL)
endif()
umicom_apply_warnings(umicom_provider_connection_checks)
umicom_apply_sanitizers(umicom_provider_connection_checks)
install(TARGETS umicom_provider_connection_checks EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Framework)
configure_file("${_umicom_connection_check_root}/cmake/UmicomConnectionCheckDependencies.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/UmicomConnectionCheckDependencies.cmake" @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/UmicomConnectionCheckDependencies.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/UmicomFramework COMPONENT Framework)
if(TARGET umicom_ui_gtk4)
    target_sources(umicom_ui_gtk4 PRIVATE
        "${_umicom_connection_check_root}/adapters/gtk4/provider_connection_check_gtk4.c")
    target_link_libraries(umicom_ui_gtk4 PUBLIC Umicom::provider_connection_checks)
endif()
install(FILES "${_umicom_connection_check_root}/docs/learning/checking-provider-connections.html"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/learning COMPONENT Learning)

if(BUILD_TESTING)
    foreach(group check catalogue)
        add_executable(umicom-connection-${group}-test "${_umicom_connection_check_root}/tests/provider_connection_checks/test_${group}.c")
        target_link_libraries(umicom-connection-${group}-test PRIVATE Umicom::provider_connection_checks)
        umicom_apply_warnings(umicom-connection-${group}-test)
        umicom_apply_sanitizers(umicom-connection-${group}-test)
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-connection-${group}-test)
        endif()
    endforeach()
    foreach(case describe local remote denied wrong-password stale changed-after-auth cancelled cancelled-after-auth tampered-plan scope header-injection missing-key)
        add_test(NAME framework.provider_connections.check.${case} COMMAND umicom-connection-check-test ${case})
        set_tests_properties(framework.provider_connections.check.${case} PROPERTIES TIMEOUT 30
            LABELS "framework;provider-connections;privacy;review;regression")
    endforeach()
    foreach(case valid missing empty unicode duplicate-data duplicate-id trailing nul bad-id malformed capacity)
        add_test(NAME framework.provider_connections.catalogue.${case} COMMAND umicom-connection-catalogue-test ${case})
        set_tests_properties(framework.provider_connections.catalogue.${case} PROPERTIES TIMEOUT 20
            LABELS "framework;provider-connections;json;regression")
    endforeach()
endif()

if(BUILD_TESTING AND UMICOM_PROVIDER_CONNECTION_CHECK_HTTP_BUILT)
    add_executable(umicom-connection-http-test "${_umicom_connection_check_root}/tests/provider_connection_checks/test_http.c")
    target_compile_definitions(umicom-connection-http-test PRIVATE UMICOM_CONNECTION_CHECK_HAS_HTTP=1)
    target_link_libraries(umicom-connection-http-test PRIVATE Umicom::provider_connection_checks PkgConfig::UMICOM_CONNECTION_CHECK_CURL)
    umicom_apply_warnings(umicom-connection-http-test)
    umicom_apply_sanitizers(umicom-connection-http-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-connection-http-test)
    endif()
    foreach(case remote local redirect denied rate-limit timeout cancel overflow option-failure header-injection)
        add_test(NAME framework.provider_connections.http.${case} COMMAND umicom-connection-http-test ${case})
        set_tests_properties(framework.provider_connections.http.${case} PROPERTIES TIMEOUT 20
            LABELS "framework;provider-connections;http;privacy;regression")
    endforeach()
endif()

if(BUILD_TESTING AND TARGET umicom_ui_gtk4)
    add_executable(umicom-connection-check-gtk-test "${_umicom_connection_check_root}/tests/provider_connection_checks/test_gtk4.c")
    target_link_libraries(umicom-connection-check-gtk-test PRIVATE Umicom::ui_gtk4)
    umicom_apply_warnings(umicom-connection-check-gtk-test)
    umicom_apply_sanitizers(umicom-connection-check-gtk-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-connection-check-gtk-test)
    endif()
    foreach(case remote local no-approval changed-password dirty stale cancel retained denied-retry missing-model)
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/provider-check-tests/${case}")
        add_test(NAME framework.provider_connections.check.gtk4.${case} COMMAND umicom-connection-check-gtk-test ${case})
        set_tests_properties(framework.provider_connections.check.gtk4.${case} PROPERTIES TIMEOUT 40 SKIP_RETURN_CODE 77
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/provider-check-tests/${case}"
            LABELS "framework;provider-connections;gtk4;privacy;ownership;regression")
    endforeach()
endif()
