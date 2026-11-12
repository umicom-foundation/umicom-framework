/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/boot_media/test_policy_observation.c
 *
 * PURPOSE:
 *   Verify media policy observations and fresh reports without filesystem or device access.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../support/assertions.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "umicom/boot_media/media.h"
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    const char *name=argv[1];
    UmiBootMediaImage image={0};
    image.bytes=1024U;image.mbrLayout=1;
    UmiBootMediaDevice device={0};
    strcpy(device.path,"synthetic-observation");
    strcpy(device.identity,"identity");
    strcpy(device.model,"model");
    strcpy(device.serial,"serial");
    strcpy(device.reason,"observed synthetic values");
    device.bytes=2048U;device.sectorBytes=512U;device.usb=1;device.removable=1;
    UmiStatus expected=UMI_STATUS_OK;
    int recognized=0;
    int *flags[]={&image.mbrLayout,&image.gptLayout,&image.iso9660,&image.elTorito,
        &image.biosEntry,&image.efiEntry,&device.usb,&device.removable,
        &device.protectedDevice,&device.inUse,&device.readOnly};
    static const char *const flag_names[]={"mbr","gpt","iso","el-torito","bios","efi","usb","removable","protected","in-use","read-only"};
    for(size_t i=0U;i<sizeof(flags)/sizeof(flags[0]);++i){
        char candidate[96];
        snprintf(candidate,sizeof(candidate),"flag-%s-negative",flag_names[i]);
        if(strcmp(candidate,name)==0){*flags[i]=-1;recognized=1;expected=UMI_STATUS_INVALID_ARGUMENT;}
        snprintf(candidate,sizeof(candidate),"flag-%s-large",flag_names[i]);
        if(strcmp(candidate,name)==0){*flags[i]=2;recognized=1;expected=UMI_STATUS_INVALID_ARGUMENT;}
    }
    char *texts[]={device.path,device.identity,device.model,device.serial,device.reason};
    const size_t capacities[]={sizeof(device.path),sizeof(device.identity),sizeof(device.model),
        sizeof(device.serial),sizeof(device.reason)};
    static const char *const text_names[]={"path","identity","model","serial","reason"};
    for(size_t i=0U;i<sizeof(texts)/sizeof(texts[0]);++i){
        char candidate[96];
        snprintf(candidate,sizeof(candidate),"text-%s-unterminated",text_names[i]);
        if(strcmp(candidate,name)==0){memset(texts[i],'x',capacities[i]);recognized=1;expected=UMI_STATUS_INVALID_ARGUMENT;}
        snprintf(candidate,sizeof(candidate),"text-%s-invalid",text_names[i]);
        if(strcmp(candidate,name)==0){texts[i][0]=(char)0xff;recognized=1;expected=UMI_STATUS_INVALID_ARGUMENT;}
        snprintf(candidate,sizeof(candidate),"empty-%s",text_names[i]);
        if(strcmp(candidate,name)==0){texts[i][0]='\0';recognized=1;
            expected=i==4U?UMI_STATUS_OK:UMI_STATUS_PERMISSION_DENIED;}
    }
    if(!strcmp(name,"valid")||!strcmp(name,"null-report")||!strcmp(name,"repeat"))recognized=1;
    else if(!strcmp(name,"null-image")||!strcmp(name,"null-device")){
        recognized=1;expected=UMI_STATUS_INVALID_ARGUMENT;
    }else if(!strcmp(name,"unicode")){
        strcpy(device.model,"Umicom \xc3\x85\xf0\x9f\x98\x80");recognized=1;
    }else if(!strcmp(name,"system")){device.protectedDevice=1;recognized=1;expected=UMI_STATUS_PERMISSION_DENIED;}
    else if(!strcmp(name,"mounted")){device.inUse=1;recognized=1;expected=UMI_STATUS_PERMISSION_DENIED;}
    else if(!strcmp(name,"fixed")){device.removable=0;recognized=1;expected=UMI_STATUS_PERMISSION_DENIED;}
    else if(!strcmp(name,"not-usb")){device.usb=0;recognized=1;expected=UMI_STATUS_PERMISSION_DENIED;}
    else if(!strcmp(name,"readonly")){device.readOnly=1;recognized=1;expected=UMI_STATUS_PERMISSION_DENIED;}
    else if(!strcmp(name,"small")){device.bytes=512U;recognized=1;expected=UMI_STATUS_CAPACITY_EXCEEDED;}
    else if(!strcmp(name,"large")){device.bytes=UMI_BOOT_MEDIA_MAX_DEVICE+512U;recognized=1;expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(!strcmp(name,"sector")){device.sectorBytes=3U;recognized=1;expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(!strcmp(name,"4kn")){
        device.bytes=4096U;device.sectorBytes=4096U;recognized=1;expected=UMI_STATUS_UNAVAILABLE;
    }else if(!strcmp(name,"optical")){
        image.mbrLayout=0;image.iso9660=1;image.elTorito=1;recognized=1;expected=UMI_STATUS_UNAVAILABLE;
    }else if(!strcmp(name,"gpt-exact")||!strcmp(name,"gpt-resize")){
        image.gptLayout=1;recognized=1;
        if(!strcmp(name,"gpt-exact"))device.bytes=image.bytes;
        else expected=UMI_STATUS_UNAVAILABLE;
    }
    if(!recognized)return 2;
    UmiBootMediaImage image_before;
    UmiBootMediaDevice device_before;
    memcpy(&image_before,&image,sizeof(image));
    memcpy(&device_before,&device,sizeof(device));
    UmiBootMediaReport report;
    memset(&report,0x5a,sizeof(report));
    UmiBootMediaReport prior;
    memcpy(&prior,&report,sizeof(report));
    if(!strcmp(name,"repeat")){
        device.readOnly=1;
        assert(UmiBootMediaCheckDevicePolicy(&image,&device,&report)==UMI_STATUS_PERMISSION_DENIED);
        device.readOnly=0;
    }
    UmiStatus status=UmiBootMediaCheckDevicePolicy(
        !strcmp(name,"null-image")?NULL:&image,!strcmp(name,"null-device")?NULL:&device,
        !strcmp(name,"null-report")?NULL:&report);
    assert(status==expected);
    assert(memcmp(&image,&image_before,sizeof(image))==0);
    assert(memcmp(&device,&device_before,sizeof(device))==0);
    if(!strcmp(name,"null-report"))assert(memcmp(&report,&prior,sizeof(report))==0);
    else{
        assert(report.status==status&&report.phase==UMI_BOOT_MEDIA_INSPECT);
        assert(!report.outputCreated&&!report.writeStarted&&!report.verified);
        assert(report.bytesDone==0U&&report.bytesTotal==0U);
        assert(report.detail[0]!='\0'&&memchr(report.detail,'\0',sizeof(report.detail))!=NULL);
    }
    return 0;
}
