/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_experience/test_main.c
 *
 * PURPOSE:
 *   Execute focused application-experience contract tests.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>

int test_experience_catalogue(void);
int test_experience_registry(void);
int test_experience_status(void);
int test_experience_priority(void);
int test_experience_profiles(void);
int test_experience_portfolio_alignment(void);

int main(void)
{
    if (test_experience_catalogue() != 0) return 1;
    if (test_experience_registry() != 0) return 2;
    if (test_experience_status() != 0) return 3;
    if (test_experience_priority() != 0) return 4;
    if (test_experience_profiles() != 0) return 5;
    if (test_experience_portfolio_alignment() != 0) return 6;
    puts("application experience tests passed");
    return 0;
}
