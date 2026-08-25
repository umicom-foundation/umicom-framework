/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_legal_entity.c
 *
 * PURPOSE:
 *   Exercise the legal entity financial-core contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return 1; } while (0)
#include <string.h>
#include "umicom/finance/core/legal_entity.h"

int main(void)
{
    UmiLegalEntity x; CHECK(umi_legal_entity_init(&x,"ID","Name","CODE")==UMI_STATUS_OK); CHECK(umi_legal_entity_is_valid(&x));
    return 0;
}
