/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_native_web/test_patch.c
 *
 * PURPOSE:
 *   Focused regression coverage for native-web patch.
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
#include "umicom/frontend/native_web/patch.h"
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); return 1; } } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiNativeWebPatch p,q; CHECK(umi_native_web_patch_init(&p,UMI_NATIVE_WEB_PATCH_SET_TEXT,"x","","a",1U)==UMI_STATUS_OK); CHECK(umi_native_web_patch_init(&q,UMI_NATIVE_WEB_PATCH_SET_TEXT,"x","","b",2U)==UMI_STATUS_OK); CHECK(umi_native_web_patch_same_slot(&p,&q));
    return 0;
}
