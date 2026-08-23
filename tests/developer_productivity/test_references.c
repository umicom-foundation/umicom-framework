/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_productivity/test_references.c
 *
 * PURPOSE:
 *   Verify reference lookup by symbol identity.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/developer_productivity/references.h"

int main(void)
{
    UmiDeveloperReferenceSet set;
    UmiDeveloperReference reference;
    UmiDeveloperReference matches[4];
    size_t count = 0U;

    umi_developer_reference_set_init(&set);
    (void)memset(&reference, 0, sizeof(reference));
    (void)strcpy(reference.symbol_id, "symbol.main");
    (void)strcpy(reference.location.uri, "src/main.c");
    reference.location.line = 5U;

    assert(umi_developer_reference_set_add(
        &set, &reference) == UMI_STATUS_OK);
    assert(umi_developer_reference_set_for_symbol(
        &set, "symbol.main", matches, 4U, &count) == UMI_STATUS_OK);
    assert(count == 1U);
    return 0;
}
