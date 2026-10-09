# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
include_guard(GLOBAL)
if(BUILD_TESTING)
    add_executable(umicom-native-arguments-copy-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/native_arguments/test_copy.c")
    add_executable(umicom-native-arguments-roundtrip-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/native_arguments/test_roundtrip.c")
    foreach(target IN ITEMS umicom-native-arguments-copy-test umicom-native-arguments-roundtrip-test)
        target_link_libraries(${target} PRIVATE Umicom::platform)
        umicom_apply_warnings(${target})
        umicom_apply_sanitizers(${target})
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${target})
        endif()
    endforeach()
    foreach(case IN ITEMS copy permutation empty raw-bytes invalid count-boundary byte-boundary)
        add_test(NAME framework.native_arguments.${case}
            COMMAND umicom-native-arguments-copy-test "${case}")
        set_tests_properties(framework.native_arguments.${case} PROPERTIES TIMEOUT 20
            LABELS "framework;platform;unicode;arguments;ownership")
    endforeach()
    add_test(NAME framework.native_arguments.native-roundtrip COMMAND umicom-native-arguments-roundtrip-test)
    set_tests_properties(framework.native_arguments.native-roundtrip PROPERTIES TIMEOUT 20
        LABELS "framework;platform;unicode;arguments;native")
endif()
