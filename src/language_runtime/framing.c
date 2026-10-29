/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/framing.c
 *
 * PURPOSE:
 *   Implement non-mutating, bounded Content-Length framing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/framing.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* The former arithmetic and eager-next-header result are retained for review.
 * The replacement checks bounded byte ownership before copies and delivers
 * each frame independently of a malformed successor. */
#if 0
/*
 * Initialise language runtime framer from caller-provided values so later operations
 * receive a known state.
 */
void umi_language_runtime_framer_init(UmiLanguageRuntimeFramer*f){/* Apply this branch only when its contract condition is satisfied. */ if(f)memset(f,0,sizeof(*f));}
/*
 * Write language runtime frame in its stable representation and report capacity or input
 * failures to the caller.
 */
UmiStatus umi_language_runtime_frame_encode(const char*j,char*out,size_t cap,size_t*len){size_t n;int h;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(j==NULL||out==NULL||cap==0||len==NULL)return UMI_STATUS_INVALID_ARGUMENT;n=strlen(j);h=snprintf(out,cap,"Content-Length: %zu\r\n\r\n",n);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(h<0||(size_t)h+n+1>cap)return UMI_STATUS_CAPACITY_EXCEEDED;memcpy(out+(size_t)h,j,n);out[(size_t)h+n]=0;*len=(size_t)h+n;return UMI_STATUS_OK;}
/* Provide the header operation used by this module and its client applications. */
static UmiStatus header(UmiLanguageRuntimeFramer*f){const char*sep,*cur,*end;size_t hlen=0,body=0;int found=0;sep=strstr(f->buffer,"\r\n\r\n");/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!sep)return UMI_STATUS_NOT_FOUND;hlen=(size_t)(sep-f->buffer)+4U;cur=f->buffer;/* Continue only while work remains available; the loop body advances the state on each pass. */ while(cur<sep){end=strstr(cur,"\r\n");/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!end||end>sep)end=sep;size_t n=(size_t)(end-cur);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(n>=15U&&strncmp(cur,"Content-Length:",15U)==0){char b[32],*e;size_t s=15U,m;/* Continue only while work remains available; the loop body advances the state on each pass. */ while(s<n&&isspace((unsigned char)cur[s]))s++;m=n-s;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(m==0||m>=sizeof(b))return UMI_STATUS_PARSE_ERROR;memcpy(b,cur+s,m);b[m]=0;unsigned long long v=strtoull(b,&e,10);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(e==b||*e||v>UMI_LANGUAGE_RUNTIME_JSON_CAPACITY-1U)return UMI_STATUS_PARSE_ERROR;body=(size_t)v;found=1;}cur=end+2;}/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!found)return UMI_STATUS_PARSE_ERROR;f->header_length=hlen;f->expected_body=body;f->header_ready=1;return UMI_STATUS_OK;}
/*
 * Provide the language runtime framer feed operation used by this module and its client
 * applications.
 */
UmiStatus umi_language_runtime_framer_feed(UmiLanguageRuntimeFramer*f,const void*b,size_t n){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(f==NULL||(b==NULL&&n))return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(f->length+n>=sizeof(f->buffer))return UMI_STATUS_CAPACITY_EXCEEDED;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(n){memcpy(f->buffer+f->length,b,n);f->length+=n;}f->buffer[f->length]=0;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!f->header_ready){s=header(f);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK&&s!=UMI_STATUS_NOT_FOUND)return s;}return UMI_STATUS_OK;}
/*
 * Provide the language runtime framer has message operation used by this module and its
 * client applications.
 */
int umi_language_runtime_framer_has_message(const UmiLanguageRuntimeFramer*f){return f&&f->header_ready&&f->length>=f->header_length+f->expected_body;}
/*
 * Provide the language runtime framer pop operation used by this module and its client
 * applications.
 */
UmiStatus umi_language_runtime_framer_pop(UmiLanguageRuntimeFramer*f,char*out,size_t cap,size_t*len){size_t used,remain;UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(f==NULL||out==NULL||cap==0||len==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!umi_language_runtime_framer_has_message(f))return UMI_STATUS_NOT_FOUND;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(f->expected_body+1>cap)return UMI_STATUS_CAPACITY_EXCEEDED;memcpy(out,f->buffer+f->header_length,f->expected_body);out[f->expected_body]=0;*len=f->expected_body;used=f->header_length+f->expected_body;remain=f->length-used;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(remain)memmove(f->buffer,f->buffer+used,remain);f->length=remain;f->buffer[remain]=0;f->expected_body=f->header_length=0;f->header_ready=0;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(remain){s=header(f);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK&&s!=UMI_STATUS_NOT_FOUND)return s;}return UMI_STATUS_OK;}

#endif

/* Framing is a byte boundary. Check remaining capacity with subtraction before
 * copying, and keep a malformed next header separate from a valid current
 * message. The public layout is retained; header_ready uses -1 for a sticky
 * malformed-header result, 0 for incomplete, and 1 for admitted header data. */
static UmiStatus FramerState(const UmiLanguageRuntimeFramer *framer)
{
    if (framer == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (framer->length >= sizeof(framer->buffer) ||
        framer->buffer[framer->length] != '\0' ||
        framer->header_ready < -1 || framer->header_ready > 1)
        return UMI_STATUS_INVALID_STATE;
    if (framer->header_ready != 1)
        return framer->header_length == 0U && framer->expected_body == 0U ?
            UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
    if (framer->header_length < 4U || framer->header_length > framer->length ||
        framer->expected_body >= UMI_LANGUAGE_RUNTIME_JSON_CAPACITY ||
        framer->expected_body > sizeof(framer->buffer)-1U-framer->header_length)
        return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
static unsigned char FramerFold(unsigned char ch)
{ return ch >= 'A' && ch <= 'Z' ? (unsigned char)(ch + ('a'-'A')) : ch; }
static int FramerName(const char *text, size_t length, const char *expected)
{
    if (length != strlen(expected)) return 0;
    for (size_t i=0U;i<length;++i)
        if (FramerFold((unsigned char)text[i]) != FramerFold((unsigned char)expected[i])) return 0;
    return 1;
}
static int FramerFieldCharacter(unsigned char ch)
{
    return (ch>='a' && ch<='z') || (ch>='A' && ch<='Z') || (ch>='0' && ch<='9') ||
        ch==0x60U || (ch!=0U && strchr("!#$%&'*+-.^_|~",(int)ch)!=NULL);
}
static int FramerSpace(char ch) { return ch==' ' || ch=='\t'; }
static int FramerUtf8Type(const char *text, size_t length)
{
    /* Unknown media parameters remain opaque. A declared charset cannot
     * silently switch a UTF-8 protocol parser to another byte interpretation. */
    size_t at=0U;
    int charset_seen=0;
    while (at<length && text[at]!=';') ++at;
    while (at<length) {
        ++at;
        while (at<length && FramerSpace(text[at])) ++at;
        size_t begin=at;
        while (at<length && text[at]!='=' && text[at]!=';') ++at;
        size_t end=at;
        while (end>begin && FramerSpace(text[end-1U])) --end;
        int charset=FramerName(text+begin,end-begin,"charset");
        if (at==length || text[at]!='=') { if (charset) return 0; continue; }
        ++at;
        while (at<length && FramerSpace(text[at])) ++at;
        begin=at;
        while (at<length && text[at]!=';') ++at;
        end=at;
        while (end>begin && FramerSpace(text[end-1U])) --end;
        if (!charset) continue;
        if (charset_seen++) return 0;
        if (end>begin && text[begin]=='"') {
            if (end-begin<2U || text[end-1U]!='"') return 0;
            ++begin; --end;
        }
        if (!FramerName(text+begin,end-begin,"utf-8") &&
            !FramerName(text+begin,end-begin,"utf8")) return 0;
    }
    return 1;
}
static UmiStatus FramerHeader(UmiLanguageRuntimeFramer *framer)
{
    size_t separator=SIZE_MAX;
    for (size_t i=0U;i+3U<framer->length;++i)
        if (memcmp(framer->buffer+i,"\r\n\r\n",4U)==0) { separator=i; break; }
    if (separator==SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    size_t at=0U,body=0U,header_bytes=separator+4U;
    int found_length=0,found_type=0;
    while (at<separator) {
        size_t end=at;
        while (end+1U<framer->length &&
            !(framer->buffer[end]=='\r' && framer->buffer[end+1U]=='\n')) ++end;
        if (end>separator) goto malformed;
        size_t colon=at;
        while (colon<end && framer->buffer[colon]!=':') {
            if (!FramerFieldCharacter((unsigned char)framer->buffer[colon])) goto malformed;
            ++colon;
        }
        if (colon==at || colon==end) goto malformed;
        size_t begin=colon+1U,value_end=end;
        while (begin<value_end && FramerSpace(framer->buffer[begin])) ++begin;
        while (value_end>begin && FramerSpace(framer->buffer[value_end-1U])) --value_end;
        for(size_t i=begin;i<value_end;++i) {
            unsigned char ch=(unsigned char)framer->buffer[i];
            if ((ch<32U && ch!='\t') || ch>=127U) goto malformed;
        }
        if (FramerName(framer->buffer+at,colon-at,"Content-Length")) {
            if (found_length++ || begin==value_end) goto malformed;
            for(size_t i=begin;i<value_end;++i) {
                unsigned char ch=(unsigned char)framer->buffer[i];
                if(ch<'0' || ch>'9') goto malformed;
                size_t digit=(size_t)(ch-'0');
                if(body>(UMI_LANGUAGE_RUNTIME_JSON_CAPACITY-1U-digit)/10U) goto malformed;
                body=body*10U+digit;
            }
        } else if (FramerName(framer->buffer+at,colon-at,"Content-Type")) {
            if (found_type++ || !FramerUtf8Type(framer->buffer+begin,value_end-begin)) goto malformed;
        }
        at=end+2U;
    }
    if (!found_length || body>sizeof(framer->buffer)-1U-header_bytes) goto malformed;
    framer->header_length=header_bytes;
    framer->expected_body=body;
    framer->header_ready=1;
    return UMI_STATUS_OK;
malformed:
    framer->header_length=0U;
    framer->expected_body=0U;
    framer->header_ready=-1;
    return UMI_STATUS_PARSE_ERROR;
}
void umi_language_runtime_framer_init(UmiLanguageRuntimeFramer *framer)
{ if (framer!=NULL) memset(framer,0,sizeof(*framer)); }

UmiStatus umi_language_runtime_frame_encode(const char *json, char *out,
    size_t capacity, size_t *out_length)
{
    if(json==NULL || out==NULL || capacity==0U || out_length==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t length=0U;
    while(length<UMI_LANGUAGE_RUNTIME_JSON_CAPACITY && json[length]!='\0') ++length;
    if(length==UMI_LANGUAGE_RUNTIME_JSON_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    char header_bytes[64];
    int written=snprintf(header_bytes,sizeof(header_bytes),"Content-Length: %zu\r\n\r\n",length);
    if(written<0 || (size_t)written>=sizeof(header_bytes)) return UMI_STATUS_CAPACITY_EXCEEDED;
    size_t header_length=(size_t)written;
    if(header_length>=capacity || length>capacity-1U-header_length) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Moving the body first also supports encoding into the original JSON
     * storage when the caller supplies enough writable destination capacity. */
    memmove(out+header_length,json,length);
    memcpy(out,header_bytes,header_length);
    out[header_length+length]='\0';
    *out_length=header_length+length;
    return UMI_STATUS_OK;
}
UmiStatus umi_language_runtime_framer_status(const UmiLanguageRuntimeFramer *framer)
{
    UmiStatus status=FramerState(framer);
    if(status!=UMI_STATUS_OK) return status;
    if(framer->header_ready==-1) return UMI_STATUS_PARSE_ERROR;
    if(framer->header_ready==1 && framer->length-framer->header_length>=framer->expected_body)
        return UMI_STATUS_OK;
    return UMI_STATUS_NOT_FOUND;
}
UmiStatus umi_language_runtime_framer_feed(UmiLanguageRuntimeFramer *framer,
    const void *bytes, size_t count)
{
    if(framer==NULL || (bytes==NULL && count!=0U)) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status=FramerState(framer);
    if(status!=UMI_STATUS_OK) return status;
    if(framer->header_ready==-1) return UMI_STATUS_PARSE_ERROR;
    if(count>sizeof(framer->buffer)-1U-framer->length) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* JSON text cannot contain a literal NUL; escaped \\u0000 remains bytes
     * for the JSON layer to interpret. Refuse before changing buffered state. */
    if(count!=0U && memchr(bytes,'\0',count)!=NULL) return UMI_STATUS_PARSE_ERROR;
    if(count!=0U) memmove(framer->buffer+framer->length,bytes,count);
    framer->length+=count;
    framer->buffer[framer->length]='\0';
    if(framer->header_ready==0) {
        status=FramerHeader(framer);
        if(status!=UMI_STATUS_OK && status!=UMI_STATUS_NOT_FOUND) return status;
    }
    return UMI_STATUS_OK;
}
int umi_language_runtime_framer_has_message(const UmiLanguageRuntimeFramer *framer)
{ return umi_language_runtime_framer_status(framer)==UMI_STATUS_OK; }

UmiStatus umi_language_runtime_framer_pop(UmiLanguageRuntimeFramer *framer,
    char *out, size_t capacity, size_t *out_length)
{
    if(framer==NULL || out==NULL || capacity==0U || out_length==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status=umi_language_runtime_framer_status(framer);
    if(status!=UMI_STATUS_OK) return status;
    if(framer->expected_body>=capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    size_t body=framer->expected_body,used=framer->header_length+body;
    size_t remaining=framer->length-used;
    memcpy(out,framer->buffer+framer->header_length,body);
    out[body]='\0'; *out_length=body;
    if(remaining!=0U) memmove(framer->buffer,framer->buffer+used,remaining);
    framer->length=remaining;framer->buffer[remaining]='\0';
    framer->header_length=framer->expected_body=0U;framer->header_ready=0;
    /* A malformed following header belongs to the NEXT result. Do not consume
     * the current valid frame and then report its successor's error as this
     * frame's failure. Status exposes buffered failure without another read. */
    if(remaining!=0U) (void)FramerHeader(framer);
    return UMI_STATUS_OK;
}
