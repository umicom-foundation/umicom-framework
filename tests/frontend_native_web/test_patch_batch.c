/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_native_web/test_patch_batch.c
 *
 * PURPOSE:
 *   Focused regression coverage for native-web patch batch.
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
#include "umicom/frontend/native_web/patch_batch.h"
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); return 1; } } while (0)
int main(void)
{
    UmiNativeWebPatchBatch b; UmiNativeWebPatch p; umi_native_web_patch_batch_init(&b,3U); CHECK(umi_native_web_patch_init(&p,UMI_NATIVE_WEB_PATCH_FOCUS,"x",NULL,NULL,3U)==UMI_STATUS_OK); CHECK(umi_native_web_patch_batch_add(&b,&p)==UMI_STATUS_OK); CHECK(b.count==1U);
    return 0;
}
