/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT */
#ifndef UMICOM_TEST_IBKR_FIXTURE_H
#define UMICOM_TEST_IBKR_FIXTURE_H
#include "internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x);return 1;}}while(0)
typedef struct Fixture {
    UmiIbkrConnection *c;
    unsigned char input[131080],output[32768];
    size_t inSize,inAt,outSize,readStep,writeStep;
    unsigned closes,opens;
    UmiStatus openStatus,readyStatus,readStatus,writeStatus;
    bool eof,zeroWrite;
} Fixture;
static UmiStatus FOpen(void *p,uint16_t port){Fixture *f=p;(void)port;++f->opens;return f->openStatus;}
static UmiStatus FReady(void *p){return ((Fixture *)p)->readyStatus;}
static UmiStatus FRead(void *p,void *buffer,size_t count,size_t *out)
{
    Fixture *f=p;*out=0;if(f->readStatus!=UMI_STATUS_OK)return f->readStatus;
    if(f->inAt==f->inSize)return f->eof?UMI_STATUS_OK:UMI_STATUS_BUSY;
    if(count>f->inSize-f->inAt)count=f->inSize-f->inAt;
    if(f->readStep&&count>f->readStep)count=f->readStep;
    memcpy(buffer,f->input+f->inAt,count);f->inAt+=count;*out=count;return UMI_STATUS_OK;
}
static UmiStatus FWrite(void *p,const void *buffer,size_t count,size_t *out)
{
    Fixture *f=p;*out=0;if(f->writeStatus!=UMI_STATUS_OK)return f->writeStatus;
    if(f->zeroWrite)return UMI_STATUS_OK;
    if(f->writeStep&&count>f->writeStep)count=f->writeStep;
    if(count>sizeof f->output-f->outSize)return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(f->output+f->outSize,buffer,count);f->outSize+=count;*out=count;return UMI_STATUS_OK;
}
static void FClose(void *p){++((Fixture *)p)->closes;}
static Fixture *New(void)
{
    Fixture *f=calloc(1,sizeof *f);if(!f)return NULL;
    UmiIbkrConnectionOptions o=UmiIbkrConnectionOptionsDefault();
    UmiIbkrIo io={f,FOpen,FReady,FRead,FWrite,FClose};
    if(UmiIbkrConnectionCreateWithIo(&o,&io,&f->c)!=UMI_STATUS_OK){free(f);return NULL;}return f;
}
static void Delete(Fixture *f){if(f){UmiIbkrConnectionDestroy(f->c);free(f);}}
static int Feed(Fixture *f,const char *const *fields,size_t count)
{
    size_t size=0;for(size_t i=0;i<count;++i)size+=strlen(fields[i])+1U;
    if(size+4U>sizeof f->input-f->inSize)return 1;
    unsigned char *p=f->input+f->inSize;p[0]=(unsigned char)(size>>24U);p[1]=(unsigned char)(size>>16U);p[2]=(unsigned char)(size>>8U);p[3]=(unsigned char)size;p+=4;
    for(size_t i=0;i<count;++i){size_t n=strlen(fields[i])+1U;memcpy(p,fields[i],n);p+=n;}
    f->inSize+=size+4U;return 0;
}
#define FEED(f,...) do{const char *fields[]={__VA_ARGS__};CHECK(Feed((f),fields,sizeof fields/sizeof fields[0])==0);}while(0)
static int Connect(Fixture *f)
{
    CHECK(UmiIbkrConnectionOpen(f->c,0)==UMI_STATUS_OK);
    CHECK(UmiIbkrConnectionPump(f->c,0)==UMI_STATUS_OK);
    CHECK(f->outSize==17U&&memcmp(f->output,"API\0",4)==0);
    FEED(f,"176","20260928 09:00:00 UTC");CHECK(UmiIbkrConnectionPump(f->c,1)==UMI_STATUS_OK);
    CHECK(UmiIbkrConnectionPump(f->c,2)==UMI_STATUS_OK);
    FEED(f,"15","1","DU123,DU456");FEED(f,"9","1","42");
    CHECK(UmiIbkrConnectionPump(f->c,3)==UMI_STATUS_OK);CHECK(UmiIbkrConnectionPump(f->c,4)==UMI_STATUS_OK);
    FEED(f,"49","1","1790586000");CHECK(UmiIbkrConnectionPump(f->c,5)==UMI_STATUS_OK);
    CHECK(f->c->snapshot.state==UMI_IBKR_READY);return 0;
}
static int PositionFeed(Fixture *f,const char *account,const char *quantity,const char *cost)
{
    const char *fields[]={"61","3",account,"123","WORKSHOP","STK","","0","","","SMART","GBP","WORKSHOP","WORKSHOP",quantity,cost};
    return Feed(f,fields,sizeof fields/sizeof fields[0]);
}
#endif
