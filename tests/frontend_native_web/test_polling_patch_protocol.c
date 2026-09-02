/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_native_web/test_polling_patch_protocol.c
 *
 * PURPOSE:
 *   Focused regression coverage for native-web polling patch protocol.
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
#include "umicom/frontend/native_web/polling_patch_protocol.h"
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); return 1; } } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiNativeWebPatchBatch b; UmiNativeWebMarkupBuffer out; umi_native_web_patch_batch_init(&b,1U); CHECK(umi_native_web_polling_patch_protocol_encode(&b,&out)==UMI_STATUS_OK); CHECK(out.length>0U);
    return 0;
}
