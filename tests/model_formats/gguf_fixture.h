/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/model_formats/gguf_fixture.h
 *
 * PURPOSE:
 *   Construct small synthetic GGUF metadata independently of the incremental reader.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_TEST_GGUF_FIXTURE_H
#define UMICOM_TEST_GGUF_FIXTURE_H
#include "umicom/model_formats/gguf.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define GGUF_CHECK(condition) do { if(!(condition)) { fprintf(stderr,"line %d: %s\n",__LINE__,#condition); return 1; } } while(0)
typedef struct GgufFixture { unsigned char bytes[8192]; size_t size; uint64_t entries; } GgufFixture;
static inline void fixture_number_at(unsigned char *out,uint64_t value,size_t width)
{
    for(size_t i=0U;i<width;++i) out[i]=(unsigned char)(value>>(8U*i));
}
static inline void fixture_number(GgufFixture *f,uint64_t value,size_t width)
{
    if(width>sizeof(f->bytes)-f->size) abort();
    fixture_number_at(f->bytes+f->size,value,width);f->size+=width;
}
static inline void fixture_bytes(GgufFixture *f,const void *bytes,size_t size)
{
    if(size>sizeof(f->bytes)-f->size) abort();
    if(size!=0U) memcpy(f->bytes+f->size,bytes,size);
    f->size+=size;
}
static inline void fixture_start(GgufFixture *f)
{
    memset(f,0,sizeof(*f));fixture_bytes(f,"GGUF",4U);
    fixture_number(f,3U,4U);fixture_number(f,1U,8U);fixture_number(f,0U,8U);
}
static inline void fixture_string(GgufFixture *f,const char *text)
{
    size_t size=strlen(text);fixture_number(f,size,8U);fixture_bytes(f,text,size);
}
static inline void fixture_key(GgufFixture *f,const char *key,uint32_t type)
{
    fixture_string(f,key);fixture_number(f,type,4U);
    ++f->entries;fixture_number_at(f->bytes+16U,f->entries,8U);
}
static inline void fixture_text(GgufFixture *f,const char *key,const char *value)
{
    fixture_key(f,key,8U);fixture_string(f,value);
}
static inline void fixture_model(GgufFixture *f)
{
    fixture_start(f);fixture_text(f,"general.architecture","llama");
}
#endif
