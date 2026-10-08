# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Pure planning tests do not require QEMU, guest images or a live process.
add_executable(umicom-vm-boot-plan-test test_boot_plan.c)
add_executable(umicom-vm-boot-cli-test test_boot_cli.c)
foreach(target IN ITEMS umicom-vm-boot-plan-test umicom-vm-boot-cli-test)
    target_link_libraries(${target} PRIVATE Umicom::vm_manager)
    set_target_properties(${target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(${target})
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(${target})
    endif()
endforeach()
foreach(case IN ITEMS umicom-kernel linux-x86_64 linux-riscv64 linux-aarch64 pc-iso pc-disk
    catalogue ownership drive-escaping linux-assets null missing-firmware relative-path
    unterminated-path control-path invalid-utf8 kernel-smp memory kernel-memory extra-initrd
    unknown-target arm-firmware linux-missing-kernel command-control command-unterminated disk-iso-conflict)
    add_test(NAME framework.vm_manager.boot.plan.${case} COMMAND umicom-vm-boot-plan-test ${case})
    set_tests_properties(framework.vm_manager.boot.plan.${case} PROPERTIES
        LABELS "framework;vm-manager;boot;plan" TIMEOUT 10)
endforeach()
foreach(case IN ITEMS plan duplicate unknown missing-value missing-review bad-number overflow
    duplicate-target unused-option plan-with-review)
    add_test(NAME framework.vm_manager.boot.cli.${case} COMMAND umicom-vm-boot-cli-test ${case})
    set_tests_properties(framework.vm_manager.boot.cli.${case} PROPERTIES
        LABELS "framework;vm-manager;boot;command" TIMEOUT 10)
endforeach()
add_test(NAME framework.vm_manager.boot.command.help COMMAND umicom-vm qemu --help)
add_test(NAME framework.vm_manager.boot.command.targets COMMAND umicom-vm qemu targets)

# A small real child process checks protocol ownership. It is an inert fixture,
# never an emulator or evidence that a Linux or Umicom guest has booted.
if(WIN32 OR CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_executable(umicom-vm-boot-peer boot_peer.c)
    add_executable(umicom-vm-boot-session-test test_boot_session.c)
    target_link_libraries(umicom-vm-boot-session-test PRIVATE Umicom::vm_manager)
    foreach(target IN ITEMS umicom-vm-boot-peer umicom-vm-boot-session-test)
        set_target_properties(${target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings(${target})
        endif()
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers(${target})
        endif()
    endforeach()
    if(MINGW)
        target_link_options(umicom-vm-boot-peer PRIVATE -static-libgcc)
    endif()
    foreach(case IN ITEMS stable-review changed-options changed-input empty-input bad-fingerprint
        running-child bad-handshake lifecycle report-alias)
        add_test(NAME framework.vm_manager.boot.session.${case}
            COMMAND umicom-vm-boot-session-test ${case}
                $<TARGET_FILE:umicom-vm-boot-peer> "${CMAKE_CURRENT_BINARY_DIR}")
        set_tests_properties(framework.vm_manager.boot.session.${case} PROPERTIES
            LABELS "framework;vm-manager;boot;native;inert-peer" TIMEOUT 30)
    endforeach()
endif()

# Configuration persistence uses the real Data Server, including its capacity
# limits and transaction ownership. SQLite absence is an explicit skip only.
add_executable(umicom-vm-boot-profiles-test test_boot_profiles.c)
target_link_libraries(umicom-vm-boot-profiles-test PRIVATE Umicom::vm_manager)
set_target_properties(umicom-vm-boot-profiles-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom-vm-boot-profiles-test)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom-vm-boot-profiles-test)
endif()
foreach(case IN ITEMS roundtrip conflict remove borrowed-transaction visit corrupt key-mismatch
    trailing capacity backend-capacity invalid)
    add_test(NAME framework.vm_manager.boot.profiles.${case} COMMAND umicom-vm-boot-profiles-test ${case})
    set_tests_properties(framework.vm_manager.boot.profiles.${case} PROPERTIES
        LABELS "framework;vm-manager;boot;data" TIMEOUT 20)
endforeach()
add_test(NAME framework.vm_manager.boot.profiles.sqlite
    COMMAND umicom-vm-boot-profiles-test sqlite "${CMAKE_CURRENT_BINARY_DIR}")
set_tests_properties(framework.vm_manager.boot.profiles.sqlite PROPERTIES
    LABELS "framework;vm-manager;boot;sqlite" SKIP_RETURN_CODE 77 TIMEOUT 20)
add_test(NAME framework.vm_manager.boot.profiles.help COMMAND umicom-vm qemu profiles --help)

add_test(NAME framework.vm_manager.boot.plan_lesson COMMAND umicom-vm-boot-plan-example)

add_executable(umicom-vm-boot-profile-cli-test test_boot_profile_cli.c)
target_link_libraries(umicom-vm-boot-profile-cli-test PRIVATE Umicom::vm_manager)
set_target_properties(umicom-vm-boot-profile-cli-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom-vm-boot-profile-cli-test)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom-vm-boot-profile-cli-test)
endif()
foreach(case IN ITEMS empty-name bad-id relative-database duplicate-database overflow-revision save-with-approval missing-target)
    add_test(NAME framework.vm_manager.boot.profile_command.${case} COMMAND umicom-vm-boot-profile-cli-test ${case})
    set_tests_properties(framework.vm_manager.boot.profile_command.${case} PROPERTIES
        LABELS "framework;vm-manager;boot;command" TIMEOUT 10)
endforeach()
add_test(NAME framework.vm_manager.boot.profile_command.flow
    COMMAND umicom-vm-boot-profile-cli-test flow "${CMAKE_CURRENT_BINARY_DIR}")
set_tests_properties(framework.vm_manager.boot.profile_command.flow PROPERTIES
    LABELS "framework;vm-manager;boot;command;sqlite" SKIP_RETURN_CODE 77 TIMEOUT 20)
