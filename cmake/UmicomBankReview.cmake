# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Extend the canonical service, not another application-local banking engine.
include_guard(GLOBAL)
include(GNUInstallDirs)
if(NOT TARGET umicom_bank_operations)
    message(FATAL_ERROR "Bank review requires the canonical bank operations owner")
endif()
set(_umi_review_root "${CMAKE_CURRENT_LIST_DIR}/..")
target_sources(umicom_bank_operations PRIVATE
    "${_umi_review_root}/src/bank_operations/review.c"
    "${_umi_review_root}/src/bank_operations/review_text.c")
function(umicom_bank_review_target target)
    set_target_properties(${target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED ON C_EXTENSIONS OFF)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(${target})
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(${target})
    endif()
endfunction()
add_executable(umicom-bank-review
    "${_umi_review_root}/examples/bank_review/main.c"
    "${_umi_review_root}/examples/bank_review/lesson.c")
target_link_libraries(umicom-bank-review PRIVATE Umicom::bank_operations)
umicom_bank_review_target(umicom-bank-review)
install(TARGETS umicom-bank-review RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Framework)
install(FILES "${_umi_review_root}/docs/learning/bank-review.html"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/learning COMPONENT Learning)
if(BUILD_TESTING)
    # Include in the current scope: works in ordinary and deferred composition.
    include("${_umi_review_root}/tests/bank_review/CMakeLists.txt")
endif()
unset(_umi_review_root)
