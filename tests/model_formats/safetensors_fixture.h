/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/model_formats/safetensors_fixture.h
 *
 * PURPOSE:
 *   Construct synthetic header prefixes for native model-format regressions without model files or inference.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_SAFETENSORS_FIXTURE_H
#define UMICOM_TEST_SAFETENSORS_FIXTURE_H
#include "umicom/model_formats/safetensors.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static const char tensor_fixture_json[]=
    "{\"weight\":{\"dtype\":\"F32\",\"shape\":[2,3],\"data_offsets\":[0,24]},"
    "\"__metadata__\":{\"format\":\"pt\"}}";
static void tensor_fixture_length(unsigned char *prefix,uint64_t length)
{
    for(size_t i=0U;i<8U;++i) prefix[i]=(unsigned char)(length>>(8U*i));
}
static unsigned char *tensor_fixture_prefix(const char *json,size_t extra,size_t *length)
{
    size_t header=strlen(json);
    *length=8U+header+extra;
    unsigned char *prefix=malloc(*length);
    if(prefix==NULL) return NULL;
    tensor_fixture_length(prefix,(uint64_t)header);
    memcpy(prefix+8U,json,header);
    memset(prefix+8U+header,0xcd,extra);
    return prefix;
}
#endif
