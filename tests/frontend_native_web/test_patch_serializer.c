/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_native_web/test_patch_serializer.c
 *
 * PURPOSE:
 *   Focused regression coverage for native-web patch serializer.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include "umicom/frontend/native_web/patch_serializer.h"
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); return 1; } } while (0)
int main(void)
{
    UmiNativeWebPatchBatch b; UmiNativeWebPatch p; UmiNativeWebMarkupBuffer out; umi_native_web_patch_batch_init(&b,1U); CHECK(umi_native_web_patch_init(&p,UMI_NATIVE_WEB_PATCH_SET_TEXT,"x",NULL,"hello",1U)==UMI_STATUS_OK); CHECK(umi_native_web_patch_batch_add(&b,&p)==UMI_STATUS_OK); CHECK(umi_native_web_patch_serialize(&b,&out)==UMI_STATUS_OK); CHECK(strstr(out.data,"\"t\":\"x\"")!=NULL);
    return 0;
}
