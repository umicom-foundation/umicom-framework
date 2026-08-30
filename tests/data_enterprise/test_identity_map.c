/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_identity_map.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the identity map enterprise data capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/identity_map.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void) {
    UmiDataIdentityMap map; uint64_t token=0U; umi_data_identity_map_init(&map); CHECK(umi_data_identity_map_bind(&map,"Order","42",1001U)==UMI_STATUS_OK); CHECK(umi_data_identity_map_find(&map,"Order","42",&token)==UMI_STATUS_OK); CHECK(token==1001U); CHECK(umi_data_identity_map_bind(&map,"Order","42",2002U)==UMI_STATUS_INVALID_STATE);
    return 0;
}
