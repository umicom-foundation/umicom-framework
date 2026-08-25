/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_types.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the types enterprise data capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/types.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void) {
    char text[16];
    CHECK(umi_data_enterprise_copy_text(text, sizeof(text), "schema") == UMI_STATUS_OK);
    CHECK(strcmp(text, "schema") == 0);
    CHECK(umi_data_enterprise_copy_text(text, 3U, "schema") == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(umi_data_enterprise_hash64("abc", 3U, 0U) != 0U);
    CHECK(umi_data_enterprise_percent(1U, 4U) == 25.0);
    CHECK(strcmp(umi_data_enterprise_state_text(UMI_DATA_ENTERPRISE_READY), "ready") == 0);
    return 0;
}
