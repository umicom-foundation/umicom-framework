/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_legal_entity_registry.c
 *
 * PURPOSE:
 *   Exercise the legal entity registry financial-core contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return 1; } while (0)
#include <string.h>
#include "umicom/finance/core/legal_entity_registry.h"

int main(void)
{
    UmiLegalEntityRegistry r; UmiLegalEntity x; umi_legal_entity_registry_init(&r); CHECK(umi_legal_entity_init(&x,"LE","Entity","GB")==UMI_STATUS_OK); CHECK(umi_legal_entity_registry_add(&r,&x)==UMI_STATUS_OK);
    return 0;
}
