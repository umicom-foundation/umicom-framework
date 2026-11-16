/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/local_models/bundle_fixture.h
 *
 * PURPOSE:
 *   Provide synthetic component identities shared by model bundle and allocation regressions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_TEST_MODEL_BUNDLE_FIXTURE_H
#define UMICOM_TEST_MODEL_BUNDLE_FIXTURE_H
#include "umicom/local_models/bundle.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char bundle_repository[] = "umicom-fixtures/synthetic";
static const char bundle_revision[] = "0123456789abcdef0123456789abcdef01234567";
static const char bundle_digest[] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
typedef struct BundleFixture {
    UmiModelBundlePlan plan;
    unsigned char before[sizeof(UmiModelBundlePlan)];
    UmiModelBundleFile files[UMI_MODEL_BUNDLE_MAX_FILES];
} BundleFixture;

static UmiModelBundleFile bundle_fixture_file(const char *name,
    UmiModelArtifactFormat format, uint64_t bytes)
{
    UmiModelBundleFile file = {0};
    snprintf(file.filename,sizeof(file.filename),"%s",name);
    memcpy(file.sha256,bundle_digest,sizeof(bundle_digest));
    file.format=format;file.bytes=bytes;
    return file;
}
static BundleFixture *bundle_fixture_create(void)
{
    BundleFixture *f=calloc(1U,sizeof(*f));
    if(f==NULL) return NULL;
    f->files[0]=bundle_fixture_file("model.gguf",UMI_MODEL_ARTIFACT_GGUF,24U);
    f->files[1]=bundle_fixture_file("unet/model.safetensors",UMI_MODEL_ARTIFACT_SAFETENSORS,10U);
    f->files[2]=bundle_fixture_file("config.json",UMI_MODEL_ARTIFACT_JSON,2U);
    memset(&f->plan,0xa5,sizeof(f->plan));
    memcpy(f->before,&f->plan,sizeof(f->plan));
    return f;
}
#endif
