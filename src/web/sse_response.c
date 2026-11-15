/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/sse_response.c
 *
 * PURPOSE:
 *   Gate event-stream bodies with bounded HTTP response evidence and shared field validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/web/sse_response.h"
#include "connection_internal.h"
#include <stdlib.h>
#include <string.h>
struct UmiWebSseResponse {
    UmiWebSseResponseLimits limits;
    UmiWebSseResponseSnapshot snapshot;
    bool content_type,content_encoding,http11;
};
static UmiStatus ResponseFail(UmiWebSseResponse *response,UmiStatus status)
{
    response->snapshot.state=UMI_WEB_SSE_RESPONSE_FAILED;
    response->snapshot.failure=status;return status;
}
UmiStatus UmiWebSseResponseCreate(const UmiWebSseResponseLimits *limits,UmiWebSseResponse **out)
{
    if(limits==NULL || out==NULL || limits->line_bytes<2U || limits->line_bytes>65536U ||
        limits->total_header_bytes<limits->line_bytes || limits->total_header_bytes>1048576U ||
        limits->header_lines==0U || limits->header_lines>4096U ||
        limits->informational_responses>16U || limits->body_bytes==0U)return UMI_STATUS_INVALID_ARGUMENT;
    UmiWebSseResponse *response=calloc(1U,sizeof *response);
    if(response==NULL)return UMI_STATUS_OUT_OF_MEMORY;
    response->limits=*limits;*out=response;return UMI_STATUS_OK;
}
void UmiWebSseResponseDestroy(UmiWebSseResponse *response){free(response);}
static void Trim(const unsigned char **bytes,size_t *length)
{
    while(*length>0U && ((*bytes)[0]==' ' || (*bytes)[0]=='\t')){++*bytes;--*length;}
    while(*length>0U && ((*bytes)[*length-1U]==' ' || (*bytes)[*length-1U]=='\t'))--*length;
}
static UmiStatus MediaType(const unsigned char *value,size_t bytes)
{
    const unsigned char *separator=memchr(value,';',bytes);
    size_t type_bytes=separator?(size_t)(separator-value):bytes;
    const unsigned char *type=value;Trim(&type,&type_bytes);
    if(!UmiWebFieldEqual(type,type_bytes,"text/event-stream"))return UMI_STATUS_UNAVAILABLE;
    if(separator==NULL)return UMI_STATUS_OK;
    const unsigned char *parameter=separator+1U;size_t parameter_bytes=bytes-(size_t)(parameter-value);
    Trim(&parameter,&parameter_bytes);
    const unsigned char *equals=memchr(parameter,'=',parameter_bytes);
    if(equals==NULL)return UMI_STATUS_UNAVAILABLE;
    size_t name_bytes=(size_t)(equals-parameter);const unsigned char *name=parameter;
    Trim(&name,&name_bytes);
    if(!UmiWebFieldEqual(name,name_bytes,"charset"))return UMI_STATUS_UNAVAILABLE;
    const unsigned char *charset=equals+1U;size_t charset_bytes=parameter_bytes-(size_t)(charset-parameter);
    Trim(&charset,&charset_bytes);
    if(charset_bytes>=2U && charset[0]=='"' && charset[charset_bytes-1U]=='"'){
        ++charset;charset_bytes-=2U;
    }
    return UmiWebFieldEqual(charset,charset_bytes,"utf-8")?UMI_STATUS_OK:UMI_STATUS_UNAVAILABLE;
}
static UmiStatus StatusLine(UmiWebSseResponse *response,const unsigned char *line,size_t bytes)
{
    if(bytes<12U || (memcmp(line,"HTTP/1.0 ",9U)!=0 && memcmp(line,"HTTP/1.1 ",9U)!=0))
        return UMI_STATUS_PARSE_ERROR;
    if(line[9]<'1' || line[9]>'5' || line[10]<'0' || line[10]>'9' ||
        line[11]<'0' || line[11]>'9' || (bytes>12U && line[12]!=' '))return UMI_STATUS_PARSE_ERROR;
    uint32_t status=(uint32_t)(line[9]-'0')*100U+(uint32_t)(line[10]-'0')*10U+(uint32_t)(line[11]-'0');
    response->snapshot.http_status=status;
    if(status==101U || (status>=200U && status!=200U))return UMI_STATUS_UNAVAILABLE;
    if(status<200U){
        if(response->snapshot.informational_responses==response->limits.informational_responses)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        ++response->snapshot.informational_responses;
    }
    response->http11=line[7]=='1';
    response->content_type=false;response->content_encoding=false;
    response->snapshot.has_content_length=false;response->snapshot.chunked=false;
    response->snapshot.declared_body_bytes=0U;
    response->snapshot.state=UMI_WEB_SSE_RESPONSE_HEADERS;
    return UMI_STATUS_OK;
}
static bool ForbiddenTrailer(const unsigned char *name,size_t bytes)
{
    return UmiWebFieldEqual(name,bytes,"content-type") || UmiWebFieldEqual(name,bytes,"content-encoding") ||
        UmiWebFieldEqual(name,bytes,"content-length") || UmiWebFieldEqual(name,bytes,"transfer-encoding") ||
        UmiWebFieldEqual(name,bytes,"location") || UmiWebFieldEqual(name,bytes,"host") ||
        UmiWebFieldEqual(name,bytes,"connection") || UmiWebFieldEqual(name,bytes,"trailer") ||
        UmiWebFieldEqual(name,bytes,"upgrade");
}
static UmiStatus Field(UmiWebSseResponse *response,const unsigned char *line,size_t bytes,bool trailer)
{
    const unsigned char *colon=memchr(line,':',bytes);
    if(colon==NULL || colon==line)return UMI_STATUS_PARSE_ERROR;
    size_t name_bytes=(size_t)(colon-line);
    for(size_t i=0U;i<name_bytes;++i)if(!UmiWebFieldToken(line[i]))return UMI_STATUS_PARSE_ERROR;
    const unsigned char *value=colon+1U;size_t value_bytes=bytes-(size_t)(value-line);Trim(&value,&value_bytes);
    if(trailer)return ForbiddenTrailer(line,name_bytes)?UMI_STATUS_PARSE_ERROR:UMI_STATUS_OK;
    if(UmiWebFieldEqual(line,name_bytes,"content-type")){
        if(response->content_type)return UMI_STATUS_PARSE_ERROR;
        UmiStatus status=MediaType(value,value_bytes);
        if(status!=UMI_STATUS_OK)return status;
        response->content_type=true;
    }else if(UmiWebFieldEqual(line,name_bytes,"content-encoding")){
        if(response->content_encoding)return UMI_STATUS_PARSE_ERROR;
        if(!UmiWebFieldEqual(value,value_bytes,"identity"))return UMI_STATUS_UNAVAILABLE;
        response->content_encoding=true;
    }else if(UmiWebFieldEqual(line,name_bytes,"content-length")){
        if(response->snapshot.http_status<200U || response->snapshot.has_content_length ||
            response->snapshot.chunked || value_bytes==0U)return UMI_STATUS_PARSE_ERROR;
        uint64_t length=0U;
        for(size_t i=0U;i<value_bytes;++i){
            if(value[i]<'0' || value[i]>'9')return UMI_STATUS_PARSE_ERROR;
            uint64_t digit=(uint64_t)(value[i]-'0');
            if(length>(UINT64_MAX-digit)/10U)return UMI_STATUS_CAPACITY_EXCEEDED;
            length=length*10U+digit;
        }
        if(length>response->limits.body_bytes)return UMI_STATUS_CAPACITY_EXCEEDED;
        response->snapshot.declared_body_bytes=length;response->snapshot.has_content_length=true;
    }else if(UmiWebFieldEqual(line,name_bytes,"transfer-encoding")){
        if(response->snapshot.http_status<200U || response->snapshot.chunked ||
            response->snapshot.has_content_length)return UMI_STATUS_PARSE_ERROR;
        if(!response->http11 || !UmiWebFieldEqual(value,value_bytes,"chunked"))return UMI_STATUS_UNAVAILABLE;
        response->snapshot.chunked=true;
    }
    return UMI_STATUS_OK;
}
UmiStatus UmiWebSseResponseHeader(UmiWebSseResponse *response,const void *bytes,size_t length)
{
    if(response==NULL || bytes==NULL || length==0U)return UMI_STATUS_INVALID_ARGUMENT;
    UmiWebSseResponseSnapshot *snapshot=&response->snapshot;
    if(snapshot->state==UMI_WEB_SSE_RESPONSE_FAILED)return snapshot->failure;
    if(snapshot->state==UMI_WEB_SSE_RESPONSE_COMPLETE)return UMI_STATUS_INVALID_STATE;
    if(length>response->limits.line_bytes || length>response->limits.total_header_bytes-snapshot->header_bytes ||
        snapshot->header_lines==response->limits.header_lines)return ResponseFail(response,UMI_STATUS_CAPACITY_EXCEEDED);
    snapshot->header_bytes+=length;++snapshot->header_lines;
    const unsigned char *line=bytes;
    if(length<2U || line[length-2U]!='\r' || line[length-1U]!='\n')return ResponseFail(response,UMI_STATUS_PARSE_ERROR);
    size_t payload=length-2U;
    for(size_t i=0U;i<payload;++i)
        if((line[i]<0x20U && line[i]!='\t') || line[i]==0x7fU)return ResponseFail(response,UMI_STATUS_PARSE_ERROR);
    UmiStatus status=UMI_STATUS_OK;
    if(snapshot->state==UMI_WEB_SSE_RESPONSE_STATUS)status=StatusLine(response,line,payload);
    else if(snapshot->state==UMI_WEB_SSE_RESPONSE_HEADERS){
        if(payload==0U){
            if(snapshot->http_status<200U)snapshot->state=UMI_WEB_SSE_RESPONSE_STATUS;
            else if(!response->content_type)status=UMI_STATUS_UNAVAILABLE;
            else snapshot->state=UMI_WEB_SSE_RESPONSE_READY;
        }else status=Field(response,line,payload,false);
    }else if(snapshot->state==UMI_WEB_SSE_RESPONSE_BODY || snapshot->state==UMI_WEB_SSE_RESPONSE_TRAILERS){
        /* HTTP/1 trailers require chunked transfer framing; a body phase alone does not authorize additional headers. The former implementation is retained for owner review. */
#if 0
if(payload==0U)snapshot->state=UMI_WEB_SSE_RESPONSE_TRAILERS_DONE;
        else{status=Field(response,line,payload,true);if(status==UMI_STATUS_OK)snapshot->state=UMI_WEB_SSE_RESPONSE_TRAILERS;}
#endif
if(!snapshot->chunked)status=UMI_STATUS_PARSE_ERROR;
        else if(payload==0U)snapshot->state=UMI_WEB_SSE_RESPONSE_TRAILERS_DONE;
        else{status=Field(response,line,payload,true);if(status==UMI_STATUS_OK)snapshot->state=UMI_WEB_SSE_RESPONSE_TRAILERS;}
    }else status=UMI_STATUS_PARSE_ERROR;
    return status==UMI_STATUS_OK?status:ResponseFail(response,status);
}
UmiStatus UmiWebSseResponseBody(UmiWebSseResponse *response,size_t bytes)
{
    if(response==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    UmiWebSseResponseSnapshot *snapshot=&response->snapshot;
    if(snapshot->state==UMI_WEB_SSE_RESPONSE_FAILED)return snapshot->failure;
    if(snapshot->state==UMI_WEB_SSE_RESPONSE_COMPLETE)return UMI_STATUS_INVALID_STATE;
    if(snapshot->state!=UMI_WEB_SSE_RESPONSE_READY && snapshot->state!=UMI_WEB_SSE_RESPONSE_BODY)
        return ResponseFail(response,UMI_STATUS_PARSE_ERROR);
    if(bytes>response->limits.body_bytes-snapshot->body_bytes ||
        (snapshot->has_content_length && bytes>snapshot->declared_body_bytes-snapshot->body_bytes))
        return ResponseFail(response,UMI_STATUS_CAPACITY_EXCEEDED);
    snapshot->body_bytes+=(uint64_t)bytes;snapshot->state=UMI_WEB_SSE_RESPONSE_BODY;return UMI_STATUS_OK;
}
UmiStatus UmiWebSseResponseFinish(UmiWebSseResponse *response)
{
    if(response==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    UmiWebSseResponseSnapshot *snapshot=&response->snapshot;
    if(snapshot->state==UMI_WEB_SSE_RESPONSE_FAILED)return snapshot->failure;
    if(snapshot->state==UMI_WEB_SSE_RESPONSE_COMPLETE)return UMI_STATUS_OK;
    if(snapshot->state==UMI_WEB_SSE_RESPONSE_STATUS || snapshot->state==UMI_WEB_SSE_RESPONSE_HEADERS ||
        (snapshot->has_content_length && snapshot->declared_body_bytes!=snapshot->body_bytes))
        return ResponseFail(response,UMI_STATUS_PARSE_ERROR);
    snapshot->state=UMI_WEB_SSE_RESPONSE_COMPLETE;return UMI_STATUS_OK;
}
UmiStatus UmiWebSseResponseInspect(const UmiWebSseResponse *response,UmiWebSseResponseSnapshot *out)
{
    if(response==NULL || out==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    *out=response->snapshot;return UMI_STATUS_OK;
}
