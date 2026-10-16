#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomRepositoryPublication.cmake
# Purpose: Register native workspace publication and focused regression coverage.
# Author: Sammy Hegab, Umicom Foundation
# Licence: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

# Extend the established repository library so every product shares one policy.
target_sources(umicom_repository PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/operations/publication_policy.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/operations/publication_git.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/operations/publication_lock.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/operations/publication_discovery.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/operations/publication_inspection.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/operations/publication_recovery.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/operations/publication_plan.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/operations/publication_execution.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/operations/publication_service.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/operations/publication_progress.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/operations/publication_report.c")

# Use the established JSON escaping service for publication evidence.
target_link_libraries(umicom_repository PRIVATE Umicom::protocol)

if(BUILD_TESTING)
    # Existing cases remain registered; evidence and graph refresh add regressions.
    foreach(_case policy plan execution progress report report_file)
        add_executable("umicom-repo-publication-${_case}-test"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/repository_operations/test_publication_${_case}.c")
        target_link_libraries("umicom-repo-publication-${_case}-test" PRIVATE Umicom::repository)
        target_include_directories("umicom-repo-publication-${_case}-test" PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../src/repository/operations")
        umicom_apply_warnings("umicom-repo-publication-${_case}-test")
        umicom_apply_sanitizers("umicom-repo-publication-${_case}-test")
        add_test(NAME "framework.repository.publication.${_case}"
            COMMAND "umicom-repo-publication-${_case}-test")
        set_tests_properties("framework.repository.publication.${_case}" PROPERTIES
            LABELS "framework;repository;publication;regression" TIMEOUT 60)
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target("umicom-repo-publication-${_case}-test")
        endif()
    endforeach()
endif()

# Ship the public command guide with the Framework documentation.
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/guides/repository-publication.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/docs/guides"
    COMPONENT Framework)
