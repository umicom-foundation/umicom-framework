/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/local_models/test_cli_tensor.c
 *
 * PURPOSE:
 *   Verify tensor inspection command admission, reported identity and failure propagation without network or process execution.
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
#include "umicom/model_formats/safetensors_file.h"
#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
static char transcript[8192];
static size_t transcript_size;
static unsigned tensor_reads,gguf_reads;
static int fail_output;
static UmiStatus tensor_status=UMI_STATUS_OK;
static int tensor_printf(const char *format,...)
{
    va_list args;va_start(args,format);
    int n=vsnprintf(transcript+transcript_size,sizeof(transcript)-transcript_size,format,args);
    va_end(args);
    assert(n>=0 && (size_t)n<sizeof(transcript)-transcript_size);
    transcript_size+=(size_t)n;return n;
}
static int tensor_puts(const char *text) { return tensor_printf("%s\n",text); }
static int tensor_flush(FILE *stream) { return stream==stdout && fail_output?EOF:fflush(stream); }
#ifdef UMICOM_MODELS_HAS_TENSOR_INSPECTION
static UmiStatus tensor_inspect(const char *path,const UmiSafetensorsFileIdentity *expected,
    const UmiCancellationToken *cancel,UmiSafetensorsFileInspection *out)
{
    assert(strcmp(path,"fixture.safetensors")==0 && expected==NULL && cancel==NULL && out!=NULL);
    ++tensor_reads;
    if(tensor_status!=UMI_STATUS_OK) return tensor_status;
    memset(out,0,sizeof(*out));out->file.bytes=125U;
    out->layout.header_bytes=93U;out->layout.data_bytes=24U;
    out->layout.tensors=1U;out->layout.metadata_entries=1U;out->layout.elements=6U;out->layout.maximum_rank=2U;
    return UMI_STATUS_OK;
}
#endif
static UmiStatus tensor_gguf(const char *path,const char *expected,
    const UmiCancellationToken *cancel,UmiModelInspection *out)
{
    (void)expected;(void)cancel;assert(strcmp(path,"fixture.gguf")==0);
    ++gguf_reads;memset(out,0,sizeof(*out));out->bytes=24U;out->gguf_version=3U;
    return UMI_STATUS_OK;
}
/* Exercise the actual shared parser. Only inspection and stdout are injected;
 * these commands never select a download, network or process execution path. */
#define printf tensor_printf
#define puts tensor_puts
#define fflush tensor_flush
#define UmiModelInspect tensor_gguf
#ifdef UMICOM_MODELS_HAS_TENSOR_INSPECTION
#define UmiSafetensorsFileInspect tensor_inspect
#endif
#include "../../src/local_models/cli.c"
#ifdef UMICOM_MODELS_HAS_TENSOR_INSPECTION
#undef UmiSafetensorsFileInspect
#endif
#undef UmiModelInspect
#undef fflush
#undef puts
#undef printf
int main(int argc,char **argv)
{
    assert(argc==2);const char *mode=argv[1];
    UmiModelSession *session=NULL;
    char *command[]={"inspect-safetensors","fixture.safetensors","--apply"};
    int count=2,wanted=0;unsigned reads=1U;
    if(strcmp(mode,"unavailable")==0) {
#ifndef UMICOM_MODELS_HAS_TENSOR_INSPECTION
        assert(UmiModelsCommandWithSession(2,command,&session)==1);
        assert(tensor_reads==0U && session==NULL);
        char *help[]={"--help"};assert(UmiModelsCommandWithSession(1,help,&session)==0);
        assert(strstr(transcript,"Safetensors file inspection: unavailable")!=NULL);return 0;
#else
        return 2;
#endif
    }
#ifndef UMICOM_MODELS_HAS_TENSOR_INSPECTION
    return 2;
#endif
    if(strcmp(mode,"missing-path")==0) { count=1;wanted=2;reads=0U; }
    else if(strcmp(mode,"extra-argument")==0) { count=3;wanted=2;reads=0U; }
    else if(strcmp(mode,"null-path")==0) { command[1]=NULL;wanted=2;reads=0U; }
    else if(strcmp(mode,"null-owner")==0) {
        assert(UmiModelsCommandWithSession(2,command,NULL)==2 && tensor_reads==0U);return 0;
    } else if(strcmp(mode,"occupied-owner")==0) {
        max_align_t storage;session=(UmiModelSession *)&storage;
        assert(UmiModelsCommandWithSession(2,command,&session)==1);
        assert(session==(UmiModelSession *)&storage && tensor_reads==0U);return 0;
    } else if(strcmp(mode,"io-error")==0) { tensor_status=UMI_STATUS_IO_ERROR;wanted=1; }
    else if(strcmp(mode,"bad-format")==0) { tensor_status=UMI_STATUS_PARSE_ERROR;wanted=1; }
    else if(strcmp(mode,"bad-path")==0) { tensor_status=UMI_STATUS_INVALID_ARGUMENT;wanted=2; }
    else if(strcmp(mode,"cancelled")==0) { tensor_status=UMI_STATUS_CANCELLED;wanted=1; }
    else if(strcmp(mode,"output-error")==0) { fail_output=1;wanted=1; }
    else if(strcmp(mode,"gguf-compatible")==0) {
        command[0]="inspect";command[1]="fixture.gguf";reads=0U;
    } else if(strcmp(mode,"help")==0) {
        command[0]="--help";count=1;reads=0U;
    } else assert(strcmp(mode,"valid")==0);
    assert(UmiModelsCommandWithSession(count,command,&session)==wanted);
    assert(session==NULL && tensor_reads==reads);
    if(strcmp(mode,"gguf-compatible")==0) assert(gguf_reads==1U && strstr(transcript,"GGUF: v3")!=NULL);
    else assert(gguf_reads==0U);
    if(strcmp(mode,"valid")==0) {
        assert(strstr(transcript,"Bytes: 125\nSHA256: 0000000000000000000000000000000000000000000000000000000000000000")!=NULL);
        assert(strstr(transcript,"Safetensors: tensors: 1; metadata: 1; elements: 6")!=NULL);
        assert(strstr(transcript,"Header bytes: 93; tensor data bytes: 24; maximum rank: 2")!=NULL);
        assert(strstr(transcript,"remain unverified")!=NULL);
    }
    if(strcmp(mode,"help")==0) assert(strstr(transcript,"inspect-safetensors ABSOLUTE_FILE")!=NULL);
    if(tensor_status!=UMI_STATUS_OK) assert(strstr(transcript,"SHA256:")==NULL);
    return 0;
}
