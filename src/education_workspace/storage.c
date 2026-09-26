/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/education_workspace/storage.c
 * PURPOSE: Provide the Framework-owned Education Workspace capability.
 * AUTHOR: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "private.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* The record format is deliberately small and architecture independent:
 * header = E1|catalogue revision|event count|hex UTF-8 name
 * event  = E1|action|lesson index|hex UTF-8 payload
 * All fields are validated and every event is replayed through the same model.
 * Hex encoding avoids quoting ambiguities and embedded record delimiters.
 * It is NOT encryption or an authenticity signature. */
static UmiStatus Key(const UmiEducationWorkspace *w,const char *suffix,char key[192])
{
    int n=snprintf(key,192U,"%s%s",w->prefix,suffix);
    return n<0||(size_t)n>=192U?UMI_STATUS_CAPACITY_EXCEEDED:UMI_STATUS_OK;
}
static void Hex(const char *text,char *output)
{
    static const char digits[]="0123456789abcdef";
    while (*text!='\0') {unsigned char c=(unsigned char)*text++;*output++=digits[c>>4U];*output++=digits[c&15U];}
    *output='\0';
}
static int Nibble(char c)
{return c>='0'&&c<='9'?c-'0':c>='a'&&c<='f'?c-'a'+10:-1;}
static UmiStatus Unhex(const char *text,char *output,size_t capacity)
{
    size_t n=strlen(text);
    if ((n&1U)!=0U || n/2U>=capacity) return UMI_STATUS_PARSE_ERROR;
    for (size_t i=0U;i<n;i+=2U) {
        int a=Nibble(text[i]),b=Nibble(text[i+1U]);
        if (a<0||b<0||(a==0&&b==0)) return UMI_STATUS_PARSE_ERROR;
        output[i/2U]=(char)((a<<4)|b);
    }
    output[n/2U]='\0';
    return EwTextValid(output,capacity,true)?UMI_STATUS_OK:UMI_STATUS_PARSE_ERROR;
}
static UmiStatus Number(const char *text,uint64_t maximum,uint64_t *out)
{
    uint64_t v=0U;
    if (*text=='\0' || (text[0]=='0' && text[1]!='\0')) return UMI_STATUS_PARSE_ERROR;
    while (*text!='\0') {
        if (*text<'0'||*text>'9') return UMI_STATUS_PARSE_ERROR;
        uint64_t d=(uint64_t)(*text++-'0');
        if (d>maximum || v>(maximum-d)/10U) return UMI_STATUS_PARSE_ERROR;
        v=v*10U+d;
    }
    *out=v;return UMI_STATUS_OK;
}
static UmiStatus Parts(char *record,char **parts)
{
    parts[0]=record;
    for (size_t i=1U;i<4U;++i) {
        char *p=strchr(parts[i-1U],'|');if (p==NULL) return UMI_STATUS_PARSE_ERROR;
        *p='\0';parts[i]=p+1;
    }
    return strcmp(parts[0],"E1")==0 && strchr(parts[3],'|')==NULL?UMI_STATUS_OK:UMI_STATUS_PARSE_ERROR;
}
typedef struct RecordCheck {const UmiEducationWorkspace *w;size_t count;uint64_t events;} RecordCheck;
static UmiStatus CountRecord(const char *key,const char *value,void *context)
{
    RecordCheck *check=context;(void)value;
    size_t n=strlen(check->w->prefix);
    if (strncmp(key,check->w->prefix,n)!=0) return UMI_STATUS_OK;
    const char *suffix=key+n;
    if (strcmp(suffix,"head")==0) {++check->count;return UMI_STATUS_OK;}
    if (strncmp(suffix,"event/",6U)!=0) return UMI_STATUS_PARSE_ERROR;
    uint64_t number=0U;
    if (Number(suffix+6U,UMI_EDUCATION_MAX_EVENTS,&number)!=UMI_STATUS_OK || number==0U || number>check->events)
        return UMI_STATUS_PARSE_ERROR;
    ++check->count;return UMI_STATUS_OK;
}
UmiStatus EwLoad(UmiEducationWorkspace *w,EwState *out)
{
    char key[192],record[EW_RECORD_CAPACITY],*parts[4],payload[UMI_EDUCATION_NOTE_CAPACITY];
    uint64_t version=0U,count=0U;
    UmiStatus status=Key(w,"head",key);if (status!=UMI_STATUS_OK) return status;
    status=umi_data_server_get(w->server,key,record,sizeof record);
    if (status==UMI_STATUS_NOT_FOUND) {
        RecordCheck check={w,0U,0U};
        status=umi_data_server_visit(w->server,CountRecord,&check);
        return status!=UMI_STATUS_OK?status:check.count!=0U?UMI_STATUS_PARSE_ERROR:UMI_STATUS_OK;
    }
    if (status!=UMI_STATUS_OK) return status;
    if (Parts(record,parts)!=UMI_STATUS_OK || Number(parts[1],UINT32_MAX,&version)!=UMI_STATUS_OK ||
        version!=UMI_EDUCATION_CATALOGUE_REVISION || Number(parts[2],UMI_EDUCATION_MAX_EVENTS,&count)!=UMI_STATUS_OK ||
        count==0U || Unhex(parts[3],out->displayName,sizeof out->displayName)!=UMI_STATUS_OK || out->displayName[0]=='\0')
        return UMI_STATUS_PARSE_ERROR;
    for (uint64_t n=1U;n<=count;++n) {
        char suffix[32];uint64_t action=0U,index=0U;
        int printed=snprintf(suffix,sizeof suffix,"event/%" PRIu64,n);
        if (printed<0 || (size_t)printed>=sizeof suffix) return UMI_STATUS_INTERNAL_ERROR;
        status=Key(w,suffix,key);if (status!=UMI_STATUS_OK) return status;
        status=umi_data_server_get(w->server,key,record,sizeof record);
        if (status!=UMI_STATUS_OK) return status==UMI_STATUS_NOT_FOUND?UMI_STATUS_PARSE_ERROR:status;
        if (Parts(record,parts)!=UMI_STATUS_OK || Number(parts[1],EW_QUIZ,&action)!=UMI_STATUS_OK ||
            Number(parts[2],UMI_EDUCATION_LESSONS-1U,&index)!=UMI_STATUS_OK ||
            Unhex(parts[3],payload,sizeof payload)!=UMI_STATUS_OK) return UMI_STATUS_PARSE_ERROR;
        status=EwApply(out,(EwAction)action,(size_t)index,payload,NULL);
        if (status!=UMI_STATUS_OK) return UMI_STATUS_PARSE_ERROR;
        ++out->revision;
    }
    RecordCheck check={w,0U,count};status=umi_data_server_visit(w->server,CountRecord,&check);
    return status!=UMI_STATUS_OK?status:check.count!=(size_t)count+1U?UMI_STATUS_PARSE_ERROR:UMI_STATUS_OK;
}
UmiStatus EwCommit(UmiEducationWorkspace *w,EwAction action,size_t index,
    const char *payload,UmiEducationFeedback *feedback)
{
    char key[192],record[EW_RECORD_CAPACITY],hex[UMI_EDUCATION_NOTE_CAPACITY*2U],suffix[32];
    UmiEducationFeedback result={0};
    if (umi_data_server_in_transaction(w->server)) return UMI_STATUS_BUSY;
    EwState *fresh=calloc(1U,sizeof *fresh);
    if (fresh==NULL) return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(fresh->displayName,w->state.displayName,sizeof fresh->displayName);
    UmiStatus status=umi_data_server_begin(w->server);
    if (status!=UMI_STATUS_OK) {free(fresh);return status;}
    status=EwLoad(w,fresh);
    if (status==UMI_STATUS_OK && fresh->revision!=w->state.revision) status=UMI_STATUS_BUSY;
    if (status==UMI_STATUS_OK) status=EwApply(fresh,action,index,payload,&result);
    /* Read, hint and unchanged note actions are no-ops, but still check the
     * authoritative revision before returning. Quiz submissions are attempts. */
    bool same=status==UMI_STATUS_OK && action!=EW_QUIZ &&
        memcmp(&fresh->lessons[index],&w->state.lessons[index],sizeof fresh->lessons[index])==0;
    if (status==UMI_STATUS_OK && !same && fresh->revision>=UMI_EDUCATION_MAX_EVENTS)
        status=UMI_STATUS_CAPACITY_EXCEEDED;
    if (status==UMI_STATUS_OK && !same) {
        ++fresh->revision;Hex(payload,hex);
        int n=snprintf(record,sizeof record,"E1|%u|%zu|%s",(unsigned)action,index,hex);
        if (n<0 || (size_t)n>=sizeof record) status=UMI_STATUS_CAPACITY_EXCEEDED;
        n=snprintf(suffix,sizeof suffix,"event/%" PRIu64,fresh->revision);
        if (n<0 || (size_t)n>=sizeof suffix) status=UMI_STATUS_INTERNAL_ERROR;
        if (status==UMI_STATUS_OK) status=Key(w,suffix,key);
        if (status==UMI_STATUS_OK) status=umi_data_server_set(w->server,key,record);
        Hex(fresh->displayName,hex);
        n=snprintf(record,sizeof record,"E1|%u|%" PRIu64 "|%s",UMI_EDUCATION_CATALOGUE_REVISION,fresh->revision,hex);
        if (n<0 || (size_t)n>=sizeof record) status=UMI_STATUS_CAPACITY_EXCEEDED;
        if (status==UMI_STATUS_OK) status=Key(w,"head",key);
        if (status==UMI_STATUS_OK) status=umi_data_server_set(w->server,key,record);
    }
    if (status==UMI_STATUS_OK && !same) {
        status=umi_data_server_commit(w->server);
        if (status!=UMI_STATUS_OK) {
            /* A failed commit is not silently retried. Keep the last known
             * projection and require a newly opened connection to establish
             * durable state before the learner can submit further work. */
            w->recoveryRequired=true;
            if (umi_data_server_in_transaction(w->server)) (void)umi_data_server_rollback(w->server);
        }
    } else {
        UmiStatus rollback=umi_data_server_rollback(w->server);
        if (rollback!=UMI_STATUS_OK) {w->recoveryRequired=true;status=rollback;}
    }
    if (status==UMI_STATUS_OK) {w->state=*fresh;if (feedback!=NULL) *feedback=result;}
    free(fresh);return status;
}
