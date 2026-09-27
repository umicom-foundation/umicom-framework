/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * QMP envelopes are interpreted structurally. An event never acknowledges a
 * command, an error never becomes a successful state transition, and console
 * bytes never become JSON syntax. */
#include "json_internal.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
static int Digit(unsigned char c){
    const char *alphabet="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    const char *p=strchr(alphabet,c);
    return p&&c?(int)(p-alphabet):-1;
}
static UmiStatus Unbase(const char *s,unsigned char *out,size_t *n){
    size_t length=strlen(s),used=0;
    if(length%4U)return UMI_STATUS_PARSE_ERROR;
    for(size_t i=0;i<length;i+=4U){
        int a=Digit((unsigned char)s[i]),b=Digit((unsigned char)s[i+1U]),c=Digit((unsigned char)s[i+2U]),d=Digit((unsigned char)s[i+3U]);
        if(a<0||b<0)return UMI_STATUS_PARSE_ERROR;
        size_t count=3;
        if(s[i+2U]=='='){
            if(s[i+3U]!='='||i+4U!=length||(b&15))return UMI_STATUS_PARSE_ERROR;
            c=0;
            d=0;
            count=1;
        }
        else if(s[i+3U]=='='){
            if(c<0||i+4U!=length||(c&3))return UMI_STATUS_PARSE_ERROR;
            d=0;
            count=2;
        }
        else if(c<0||d<0)return UMI_STATUS_PARSE_ERROR;
        if(count>UMI_VM_CONSOLE_CHUNK-used)return UMI_STATUS_CAPACITY_EXCEEDED;
        uint32_t value=((unsigned)a<<18)|((unsigned)b<<12)|((unsigned)c<<6)|(unsigned)d;
        out[used++]=(unsigned char)(value>>16);
        if(count>1U)out[used++]=(unsigned char)(value>>8);
        if(count>2U)out[used++]=(unsigned char)value;
    }
    *n=used;
    return UMI_STATUS_OK;
}
static UmiStatus StringOptional(const VjDocument*d,int o,const char*k,char*b,size_t n){
    int t=VjGet(d,o,k);
    return t<0?UMI_STATUS_OK:VjString(d,t,b,n);
}
UmiStatus UmiVmQmpDecode(const void *bytes,size_t n,UmiVmQmpMessage *out){
    if(!out)return UMI_STATUS_INVALID_ARGUMENT;
    memset(out,0,sizeof *out);
    VjDocument *d=malloc(sizeof *d);
    if(!d)return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus s=VjParse(bytes,n,d);
    if(s!=UMI_STATUS_OK){
        free(d);
        return s;
    }
    int greeting=VjGet(d,0,"QMP"),reply=VjGet(d,0,"return"),error=VjGet(d,0,"error"),event=VjGet(d,0,"event"),id=VjGet(d,0,"id");
    if((greeting>=0)+(reply>=0)+(error>=0)+(event>=0)!=1){
        free(d);
        return UMI_STATUS_PARSE_ERROR;
    }
    if(id>=0){
        s=VjUnsigned(d,id,&out->id);
        if(out->id>INT64_MAX)s=UMI_STATUS_PARSE_ERROR;
        out->hasId=1;
    }
    if(s==UMI_STATUS_OK&&greeting>=0){
        out->kind=UMI_VM_QMP_GREETING;
        int v=VjGet(d,greeting,"version"),q=VjGet(d,v,"qemu"),cap=VjGet(d,greeting,"capabilities");
        uint64_t parts[3]={
            0
        };
        const char*names[]={
            "major","minor","micro"
        };
        if(id>=0||cap<0||d->tokens[cap].type!=VJ_ARRAY)s=UMI_STATUS_PARSE_ERROR;
        for(size_t i=0;i<3U&&s==UMI_STATUS_OK;++i){
            s=VjUnsigned(d,VjGet(d,q,names[i]),&parts[i]);
            if(parts[i]>UINT_MAX)s=UMI_STATUS_PARSE_ERROR;
        }
        out->versionMajor=(unsigned)parts[0];
        out->versionMinor=(unsigned)parts[1];
        out->versionMicro=(unsigned)parts[2];
    }
    else if(s==UMI_STATUS_OK&&event>=0){
        out->kind=UMI_VM_QMP_EVENT;
        if(id>=0)s=UMI_STATUS_PARSE_ERROR;
        else s=VjString(d,event,out->event,sizeof out->event);
    }
    else if(s==UMI_STATUS_OK&&error>=0){
        out->kind=UMI_VM_QMP_ERROR;
        s=VjString(d,VjGet(d,error,"class"),out->errorClass,sizeof out->errorClass);
        if(s==UMI_STATUS_OK)s=VjString(d,VjGet(d,error,"desc"),out->description,sizeof out->description);
    }
    else if(s==UMI_STATUS_OK){
        out->kind=UMI_VM_QMP_RETURN;
        if(d->tokens[reply].type==VJ_OBJECT){
            out->returnIsObject=1;
            s=StringOptional(d,reply,"status",out->state,sizeof out->state);
            int running=VjGet(d,reply,"running");
            if(running>=0){
                if(d->tokens[running].type!=VJ_TRUE&&d->tokens[running].type!=VJ_FALSE)s=UMI_STATUS_PARSE_ERROR;
                out->hasRunning=1;
                out->running=d->tokens[running].type==VJ_TRUE;
            }
        }
        else if(d->tokens[reply].type==VJ_STRING){
            out->returnIsString=1;
            char encoded[UMI_VM_CONSOLE_CHUNK*2U];
            s=VjString(d,reply,encoded,sizeof encoded);
            if(s==UMI_STATUS_OK)s=Unbase(encoded,out->console,&out->consoleLength);
        }
        else s=UMI_STATUS_PARSE_ERROR;
    }
    free(d);
    if(s!=UMI_STATUS_OK)memset(out,0,sizeof *out);
    return s;
}
UmiStatus UmiVmQmpEncode(UmiVmCommand command,uint64_t id,const void *data,size_t length,char *out,size_t cap,size_t *outLength){
    if(!out||!outLength||!cap||command<UMI_VM_QUERY||command>UMI_VM_NEGOTIATE||!id||id>INT64_MAX||length>UMI_VM_CONSOLE_CHUNK||(!data&&length)||(command!=UMI_VM_CONSOLE_WRITE&&length))return UMI_STATUS_INVALID_ARGUMENT;
    *outLength=0;
    out[0]=0;
    const char *names[]={
        "query-status","stop","cont","system_powerdown","quit","ringbuf-read","ringbuf-write","qmp_capabilities"
    };
    char arguments[UMI_VM_CONSOLE_CHUNK*2U+160U];
    arguments[0]=0;
    if(command==UMI_VM_CONSOLE_READ)snprintf(arguments,sizeof arguments,",\"arguments\":{\"device\":\"console\",\"size\":%u,\"format\":\"base64\"}",UMI_VM_CONSOLE_CHUNK);
    if(command==UMI_VM_CONSOLE_WRITE){
        const unsigned char *p=data;
        const char *alphabet="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        char encoded[UMI_VM_CONSOLE_CHUNK*2U];
        size_t used=0;
        for(size_t i=0;i<length;i+=3U){
            unsigned value=(unsigned)p[i]<<16;
            size_t remain=length-i;
            if(remain>1U)value|=(unsigned)p[i+1U]<<8;
            if(remain>2U)value|=p[i+2U];
            encoded[used++]=alphabet[(value>>18)&63U];
            encoded[used++]=alphabet[(value>>12)&63U];
            encoded[used++]=remain>1U?alphabet[(value>>6)&63U]:'=';
            encoded[used++]=remain>2U?alphabet[value&63U]:'=';
        }
        encoded[used]=0;
        snprintf(arguments,sizeof arguments,",\"arguments\":{\"device\":\"console\",\"data\":\"%s\",\"format\":\"base64\"}",encoded);
    }
    int n=snprintf(out,cap,"{\"execute\":\"%s\",\"id\":%" PRIu64 "%s}\r\n",names[command],id,arguments);
    if(n<0||(size_t)n>=cap){
        out[0]=0;
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    *outLength=(size_t)n;
    return UMI_STATUS_OK;
}
