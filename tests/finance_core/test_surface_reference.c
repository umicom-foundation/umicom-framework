/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_surface_reference.c
 *
 * PURPOSE:
 *   Exercise the surface reference financial-core contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return 1; } while (0)
#include <string.h>
#include "umicom/finance/core/surface_reference.h"

int main(void)
{
    UmiSurfaceReference x; CHECK(umi_surface_reference_init(&x,"ID","Name","CODE")==UMI_STATUS_OK); CHECK(umi_surface_reference_is_valid(&x));
    return 0;
}
