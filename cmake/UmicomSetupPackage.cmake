# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Declarative build integration. Actual report parsing, hashing, selection and
# packaging are in the native C executable; no new script implementation.
include_guard(GLOBAL)
function(umicom_setup_package_targets)
    get_property(_targets GLOBAL PROPERTY UMICOM_WINDOWS_DEPLOYMENT_TARGETS)
    set(_list "UMICOM_SETUP_INPUT\t1\n")
    set(_count 0)
    set(_selected "")
    foreach(_target IN LISTS _targets)
        get_target_property(_gui "${_target}" UMICOM_WINDOWS_DEPLOYMENT_GUI)
        get_target_property(_exclude "${_target}" UMICOM_WINDOWS_INSTALLER_EXCLUDE)
        if(NOT _gui OR _exclude)
            continue()
        endif()
        if(UMICOM_WINDOWS_INSTALLER_TARGETS AND NOT _target IN_LIST UMICOM_WINDOWS_INSTALLER_TARGETS)
            continue()
        endif()
        get_target_property(_name "${_target}" UMICOM_WINDOWS_DEPLOYMENT_PRODUCT)
        if(_name MATCHES "[\t\r\n]")
            message(FATAL_ERROR "Installer product names must not contain control characters")
        endif()
        string(APPEND _list "app\t${_target}\t${_name}\t${CMAKE_BINARY_DIR}/umicom-deployment/$<CONFIG>/${_target}.runtime.cmake\n")
        list(APPEND _selected "${_target}")
        math(EXPR _count "${_count}+1")
    endforeach()
    if(_count EQUAL 0)
        return()
    endif()
    foreach(_helper IN LISTS UMICOM_WINDOWS_INSTALLER_HELPERS)
        if(TARGET "${_helper}" AND NOT _helper IN_LIST _selected)
            string(APPEND _list "shared\t${_helper}\t${_helper}\t${CMAKE_BINARY_DIR}/umicom-deployment/$<CONFIG>/${_helper}.runtime.cmake\n")
        endif()
    endforeach()
    get_filename_component(_framework "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/.." ABSOLUTE)
    string(APPEND _list "data\tshare/umicom/licences/Umicom-LICENSE.txt\t${_framework}/LICENSE\n")
    string(APPEND _list "data\tshare/umicom/docs/install-and-try-umicom.html\t${_framework}/docs/learning/install-and-try-umicom.html\n")
    string(APPEND _list "data\tbin/umicom-setup.exe\t$<TARGET_FILE:umicom-setup>\n")
    # The installed inspector is available without a development toolchain.
    string(APPEND _list "data\tbin/umicom-release-inspect.exe\t$<TARGET_FILE:umicom-release-inspect>\n")
    string(APPEND _list "data\tshare/umicom/docs/check-a-windows-release.html\t${_framework}/docs/learning/check-a-windows-release.html\n")
    # Maintenance is native Framework code. Keep its CLI beside installed
    # tools, but run updates from the separately extracted release bootstrap.
    if(TARGET umicom-maintain)
        string(APPEND _list "data\tbin/umicom-maintain.exe\t$<TARGET_FILE:umicom-maintain>\n")
        string(APPEND _list "data\tshare/umicom/docs/maintain-your-umicom-applications.html\t${_framework}/docs/learning/maintain-your-umicom-applications.html\n")
    endif()
    # Native-generated provider records are data only. The C packer validates
    # every owner/path and hashes copied bytes; CMake does not implement copying.
    set(UMICOM_QEMU_COMPONENT_INPUT "" CACHE FILEPATH "Optional reviewed native QEMU component TSV")
    if("umicom-vm-manager" IN_LIST _selected)
        string(APPEND _list "owned\tumicom-vm-manager\tbin/umicom-vm.exe\t$<TARGET_FILE:umicom-vm>\n")
        string(APPEND _list "owned\tumicom-vm-manager\tshare/umicom/docs/run-umicom-in-a-virtual-machine.html\t${_framework}/docs/learning/run-umicom-in-a-virtual-machine.html\n")
        if(UMICOM_QEMU_COMPONENT_INPUT)
            file(READ "${UMICOM_QEMU_COMPONENT_INPUT}" _runtime_component LIMIT 1048577)
            string(LENGTH "${_runtime_component}" _runtime_length)
            if(_runtime_length GREATER 1048576)
                message(FATAL_ERROR "QEMU component inventory exceeds its bound")
            endif()
            string(APPEND _list "${_runtime_component}")
        endif()
    elseif(UMICOM_QEMU_COMPONENT_INPUT)
        message(FATAL_ERROR "QEMU component requested without selecting umicom-vm-manager")
    endif()
    set(_input "${CMAKE_BINARY_DIR}/umicom-deployment/$<CONFIG>/native-suite.tsv")
    file(GENERATE OUTPUT "${_input}" CONTENT "${_list}")
    set(UMICOM_SETUP_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/packages/$<CONFIG>/native-suite" CACHE STRING
        "New output directory for a reviewed native installer release; an existing directory is never overwritten")
    get_filename_component(_output_parent "${UMICOM_SETUP_OUTPUT_DIRECTORY}" DIRECTORY)
    add_custom_target(umicom-native-installer
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${_output_parent}"
        COMMAND "$<TARGET_FILE:umicom-setup>" pack --list "${_input}"
            --output "${UMICOM_SETUP_OUTPUT_DIRECTORY}" --bootstrap "$<TARGET_FILE:umicom-setup-centre>"
        # Failure retains the new directory for diagnosis, but fails the build
        # target. The report is observation evidence, not a signed release gate.
        COMMAND "$<TARGET_FILE:umicom-release-inspect>" release
            --root "${UMICOM_SETUP_OUTPUT_DIRECTORY}"
            --output "${UMICOM_SETUP_OUTPUT_DIRECTORY}/release-inspection.json"
        DEPENDS umicom-setup umicom-setup-centre umicom-windows-runtime
        COMMENT "Preparing native Umicom Setup and its explicit offline payload"
        VERBATIM)
    add_dependencies(umicom-native-installer umicom-release-inspect)
    if(TARGET umicom-maintain)
        add_dependencies(umicom-native-installer umicom-maintain)
    endif()
    if(TARGET umicom-vm AND "umicom-vm-manager" IN_LIST _selected)
        add_dependencies(umicom-native-installer umicom-vm)
    endif()
endfunction()
