# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
include_guard(GLOBAL)
if(NOT TARGET umicom_finance_operations)
    message(FATAL_ERROR "Close review requires the canonical financial operations owner")
endif()
set(_umi_close_root "${CMAKE_CURRENT_LIST_DIR}/..")
target_sources(umicom_finance_operations PRIVATE "${_umi_close_root}/src/finance_operations/close_review.c")
# The containing helper already installs the finance_operations public headers.
add_executable(umicom-finance-close-review "${_umi_close_root}/examples/finance_close_review/main.c"
    "${_umi_close_root}/examples/finance_close_review/lesson.c")
target_link_libraries(umicom-finance-close-review PRIVATE Umicom::finance_operations)
umicom_finance_operations_configure_target(umicom-finance-close-review)
install(TARGETS umicom-finance-close-review RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Framework)
install(FILES "${_umi_close_root}/docs/learning/finance-close-review.html"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/learning COMPONENT Learning)
if(BUILD_TESTING)
    # Include definitions with explicit paths. No late test subdirectory is created.
    include("${_umi_close_root}/tests/finance_close_review/CMakeLists.txt")
endif()
unset(_umi_close_root)
