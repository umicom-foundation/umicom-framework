# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Compose the native command only after its reusable VM dependencies exist.
include_guard(GLOBAL)
if(TARGET umicom)
    if(NOT TARGET Umicom::vm_manager)
        include("${CMAKE_CURRENT_LIST_DIR}/UmicomVmManager.cmake")
    endif()
    target_sources(umicom PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tools/umicom/src/command_qemu.c")
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
    endif()
endif()
