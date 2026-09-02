/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_binding_acceptance/test_main.c
 *
 * PURPOSE:
 *   Run focused acceptance checks for every existing built-in developer toolchain binding.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdio.h>

/*
 * Exercise test toolchain binding acceptance c clang and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_toolchain_binding_acceptance_c_clang(void);
/*
 * Exercise test toolchain binding acceptance c gcc and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_toolchain_binding_acceptance_c_gcc(void);
/*
 * Exercise test toolchain binding acceptance c msvc and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_toolchain_binding_acceptance_c_msvc(void);
/*
 * Exercise test toolchain binding acceptance cpp clang and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_toolchain_binding_acceptance_cpp_clang(void);
/*
 * Exercise test toolchain binding acceptance cpp gcc and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_toolchain_binding_acceptance_cpp_gcc(void);
/*
 * Exercise test toolchain binding acceptance cpp msvc and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_toolchain_binding_acceptance_cpp_msvc(void);
/*
 * Exercise test toolchain binding acceptance assembly llvm and return a clear result when
 * the behaviour no longer matches its contract.
 */
int test_toolchain_binding_acceptance_assembly_llvm(void);
/*
 * Exercise test toolchain binding acceptance assembly nasm and return a clear result when
 * the behaviour no longer matches its contract.
 */
int test_toolchain_binding_acceptance_assembly_nasm(void);
/*
 * Exercise test toolchain binding acceptance umicc default and return a clear result when
 * the behaviour no longer matches its contract.
 */
int test_toolchain_binding_acceptance_umicc_default(void);
/*
 * Exercise test toolchain binding acceptance uai default and return a clear result when
 * the behaviour no longer matches its contract.
 */
int test_toolchain_binding_acceptance_uai_default(void);
/*
 * Exercise test toolchain binding acceptance python cpython and return a clear result when
 * the behaviour no longer matches its contract.
 */
int test_toolchain_binding_acceptance_python_cpython(void);
/*
 * Exercise test toolchain binding acceptance rust cargo and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_toolchain_binding_acceptance_rust_cargo(void);
/*
 * Exercise test toolchain binding acceptance zig default and return a clear result when
 * the behaviour no longer matches its contract.
 */
int test_toolchain_binding_acceptance_zig_default(void);
/*
 * Exercise test toolchain binding acceptance java jdk and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_toolchain_binding_acceptance_java_jdk(void);
/*
 * Exercise test toolchain binding acceptance php default and return a clear result when
 * the behaviour no longer matches its contract.
 */
int test_toolchain_binding_acceptance_php_default(void);
/*
 * Exercise test toolchain binding acceptance javascript node and return a clear result
 * when the behaviour no longer matches its contract.
 */
int test_toolchain_binding_acceptance_javascript_node(void);
/*
 * Exercise test toolchain binding acceptance typescript node and return a clear result
 * when the behaviour no longer matches its contract.
 */
int test_toolchain_binding_acceptance_typescript_node(void);
/*
 * Exercise test toolchain binding acceptance html web and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_toolchain_binding_acceptance_html_web(void);
/*
 * Exercise test toolchain binding acceptance css web and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_toolchain_binding_acceptance_css_web(void);
/*
 * Exercise test toolchain binding acceptance markdown default and return a clear result
 * when the behaviour no longer matches its contract.
 */
int test_toolchain_binding_acceptance_markdown_default(void);
/*
 * Exercise test toolchain binding acceptance bash default and return a clear result when
 * the behaviour no longer matches its contract.
 */
int test_toolchain_binding_acceptance_bash_default(void);
/*
 * Exercise test toolchain binding acceptance powershell default and return a clear result
 * when the behaviour no longer matches its contract.
 */
int test_toolchain_binding_acceptance_powershell_default(void);
/*
 * Exercise test toolchain binding acceptance cmake default and return a clear result when
 * the behaviour no longer matches its contract.
 */
int test_toolchain_binding_acceptance_cmake_default(void);
/*
 * Exercise test toolchain binding acceptance make default and return a clear result when
 * the behaviour no longer matches its contract.
 */
int test_toolchain_binding_acceptance_make_default(void);
/*
 * Exercise test toolchain binding acceptance json default and return a clear result when
 * the behaviour no longer matches its contract.
 */
int test_toolchain_binding_acceptance_json_default(void);
/*
 * Exercise test toolchain binding acceptance yaml default and return a clear result when
 * the behaviour no longer matches its contract.
 */
int test_toolchain_binding_acceptance_yaml_default(void);
/*
 * Exercise test toolchain binding acceptance toml default and return a clear result when
 * the behaviour no longer matches its contract.
 */
int test_toolchain_binding_acceptance_toml_default(void);

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_toolchain_binding_acceptance_c_clang() != 0) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_toolchain_binding_acceptance_c_gcc() != 0) return 2;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_toolchain_binding_acceptance_c_msvc() != 0) return 3;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_toolchain_binding_acceptance_cpp_clang() != 0) return 4;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_toolchain_binding_acceptance_cpp_gcc() != 0) return 5;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_toolchain_binding_acceptance_cpp_msvc() != 0) return 6;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_toolchain_binding_acceptance_assembly_llvm() != 0) return 7;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_toolchain_binding_acceptance_assembly_nasm() != 0) return 8;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_toolchain_binding_acceptance_umicc_default() != 0) return 9;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_toolchain_binding_acceptance_uai_default() != 0) return 10;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_toolchain_binding_acceptance_python_cpython() != 0) return 11;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_toolchain_binding_acceptance_rust_cargo() != 0) return 12;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_toolchain_binding_acceptance_zig_default() != 0) return 13;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_toolchain_binding_acceptance_java_jdk() != 0) return 14;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_toolchain_binding_acceptance_php_default() != 0) return 15;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_toolchain_binding_acceptance_javascript_node() != 0) return 16;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_toolchain_binding_acceptance_typescript_node() != 0) return 17;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_toolchain_binding_acceptance_html_web() != 0) return 18;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_toolchain_binding_acceptance_css_web() != 0) return 19;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_toolchain_binding_acceptance_markdown_default() != 0) return 20;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_toolchain_binding_acceptance_bash_default() != 0) return 21;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_toolchain_binding_acceptance_powershell_default() != 0) return 22;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_toolchain_binding_acceptance_cmake_default() != 0) return 23;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_toolchain_binding_acceptance_make_default() != 0) return 24;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_toolchain_binding_acceptance_json_default() != 0) return 25;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_toolchain_binding_acceptance_yaml_default() != 0) return 26;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_toolchain_binding_acceptance_toml_default() != 0) return 27;
    puts("toolchain binding acceptance tests passed");
    return 0;
}
