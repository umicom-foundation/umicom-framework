# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
cmake_minimum_required(VERSION 3.24)
project(UmicomInstalledComponents LANGUAGES C)
# Keep ordinary package discovery for users of this example. Qualification
# supplies one explicit prefix so another local installation cannot stand in
# for the SDK being checked.
if(DEFINED UMICOM_EXPECTED_SDK_PREFIX)
    if(NOT IS_ABSOLUTE "${UMICOM_EXPECTED_SDK_PREFIX}" OR
       NOT IS_DIRECTORY "${UMICOM_EXPECTED_SDK_PREFIX}")
        message(FATAL_ERROR "The expected SDK prefix must be an absolute existing directory")
    endif()
    find_package(UmicomFramework CONFIG REQUIRED
        COMPONENTS market_tape research_replay ibkr_connection
        PATHS "${UMICOM_EXPECTED_SDK_PREFIX}" NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)
    file(REAL_PATH "${UMICOM_EXPECTED_SDK_PREFIX}" _expected_prefix)
    if(NOT DEFINED UmicomFramework_CONFIG OR NOT EXISTS "${UmicomFramework_CONFIG}")
        message(FATAL_ERROR "The resolved SDK did not identify its package configuration")
    endif()
    file(REAL_PATH "${UmicomFramework_CONFIG}" _resolved_config)
    cmake_path(IS_PREFIX _expected_prefix "${_resolved_config}" NORMALIZE _selected_package)
    if(NOT _selected_package)
        message(FATAL_ERROR "The Framework package came from outside the expected SDK prefix")
    endif()
    # This source-side helper inspects imported metadata only. It never adds
    # source libraries or supplies headers to the installed consumer.
    include("${CMAKE_CURRENT_LIST_DIR}/../../../cmake/UmicomRequireInstalledSdk.cmake")
    umicom_require_installed_sdk_layout("${_expected_prefix}" _installed_header_root
        Umicom::market_tape Umicom::research_replay Umicom::ibkr_connection)
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/sdk-origin.txt"
        "prefix=${_expected_prefix}\nconfig=${_resolved_config}\nlayout=checked\npublic-header-identity=not-checked-by-subset-consumer\n")
else()
    find_package(UmicomFramework CONFIG REQUIRED COMPONENTS market_tape research_replay ibkr_connection)
endif()
add_executable(umicom-components-client ../main.c)
set_target_properties(umicom-components-client PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_link_libraries(umicom-components-client PRIVATE
    Umicom::market_tape Umicom::research_replay Umicom::ibkr_connection)
include(CTest)
if(BUILD_TESTING)
    add_test(NAME installed.component_integration.lesson COMMAND umicom-components-client)
endif()
