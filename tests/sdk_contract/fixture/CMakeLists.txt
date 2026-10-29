# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Configuration-only fixtures deliberately do not stand in for SDK binaries.
cmake_minimum_required(VERSION 3.24)
project(InstalledSdkContractFixture LANGUAGES NONE)
include("${CONTRACT_HELPER}")
# Retained fixed fixture paths; a fresh SDK per configure prevents old negative-test inputs from becoming the baseline.
if(FALSE)
if(TEST_CASE STREQUAL "spaced-prefix")
    set(_sdk "${CMAKE_CURRENT_BINARY_DIR}/SDK with spaces")
else()
    set(_sdk "${CMAKE_CURRENT_BINARY_DIR}/sdk")
endif()

endif()
# Preserve earlier artifacts for inspection while making repeated runs independent.
string(RANDOM LENGTH 16 ALPHABET 0123456789abcdef _run)
if(TEST_CASE STREQUAL "spaced-prefix")
    set(_sdk "${CMAKE_CURRENT_BINARY_DIR}/SDK with spaces-${_run}")
else()
    set(_sdk "${CMAKE_CURRENT_BINARY_DIR}/sdk-${_run}")
endif()
file(MAKE_DIRECTORY "${_sdk}/include/umicom/base" "${_sdk}/lib")
# Omitted and wrong-type marker fixtures exercise layout admission separately
# from public fingerprint equality. Retain the earlier unconditional fixture.
#[=[
file(WRITE "${_sdk}/include/umicom/base/version.h" "/* Public fixture identity. */\n#define FIXTURE_ABI 2\n")
]=]
if(TEST_CASE STREQUAL "directory-version-header")
    file(MAKE_DIRECTORY "${_sdk}/include/umicom/base/version.h")
elseif(NOT TEST_CASE STREQUAL "missing-version-header")
    file(WRITE "${_sdk}/include/umicom/base/version.h" "/* Public fixture identity. */\n#define FIXTURE_ABI 2\n")
endif()
# A real base subset can contain status.h while omitting version.h. Keep that
# unrelated header present so the negative case reaches layout validation,
# rather than failing earlier because the fingerprint input set is empty.
file(WRITE "${_sdk}/include/umicom/base/status.h" "/* Configure-only status contract. */\n")
file(WRITE "${_sdk}/lib/base.a" "configure-only library location fixture\n")
if(TEST_CASE STREQUAL "source-target")
    add_library(fixture_base INTERFACE)
    add_library(Umicom::base ALIAS fixture_base)
else()
    add_library(Umicom::base STATIC IMPORTED)
    set_target_properties(Umicom::base PROPERTIES
        IMPORTED_CONFIGURATIONS "DEBUG;RELEASE"
        IMPORTED_LOCATION_DEBUG "${_sdk}/lib/base.a"
        IMPORTED_LOCATION_RELEASE "${_sdk}/lib/base.a"
        INTERFACE_INCLUDE_DIRECTORIES "${_sdk}/include")
endif()
set(UMICOM_FRAMEWORK_INSTALL_PREFIX "${_sdk}")
umicom_compute_public_contract_fingerprint(_fingerprint "${_sdk}/include")
if(NOT TEST_CASE STREQUAL "source-target" AND NOT TEST_CASE STREQUAL "missing-identity"
   AND NOT TEST_CASE STREQUAL "layout-only")
    set_property(TARGET Umicom::base PROPERTY UMICOM_PUBLIC_CONTRACT_FINGERPRINT "${_fingerprint}")
endif()
if(TEST_CASE STREQUAL "changed-header")
    file(APPEND "${_sdk}/include/umicom/base/version.h" "#define MIXED_INSTALLATION 1\n")
elseif(TEST_CASE STREQUAL "extra-header")
    file(WRITE "${_sdk}/include/umicom/stale.h" "/* Header left by another SDK. */\n")
elseif(TEST_CASE STREQUAL "outside-library")
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/outside.a" "outside selected prefix\n")
    set_property(TARGET Umicom::base PROPERTY IMPORTED_LOCATION_RELEASE "${CMAKE_CURRENT_BINARY_DIR}/outside.a")
elseif(TEST_CASE STREQUAL "missing-library")
    set_property(TARGET Umicom::base PROPERTY IMPORTED_LOCATION_DEBUG "${_sdk}/lib/absent.a")
elseif(TEST_CASE STREQUAL "outside-headers")
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/outside-include")
    set_property(TARGET Umicom::base PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_BINARY_DIR}/outside-include")
elseif(TEST_CASE STREQUAL "wrong-prefix")
    set(UMICOM_FRAMEWORK_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}")
endif()

# A component's link interface may bring in additional Framework owners.
# These location-only fixtures exercise that graph without pretending their
# placeholder archives can compile or execute an application.
if(TEST_CASE MATCHES "^transitive-" OR TEST_CASE STREQUAL "external-target")
    set(_dependency Umicom::dependency)
    if(TEST_CASE STREQUAL "external-target")
        set(_dependency ThirdParty::dependency)
    endif()
    if(TEST_CASE STREQUAL "transitive-source")
        add_library(fixture_dependency INTERFACE)
        add_library(Umicom::dependency ALIAS fixture_dependency)
    elseif(NOT TEST_CASE STREQUAL "transitive-missing")
        file(WRITE "${_sdk}/lib/dependency.a" "configure-only dependency location fixture\n")
        add_library(${_dependency} STATIC IMPORTED)
        set_target_properties(${_dependency} PROPERTIES
            IMPORTED_CONFIGURATIONS "DEBUG;RELEASE"
            IMPORTED_LOCATION_DEBUG "${_sdk}/lib/dependency.a"
            IMPORTED_LOCATION_RELEASE "${_sdk}/lib/dependency.a"
            INTERFACE_INCLUDE_DIRECTORIES "${_sdk}/include")
    endif()
    set_property(TARGET Umicom::base PROPERTY INTERFACE_LINK_LIBRARIES "${_dependency}")
    if(TEST_CASE STREQUAL "transitive-outside-library" OR TEST_CASE STREQUAL "external-target")
        file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/outside-dependency.a" "outside selected Framework prefix\n")
        set_property(TARGET ${_dependency} PROPERTY IMPORTED_LOCATION_RELEASE
            "${CMAKE_CURRENT_BINARY_DIR}/outside-dependency.a")
    elseif(TEST_CASE STREQUAL "transitive-outside-headers")
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/outside-dependency-include")
        set_property(TARGET ${_dependency} PROPERTY INTERFACE_INCLUDE_DIRECTORIES
            "${CMAKE_CURRENT_BINARY_DIR}/outside-dependency-include")
    elseif(TEST_CASE STREQUAL "transitive-cycle")
        set_property(TARGET ${_dependency} PROPERTY INTERFACE_LINK_LIBRARIES Umicom::base)
    elseif(TEST_CASE STREQUAL "transitive-inherited-headers")
        set_property(TARGET ${_dependency} PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
        set_property(TARGET ${_dependency} PROPERTY INTERFACE_LINK_LIBRARIES Umicom::base)
    elseif(TEST_CASE STREQUAL "transitive-link-only")
        set_property(TARGET Umicom::base PROPERTY INTERFACE_LINK_LIBRARIES
            "$<LINK_ONLY:Umicom::dependency>")
    endif()
endif()
# Installed libraries commonly inherit public includes from base. These
# fixtures retain that valid topology without placing any include path on the
# selected wrapper, and without manufacturing a library for an interface target.
set(_extra_components)
if(TEST_CASE STREQUAL "inherited-headers")
    file(WRITE "${_sdk}/lib/wrapper.a" "configure-only wrapper archive\n")
    add_library(Umicom::wrapper STATIC IMPORTED)
    set_target_properties(Umicom::wrapper PROPERTIES
        IMPORTED_LOCATION "${_sdk}/lib/wrapper.a" INTERFACE_LINK_LIBRARIES Umicom::base)
    list(APPEND _extra_components Umicom::wrapper)
elseif(TEST_CASE STREQUAL "aggregate-headers")
    add_library(Umicom::Framework INTERFACE IMPORTED)
    set_property(TARGET Umicom::Framework PROPERTY INTERFACE_LINK_LIBRARIES Umicom::base)
    list(APPEND _extra_components Umicom::Framework)
elseif(TEST_CASE STREQUAL "missing-base-headers")
    set_property(TARGET Umicom::base PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
endif()
if(TEST_CASE STREQUAL "layout-only" OR TEST_CASE STREQUAL "missing-version-header" OR
   TEST_CASE STREQUAL "directory-version-header")
    umicom_require_installed_sdk_layout("${_sdk}" _layout_headers Umicom::base)
    file(REAL_PATH "${_sdk}/include" _expected_headers)
    if(NOT _layout_headers STREQUAL _expected_headers)
        message(FATAL_ERROR "Installed layout returned the wrong include root")
    endif()
    message(STATUS "Installed subset layout verified without full SDK identity")
elseif(TEST_CASE STREQUAL "missing-component")
    umicom_require_installed_sdk("${_sdk}" Umicom::absent)
else()
    umicom_require_installed_sdk("${_sdk}" Umicom::base ${_extra_components})
endif()
