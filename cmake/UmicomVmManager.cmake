# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# QMP and profiles have one owner. Presentation remains in a native adapter.
include_guard(GLOBAL)
include(GNUInstallDirs)
include("${CMAKE_CURRENT_LIST_DIR}/UmicomProcessChannel.cmake")
if(NOT TARGET Umicom::setup_centre)
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomSetupCentre.cmake")
endif()
if(NOT TARGET Umicom::os_image)
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomOsImage.cmake")
endif()
if(NOT TARGET Umicom::data)
    message(FATAL_ERROR "VM profiles require canonical Umicom::data")
endif()
set(_umi_vm_root "${CMAKE_CURRENT_LIST_DIR}/..")
add_library(umicom_vm_manager STATIC
    "${_umi_vm_root}/src/vm_manager/json.c"
    "${_umi_vm_root}/src/vm_manager/qmp.c"
    "${_umi_vm_root}/src/vm_manager/text.c"
    "${_umi_vm_root}/src/vm_manager/profile.c"
    "${_umi_vm_root}/src/vm_manager/runtime.c"
    "${_umi_vm_root}/src/vm_manager/inventory.c"
    "${_umi_vm_root}/src/vm_manager/lease.c"
    "${_umi_vm_root}/src/vm_manager/disk.c"
    "${_umi_vm_root}/src/vm_manager/session.c"
    "${_umi_vm_root}/src/vm_manager/cli.c")
add_library(Umicom::vm_manager ALIAS umicom_vm_manager)
set_target_properties(umicom_vm_manager PROPERTIES EXPORT_NAME vm_manager C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_include_directories(umicom_vm_manager PUBLIC
    $<BUILD_INTERFACE:${_umi_vm_root}/include> $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(umicom_vm_manager PUBLIC Umicom::data Umicom::os_image Umicom::setup_centre Umicom::process_channel)
add_executable(umicom-vm "${_umi_vm_root}/examples/vm_manager/main.c")
target_link_libraries(umicom-vm PRIVATE Umicom::vm_manager)
add_executable(umicom-vm-profile-example "${_umi_vm_root}/examples/vm_manager/profile_lesson.c")
target_link_libraries(umicom-vm-profile-example PRIVATE Umicom::vm_manager)
foreach(_target IN ITEMS umicom_vm_manager umicom-vm umicom-vm-profile-example)
    set_target_properties(${_target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(${_target})
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(${_target})
    endif()
endforeach()
if(MINGW)
    target_link_options(umicom-vm PRIVATE -municode -static-libgcc)
endif()
if(WIN32)
    enable_language(RC)
    add_executable(umicom-vm-manager WIN32
        "${_umi_vm_root}/examples/vm_manager/windows_main.c"
        "${_umi_vm_root}/adapters/win32/vm_manager.c"
        "${_umi_vm_root}/examples/setup_centre/resources/setup.rc")
    target_include_directories(umicom-vm-manager PRIVATE "${_umi_vm_root}/examples/setup_centre/resources")
    target_link_libraries(umicom-vm-manager PRIVATE Umicom::vm_manager comctl32 comdlg32 user32 gdi32 shell32 ole32 uuid)
    target_compile_definitions(umicom-vm-manager PRIVATE UNICODE _UNICODE)
    set_target_properties(umicom-vm-manager PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(MINGW)
        target_link_options(umicom-vm-manager PRIVATE -municode -static-libgcc)
    endif()
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-vm-manager)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-vm-manager)
    endif()
    if(COMMAND umicom_prepare_windows_application)
        umicom_prepare_windows_application(TARGET umicom-vm-manager
            PRODUCT_NAME "Umicom Virtual Machine Manager" RESOURCE_ROOT "${_umi_vm_root}/resources" GUI)
    endif()
    install(TARGETS umicom-vm-manager RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Framework)
endif()
install(TARGETS umicom_vm_manager EXPORT UmicomFrameworkTargets ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Framework)
install(TARGETS umicom-vm RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Framework)
install(DIRECTORY "${_umi_vm_root}/include/umicom/vm_manager" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom COMPONENT Framework)
install(FILES "${_umi_vm_root}/docs/learning/run-umicom-in-a-virtual-machine.html" DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/learning COMPONENT Learning)
if(BUILD_TESTING)
    add_subdirectory("${_umi_vm_root}/tests/vm_manager" "${CMAKE_CURRENT_BINARY_DIR}/vm-manager-tests")
endif()
unset(_umi_vm_root)
