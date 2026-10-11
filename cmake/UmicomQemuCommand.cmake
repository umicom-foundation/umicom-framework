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
    # Interactive C23 console qualification consumes the existing QMP VM
    # Manager and its process ownership. This is not a second QEMU launcher;
    # the Kernel remains independent and storage validation stays in CTest.
    target_sources(umicom PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/../tools/umicom/src/command_kernel_console.c")
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
        # The transport-neutral protocol is tested without QEMU or a host disk.
        add_executable(umicom-kernel-console-journey-tests
            "${CMAKE_CURRENT_LIST_DIR}/../src/vm_manager/console_journey.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/vm_manager/test_kernel_console_journey.c")
        target_include_directories(umicom-kernel-console-journey-tests PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../include")
        set_target_properties(umicom-kernel-console-journey-tests PROPERTIES
            C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings(umicom-kernel-console-journey-tests)
        endif()
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers(umicom-kernel-console-journey-tests)
        endif()
        foreach(_case IN ITEMS happy fragmented stale_boot bad_echo missing_expect unknown
                panic read_error write_error timeout clock_rollback clock_stalled output_limit
                invalid_transport invalid_count invalid_many invalid_step invalid_ascii invalid_timeout
                missing_prompt early_failure no_retry invalid_report no_input_empty)
            add_test(NAME "framework.command.qemu.console.${_case}"
                COMMAND umicom-kernel-console-journey-tests "${_case}")
        endforeach()
        # Test the actual QMP adapter with a fake session, not a fake parser.
        # This covers 2048-byte QMP replies split into 256-byte serial reads.
        add_executable(umicom-kernel-console-native-tests
            "${CMAKE_CURRENT_LIST_DIR}/../src/vm_manager/console_journey.c"
            "${CMAKE_CURRENT_LIST_DIR}/../src/vm_manager/console_journey_native.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/vm_manager/test_kernel_console_native.c")
        target_include_directories(umicom-kernel-console-native-tests PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../include")
        set_target_properties(umicom-kernel-console-native-tests PROPERTIES
            C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings(umicom-kernel-console-native-tests)
        endif()
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers(umicom-kernel-console-native-tests)
        endif()
        add_test(NAME framework.command.qemu.console.native_qmp
            COMMAND umicom-kernel-console-native-tests)
        set_tests_properties(framework.command.qemu.console.native_qmp PROPERTIES
            LABELS "framework;vm-manager;command;kernel" TIMEOUT 10)
        add_test(NAME framework.command.qemu.kernel_console_help
            COMMAND umicom qemu kernel-console --help)
        set_tests_properties(framework.command.qemu.kernel_console_help
            PROPERTIES LABELS "framework;vm-manager;command;kernel" TIMEOUT 10)
        set_tests_properties(framework.command.qemu.console.happy
            PROPERTIES LABELS "framework;vm-manager;command;kernel" TIMEOUT 10)
        add_test(NAME framework.command.qemu.kernel_qualification_plan
            COMMAND umicom-kernel-qualification-plan-tests)
        add_test(NAME framework.command.qemu.kernel_qualification_help
            COMMAND umicom qemu kernel-qualify --help)
        set_tests_properties(framework.command.qemu.kernel_qualification_plan
            framework.command.qemu.kernel_qualification_help PROPERTIES
            LABELS "framework;vm-manager;command;kernel" TIMEOUT 15)
    endif()
endif()
