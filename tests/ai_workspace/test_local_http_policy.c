/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_workspace/test_local_http_policy.c
 *
 * PURPOSE:
 *   Observe exact loopback options and force every assignment failure without creating a transfer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "local_http_internal.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#define CHECK(c) do{if(!(c)){fprintf(stderr,"%d: %s\n",__LINE__,#c);return 1;}}while(0)
typedef struct Option {
    CURLoption option;long number;curl_off_t offset;char string[96];
    const void *pointer;curl_write_callback write;curl_xferinfo_callback progress;
} Option;
static Option observed[64];static size_t calls,fail_call;static int unexpected;
static CURLcode PolicySetopt(CURL *curl,CURLoption option,...)
{
    (void)curl;
    if(calls>=sizeof observed/sizeof observed[0]){unexpected=1;return CURLE_UNKNOWN_OPTION;}
    Option *entry=&observed[calls++];entry->option=option;
    va_list args;va_start(args,option);
    switch(option){
    case CURLOPT_URL:case CURLOPT_PROXY:case CURLOPT_NOPROXY:
    case CURLOPT_PROTOCOLS_STR:case CURLOPT_REDIR_PROTOCOLS_STR:case CURLOPT_ACCEPT_ENCODING:{
        const char *text=va_arg(args,const char *);
        if(text==NULL || strlen(text)>=sizeof entry->string)unexpected=1;
        else memcpy(entry->string,text,strlen(text)+1U);
        break;
    }
    case CURLOPT_HTTPHEADER:entry->pointer=va_arg(args,struct curl_slist *);break;
    case CURLOPT_POSTFIELDS:entry->pointer=va_arg(args,const char *);break;
    case CURLOPT_WRITEDATA:case CURLOPT_HEADERDATA:case CURLOPT_XFERINFODATA:
        entry->pointer=va_arg(args,void *);break;
    case CURLOPT_WRITEFUNCTION:case CURLOPT_HEADERFUNCTION:entry->write=va_arg(args,curl_write_callback);break;
    case CURLOPT_XFERINFOFUNCTION:entry->progress=va_arg(args,curl_xferinfo_callback);break;
    case CURLOPT_POSTFIELDSIZE_LARGE:entry->offset=va_arg(args,curl_off_t);break;
    case CURLOPT_POST:case CURLOPT_HTTPGET:case CURLOPT_FOLLOWLOCATION:case CURLOPT_MAXREDIRS:
    case CURLOPT_NETRC:case CURLOPT_NOSIGNAL:case CURLOPT_CONNECTTIMEOUT_MS:case CURLOPT_TIMEOUT_MS:
    case CURLOPT_NOPROGRESS:case CURLOPT_HTTP_VERSION:case CURLOPT_HTTP_CONTENT_DECODING:
        entry->number=va_arg(args,long);break;
    default:unexpected=1;break;
    }
    va_end(args);return unexpected || (fail_call!=0U && calls==fail_call)?CURLE_UNKNOWN_OPTION:CURLE_OK;
}
/* Intercept only option assignment in the actual policy source. No fake easy
 * handle is passed to libcurl, and no network transfer/global setup is called. */
#ifdef curl_easy_setopt
#undef curl_easy_setopt
#endif
#define curl_easy_setopt PolicySetopt
#include "../../src/ai_workspace/local_http_policy.c"
#undef curl_easy_setopt
static size_t Write(char *bytes,size_t size,size_t count,void *context)
{(void)bytes;(void)context;return size*count;}
static size_t Header(char *bytes,size_t size,size_t count,void *context)
{(void)bytes;(void)context;return size*count;}
static int Progress(void *context,curl_off_t a,curl_off_t b,curl_off_t c,curl_off_t d)
{(void)context;(void)a;(void)b;(void)c;(void)d;return 0;}
static const Option *Find(CURLoption option)
{
    for(size_t i=0U;i<calls;++i)if(observed[i].option==option)return &observed[i];
    return NULL;
}
static int Number(CURLoption option,long value)
{const Option *entry=Find(option);CHECK(entry!=NULL && entry->number==value);return 0;}
static int String(CURLoption option,const char *value)
{const Option *entry=Find(option);CHECK(entry!=NULL && strcmp(entry->string,value)==0);return 0;}
static void Reset(size_t fail)
{memset(observed,0,sizeof observed);calls=0U;fail_call=fail;unexpected=0;}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    int write_context=1,header_context=2,progress_context=3;
    AwLocalHttpCallbacks callbacks={Write,&write_context,NULL,&header_context,Progress,&progress_context};
    CURL *handle=(CURL *)(uintptr_t)1U;
    struct curl_slist *headers=(struct curl_slist *)(uintptr_t)2U;
    const char *json="{\"stream\":true}";
    const char *mode=argv[1];bool failures=strncmp(mode,"fail-",5U)==0;if(failures)mode+=5;
    if(strcmp(mode,"arguments")==0){
        CHECK(AwLocalHttpConfigure(NULL,8080U,1000U,json,headers,&callbacks,false)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(AwLocalHttpConfigure(handle,1023U,1000U,json,headers,&callbacks,false)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(AwLocalHttpConfigure(handle,8080U,99U,json,headers,&callbacks,false)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(AwLocalHttpConfigure(handle,8080U,60001U,json,headers,&callbacks,false)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(AwLocalHttpConfigure(handle,8080U,1000U,json,headers,NULL,false)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(AwLocalHttpConfigure(handle,8080U,1000U,json,NULL,&callbacks,false)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(AwLocalHttpConfigure(handle,8080U,1000U,json,headers,&callbacks,true)==UMI_STATUS_INVALID_ARGUMENT);
        callbacks.header=Header;
        CHECK(AwLocalHttpConfigure(handle,8080U,1000U,NULL,headers,&callbacks,true)==UMI_STATUS_INVALID_ARGUMENT);
        callbacks.write=NULL;
        CHECK(AwLocalHttpConfigure(handle,8080U,1000U,json,headers,&callbacks,false)==UMI_STATUS_INVALID_ARGUMENT);
        callbacks.write=Write;callbacks.progress=NULL;
        CHECK(AwLocalHttpConfigure(handle,8080U,1000U,json,headers,&callbacks,false)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(calls==0U);return 0;
    }
    bool health=strcmp(mode,"health")==0,stream=strcmp(mode,"stream")==0;
    if(!health && !stream && strcmp(mode,"chat")!=0)return 2;
    uint16_t port=health?1024U:stream?65535U:8080U;
    uint32_t timeout=health?100U:stream?250U:60000U;
    if(stream)callbacks.header=Header;
    Reset(0U);
    CHECK(AwLocalHttpConfigure(handle,port,timeout,health?NULL:json,health?NULL:headers,&callbacks,stream)==UMI_STATUS_OK);
    CHECK(!unexpected);
    const size_t total=calls;CHECK(total==(health?17U:stream?25U:20U));
    CHECK(String(CURLOPT_URL,health?"http://127.0.0.1:1024/health":stream?"http://127.0.0.1:65535/v1/chat/completions":"http://127.0.0.1:8080/v1/chat/completions")==0);
    CHECK(String(CURLOPT_PROXY,"")==0 && String(CURLOPT_NOPROXY,"*")==0);
    CHECK(String(CURLOPT_PROTOCOLS_STR,"http")==0 && String(CURLOPT_REDIR_PROTOCOLS_STR,"http")==0);
    CHECK(Number(CURLOPT_FOLLOWLOCATION,0L)==0 && Number(CURLOPT_MAXREDIRS,0L)==0);
    CHECK(Number(CURLOPT_NETRC,(long)CURL_NETRC_IGNORED)==0 && Number(CURLOPT_NOSIGNAL,1L)==0);
    CHECK(Number(CURLOPT_TIMEOUT_MS,(long)timeout)==0 && Number(CURLOPT_CONNECTTIMEOUT_MS,(long)(timeout<3000U?timeout:3000U))==0);
    CHECK(Number(CURLOPT_NOPROGRESS,0L)==0);
    CHECK(Find(CURLOPT_WRITEFUNCTION)!=NULL && Find(CURLOPT_WRITEFUNCTION)->write==Write);
    CHECK(Find(CURLOPT_WRITEDATA)!=NULL && Find(CURLOPT_WRITEDATA)->pointer==&write_context);
    CHECK(Find(CURLOPT_XFERINFOFUNCTION)!=NULL && Find(CURLOPT_XFERINFOFUNCTION)->progress==Progress);
    CHECK(Find(CURLOPT_XFERINFODATA)!=NULL && Find(CURLOPT_XFERINFODATA)->pointer==&progress_context);
    if(health){
        CHECK(Number(CURLOPT_HTTPGET,1L)==0 && Find(CURLOPT_POST)==NULL && Find(CURLOPT_POSTFIELDS)==NULL);
    }else{
        CHECK(Number(CURLOPT_POST,1L)==0 && Find(CURLOPT_HTTPGET)==NULL);
        CHECK(Find(CURLOPT_HTTPHEADER)!=NULL && Find(CURLOPT_HTTPHEADER)->pointer==headers);
        CHECK(Find(CURLOPT_POSTFIELDS)!=NULL && Find(CURLOPT_POSTFIELDS)->pointer==json);
        CHECK(Find(CURLOPT_POSTFIELDSIZE_LARGE)!=NULL && Find(CURLOPT_POSTFIELDSIZE_LARGE)->offset==(curl_off_t)strlen(json));
    }
    if(stream){
        CHECK(Number(CURLOPT_HTTP_VERSION,(long)CURL_HTTP_VERSION_1_1)==0);
        CHECK(Number(CURLOPT_HTTP_CONTENT_DECODING,0L)==0 && String(CURLOPT_ACCEPT_ENCODING,"identity")==0);
        CHECK(Find(CURLOPT_HEADERFUNCTION)!=NULL && Find(CURLOPT_HEADERFUNCTION)->write==Header);
        CHECK(Find(CURLOPT_HEADERDATA)!=NULL && Find(CURLOPT_HEADERDATA)->pointer==&header_context);
    }else CHECK(Find(CURLOPT_HTTP_VERSION)==NULL && Find(CURLOPT_HEADERFUNCTION)==NULL && Find(CURLOPT_ACCEPT_ENCODING)==NULL);
    if(failures){
        for(size_t i=1U;i<=total;++i){
            Reset(i);
            CHECK(AwLocalHttpConfigure(handle,port,timeout,health?NULL:json,health?NULL:headers,&callbacks,stream)==UMI_STATUS_UNAVAILABLE);
            CHECK(calls==i && !unexpected);
        }
    }
    return 0;
}
