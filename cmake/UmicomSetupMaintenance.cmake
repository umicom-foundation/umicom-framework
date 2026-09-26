#-----------------------------------------------------------------------------
# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Maintenance shares the installer catalogue and checked filesystem backend.
# No second package model, interpreter, eraser or application-local engine.
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
if(NOT TARGET umicom_setup_centre)
    message(FATAL_ERROR "Maintenance requires the canonical Setup Centre target")
endif()
get_filename_component(_umi_maintenance_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
target_sources(umicom_setup_centre PRIVATE
    "${_umi_maintenance_root}/src/setup_centre/maintenance_plan.c"
    "${_umi_maintenance_root}/src/setup_centre/maintenance_codec.c"
    "${_umi_maintenance_root}/src/setup_centre/maintenance_transaction.c"
    "${_umi_maintenance_root}/src/setup_centre/maintenance_cli.c")
add_executable(umicom-maintain "${_umi_maintenance_root}/examples/setup_maintenance/main.c")
target_link_libraries(umicom-maintain PRIVATE Umicom::setup_centre)
set_target_properties(umicom-maintain PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(MINGW)
    target_link_options(umicom-maintain PRIVATE -municode -static-libgcc)
endif()
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom-maintain)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom-maintain)
endif()
install(TARGETS umicom-maintain RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Framework)
if(BUILD_TESTING)
    add_subdirectory("${_umi_maintenance_root}/tests/setup_maintenance" "${CMAKE_CURRENT_BINARY_DIR}/setup-maintenance-tests")
endif()
install(FILES "${_umi_maintenance_root}/docs/learning/maintain-your-umicom-applications.html"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/learning COMPONENT Learning)
unset(_umi_maintenance_root)
