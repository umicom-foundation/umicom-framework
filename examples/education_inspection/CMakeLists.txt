# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
cmake_minimum_required(VERSION 3.24)
project(UmicomEducationInspectionExample LANGUAGES C)

# An SDK consumer names its component, not a repository path. An enclosing
# Framework build may already provide the identical source-tree target.
if(NOT TARGET Umicom::education_inspection)
    find_package(UmicomFramework CONFIG REQUIRED COMPONENTS education_inspection)
endif()
add_executable(umicom-education-inspect-example main.c)
target_link_libraries(umicom-education-inspect-example PRIVATE Umicom::education_inspection)
set_target_properties(umicom-education-inspect-example PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(COMMAND umicom_education_inspection_configure_target)
    umicom_education_inspection_configure_target(umicom-education-inspect-example)
elseif(MSVC)
    target_compile_options(umicom-education-inspect-example PRIVATE /W4)
else()
    target_compile_options(umicom-education-inspect-example PRIVATE
        -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()
