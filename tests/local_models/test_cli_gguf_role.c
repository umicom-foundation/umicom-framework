/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/local_models/test_cli_gguf_role.c
 *
 * PURPOSE:
 *   Check native GGUF role command admission, output honesty and optional-component failure without launching inference.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "../support/assertions.h"
#include "umicom/local_models/manager.h"
#include "umicom/model_formats/gguf.h"
#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
static char transcript[8192];static size_t used;static unsigned reads;static bool fail_output;
/* Fixture-specific state must not shadow locals in included production sources.
 * The previous declaration is retained for review. */
#if 0
static UmiStatus observed=UMI_STATUS_OK;
#endif
static UmiStatus fixture_status=UMI_STATUS_OK;
static UmiGgufRole selected_role=UMI_GGUF_ROLE_AUXILIARY;
static int role_printf(const char *format,...)
{
    va_list args;va_start(args,format);int n=vsnprintf(transcript+used,sizeof(transcript)-used,format,args);va_end(args);
    assert(n>=0 && (size_t)n<sizeof(transcript)-used);used+=(size_t)n;return n;
}
static int role_puts(const char *text) {return role_printf("%s\n",text);}
static int role_flush(FILE *stream) {return stream==stdout && fail_output?EOF:fflush(stream);}
#ifdef UMICOM_MODELS_HAS_TENSOR_INSPECTION
static UmiStatus role_inspect(const char *path,const UmiGgufFileIdentity *expected,
    const UmiCancellationToken *cancel,UmiGgufFileInspection *out)
{
    assert(strcmp(path,"fixture.gguf")==0 && expected==NULL && cancel==NULL && out!=NULL);++reads;
    if(fixture_status!=UMI_STATUS_OK)return fixture_status;
    memset(out,0,sizeof(*out));out->file.bytes=256U;out->metadata.role=selected_role;
    out->metadata.has_architecture=selected_role!=UMI_GGUF_ROLE_UNKNOWN;
    strcpy(out->metadata.architecture,selected_role==UMI_GGUF_ROLE_AUXILIARY?"controlvector":"llama");
    out->metadata.metadata_bytes=100U;out->metadata.entries=2U;out->metadata.tensors=1U;
    return UMI_STATUS_OK;
}
#endif
#define printf role_printf
#define puts role_puts
#define fflush role_flush
#ifdef UMICOM_MODELS_HAS_TENSOR_INSPECTION
#define UmiGgufFileInspect role_inspect
#endif
#include "../../src/local_models/cli.c"
#ifdef UMICOM_MODELS_HAS_TENSOR_INSPECTION
#undef UmiGgufFileInspect
#endif
#undef fflush
#undef puts
#undef printf
int main(int argc,char **argv)
{
    assert(argc==2);const char *mode=argv[1];
    UmiModelSession *session=NULL;char *command[]={"inspect-role","fixture.gguf","--apply"};
    int count=2,wanted=0;unsigned expected_reads=1U;
    if(strcmp(mode,"unavailable")==0) {
#ifndef UMICOM_MODELS_HAS_TENSOR_INSPECTION
        assert(UmiModelsCommandWithSession(count,command,&session)==1 && reads==0U && session==NULL);return 0;
#else
        return 2;
#endif
    }
#ifndef UMICOM_MODELS_HAS_TENSOR_INSPECTION
    return 2;
#endif
    if(strcmp(mode,"missing-path")==0){count=1;wanted=2;expected_reads=0U;}
    else if(strcmp(mode,"extra-argument")==0){count=3;wanted=2;expected_reads=0U;}
    else if(strcmp(mode,"null-path")==0){command[1]=NULL;wanted=2;expected_reads=0U;}
    else if(strcmp(mode,"null-owner")==0){assert(UmiModelsCommandWithSession(count,command,NULL)==2 && reads==0U);return 0;}
    else if(strcmp(mode,"occupied-owner")==0){max_align_t storage;session=(UmiModelSession *)&storage;assert(UmiModelsCommandWithSession(count,command,&session)==1 && reads==0U && session==(UmiModelSession *)&storage);return 0;}
    else if(strcmp(mode,"io-error")==0){fixture_status=UMI_STATUS_IO_ERROR;wanted=1;}
    else if(strcmp(mode,"bad-format")==0){fixture_status=UMI_STATUS_PARSE_ERROR;wanted=1;}
    else if(strcmp(mode,"bad-path")==0){fixture_status=UMI_STATUS_INVALID_ARGUMENT;wanted=2;}
    else if(strcmp(mode,"cancelled")==0){fixture_status=UMI_STATUS_CANCELLED;wanted=1;}
    else if(strcmp(mode,"output-error")==0){fail_output=true;wanted=1;}
    else if(strcmp(mode,"candidate")==0)selected_role=UMI_GGUF_ROLE_MODEL_CANDIDATE;
    else if(strcmp(mode,"unknown")==0)selected_role=UMI_GGUF_ROLE_UNKNOWN;
    else if(strcmp(mode,"vocabulary")==0)selected_role=UMI_GGUF_ROLE_VOCABULARY_ONLY;
    else if(strcmp(mode,"help")==0){command[0]="--help";count=1;expected_reads=0U;}
    else assert(strcmp(mode,"auxiliary")==0);
    assert(UmiModelsCommandWithSession(count,command,&session)==wanted && session==NULL && reads==expected_reads);
    if(strcmp(mode,"auxiliary")==0)assert(strstr(transcript,"Role: auxiliary\nArchitecture: controlvector")!=NULL);
    if(strcmp(mode,"candidate")==0)assert(strstr(transcript,"Role: model-candidate")!=NULL);
    if(strcmp(mode,"unknown")==0)assert(strstr(transcript,"Role: unknown\nArchitecture: unknown")!=NULL);
    if(strcmp(mode,"vocabulary")==0)assert(strstr(transcript,"Role: vocabulary-only")!=NULL);
    if(wanted==0 && expected_reads!=0U)assert(strstr(transcript,"remain unverified")!=NULL);
    if(fixture_status!=UMI_STATUS_OK)assert(strstr(transcript,"SHA256:")==NULL);
    if(strcmp(mode,"help")==0)assert(strstr(transcript,"inspect-role ABSOLUTE_GGUF")!=NULL);
    return 0;
}
