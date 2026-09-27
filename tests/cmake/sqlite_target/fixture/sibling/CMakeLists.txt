# The legacy imported target is intentionally directory-local in each sibling.
add_library(SQLite::SQLite3 INTERFACE IMPORTED)
umicom_resolve_sqlite_target(chosen)
get_filename_component(name "${CMAKE_CURRENT_BINARY_DIR}" NAME)
add_executable("consumer-${name}" ../main.c)
target_link_libraries("consumer-${name}" PRIVATE ${chosen})
