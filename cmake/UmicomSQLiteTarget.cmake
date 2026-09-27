# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Dependency naming belongs to Framework. Call after find_package(SQLite3).
# The guard protects only this function definition: imported targets and aliases
# must still be resolved in EACH calling directory, including sibling projects.
include_guard(GLOBAL)
function(umicom_resolve_sqlite_target out_target)
    if("${out_target}" STREQUAL "")
        message(FATAL_ERROR "A result variable is required for SQLite target resolution")
    endif()
    if(TARGET SQLite3::SQLite3)
        set(${out_target} SQLite3::SQLite3 PARENT_SCOPE)
    elseif(TARGET SQLite::SQLite3)
        # CMake before 4.3 supplies the older spelling. An alias preserves its
        # include paths and transitive dependencies without a second library.
        add_library(SQLite3::SQLite3 ALIAS SQLite::SQLite3)
        set(${out_target} SQLite3::SQLite3 PARENT_SCOPE)
    else()
        message(FATAL_ERROR "SQLite was requested but neither supported imported target is visible in this directory")
    endif()
endfunction()
