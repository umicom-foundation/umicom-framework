/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_experience_acceptance/test_main.c
 *
 * PURPOSE:
 *   Run focused invariant checks for every canonical Framework-owned application experience.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <stdio.h>

int test_application_experience_acceptance_studio(void);
int test_application_experience_acceptance_trader(void);
int test_application_experience_acceptance_ai_creator(void);
int test_application_experience_acceptance_bank(void);
int test_application_experience_acceptance_tms(void);
int test_application_experience_acceptance_llm(void);
int test_application_experience_acceptance_exchange(void);
int test_application_experience_acceptance_music(void);
int test_application_experience_acceptance_media(void);
int test_application_experience_acceptance_accountant(void);
int test_application_experience_acceptance_rag(void);
int test_application_experience_acceptance_desktop(void);
int test_application_experience_acceptance_os(void);
int test_application_experience_acceptance_games(void);
int test_application_experience_acceptance_kitchen(void);
int test_application_experience_acceptance_cad(void);
int test_application_experience_acceptance_author(void);
int test_application_experience_acceptance_web_studio(void);
int test_application_experience_acceptance_mobile_studio(void);
int test_application_experience_acceptance_database_studio(void);
int test_application_experience_acceptance_integration_studio(void);
int test_application_experience_acceptance_operations(void);
int test_application_experience_acceptance_security_centre(void);
int test_application_experience_acceptance_marketplace(void);
int test_application_experience_acceptance_education(void);

int main(void)
{
    if (test_application_experience_acceptance_studio() != 0) return 1;
    if (test_application_experience_acceptance_trader() != 0) return 2;
    if (test_application_experience_acceptance_ai_creator() != 0) return 3;
    if (test_application_experience_acceptance_bank() != 0) return 4;
    if (test_application_experience_acceptance_tms() != 0) return 5;
    if (test_application_experience_acceptance_llm() != 0) return 6;
    if (test_application_experience_acceptance_exchange() != 0) return 7;
    if (test_application_experience_acceptance_music() != 0) return 8;
    if (test_application_experience_acceptance_media() != 0) return 9;
    if (test_application_experience_acceptance_accountant() != 0) return 10;
    if (test_application_experience_acceptance_rag() != 0) return 11;
    if (test_application_experience_acceptance_desktop() != 0) return 12;
    if (test_application_experience_acceptance_os() != 0) return 13;
    if (test_application_experience_acceptance_games() != 0) return 14;
    if (test_application_experience_acceptance_kitchen() != 0) return 15;
    if (test_application_experience_acceptance_cad() != 0) return 16;
    if (test_application_experience_acceptance_author() != 0) return 17;
    if (test_application_experience_acceptance_web_studio() != 0) return 18;
    if (test_application_experience_acceptance_mobile_studio() != 0) return 19;
    if (test_application_experience_acceptance_database_studio() != 0) return 20;
    if (test_application_experience_acceptance_integration_studio() != 0) return 21;
    if (test_application_experience_acceptance_operations() != 0) return 22;
    if (test_application_experience_acceptance_security_centre() != 0) return 23;
    if (test_application_experience_acceptance_marketplace() != 0) return 24;
    if (test_application_experience_acceptance_education() != 0) return 25;
    puts("application experience acceptance tests passed");
    return 0;
}
