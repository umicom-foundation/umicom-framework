# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Add native shutdown acceptance to the canonical platform target; never fork it.
get_property(_umi_queue_registered GLOBAL PROPERTY UMICOM_QUEUE_SHUTDOWN_REGISTERED)
if(_umi_queue_registered OR NOT TARGET Umicom::platform)
    return()
endif()
get_target_property(_umi_queue_owner Umicom::platform ALIASED_TARGET)
if(NOT _umi_queue_owner)
    set(_umi_queue_owner Umicom::platform)
endif()
get_target_property(_umi_queue_sources "${_umi_queue_owner}" SOURCES)
foreach(_umi_queue_required IN ITEMS threading task task_queue)
    set(_umi_queue_found OFF)
    foreach(_umi_queue_source IN LISTS _umi_queue_sources)
        if(_umi_queue_source MATCHES "(^|[/\\])src[/\\]platform[/\\]${_umi_queue_required}\\.c$")
            set(_umi_queue_found ON)
        endif()
    endforeach()
    if(NOT _umi_queue_found)
        # A report-only host keeps its original composition. Registration is
        # not sealed until the real dependency sources have been declared.
        return()
    endif()
endforeach()
set_property(GLOBAL PROPERTY UMICOM_QUEUE_SHUTDOWN_REGISTERED TRUE)
include(GNUInstallDirs)
set(_umi_queue_shutdown_root "${CMAKE_CURRENT_LIST_DIR}/..")
option(UMICOM_QUEUE_SHUTDOWN_BUILD_EXAMPLE "Build the Notes queue shutdown lesson" ON)
if(UMICOM_QUEUE_SHUTDOWN_BUILD_EXAMPLE)
    add_executable(umicom-queue-shutdown
        "${_umi_queue_shutdown_root}/examples/queue_shutdown/notes_index.c")
    target_link_libraries(umicom-queue-shutdown PRIVATE Umicom::platform)
    set_target_properties(umicom-queue-shutdown PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-queue-shutdown)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-queue-shutdown)
    endif()
    install(TARGETS umicom-queue-shutdown
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Framework)
endif()
if(BUILD_TESTING)
    # This file can be reached during deferred completion. Include test targets
    # using their list-file paths; do not create a deferred CMake subdirectory.
    include("${_umi_queue_shutdown_root}/tests/queue_shutdown/CMakeLists.txt")
endif()
install(FILES "${_umi_queue_shutdown_root}/docs/learning/close-a-background-queue.html"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/learning COMPONENT Learning)
unset(_umi_queue_shutdown_root)
