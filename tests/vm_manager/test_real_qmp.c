/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Optional actual-QEMU protocol check. No kernel or guest OS is booted. */
#include "umicom/vm_manager/qmp_channel.h"
#include <stdio.h>
#include <string.h>
int main(int argc,char**argv){
    if(argc!=3||!argv[1][0]){
        puts("NOT RUN: no QEMU executable was selected. Inert peer tests do not replace this check.");
        return 77;
    }
    const char*args[]={
        "-machine","none","-accel","tcg","-nodefaults","-no-user-config","-display","none","-S","-qmp","stdio","-monitor","none","-chardev","ringbuf,id=console,size=65536"
    };
    UmiProcessChannelRequest request={
        argv[1],args,sizeof args/sizeof args[0],argv[2]
    };
    UmiProcessChannel*channel=NULL;
    UmiVmSession*session=NULL;
    UmiVmReport report;
    UmiStatus status=UmiProcessChannelOpen(&request,&channel);
    if(status==UMI_STATUS_OK)status=UmiVmQmpAdoptChannel(channel,&session,&report);
    if(status==UMI_STATUS_OK){
        channel=NULL;
        UmiVmSnapshot snapshot;
        status=UmiVmObserve(session,&snapshot);
        if(status==UMI_STATUS_OK)printf("Actual QEMU QMP version: %s; state: %s. No guest OS was started.\n",snapshot.qemuVersion,snapshot.state);
    }
    unsigned char output[2048];
    size_t length=0;
    if(status==UMI_STATUS_OK)status=UmiVmControl(session,UMI_VM_QUERY,NULL,0,output,sizeof output,&length,&report);
    if(status==UMI_STATUS_OK)status=UmiVmControl(session,UMI_VM_QUIT,NULL,0,output,sizeof output,&length,&report);
    UmiVmSessionDestroy(session);
    UmiProcessChannelDestroy(channel);
    return status==UMI_STATUS_OK?0:1;
}
