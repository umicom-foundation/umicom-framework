# Umicom Framework
# File: cmake/UmicomCodingMaterializerAdmission.cmake
#
# PURPOSE:
# Register bounded workspace materialization and complete-value admission checks.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

if(BUILD_TESTING AND TARGET Umicom::developer)
    add_executable(umicom-ai-coding-materializer-admission-test tests/ai_coding_runtime/test_materializer_admission.c)
    target_link_libraries(umicom-ai-coding-materializer-admission-test PRIVATE Umicom::developer)
    set_target_properties(umicom-ai-coding-materializer-admission-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-ai-coding-materializer-admission-test)
    umicom_apply_sanitizers(umicom-ai-coding-materializer-admission-test)
    foreach(_case IN ITEMS null not-ready count path unsafe language summary priority tokens classification active-path active-unsafe duplicate token-overflow adapter-version adapter-read)
        set(_name "framework.ai.coding-materializer.plan.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-ai-coding-materializer-admission-test plan "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;context;admission")
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-materializer-admission-test)
    endforeach()
    foreach(_case IN ITEMS capacity-length size-max embedded-nul error late-error late-oversize)
        set(_name "framework.ai.coding-materializer.read.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-ai-coding-materializer-admission-test read "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;context;admission")
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-materializer-admission-test)
    endforeach()
    foreach(_case IN ITEMS normal empty exact-max without-terminator skips capacity snapshot zero-files token-maximum)
        set(_name "framework.ai.coding-materializer.success.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-ai-coding-materializer-admission-test success "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;context;admission")
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-materializer-admission-test)
    endforeach()
    foreach(_case IN ITEMS count skips path unsafe language length nul terminator hash tokens aggregate duplicate overflow)
        set(_name "framework.ai.coding-materializer.value.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-ai-coding-materializer-admission-test value "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;context;admission")
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-materializer-admission-test)
    endforeach()
    foreach(_case IN ITEMS null unterminated unsafe missing present alias)
        set(_name "framework.ai.coding-materializer.find.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-ai-coding-materializer-admission-test find "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;context;admission")
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-materializer-admission-test)
    endforeach()
endif()
