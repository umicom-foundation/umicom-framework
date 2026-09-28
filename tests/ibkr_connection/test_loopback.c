/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * A labelled local protocol peer, not a TWS process and not a broker account.
 * Every outgoing frame is checked against the read-only request sequence. */
#define _POSIX_C_SOURCE 200809L
#include "umicom/broker_connectivity/connection.h"
#include <unistd.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#define REQUIRE(x) do{if(!(x)){fprintf(stderr,"line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static int Exact(int fd,void *buffer,size_t length,bool sendIt)
{
    unsigned char *p=buffer;while(length){ssize_t n=sendIt?send(fd,p,length,MSG_NOSIGNAL):recv(fd,p,length,0);
        if(n<0&&errno==EINTR)continue;
        if(n<=0)return 1;
        p+=(size_t)n;length-=(size_t)n;}return 0;
}
static int Packet(int fd,const char *const *fields,size_t count)
{
    unsigned char buffer[4096];size_t n=4;
    for(size_t i=0;i<count;++i){size_t size=strlen(fields[i])+1;if(n+size>sizeof buffer)return 1;memcpy(buffer+n,fields[i],size);n+=size;}
    size_t size=n-4;buffer[0]=(unsigned char)(size>>24U);buffer[1]=(unsigned char)(size>>16U);buffer[2]=(unsigned char)(size>>8U);buffer[3]=(unsigned char)size;
    /* Deliberate fragmentation exercises framing without a scripted adapter. */
    for(size_t i=0;i<n;++i)if(Exact(fd,buffer+i,1,true))return 1;
    return 0;
}
#define SEND(fd,...) do{const char *f[]={__VA_ARGS__};REQUIRE(Packet(fd,f,sizeof f/sizeof f[0])==0);}while(0)
static int Receive(int fd,unsigned char *buffer,size_t *out)
{
    unsigned char h[4];if(Exact(fd,h,4,false))return 1;
    size_t size=((size_t)h[0]<<24U)|((size_t)h[1]<<16U)|((size_t)h[2]<<8U)|h[3];
    if(!size||size>4096U)return 1;
    if(Exact(fd,buffer,size,false))return 1;
    *out=size;return 0;
}
static int Expect(int fd,const char *const *fields,size_t count)
{
    unsigned char actual[4096],expected[4096];size_t n=0,size;
    for(size_t i=0;i<count;++i){size_t add=strlen(fields[i])+1;memcpy(expected+n,fields[i],add);n+=add;}
    REQUIRE(Receive(fd,actual,&size)==0);REQUIRE(size==n&&!memcmp(actual,expected,n));return 0;
}
#define EXPECT(fd,...) do{const char *f[]={__VA_ARGS__};REQUIRE(Expect(fd,f,sizeof f/sizeof f[0])==0);}while(0)
static int Peer(int listener)
{
    alarm(8);
    int fd=accept(listener,NULL,NULL);REQUIRE(fd>=0);close(listener);
    unsigned char buffer[4096];size_t size=0;
    REQUIRE(Exact(fd,buffer,4,false)==0&&!memcmp(buffer,"API\0",4));
    REQUIRE(Receive(fd,buffer,&size)==0&&size==9&&!memcmp(buffer,"v151..176",9));
    SEND(fd,"176","20260928 09:00:00 UTC");
    EXPECT(fd,"71","2","35","");
    SEND(fd,"15","1","DU123");SEND(fd,"9","1","44");
    EXPECT(fd,"49","1");SEND(fd,"49","1","1790586000");
    EXPECT(fd,"62","1","35001","All","NetLiquidation,TotalCashValue,BuyingPower,AvailableFunds");
    EXPECT(fd,"61","1");
    SEND(fd,"63","1","35001","DU123","TotalCashValue","100.25","GBP");
    SEND(fd,"61","3","DU123","123","WORKSHOP","STK","","0","","","SMART","GBP","WORKSHOP","WORKSHOP","2.125","19.01");
    SEND(fd,"64","1","35001");SEND(fd,"62","1");
    EXPECT(fd,"63","1","35001");EXPECT(fd,"64","1");
    /* Do not race an EOF against the parent's observation capture. */
    char one;REQUIRE(recv(fd,&one,1,0)==0);close(fd);return 0;
}
int main(int argc,char **argv)
{
    REQUIRE(argc==2);
    int listener=socket(AF_INET,SOCK_STREAM,0);REQUIRE(listener>=0);
    struct sockaddr_in address={0};address.sin_family=AF_INET;address.sin_addr.s_addr=htonl(INADDR_LOOPBACK);
    REQUIRE(bind(listener,(struct sockaddr *)&address,sizeof address)==0);
    socklen_t length=sizeof address;REQUIRE(getsockname(listener,(struct sockaddr *)&address,&length)==0);REQUIRE(listen(listener,1)==0);
    pid_t child=fork();REQUIRE(child>=0);if(child==0)_exit(Peer(listener));close(listener);
    UmiIbkrConnectionOptions options=UmiIbkrConnectionOptionsDefault();options.adapter.port=ntohs(address.sin_port);
    if(!strcmp(argv[1],"live")){options.environment=UMI_TRADING_LIVE;options.adapter.paperOnly=0;options.acknowledgeLive=true;}
    UmiIbkrConnection *connection=NULL;UmiIbkrConnectionSnapshot *snapshot=malloc(sizeof *snapshot);REQUIRE(snapshot!=NULL);
    UmiStatus status=UmiIbkrConnectionCreate(&options,&connection);REQUIRE(status==UMI_STATUS_OK);
    uint64_t begin=UmiIbkrMonotonicMilliseconds();status=UmiIbkrConnectionOpen(connection,begin);
    bool completed=false;
    while(status==UMI_STATUS_OK&&UmiIbkrMonotonicMilliseconds()-begin<5000U){
        uint64_t now=UmiIbkrMonotonicMilliseconds();status=UmiIbkrConnectionPump(connection,now);
        (void)UmiIbkrConnectionCopy(connection,snapshot);
        if(status==UMI_STATUS_OK&&snapshot->state==UMI_IBKR_READY&&!snapshot->requestIssued)
            status=UmiIbkrConnectionReadAccount(connection,"DU123",now);
        if(snapshot->summaryComplete&&snapshot->positionsComplete){
            /* Flush the two cancel-subscription frames before closing. */
            status=UmiIbkrConnectionPump(connection,now);completed=true;break;
        }
        const struct timespec pause={0,2000000L};(void)nanosleep(&pause,NULL);
    }
    UmiIbkrConnectionClose(connection);int childStatus=0;
    if(!completed)kill(child,SIGKILL);
    REQUIRE(waitpid(child,&childStatus,0)==child);
    int failed=status!=UMI_STATUS_OK||!completed||!WIFEXITED(childStatus)||WEXITSTATUS(childStatus)!=0;
    failed=failed||strcmp(snapshot->values[0].value,"100.25")||strcmp(snapshot->positions[0].quantity,"2.125")||snapshot->environmentAttested;
    UmiIbkrConnectionDestroy(connection);free(snapshot);
    if(failed){fprintf(stderr,"Loopback peer did not complete (status %d, child %d).\n",status,childStatus);return 1;}
    puts("Native sockets and read-only frames verified against an inert loopback peer; no broker session was used.");return 0;
}
