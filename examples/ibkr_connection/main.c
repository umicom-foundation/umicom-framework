/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/ibkr_connection/main.c
 * PURPOSE:
 *   Provide native console access to broker connection inspection.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT */
#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "umicom/broker_connectivity/connection.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <time.h>
#endif
static void Pause(void)
{
#ifdef _WIN32
    Sleep(10U);
#else
    const struct timespec t={0,10000000L};(void)nanosleep(&t,NULL);
#endif
}
static int Usage(void)
{
    puts("umicom-broker-connect --self-test\numicom-broker-connect connect --mode paper|live --program tws|gateway [--port N] [--client-id N] [--account ID] [--acknowledge-live]");
    puts("Connection is read-only and local (127.0.0.1). Log into TWS/Gateway separately.\nWithout --account, list the authorised accounts and disconnect. No order operations exist.\nNative legacy framed protocol profile: negotiated versions 151 through 176.");
    return 0;
}
static int Number(const char *s,unsigned limit,unsigned *out)
{
    if(!s||!*s)return 0;
    unsigned value=0;
    for(size_t i=0;s[i];++i){if(s[i]<'0'||s[i]>'9')return 0;unsigned d=(unsigned)(s[i]-'0');if(value>(limit-d)/10U)return 0;value=value*10U+d;}
    if(!value)return 0;
    *out=value;return 1;
}
static void Print(const UmiIbkrConnectionSnapshot *s)
{
    printf("Requested mode: %s; server mode attested: NO; orders: unavailable\n",
        s->requestedEnvironment==UMI_TRADING_LIVE?"LIVE":"PAPER");
    printf("State: %s; protocol: %d\n",UmiIbkrConnectionStateName(s->state),s->protocolVersion);
    printf("Authorised accounts (%zu):\n",s->accountCount);
    for(size_t i=0;i<s->accountCount;++i)printf("  %s\n",s->accounts[i]);
    if(s->requestIssued){
        printf("Selected account: %s; summary complete: %s; positions complete: %s\n",s->selectedAccount,
            s->summaryComplete?"yes":"NO",s->positionsComplete?"yes":"NO");
        for(size_t i=0;i<s->valueCount;++i)printf("%s: %s %s\n",s->values[i].tag,s->values[i].value[0]?s->values[i].value:"(unset)",s->values[i].currency);
        for(size_t i=0;i<s->positionCount;++i){const UmiIbkrPositionObservation *p=&s->positions[i];
            printf("%s | %s | %s | %s | quantity %s | average cost %s | %s\n",p->contractId,p->symbol,p->securityType,p->localSymbol,p->quantity,p->averageCost,p->currency);}
        puts("The two responses have independent completion times; this is not an atomic broker snapshot.");
    }
}
int main(int argc,char **argv)
{
    if(argc==2&&!strcmp(argv[1],"--help"))return Usage();
    if(argc==2&&!strcmp(argv[1],"--self-test")){
        UmiIbkrConnectionOptions o=UmiIbkrConnectionOptionsDefault();
        if(UmiIbkrConnectionValidate(&o)!=UMI_STATUS_OK)return 1;
        o.environment=UMI_TRADING_LIVE;o.adapter.paperOnly=0;
        if(UmiIbkrConnectionValidate(&o)!=UMI_STATUS_PERMISSION_DENIED)return 1;
        puts("Native Paper/Live policy self-test passed. No socket, file or order was created.");return 0;
    }
    if(argc<2||strcmp(argv[1],"connect")){Usage();return 2;}
    UmiIbkrConnectionOptions o=UmiIbkrConnectionOptionsDefault();
    const char *account=NULL;unsigned port=0,seen=0;
    for(int i=2;i<argc;++i){
        unsigned bit=0;
        if(!strcmp(argv[i],"--acknowledge-live")){bit=32U;o.acknowledgeLive=true;}
        else{
            if(i+1>=argc){Usage();return 2;}
            const char *name=argv[i++],*value=argv[i];
            if(!strcmp(name,"--mode")){bit=1U;if(!strcmp(value,"paper"))o.environment=UMI_TRADING_PAPER;else if(!strcmp(value,"live"))o.environment=UMI_TRADING_LIVE;else return 2;}
            else if(!strcmp(name,"--program")){bit=2U;if(!strcmp(value,"tws"))o.program=UMI_IBKR_TWS;else if(!strcmp(value,"gateway"))o.program=UMI_IBKR_GATEWAY;else return 2;}
            else if(!strcmp(name,"--port")){bit=4U;if(!Number(value,65535U,&port))return 2;}
            else if(!strcmp(name,"--client-id")){bit=8U;unsigned id;if(!Number(value,2147483647U,&id))return 2;o.adapter.clientId=(int)id;}
            else if(!strcmp(name,"--account")){bit=16U;if(strlen(value)==0||strlen(value)>=64U)return 2;account=value;}
            else return 2;
        }
        if(seen&bit)return 2;
        seen|=bit;
    }
    if((seen&3U)!=3U){Usage();return 2;}
    o.adapter.paperOnly=o.environment==UMI_TRADING_PAPER?1:0;
    o.adapter.port=port?(uint16_t)port:UmiIbkrDefaultPort(o.program,o.environment);
    UmiIbkrConnection *c=NULL;UmiIbkrConnectionSnapshot *s=malloc(sizeof *s);
    if(!s)return 1;
    UmiStatus status=UmiIbkrConnectionCreate(&o,&c);
    if(status==UMI_STATUS_OK)status=UmiIbkrConnectionOpen(c,UmiIbkrMonotonicMilliseconds());
    bool done=false;
    while(status==UMI_STATUS_OK&&!done){
        uint64_t now=UmiIbkrMonotonicMilliseconds();status=UmiIbkrConnectionPump(c,now);
        (void)UmiIbkrConnectionCopy(c,s);
        if(status==UMI_STATUS_OK&&s->state==UMI_IBKR_READY){
            if(!account)done=true;
            else if(!s->requestIssued)status=UmiIbkrConnectionReadAccount(c,account,now);
            else done=s->summaryComplete&&s->positionsComplete;
        }
        if(!done&&status==UMI_STATUS_OK)Pause();
    }
    if(status==UMI_STATUS_OK&&done)Print(s);
    else{
        fprintf(stderr,"Connection inspection failed: %s\n",umi_status_text(status));
        if(c){(void)UmiIbkrConnectionCopy(c,s);fprintf(stderr,"%s\n",s->message);}
    }
    UmiIbkrConnectionDestroy(c);free(s);return status==UMI_STATUS_OK&&done?0:1;
}
