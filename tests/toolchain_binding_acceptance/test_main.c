/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_binding_acceptance/test_main.c
 *
 * PURPOSE:
 *   Run focused acceptance checks for every existing built-in developer toolchain binding.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <stdio.h>

int test_toolchain_binding_acceptance_c_clang(void);
int test_toolchain_binding_acceptance_c_gcc(void);
int test_toolchain_binding_acceptance_c_msvc(void);
int test_toolchain_binding_acceptance_cpp_clang(void);
int test_toolchain_binding_acceptance_cpp_gcc(void);
int test_toolchain_binding_acceptance_cpp_msvc(void);
int test_toolchain_binding_acceptance_assembly_llvm(void);
int test_toolchain_binding_acceptance_assembly_nasm(void);
int test_toolchain_binding_acceptance_umicc_default(void);
int test_toolchain_binding_acceptance_uai_default(void);
int test_toolchain_binding_acceptance_python_cpython(void);
int test_toolchain_binding_acceptance_rust_cargo(void);
int test_toolchain_binding_acceptance_zig_default(void);
int test_toolchain_binding_acceptance_java_jdk(void);
int test_toolchain_binding_acceptance_php_default(void);
int test_toolchain_binding_acceptance_javascript_node(void);
int test_toolchain_binding_acceptance_typescript_node(void);
int test_toolchain_binding_acceptance_html_web(void);
int test_toolchain_binding_acceptance_css_web(void);
int test_toolchain_binding_acceptance_markdown_default(void);
int test_toolchain_binding_acceptance_bash_default(void);
int test_toolchain_binding_acceptance_powershell_default(void);
int test_toolchain_binding_acceptance_cmake_default(void);
int test_toolchain_binding_acceptance_make_default(void);
int test_toolchain_binding_acceptance_json_default(void);
int test_toolchain_binding_acceptance_yaml_default(void);
int test_toolchain_binding_acceptance_toml_default(void);

int main(void)
{
    if (test_toolchain_binding_acceptance_c_clang() != 0) return 1;
    if (test_toolchain_binding_acceptance_c_gcc() != 0) return 2;
    if (test_toolchain_binding_acceptance_c_msvc() != 0) return 3;
    if (test_toolchain_binding_acceptance_cpp_clang() != 0) return 4;
    if (test_toolchain_binding_acceptance_cpp_gcc() != 0) return 5;
    if (test_toolchain_binding_acceptance_cpp_msvc() != 0) return 6;
    if (test_toolchain_binding_acceptance_assembly_llvm() != 0) return 7;
    if (test_toolchain_binding_acceptance_assembly_nasm() != 0) return 8;
    if (test_toolchain_binding_acceptance_umicc_default() != 0) return 9;
    if (test_toolchain_binding_acceptance_uai_default() != 0) return 10;
    if (test_toolchain_binding_acceptance_python_cpython() != 0) return 11;
    if (test_toolchain_binding_acceptance_rust_cargo() != 0) return 12;
    if (test_toolchain_binding_acceptance_zig_default() != 0) return 13;
    if (test_toolchain_binding_acceptance_java_jdk() != 0) return 14;
    if (test_toolchain_binding_acceptance_php_default() != 0) return 15;
    if (test_toolchain_binding_acceptance_javascript_node() != 0) return 16;
    if (test_toolchain_binding_acceptance_typescript_node() != 0) return 17;
    if (test_toolchain_binding_acceptance_html_web() != 0) return 18;
    if (test_toolchain_binding_acceptance_css_web() != 0) return 19;
    if (test_toolchain_binding_acceptance_markdown_default() != 0) return 20;
    if (test_toolchain_binding_acceptance_bash_default() != 0) return 21;
    if (test_toolchain_binding_acceptance_powershell_default() != 0) return 22;
    if (test_toolchain_binding_acceptance_cmake_default() != 0) return 23;
    if (test_toolchain_binding_acceptance_make_default() != 0) return 24;
    if (test_toolchain_binding_acceptance_json_default() != 0) return 25;
    if (test_toolchain_binding_acceptance_yaml_default() != 0) return 26;
    if (test_toolchain_binding_acceptance_toml_default() != 0) return 27;
    puts("toolchain binding acceptance tests passed");
    return 0;
}
