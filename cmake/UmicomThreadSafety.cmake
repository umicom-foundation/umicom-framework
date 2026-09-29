#-----------------------------------------------------------------------------
# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Compile the ownership lesson and tests against the real platform owner.
# A report-only platform subset does not necessarily contain threading.c.
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
include(GNUInstallDirs)
if(NOT TARGET Umicom::platform)
    return()
endif()
get_target_property(_umi_thread_owner Umicom::platform ALIASED_TARGET)
if(NOT _umi_thread_owner)
    set(_umi_thread_owner Umicom::platform)
endif()
get_target_property(_umi_thread_sources "${_umi_thread_owner}" SOURCES)
set(_umi_has_threads OFF)
foreach(_umi_source IN LISTS _umi_thread_sources)
    if(_umi_source MATCHES "(^|[/\\])src[/\\]platform[/\\]threading\\.c$")
        set(_umi_has_threads ON)
    endif()
endforeach()
if(NOT _umi_has_threads)
    # Do not synthesize a second implementation for a deliberately minimal host.
    return()
endif()
set(_umi_thread_root "${CMAKE_CURRENT_LIST_DIR}/..")

function(umicom_thread_safety_target target)
    set_target_properties(${target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
        if(UMICOM_THREAD_SAFETY_STRICT)
            target_compile_options(${target} PRIVATE -Werror)
        endif()
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(${target})
    endif()
    if(UMICOM_THREAD_SAFETY_SANITIZERS AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
        target_link_options(${target} PRIVATE -fsanitize=address,undefined)
    endif()
endfunction()

if(NOT TARGET umicom-thread-lifecycle)
    add_executable(umicom-thread-lifecycle "${_umi_thread_root}/examples/thread_safety/notes_worker.c")
    target_link_libraries(umicom-thread-lifecycle PRIVATE Umicom::platform)
    umicom_thread_safety_target(umicom-thread-lifecycle)
    include(GNUInstallDirs)
    install(TARGETS umicom-thread-lifecycle RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Framework)
endif()
if(BUILD_TESTING AND NOT TARGET umicom-test-thread-safety)
    # Include is valid in ordinary and deferred composition. Sources below use
    # CMAKE_CURRENT_LIST_DIR, not the caller's source directory.
    include("${_umi_thread_root}/tests/thread_safety/CMakeLists.txt")
endif()
install(FILES "${_umi_thread_root}/docs/learning/background-work-and-memory.html"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/learning COMPONENT Learning)
