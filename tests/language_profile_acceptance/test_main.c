/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_profile_acceptance/test_main.c
 *
 * PURPOSE:
 *   Run focused acceptance checks for every existing built-in language profile.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <stdio.h>

int test_language_profile_acceptance_definition_catalogue(void);
int test_language_profile_acceptance_c(void);
int test_language_profile_acceptance_cpp(void);
int test_language_profile_acceptance_assembly(void);
int test_language_profile_acceptance_umicc(void);
int test_language_profile_acceptance_uai(void);
int test_language_profile_acceptance_python(void);
int test_language_profile_acceptance_rust(void);
int test_language_profile_acceptance_zig(void);
int test_language_profile_acceptance_java(void);
int test_language_profile_acceptance_php(void);
int test_language_profile_acceptance_javascript(void);
int test_language_profile_acceptance_typescript(void);
int test_language_profile_acceptance_html(void);
int test_language_profile_acceptance_css(void);
int test_language_profile_acceptance_markdown(void);
int test_language_profile_acceptance_bash(void);
int test_language_profile_acceptance_powershell(void);
int test_language_profile_acceptance_cmake(void);
int test_language_profile_acceptance_make(void);
int test_language_profile_acceptance_json(void);
int test_language_profile_acceptance_yaml(void);
int test_language_profile_acceptance_toml(void);

int main(void)
{
    if (test_language_profile_acceptance_c() != 0) return 1;
    if (test_language_profile_acceptance_cpp() != 0) return 2;
    if (test_language_profile_acceptance_assembly() != 0) return 3;
    if (test_language_profile_acceptance_umicc() != 0) return 4;
    if (test_language_profile_acceptance_uai() != 0) return 5;
    if (test_language_profile_acceptance_python() != 0) return 6;
    if (test_language_profile_acceptance_rust() != 0) return 7;
    if (test_language_profile_acceptance_zig() != 0) return 8;
    if (test_language_profile_acceptance_java() != 0) return 9;
    if (test_language_profile_acceptance_php() != 0) return 10;
    if (test_language_profile_acceptance_javascript() != 0) return 11;
    if (test_language_profile_acceptance_typescript() != 0) return 12;
    if (test_language_profile_acceptance_html() != 0) return 13;
    if (test_language_profile_acceptance_css() != 0) return 14;
    if (test_language_profile_acceptance_markdown() != 0) return 15;
    if (test_language_profile_acceptance_bash() != 0) return 16;
    if (test_language_profile_acceptance_powershell() != 0) return 17;
    if (test_language_profile_acceptance_cmake() != 0) return 18;
    if (test_language_profile_acceptance_make() != 0) return 19;
    if (test_language_profile_acceptance_json() != 0) return 20;
    if (test_language_profile_acceptance_yaml() != 0) return 21;
    if (test_language_profile_acceptance_toml() != 0) return 22;
    if (test_language_profile_acceptance_definition_catalogue() != 0) return 100;
    puts("language profile acceptance tests passed");
    return 0;
}
