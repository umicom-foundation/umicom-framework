# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Add capabilities to the existing authority, not an application-local store.
include_guard(GLOBAL)
set(_ews_recovery_root "${CMAKE_CURRENT_LIST_DIR}/..")
if(NOT TARGET umicom_enterprise_workspace)
    message(FATAL_ERROR "Enterprise recovery requires the canonical workspace")
endif()
target_sources(umicom_enterprise_workspace PRIVATE
    "${_ews_recovery_root}/src/enterprise_workspace/query.c"
    "${_ews_recovery_root}/src/enterprise_workspace/recovery.c"
    "${_ews_recovery_root}/src/enterprise_workspace/recovery_report.c")
add_executable(umicom-enterprise-recovery
    "${_ews_recovery_root}/examples/enterprise_recovery/main.c"
    "${_ews_recovery_root}/examples/enterprise_recovery/lesson.c")
target_link_libraries(umicom-enterprise-recovery PRIVATE Umicom::enterprise_workspace)
umicom_enterprise_configure_target(umicom-enterprise-recovery)
install(TARGETS umicom-enterprise-recovery RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
install(FILES "${_ews_recovery_root}/docs/learning/ENTERPRISE_RECOVERY.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/docs/learning" COMPONENT Framework)
if(BUILD_TESTING)
    # Include is valid both during ordinary and deferred component composition.
    include("${_ews_recovery_root}/tests/enterprise_recovery/CMakeLists.txt")
endif()
unset(_ews_recovery_root)
