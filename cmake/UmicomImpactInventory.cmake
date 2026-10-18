#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomImpactInventory.cmake
# Purpose: Add explicit dependency and test ownership to existing inventories.
# Author and Organisation: Sammy Hegab, Umicom Foundation
# Licence: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

# Call in the directory that registered the test. Declare all executable and
# fixture targets it needs; tests which inspect only files may use ALWAYS.
function(umicom_test_impact_owners test_name)
    cmake_parse_arguments(PARSE_ARGV 1 IMPACT "ALWAYS" "" "TARGETS")
    if(IMPACT_UNPARSED_ARGUMENTS OR IMPACT_KEYWORDS_MISSING_VALUES)
        message(FATAL_ERROR "Invalid test impact ownership arguments: ${test_name}")
    endif()
    if(NOT TEST "${test_name}")
        message(FATAL_ERROR "Declare impact ownership after add_test: ${test_name}")
    endif()
    foreach(_target IN LISTS IMPACT_TARGETS)
        if(NOT TARGET "${_target}")
            message(FATAL_ERROR "Unknown impact test owner: ${_target}")
        endif()
        get_property(_alias TARGET "${_target}" PROPERTY ALIASED_TARGET)
        if(_alias)
            set(_target "${_alias}")
        endif()
        set_property(TEST "${test_name}" APPEND PROPERTY UMICOM_IMPACT_TARGETS "${_target}")
    endforeach()
    if(IMPACT_ALWAYS)
        set_property(TEST "${test_name}" PROPERTY UMICOM_IMPACT_ALWAYS TRUE)
    endif()
endfunction()

# Unresolved expressions stay explicit. Never infer a complete graph from raw
# string fragments: configuration-dependent links require broader validation.
function(_umicom_impact_target_detail target raw output)
    set(_detail "impact-version=1\n")
    set(_complete 1)
    foreach(_property IN ITEMS LINK_LIBRARIES INTERFACE_LINK_LIBRARIES MANUALLY_ADDED_DEPENDENCIES)
        get_property(_links TARGET "${target}" PROPERTY "${_property}")
        foreach(_link IN LISTS _links)
            if(_link MATCHES "^\\$<(LINK_ONLY|BUILD_INTERFACE):([^<>]+)>$")
                set(_link "${CMAKE_MATCH_2}")
            elseif(_link MATCHES "^\\$<INSTALL_INTERFACE:[^<>]*>$")
                continue()
            endif()
            if(_link MATCHES "\\$<")
                set(_complete 0)
            elseif(TARGET "${_link}")
                get_property(_alias TARGET "${_link}" PROPERTY ALIASED_TARGET)
                if(_alias)
                    set(_link "${_alias}")
                endif()
                get_property(_imported TARGET "${_link}" PROPERTY IMPORTED)
                # An imported interface can route back to a local build target.
                # Its closure is not in this inventory, so do not silently treat
                # it as a dependency-free system library.
                if(_imported)
                    get_property(_imported_links TARGET "${_link}" PROPERTY INTERFACE_LINK_LIBRARIES)
                    get_property(_imported_sources TARGET "${_link}" PROPERTY INTERFACE_SOURCES)
                    if(_imported_links OR _imported_sources)
                        set(_complete 0)
                    endif()
                endif()
                if(NOT _imported)
                    string(HEX "${_link}" _hex)
                    string(APPEND _detail "dependency=${_hex}\n")
                endif()
            elseif(_link MATCHES "::")
                set(_complete 0)
            endif()
        endforeach()
    endforeach()
    string(APPEND _detail "complete=${_complete}\nimpact-end\n${raw}")
    set("${output}" "${_detail}" PARENT_SCOPE)
endfunction()

function(_umicom_impact_test_detail test_name directory raw output)
    # Reading properties across directory instances requires CMake 3.28. Older
    # versions retain registration and let the planner widen missing ownership.
    if(CMAKE_VERSION VERSION_LESS "3.28")
        set("${output}" "${raw}" PARENT_SCOPE)
        return()
    endif()
    get_property(_targets TEST "${test_name}" DIRECTORY "${directory}" PROPERTY UMICOM_IMPACT_TARGETS)
    get_property(_always TEST "${test_name}" DIRECTORY "${directory}" PROPERTY UMICOM_IMPACT_ALWAYS)
    set(_detail "impact-version=1\n")
    list(REMOVE_DUPLICATES _targets)
    foreach(_target IN LISTS _targets)
        string(HEX "${_target}" _hex)
        string(APPEND _detail "target=${_hex}\n")
    endforeach()
    if(_always)
        string(APPEND _detail "always=1\n")
    endif()
    # CTest can add setup/cleanup tests or order dependencies beyond the chosen
    # names. Until these edges are represented explicitly, broaden selection
    # so required fixture executables cannot be missed by a narrow build.
    set(_external_test_edges FALSE)
    foreach(_property IN ITEMS DEPENDS FIXTURES_REQUIRED FIXTURES_SETUP FIXTURES_CLEANUP)
        get_property(_value TEST "${test_name}" DIRECTORY "${directory}" PROPERTY "${_property}")
        if(_value)
            set(_external_test_edges TRUE)
        endif()
    endforeach()
    if((_targets OR _always) AND NOT _external_test_edges)
        string(APPEND _detail "complete=1\n")
    else()
        string(APPEND _detail "complete=0\n")
    endif()
    string(APPEND _detail "impact-end\n${raw}")
    set("${output}" "${_detail}" PARENT_SCOPE)
endfunction()
