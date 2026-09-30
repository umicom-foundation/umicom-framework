/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/protocol.c
 * PURPOSE:
 *   Bounded legacy TWS field decoding for a read-only observation connection. Protocol field
 *   layouts are isolated here; no vendor code is redistributed.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Bounded legacy TWS field decoding for a read-only observation connection.
 * Protocol field layouts are isolated here; no vendor code is redistributed.
 *---------------------------------------------------------------------------*/
#include "internal.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

bool UmiIbkrText(const char *s,size_t cap,bool allowEmpty)
{
    if(!s||!cap)return false;
    size_t length=0;
    while(length<cap && s[length]!=0) ++length;
    if(length==cap||(!allowEmpty&&length==0))return false;
    const char *end=s+length;
    const unsigned char *p=(const unsigned char *)s,*limit=(const unsigned char *)end;
    while(p<limit){
        unsigned a=*p++;
        if(a<32U||a==127U)return false;
        if(a<128U)continue;
        unsigned n;uint32_t point;
        if(a>=0xc2U&&a<=0xdfU){n=1;point=a&31U;}
        else if(a>=0xe0U&&a<=0xefU){n=2;point=a&15U;}
        else if(a>=0xf0U&&a<=0xf4U){n=3;point=a&7U;}
        else return false;
        if((size_t)(limit-p)<n)return false;
        for(unsigned i=0;i<n;++i){unsigned b=*p++;if((b&0xc0U)!=0x80U)return false;point=(point<<6U)|(b&63U);}
        if((n==2U&&point<0x800U)||(n==3U&&point<0x10000U)||point>0x10ffffU||(point>=0xd800U&&point<=0xdfffU))return false;
    }
    return true;
}
bool UmiIbkrUnsigned(const char *s,uint64_t *out)
{
    if(!s||!out||!*s)return false;
    uint64_t n=0;
    for(size_t i=0;s[i];++i){
        unsigned char c=(unsigned char)s[i];if(c<'0'||c>'9')return false;
        unsigned digit=(unsigned)(c-'0');if(n>(UINT64_MAX-digit)/10U)return false;n=n*10U+digit;
    }
    *out=n;return true;
}
bool UmiIbkrDecimalText(const char *s)
{
    if(!UmiIbkrText(s,96U,false))return false;
    size_t i=0,digits=0;
    if(s[i]=='-'||s[i]=='+')++i;
    while(s[i]>='0'&&s[i]<='9'){++i;++digits;}
    if(s[i]=='.'){++i;while(s[i]>='0'&&s[i]<='9'){++i;++digits;}}
    if(!digits)return false;
    if(s[i]=='e'||s[i]=='E'){
        ++i;if(s[i]=='-'||s[i]=='+')++i;size_t start=i;
        while(s[i]>='0'&&s[i]<='9')++i;
        if(i==start)return false;
    }
    return s[i]==0;
}
static bool Account(const char *s)
{
    if(!UmiIbkrText(s,64U,false))return false;
    for(size_t i=0;s[i];++i){char a=s[i];if(!((a>='A'&&a<='Z')||(a>='a'&&a<='z')||(a>='0'&&a<='9')||a=='-'||a=='_'))return false;}
    return true;
}
static UmiStatus Ignore(UmiIbkrConnection *c)
{
    if(c->snapshot.ignoredFrames==UINT64_MAX)return UMI_STATUS_CAPACITY_EXCEEDED;
    ++c->snapshot.ignoredFrames;return UMI_STATUS_OK;
}
static UmiStatus Fields(const unsigned char *body,size_t length,char **out,size_t *count,char *copy)
{
    if(!length||body[length-1U]!=0)return UMI_STATUS_PARSE_ERROR;
    memcpy(copy,body,length);copy[length]=0;
    size_t start=0,n=0;
    for(size_t i=0;i<length;++i)if(copy[i]==0){
        if(n==32U)return UMI_STATUS_CAPACITY_EXCEEDED;
        out[n++]=copy+start;start=i+1U;
    }
    *count=n;return UMI_STATUS_OK;
}
static int CompareAccount(const void *a,const void *b){return strcmp(a,b);}
static UmiStatus Accounts(UmiIbkrConnection *c,const char *text)
{
    char accounts[UMI_IBKR_ACCOUNT_LIMIT][64]={{0}};size_t count=0,at=0;
    if(!UmiIbkrText(text,UMI_IBKR_FRAME_LIMIT,true))return UMI_STATUS_PARSE_ERROR;
    size_t length=strlen(text);
    while(at<length){
        const char *comma=strchr(text+at,',');size_t size=comma?(size_t)(comma-(text+at)):length-at;
        if(count==UMI_IBKR_ACCOUNT_LIMIT)return UMI_STATUS_CAPACITY_EXCEEDED;
        if(!size||size>=64U)return UMI_STATUS_PARSE_ERROR;
        memcpy(accounts[count],text+at,size);
        if(!Account(accounts[count]))return UMI_STATUS_PARSE_ERROR;
        ++count;at+=size;
        /* A single terminal separator is accepted; empty interior accounts
         * still fail. The provider has historically emitted both forms. */
        if(comma)++at;
    }
    qsort(accounts,count,sizeof accounts[0],CompareAccount);
    for(size_t i=1;i<count;++i)if(strcmp(accounts[i-1U],accounts[i])==0)return UMI_STATUS_PARSE_ERROR;
    if(c->accountsReceived){
        if(count!=c->snapshot.accountCount||memcmp(accounts,c->snapshot.accounts,sizeof accounts)!=0)
            return UMI_STATUS_PERMISSION_DENIED;
        return UMI_STATUS_OK;
    }
    memcpy(c->snapshot.accounts,accounts,sizeof accounts);c->snapshot.accountCount=count;c->accountsReceived=true;
    return UMI_STATUS_OK;
}
static UmiStatus Summary(UmiIbkrConnection *c,char **f,size_t n)
{
    uint64_t id;
    if(n!=7U||strcmp(f[1],"1")||!UmiIbkrUnsigned(f[2],&id))return UMI_STATUS_PARSE_ERROR;
    if(!c->snapshot.requestIssued||id!=35001U||strcmp(f[3],c->snapshot.selectedAccount)||c->snapshot.summaryComplete)return Ignore(c);
    UmiIbkrAccountValue row={0};
    if(!UmiIbkrText(f[4],sizeof row.tag,false)||!UmiIbkrText(f[5],sizeof row.value,true)||!UmiIbkrText(f[6],sizeof row.currency,true))return UMI_STATUS_PARSE_ERROR;
    (void)snprintf(row.tag,sizeof row.tag,"%s",f[4]);
    (void)snprintf(row.value,sizeof row.value,"%s",f[5]);
    (void)snprintf(row.currency,sizeof row.currency,"%s",f[6]);
    size_t index=c->snapshot.valueCount;
    for(size_t i=0;i<c->snapshot.valueCount;++i)if(!strcmp(row.tag,c->snapshot.values[i].tag)&&!strcmp(row.currency,c->snapshot.values[i].currency)){index=i;break;}
    if(index==UMI_IBKR_VALUE_LIMIT)return UMI_STATUS_CAPACITY_EXCEEDED;
    c->snapshot.values[index]=row;
    if(index==c->snapshot.valueCount)++c->snapshot.valueCount;
    return UMI_STATUS_OK;
}
static UmiStatus Position(UmiIbkrConnection *c,char **f,size_t n)
{
    if(n!=16U||strcmp(f[1],"3"))return UMI_STATUS_PARSE_ERROR;
    if(!c->snapshot.requestIssued||strcmp(f[2],c->snapshot.selectedAccount)||c->snapshot.positionsComplete)return Ignore(c);
    UmiIbkrPositionObservation row={0};
    char *dest[]={row.contractId,row.symbol,row.securityType,row.expiry,row.strike,row.right,row.multiplier,row.exchange,row.currency,row.localSymbol,row.tradingClass,row.quantity,row.averageCost};
    const size_t cap[]={sizeof row.contractId,sizeof row.symbol,sizeof row.securityType,sizeof row.expiry,sizeof row.strike,sizeof row.right,sizeof row.multiplier,sizeof row.exchange,sizeof row.currency,sizeof row.localSymbol,sizeof row.tradingClass,sizeof row.quantity,sizeof row.averageCost};
    for(size_t i=0;i<13U;++i){
        if(!UmiIbkrText(f[i+3U],cap[i],true))return UMI_STATUS_PARSE_ERROR;
        memcpy(dest[i],f[i+3U],strlen(f[i+3U])+1U);
    }
    uint64_t id;
    if(!UmiIbkrUnsigned(row.contractId,&id)||!row.symbol[0]||!row.securityType[0]||!UmiIbkrDecimalText(row.quantity)||!UmiIbkrDecimalText(row.averageCost))return UMI_STATUS_PARSE_ERROR;
    size_t index=c->snapshot.positionCount;
    for(size_t i=0;i<c->snapshot.positionCount;++i){
        const UmiIbkrPositionObservation *old=&c->snapshot.positions[i];
        if(!strcmp(row.contractId,old->contractId)&&!strcmp(row.localSymbol,old->localSymbol)&&!strcmp(row.exchange,old->exchange)){index=i;break;}
    }
    if(index==UMI_IBKR_POSITION_LIMIT)return UMI_STATUS_CAPACITY_EXCEEDED;
    c->snapshot.positions[index]=row;
    if(index==c->snapshot.positionCount)++c->snapshot.positionCount;
    return UMI_STATUS_OK;
}
static UmiStatus ProviderMessage(UmiIbkrConnection *c,char **f,size_t n)
{
    uint64_t code;
    if((n!=5U&&n!=6U)||strcmp(f[1],"2")||!UmiIbkrUnsigned(f[3],&code)||code>INT_MAX||!UmiIbkrText(f[4],UMI_IBKR_FRAME_LIMIT,true))return UMI_STATUS_PARSE_ERROR;
    c->snapshot.providerCode=(int)code;
    /* Never crop UTF-8 in the middle of a code point. Long provider text
     * is explicitly omitted, not published as a complete diagnostic. */
    if(strlen(f[4])>440U)
        (void)snprintf(c->snapshot.message,sizeof c->snapshot.message,
            "IBKR %d: provider diagnostic exceeds the display limit; text omitted.",(int)code);
    else
        (void)snprintf(c->snapshot.message,sizeof c->snapshot.message,"IBKR %d: %s",(int)code,f[4]);
    /* Farm status notifications are not order failures. Any other provider
     * error makes this bounded inspection stop instead of publishing a partial
     * snapshot as complete. Backend recovery requires an explicit reconnect. */
    if(code==2104U||code==2106U||code==2107U||code==2108U||code==2158U)return UMI_STATUS_OK;
    return UMI_STATUS_UNAVAILABLE;
}
static UmiStatus Decode(UmiIbkrConnection *c,const unsigned char *body,size_t length,uint64_t now)
{
    const unsigned char *first=memchr(body,0,length);uint64_t id;
    if(!first||(size_t)(first-body)>20U)return UMI_STATUS_PARSE_ERROR;
    char number[21]={0};memcpy(number,body,(size_t)(first-body));
    if(!UmiIbkrUnsigned(number,&id))return UMI_STATUS_PARSE_ERROR;
    bool handshake=c->snapshot.state==UMI_IBKR_HANDSHAKE;
    if(!handshake&&id!=4U&&id!=9U&&id!=15U&&id!=49U&&id!=61U&&id!=62U&&id!=63U&&id!=64U)return Ignore(c);
    /* One bounded allocation per decoded control/observation message, released
     * before return. An ignored message is never parsed as a known structure. */
    char *copy=malloc(length+1U);if(!copy)return UMI_STATUS_OUT_OF_MEMORY;
    char *f[32]={0};size_t n=0;
    UmiStatus status=Fields(body,length,f,&n,copy);
    if(status!=UMI_STATUS_OK){free(copy);return status;}
    if(handshake){
        if(n!=2U||id<151U||id>176U||!UmiIbkrText(f[1],128U,false))status=UMI_STATUS_UNAVAILABLE;
        else{
            char client[24];(void)snprintf(client,sizeof client,"%d",c->options.adapter.clientId);
            const char *start[]={"71","2",client,""};
            status=UmiIbkrQueueFields(c,start,4);
            if(status==UMI_STATUS_OK){c->snapshot.protocolVersion=(int)id;c->snapshot.state=UMI_IBKR_WAITING;}
        }
    }else switch(id){
    case 4: status=ProviderMessage(c,f,n);break;
    case 9:{uint64_t order;
        if(n!=3U||strcmp(f[1],"1")||!UmiIbkrUnsigned(f[2],&order)||order>INT_MAX)status=UMI_STATUS_PARSE_ERROR;
        else c->nextIdReceived=true;
        break;}
    case 15:status=n==3U&&!strcmp(f[1],"1")?Accounts(c,f[2]):UMI_STATUS_PARSE_ERROR;break;
    case 49:{uint64_t seconds;
        if(n!=3U||strcmp(f[1],"1")||!UmiIbkrUnsigned(f[2],&seconds)||!seconds)status=UMI_STATUS_PARSE_ERROR;
        else if(!c->pingPending)status=Ignore(c);
        else{c->snapshot.serverEpochSeconds=seconds;c->snapshot.clockReceived=true;c->snapshot.clockReceivedAtMilliseconds=now;c->pingPending=false;}
        break;}
    case 61:status=Position(c,f,n);break;
    case 62:
        if(n!=2U||strcmp(f[1],"1"))status=UMI_STATUS_PARSE_ERROR;
        else if(!c->snapshot.requestIssued||c->snapshot.positionsComplete)status=Ignore(c);
        else{const char *cancel[]={"64","1"};status=UmiIbkrQueueFields(c,cancel,2);
            if(status==UMI_STATUS_OK){c->snapshot.positionsComplete=true;c->snapshot.positionsAtMilliseconds=now;}}
        break;
    case 63:status=Summary(c,f,n);break;
    case 64:{uint64_t request;
        if(n!=3U||strcmp(f[1],"1")||!UmiIbkrUnsigned(f[2],&request))status=UMI_STATUS_PARSE_ERROR;
        else if(!c->snapshot.requestIssued||request!=35001U||c->snapshot.summaryComplete)status=Ignore(c);
        else{const char *cancel[]={"63","1","35001"};status=UmiIbkrQueueFields(c,cancel,3);
            if(status==UMI_STATUS_OK){c->snapshot.summaryComplete=true;c->snapshot.summaryAtMilliseconds=now;}}
        break;}
    default:status=UMI_STATUS_PARSE_ERROR;break;
    }
    free(copy);return status;
}
UmiStatus UmiIbkrProcessFrame(UmiIbkrConnection *c,const unsigned char *body,size_t length,uint64_t now)
{
    if(!c||!body||!length||length>UMI_IBKR_FRAME_LIMIT)return UMI_STATUS_INVALID_ARGUMENT;
    c->snapshot.providerCode=0;
    UmiStatus status=Decode(c,body,length,now);
    if(status!=UMI_STATUS_OK&&!c->snapshot.providerCode)
        (void)snprintf(c->snapshot.message,sizeof c->snapshot.message,"Unsupported, malformed or over-capacity API message (%s). Reconnect after reviewing the cause.",umi_status_text(status));
    return status;
}
