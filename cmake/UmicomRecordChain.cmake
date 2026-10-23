# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Receipt integrity is a headless SDK capability; it never acquires financial authority.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomSdkComponent.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/UmicomRecordChainSources.cmake")
if(NOT TARGET Umicom::base)
    message(FATAL_ERROR "Record chain requires Umicom::base")
endif()
include_guard(GLOBAL)
get_filename_component(_record_chain_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
add_library(umicom_record_chain STATIC)
add_library(Umicom::record_chain ALIAS umicom_record_chain)
umicom_add_record_chain_sources(umicom_record_chain)
install(TARGETS umicom_record_chain EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT Framework)
umicom_register_sdk_libraries(umicom_record_chain)
if(COMMAND umicom_release_inventory_own_headers)
    umicom_release_inventory_own_headers(umicom_record_chain
        include/umicom/record_chain/chain.h include/umicom/record_chain/archive.h)
endif()
# Independent composition owns its public headers; full root installs remain compatible.
foreach(_component IN ITEMS Framework FrameworkSDK)
    if(_component STREQUAL "FrameworkSDK")
        set(_exclude EXCLUDE_FROM_ALL)
    else()
        set(_exclude "")
    endif()
    install(DIRECTORY "${_record_chain_root}/include/umicom/record_chain"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom" COMPONENT "${_component}" ${_exclude})
    install(FILES "${_record_chain_root}/docs/guides/record-integrity.html"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom-framework/docs/guides"
        COMPONENT "${_component}" ${_exclude})
endforeach()
if(BUILD_TESTING)
    include("${_record_chain_root}/tests/record_chain/tests.cmake")
endif()
unset(_exclude)
unset(_component)
