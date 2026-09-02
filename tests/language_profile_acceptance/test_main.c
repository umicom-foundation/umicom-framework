/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_profile_acceptance/test_main.c
 *
 * PURPOSE:
 *   Run focused acceptance checks for every existing built-in language profile.
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
 * Exercise test language profile acceptance definition catalogue and return a clear result
 * when the behaviour no longer matches its contract.
 */
int test_language_profile_acceptance_definition_catalogue(void);
/*
 * Exercise test language profile acceptance c and return a clear result when the behaviour
 * no longer matches its contract.
 */
int test_language_profile_acceptance_c(void);
/*
 * Exercise test language profile acceptance cpp and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_language_profile_acceptance_cpp(void);
/*
 * Exercise test language profile acceptance assembly and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_language_profile_acceptance_assembly(void);
/*
 * Exercise test language profile acceptance umicc and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_language_profile_acceptance_umicc(void);
/*
 * Exercise test language profile acceptance uai and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_language_profile_acceptance_uai(void);
/*
 * Exercise test language profile acceptance python and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_language_profile_acceptance_python(void);
/*
 * Exercise test language profile acceptance rust and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_language_profile_acceptance_rust(void);
/*
 * Exercise test language profile acceptance zig and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_language_profile_acceptance_zig(void);
/*
 * Exercise test language profile acceptance java and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_language_profile_acceptance_java(void);
/*
 * Exercise test language profile acceptance php and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_language_profile_acceptance_php(void);
/*
 * Exercise test language profile acceptance javascript and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_language_profile_acceptance_javascript(void);
/*
 * Exercise test language profile acceptance typescript and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_language_profile_acceptance_typescript(void);
/*
 * Exercise test language profile acceptance html and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_language_profile_acceptance_html(void);
/*
 * Exercise test language profile acceptance css and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_language_profile_acceptance_css(void);
/*
 * Exercise test language profile acceptance markdown and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_language_profile_acceptance_markdown(void);
/*
 * Exercise test language profile acceptance bash and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_language_profile_acceptance_bash(void);
/*
 * Exercise test language profile acceptance powershell and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_language_profile_acceptance_powershell(void);
/*
 * Exercise test language profile acceptance cmake and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_language_profile_acceptance_cmake(void);
/*
 * Exercise test language profile acceptance make and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_language_profile_acceptance_make(void);
/*
 * Exercise test language profile acceptance json and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_language_profile_acceptance_json(void);
/*
 * Exercise test language profile acceptance yaml and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_language_profile_acceptance_yaml(void);
/*
 * Exercise test language profile acceptance toml and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_language_profile_acceptance_toml(void);

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_language_profile_acceptance_c() != 0) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_language_profile_acceptance_cpp() != 0) return 2;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_language_profile_acceptance_assembly() != 0) return 3;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_language_profile_acceptance_umicc() != 0) return 4;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_language_profile_acceptance_uai() != 0) return 5;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_language_profile_acceptance_python() != 0) return 6;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_language_profile_acceptance_rust() != 0) return 7;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_language_profile_acceptance_zig() != 0) return 8;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_language_profile_acceptance_java() != 0) return 9;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_language_profile_acceptance_php() != 0) return 10;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_language_profile_acceptance_javascript() != 0) return 11;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_language_profile_acceptance_typescript() != 0) return 12;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_language_profile_acceptance_html() != 0) return 13;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_language_profile_acceptance_css() != 0) return 14;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_language_profile_acceptance_markdown() != 0) return 15;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_language_profile_acceptance_bash() != 0) return 16;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_language_profile_acceptance_powershell() != 0) return 17;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_language_profile_acceptance_cmake() != 0) return 18;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_language_profile_acceptance_make() != 0) return 19;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_language_profile_acceptance_json() != 0) return 20;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_language_profile_acceptance_yaml() != 0) return 21;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_language_profile_acceptance_toml() != 0) return 22;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_language_profile_acceptance_definition_catalogue() != 0) return 100;
    puts("language profile acceptance tests passed");
    return 0;
}
