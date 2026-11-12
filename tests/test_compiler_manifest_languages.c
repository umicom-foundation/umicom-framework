/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_compiler_manifest_languages.c
 *
 * PURPOSE:
 *   Verify bounded language projection, distinct masks and unchanged outputs on malformed manifests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/compiler/manifest.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { (void)fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c); goto cleanup; } } while (0)
int main(void)
{
    int result = 1;
    UmiCompilerProjectManifest *manifest = calloc(1U,sizeof(*manifest));
    UmiCompilerProjectManifest *saved = malloc(sizeof(*saved));
    UmiCompilerUnitManifest unit = {0};
    uint32_t mask = UINT32_C(0x5a5a5a5a);
    CHECK(manifest != NULL && saved != NULL);
    CHECK(umi_compiler_manifest_init(manifest,"project","Project","x86_64-unknown-linux-gnu") == UMI_STATUS_OK);
    CHECK(umi_compiler_manifest_languages(manifest,&mask) == UMI_STATUS_OK && mask == 0U);
    (void)strcpy(unit.unit_id,"c");
    (void)strcpy(unit.source,"first.c");
    (void)strcpy(unit.output,"first.o");
    unit.language = UMI_COMPILER_LANGUAGE_C;
    CHECK(umi_compiler_manifest_add_unit(manifest,&unit) == UMI_STATUS_OK);
    (void)strcpy(unit.unit_id,"second");
    (void)strcpy(unit.source,"second.c");
    CHECK(umi_compiler_manifest_add_unit(manifest,&unit) == UMI_STATUS_OK);
    (void)strcpy(unit.unit_id,"rust");
    (void)strcpy(unit.source,"third.rs");
    unit.language = UMI_COMPILER_LANGUAGE_RUST;
    CHECK(umi_compiler_manifest_add_unit(manifest,&unit) == UMI_STATUS_OK);
    CHECK(umi_compiler_manifest_languages(manifest,&mask) == UMI_STATUS_OK);
    CHECK(mask == (UMI_COMPILER_LANGUAGE_BIT(UMI_COMPILER_LANGUAGE_C) |
                   UMI_COMPILER_LANGUAGE_BIT(UMI_COMPILER_LANGUAGE_RUST)));
    mask = UINT32_C(0x5a5a5a5a);
    manifest->units[2].language = (UmiCompilerLanguage)99;
    CHECK(umi_compiler_manifest_languages(manifest,&mask) == UMI_STATUS_INVALID_STATE);
    CHECK(mask == UINT32_C(0x5a5a5a5a));
    manifest->units[2].language = UMI_COMPILER_LANGUAGE_RUST;
    (void)memset(manifest->units[2].source,'x',sizeof(manifest->units[2].source));
    CHECK(umi_compiler_manifest_languages(manifest,&mask) == UMI_STATUS_INVALID_STATE);
    CHECK(mask == UINT32_C(0x5a5a5a5a));
    (void)strcpy(manifest->units[2].source,"third.rs");
    manifest->unit_count = UMI_COMPILER_MAX_UNITS + 1U;
    CHECK(umi_compiler_manifest_languages(manifest,&mask) == UMI_STATUS_INVALID_STATE);
    CHECK(mask == UINT32_C(0x5a5a5a5a));
    manifest->unit_count = 3U;
    (void)memcpy(saved,manifest,sizeof(*saved));
    CHECK(umi_compiler_manifest_languages(manifest,(uint32_t *)(void *)&manifest->revision) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(saved,manifest,sizeof(*saved)) == 0);
    CHECK(umi_compiler_manifest_languages(NULL,&mask) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_compiler_manifest_languages(manifest,NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(mask == UINT32_C(0x5a5a5a5a));
    result = 0;
cleanup:
    free(saved);
    free(manifest);
    return result;
}
