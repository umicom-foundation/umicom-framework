# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Compose the native command only after its reusable VM dependencies exist.
include_guard(GLOBAL)
if(TARGET umicom)
    if(NOT TARGET Umicom::vm_manager)
        include("${CMAKE_CURRENT_LIST_DIR}/UmicomVmManager.cmake")
    endif()
    target_sources(umicom PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tools/umicom/src/command_qemu.c")
    # Framework-owned C23 qualification delegates builds and QEMU guest tests to
    # existing service contracts. It never installs a script or modifies Kernel.
    target_sources(umicom PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/../tools/umicom/src/kernel_qualification_plan.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tools/umicom/src/command_kernel_qualification.c")
    target_link_libraries(umicom PRIVATE Umicom::vm_manager)
    if(WIN32)
        # Decode original Windows Unicode arguments without changing other commands.
        target_link_libraries(umicom PRIVATE shell32)
    endif()
    if(BUILD_TESTING)
        add_test(NAME framework.command.qemu.help COMMAND umicom qemu --help)
        add_test(NAME framework.command.qemu.targets COMMAND umicom qemu targets)
        set_tests_properties(framework.command.qemu.help framework.command.qemu.targets
            PROPERTIES LABELS "framework;vm-manager;command" TIMEOUT 10)
        # The native plan is tested without an emulator or an unrelated OS tree.
        add_executable(umicom-kernel-qualification-plan-tests
            "${CMAKE_CURRENT_LIST_DIR}/../tools/umicom/src/kernel_qualification_plan.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/vm_manager/test_kernel_qualification_plan.c")
        target_include_directories(umicom-kernel-qualification-plan-tests PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tools/umicom/src")
        set_target_properties(umicom-kernel-qualification-plan-tests PROPERTIES
            C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings(umicom-kernel-qualification-plan-tests)
        endif()
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers(umicom-kernel-qualification-plan-tests)
        endif()
        add_test(NAME framework.command.qemu.kernel_qualification_plan
            COMMAND umicom-kernel-qualification-plan-tests)
        add_test(NAME framework.command.qemu.kernel_qualification_help
            COMMAND umicom qemu kernel-qualify --help)
        set_tests_properties(framework.command.qemu.kernel_qualification_plan
            framework.command.qemu.kernel_qualification_help PROPERTIES
            LABELS "framework;vm-manager;command;kernel" TIMEOUT 15)
    endif()
endif()
