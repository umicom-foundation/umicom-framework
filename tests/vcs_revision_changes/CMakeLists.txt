#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/vcs_revision_changes/CMakeLists.txt
# Purpose: Register committed-change parser regression scenarios.
# Author and Organisation: Sammy Hegab, Umicom Foundation
# Licence: MIT
#-----------------------------------------------------------------------------
add_executable(umicom-vcs-revision-changes-test "${CMAKE_CURRENT_LIST_DIR}/test_parse.c")
target_link_libraries(umicom-vcs-revision-changes-test PRIVATE Umicom::vcs)
set_target_properties(umicom-vcs-revision-changes-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
umicom_apply_warnings(umicom-vcs-revision-changes-test)
umicom_apply_sanitizers(umicom-vcs-revision-changes-test)
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-vcs-revision-changes-test)
endif()
foreach(_case IN ITEMS basic-sort rename-pair type gitlink sha256 empty malformed truncated duplicate bad-path capacity ownership invalid-request)
    set(_name "framework.vcs.revision_changes.${_case}")
    add_test(NAME "${_name}" COMMAND umicom-vcs-revision-changes-test "${_case}")
    set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;vcs;build-impact;regression")
    umicom_test_impact_owners("${_name}" TARGETS umicom-vcs-revision-changes-test)
endforeach()
