/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_regulatory/test_taxonomy_dimension.c
 *
 * PURPOSE:
 *   Implement the test taxonomy dimension behavior for
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

#include "umicom/finance/regulatory/taxonomy_dimension.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiTaxonomyDimension record;
    CHECK(umi_reg_taxonomy_dimension_init(&record, "dimension_id-1", "taxonomy_id-1", "Label", 1) == UMI_STATUS_OK);
    CHECK(record.dimension_id[0] != '\0');
    CHECK(record.taxonomy_id[0] != '\0');
    CHECK(record.label[0] != '\0');
    return 0;
}
