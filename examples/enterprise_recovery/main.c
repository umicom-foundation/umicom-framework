/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/enterprise_recovery/main.c
 * PURPOSE: Offer explicit memory-only practice commands.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "lesson.h"
#include <stdio.h>
#include <string.h>
int main(int argc, char **argv)
{
    if (argc == 2 && strcmp(argv[1], "--self-test") == 0) return UmicomEnterpriseRecoveryLesson(false);
    if (argc == 2 && strcmp(argv[1], "demo") == 0) return UmicomEnterpriseRecoveryLesson(true);
    puts("umicom-enterprise-recovery --self-test | demo | --help\nMemory-only practice; no database path or external connector is opened.");
    return argc == 2 && strcmp(argv[1], "--help") == 0 ? 0 : 2;
}
