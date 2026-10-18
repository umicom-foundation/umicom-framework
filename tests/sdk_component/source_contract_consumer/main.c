/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_component/source_contract_consumer/main.c
 * PURPOSE: Exercise the installed source-contract API through its public header.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/source_contracts/source_contracts.h"
#include <stdio.h>
#include <string.h>

int main(void)
{
    const char text[] = "/* File: example.c\n Purpose: Installed SDK probe.\n"
        " Author: Umicom Foundation\n Organisation: Umicom Foundation\n Licence: MIT\n */\n";
    UmiSourceAttribution attribution = {0};
    if (UmiSourceContractReadAttribution(text, strlen(text), &attribution) != 0 ||
        !attribution.leading_comment || !attribution.file || !attribution.purpose ||
        !attribution.author || !attribution.organisation || !attribution.licence) {
        (void)fputs("Installed source-contract library did not parse public attribution.\n", stderr);
        return 1;
    }
    (void)puts("Installed source-contract header and archive are usable.");
    return 0;
}
