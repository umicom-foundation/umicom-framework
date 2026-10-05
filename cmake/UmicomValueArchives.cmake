#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomValueArchives.cmake
# PURPOSE: Connect typed portable state to its real base owner and SDK examples.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
target_sources(umicom_base PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../src/base/value_archive.c")
include("${CMAKE_CURRENT_LIST_DIR}/UmicomImplementationFingerprint.cmake")
umicom_attach_implementation_fingerprint(umicom_base "${CMAKE_CURRENT_LIST_DIR}/..")

if(BUILD_TESTING)
    add_executable(umicom-value-archive-envelope-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/value_archive/test_envelope.c")
    target_link_libraries(umicom-value-archive-envelope-test PRIVATE
        Umicom::platform Umicom::chart Umicom::sdk_runtime)
    umicom_apply_warnings(umicom-value-archive-envelope-test)
    umicom_apply_sanitizers(umicom-value-archive-envelope-test)
    foreach(case golden malformed numeric)
        add_test(NAME "framework.value_archive.${case}" COMMAND umicom-value-archive-envelope-test "${case}")
        set_tests_properties("framework.value_archive.${case}" PROPERTIES TIMEOUT 60 LABELS "framework;value-archive;regression")
    endforeach()
    add_executable(umicom-value-archive-collection-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/value_archive/test_collection.c")
    target_link_libraries(umicom-value-archive-collection-test PRIVATE Umicom::platform)
    umicom_apply_warnings(umicom-value-archive-collection-test)
    umicom_apply_sanitizers(umicom-value-archive-collection-test)
    foreach(case duplicate late-schema late-text count row-length)
        add_test(NAME "framework.value_archive.collection.${case}" COMMAND umicom-value-archive-collection-test "${case}")
        set_tests_properties("framework.value_archive.collection.${case}" PROPERTIES TIMEOUT 60 LABELS "framework;value-archive;regression")
    endforeach()
    add_test(NAME framework.value_archive.publication_edges COMMAND umicom-publication-edges-test archive-edges)
    set_tests_properties(framework.value_archive.publication_edges PROPERTIES TIMEOUT 60 LABELS "framework;value-archive;regression")
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-value-archive-collection-test)
    endif()
    add_executable(umicom-portable-state-example "${CMAKE_CURRENT_LIST_DIR}/../examples/value_archive/main.c")
    target_link_libraries(umicom-portable-state-example PRIVATE Umicom::platform Umicom::data)
    umicom_apply_warnings(umicom-portable-state-example)
    umicom_apply_sanitizers(umicom-portable-state-example)
    add_test(NAME framework.value_archive.storage_example COMMAND umicom-portable-state-example)
    set_tests_properties(framework.value_archive.storage_example PROPERTIES TIMEOUT 60 LABELS "framework;value-archive;data;example")
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-value-archive-envelope-test)
        umicom_register_validation_target(umicom-portable-state-example)
    endif()
endif()
install(DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/../examples/value_archive/"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom-framework/examples/value_archive" COMPONENT Framework)
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/PORTABLE_STATE_ARCHIVES.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom-framework/docs" COMPONENT Framework)
