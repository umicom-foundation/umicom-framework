/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_native_web/test_asset_bundle.c
 *
 * PURPOSE:
 *   Focused regression coverage for native-web asset bundle.
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
#include "umicom/frontend/native_web/asset_bundle.h"
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); return 1; } } while (0)
int main(void)
{
    UmiNativeWebAssetBundle b; UmiNativeWebAsset a={0}; umi_native_web_asset_bundle_init(&b); strcpy(a.path,"/umi.js"); strcpy(a.content_type,"text/javascript"); CHECK(umi_native_web_asset_bundle_add(&b,&a)==UMI_STATUS_OK); CHECK(umi_native_web_asset_bundle_add(&b,&a)==UMI_STATUS_ALREADY_EXISTS);
    return 0;
}
