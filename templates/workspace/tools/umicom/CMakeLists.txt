#-----------------------------------------------------------------------------
# Umicom Framework
# File: templates/workspace/tools/umicom/CMakeLists.txt
#
# PURPOSE:
#   Provide reusable umicom source for projects created with Umicom tooling.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

set(UMICOM_TOOL_SOURCES src/main.c)

# Apply this branch only when its contract condition is satisfied.
if(UMICOM_BUILD_GTK4 AND TARGET PkgConfig::GTK4)
    list(APPEND UMICOM_TOOL_SOURCES src/wizard_gtk.c)
    set(UMICOM_TOOL_HAS_GTK4 ON)
# Use this fallback path when the earlier condition does not apply.
else()
    list(APPEND UMICOM_TOOL_SOURCES src/wizard_stub.c)
    set(UMICOM_TOOL_HAS_GTK4 OFF)
endif()

add_executable(umicom ${UMICOM_TOOL_SOURCES})
target_link_libraries(umicom PRIVATE Umicom::Framework)
target_include_directories(umicom PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_compile_definitions(umicom PRIVATE
    UMICOM_TEMPLATE_ROOT="${CMAKE_SOURCE_DIR}/templates"
)

# Apply this branch only when its contract condition is satisfied.
if(UMICOM_TOOL_HAS_GTK4)
    target_link_libraries(umicom PRIVATE PkgConfig::GTK4)
endif()

umicom_apply_warnings(umicom)
umicom_apply_sanitizers(umicom)

install(TARGETS umicom RUNTIME DESTINATION bin)
install(DIRECTORY "${CMAKE_SOURCE_DIR}/templates/" DESTINATION share/umicom/templates)
