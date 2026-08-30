/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_regulatory/test_taxonomy_member.c
 *
 * PURPOSE:
 *   Implement the test taxonomy member behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/regulatory/taxonomy_member.h"

int main(void)
{
    UmiTaxonomyMember record;
    CHECK(umi_reg_taxonomy_member_init(&record, "member_id-1", "dimension_id-1", "Label", 1) == UMI_STATUS_OK);
    CHECK(record.member_id[0] != '\0');
    CHECK(record.dimension_id[0] != '\0');
    CHECK(record.label[0] != '\0');
    return 0;
}
