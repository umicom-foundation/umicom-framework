/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vm_manager/test_codec.c
 * PURPOSE:
 *   Protocol data only. No transcript is represented as a QEMU execution.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Protocol data only. No transcript is represented as a QEMU execution. */
#include "umicom/vm_manager/qmp.h"
#include "../../src/vm_manager/json_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do{if(!(c)){fprintf(stderr,"line %d: %s\n",__LINE__,#c);return 1;}}while(0)
typedef struct Case {
    const char*name,*json;
    int accepted;
}
Case;
static const Case Cases[]={
    {
        "greeting","{\"QMP\":{\"version\":{\"qemu\":{\"major\":10,\"minor\":1,\"micro\":0},\"package\":\"\"},\"capabilities\":[]}}",1
    },  {
        "status","{\"id\":12,\"return\":{\"status\":\"paused\",\"singlestep\":false,\"running\":false}}\r\n",1
    },  {
        "event","{\"event\":\"STOP\",\"timestamp\":{\"seconds\":1,\"microseconds\":2}}",1
    },  {
        "error","{\"error\":{\"class\":\"GenericError\",\"desc\":\"Machine is not running\"},\"id\":3}",1
    },  {
        "console","{\"return\":\"aGVsbG8K\",\"id\":4}",1
    },  {
        "empty_console","{\"return\":\"\",\"id\":4}",1
    },  {
        "reordered","{\"return\":{\"running\":true,\"extra\":[null,3.14,1e4],\"status\":\"running\"},\"id\":7}",1
    },  {
        "escaped_key","{\"retu\\u0072n\":{},\"id\":5}",1
    },  {
        "unicode_pair","{\"error\":{\"class\":\"GenericError\",\"desc\":\"\\ud83d\\ude00\"},\"id\":1}",1
    },  {
        "utf8","{\"error\":{\"class\":\"GenericError\",\"desc\":\"caf\xc3\xa9\"},\"id\":1}",1
    },  {
        "duplicate_id","{\"return\":{},\"id\":1,\"id\":2}",0
    },  {
        "duplicate_escaped","{\"return\":{},\"id\":1,\"\\u0069d\":2}",0
    },  {
        "duplicate_nested","{\"return\":{\"running\":true,\"running\":false},\"id\":1}",0
    },  {
        "two_envelopes","{\"return\":{},\"event\":\"STOP\",\"id\":1}",0
    },  {
        "trailing_object","{\"return\":{},\"id\":1}{}",0
    },  {
        "trailing_comma","{\"return\":{},\"id\":1,}",0
    },  {
        "trailing_array_comma","{\"return\":{\"x\":[1,]},\"id\":1}",0
    },  {
        "unquoted_key","{return:{},\"id\":1}",0
    },  {
        "unknown_primitive","{\"return\":{\"x\":NaN},\"id\":1}",0
    },  {
        "leading_zero","{\"return\":{},\"id\":01}",0
    },  {
        "fraction_id","{\"return\":{},\"id\":1.5}",0
    },  {
        "negative_id","{\"return\":{},\"id\":-1}",0
    },  {
        "overflow_id","{\"return\":{},\"id\":18446744073709551616}",0
    },  {
        "string_id","{\"return\":{},\"id\":\"1\"}",0
    },  {
        "invalid_running","{\"return\":{\"running\":0,\"status\":\"paused\"},\"id\":1}",0
    },  {
        "event_with_id","{\"event\":\"STOP\",\"id\":1}",0
    },  {
        "greeting_bad_version","{\"QMP\":{\"version\":{\"qemu\":{\"major\":\"1\",\"minor\":0,\"micro\":0}},\"capabilities\":[]}}",0
    },  {
        "base64_padding","{\"return\":\"YQ=A\",\"id\":1}",0
    },  {
        "base64_noncanonical","{\"return\":\"YR==\",\"id\":1}",0
    },  {
        "base64_alphabet","{\"return\":\"!!!!\",\"id\":1}",0
    },  {
        "high_surrogate","{\"return\":{\"status\":\"\\ud800\"},\"id\":1}",0
    },  {
        "low_surrogate","{\"return\":{\"status\":\"\\udc00\"},\"id\":1}",0
    },  {
        "overlong_utf8","{\"return\":{\"status\":\"\xc0\xaf\"},\"id\":1}",0
    },  {
        "decoded_nul","{\"return\":{\"status\":\"\\u0000\"},\"id\":1}",0
    },  {
        "raw_control","{\"return\":{\"status\":\"a\001b\"},\"id\":1}",0
    },  {
        "wrong_root","[1,2]",0
    },  {
        "empty","",0
    },  {
        "empty_object","{}",0
    },  {
        "number_exponent","{\"return\":{\"x\":1e},\"id\":1}",0
    },  {
        "whitespace_control","{\"return\":{}\v,\"id\":1}",0
    }
};
int main(int argc,char**argv){
    CHECK(argc==2);
    UmiVmQmpMessage m;
    for(size_t i=0;i<sizeof Cases/sizeof Cases[0];++i)if(!strcmp(argv[1],Cases[i].name)){
        UmiStatus s=UmiVmQmpDecode(Cases[i].json,strlen(Cases[i].json),&m);
        CHECK((s==UMI_STATUS_OK)==Cases[i].accepted);
        return 0;
    }
    if(!strcmp(argv[1],"encode")){
        char b[4096];
        size_t n;
        for(int c=0;c<8;++c){
            CHECK(UmiVmQmpEncode((UmiVmCommand)c,55,NULL,0,b,sizeof b,&n)==UMI_STATUS_OK);
            CHECK(strstr(b,"\"id\":55"));
            CHECK(n>=2&&b[n-2]=='\r'&&b[n-1]=='\n');
        }
        CHECK(UmiVmQmpEncode((UmiVmCommand)99,1,NULL,0,b,sizeof b,&n)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiVmQmpEncode(UMI_VM_QUERY,0,NULL,0,b,sizeof b,&n)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiVmQmpEncode(UMI_VM_QUERY,1,NULL,0,b,3,&n)==UMI_STATUS_CAPACITY_EXCEEDED);
        return 0;
    }
    if(!strcmp(argv[1],"console_roundtrip")){
        unsigned char data[2048];
        for(size_t i=0;i<sizeof data;++i)data[i]=(unsigned char)i;
        char b[4096];
        size_t n;
        CHECK(UmiVmQmpEncode(UMI_VM_CONSOLE_WRITE,9,data,sizeof data,b,sizeof b,&n)==UMI_STATUS_OK);
        char *start=strstr(b,"\"data\":\"");
        CHECK(start);
        start+=8;
        char*end=strchr(start,'"');
        CHECK(end);
        *end=0;
        char reply[4096];
        int size=snprintf(reply,sizeof reply,"{\"return\":\"%s\",\"id\":9}",start);
        CHECK(size>0&&(size_t)size<sizeof reply);
        CHECK(UmiVmQmpDecode(reply,(size_t)size,&m)==UMI_STATUS_OK);
        CHECK(m.consoleLength==sizeof data&&!memcmp(data,m.console,sizeof data));
        return 0;
    }
    if(!strcmp(argv[1],"bounds")){
        char deep[1024];
        size_t n=0;
        n+=(size_t)sprintf(deep,"{\"return\":{\"x\":");
        for(int i=0;i<40;++i)deep[n++]='[';
        deep[n++]='0';
        for(int i=0;i<40;++i)deep[n++]=']';
        n+=(size_t)sprintf(deep+n,"},\"id\":1}");
        CHECK(UmiVmQmpDecode(deep,n,&m)!=UMI_STATUS_OK);
        char nul[]="{\"return\":{},\"id\":1}\0extra";
        CHECK(UmiVmQmpDecode(nul,sizeof nul-1,&m)!=UMI_STATUS_OK);
        char*large=calloc(UMI_VM_QMP_FRAME+2U,1);
        CHECK(large);
        CHECK(UmiVmQmpDecode(large,UMI_VM_QMP_FRAME+1U,&m)!=UMI_STATUS_OK);
        free(large);
        return 0;
    }
    if(!strcmp(argv[1],"mutation")){
        const char*valid="{\"return\":{\"running\":false,\"status\":\"paused\"},\"id\":1}";
        char b[256];
        unsigned state=17;
        size_t n=strlen(valid);
        for(unsigned i=0;i<10000;++i){
            memcpy(b,valid,n);
            state=state*1664525U+1013904223U;
            size_t at=state%n;
            state=state*1664525U+1013904223U;
            b[at]=(char)(state>>24);
            UmiStatus s=UmiVmQmpDecode(b,n,&m);
            CHECK(s==UMI_STATUS_OK||s==UMI_STATUS_PARSE_ERROR||s==UMI_STATUS_CAPACITY_EXCEEDED||s==UMI_STATUS_INVALID_ARGUMENT);
        }
        return 0;
    }
    return 2;
}
