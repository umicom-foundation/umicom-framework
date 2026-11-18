/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application/test_native_location.c
 *
 * PURPOSE:
 *   Verify canonical explicit-root GUI resolution and unchanged outputs on invalid or unavailable locations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/application/native_discovery.h"
#include "umicom/application/portfolio.h"
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#define ROOT "C:/Umicom Apps/bin"
#else
#define ROOT "/opt/Umicom Apps/bin"
#endif
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"line %d: %s\n",__LINE__,#x); return 1; } } while (0)
typedef struct Probe { unsigned calls; bool present; UmiStatus status; char path[UMI_APPLICATION_RUNTIME_PATH_CAPACITY]; } Probe;
static UmiStatus inspect(const char *path,bool *present,void *context)
{
    Probe *probe=context; ++probe->calls;
    (void)snprintf(probe->path,sizeof probe->path,"%s",path);
    *present=probe->present; return probe->status;
}
int main(int argc,char **argv)
{
    UmiApplicationNativeDiscoveryConfig config=umi_application_native_discovery_config_default();
    char output[UMI_APPLICATION_RUNTIME_PATH_CAPACITY],saved[sizeof output];
    if(argc==2){
        /* Read-only diagnosis accepts one explicit root. No process is launched. */
        config.executable_root=argv[1];
        const char *ids[]={"org.umicom.desktop","org.umicom.studio","org.umicom.bank","org.umicom.tms"};
        for(size_t i=0;i<sizeof ids/sizeof ids[0];++i){
            UmiStatus status=umi_application_native_discovery_resolve(&config,ids[i],output,sizeof output);
            printf("%s: status=%d%s%s\n",ids[i],(int)status,status==UMI_STATUS_OK?" path=":"",status==UMI_STATUS_OK?output:"");
            CHECK(status==UMI_STATUS_OK);
        }
        return 0;
    }
    CHECK(argc==1);
    config.executable_root=ROOT;
    Probe probe={.present=true,.status=UMI_STATUS_OK};
    config.probe=inspect;config.probe_context=&probe;
    memset(output,0x6b,sizeof output);
    CHECK(umi_application_native_discovery_resolve(&config,"org.umicom.studio",output,sizeof output)==UMI_STATUS_OK);
    CHECK(strstr(output,"/umicom-studio-ide")!=NULL && strstr(output,"console")==NULL);
    CHECK(strcmp(output,probe.path)==0 && probe.calls==1);
    const size_t exact=strlen(output)+1U;
    CHECK(output[exact]==0x6b);
    CHECK(umi_application_native_discovery_resolve(&config,"org.umicom.studio",output,exact)==UMI_STATUS_OK);
    memcpy(saved,output,sizeof output);
    unsigned calls=probe.calls;
    CHECK(umi_application_native_discovery_resolve(&config,"org.umicom.studio",output,exact-1U)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(probe.calls==calls && memcmp(saved,output,sizeof output)==0);
    CHECK(umi_application_native_discovery_resolve(&config,"org.umicom.studio",output,0)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(umi_application_native_discovery_resolve(&config,"org.umicom.studio",NULL,exact)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_application_native_discovery_resolve(&config,"../../other",output,sizeof output)==UMI_STATUS_NOT_FOUND);
    CHECK(umi_application_native_discovery_resolve(&config,NULL,output,sizeof output)==UMI_STATUS_NOT_FOUND);
    CHECK(probe.calls==calls && memcmp(saved,output,sizeof output)==0);
    probe.present=false;
    CHECK(umi_application_native_discovery_resolve(&config,"org.umicom.bank",output,sizeof output)==UMI_STATUS_NOT_FOUND);
    CHECK(memcmp(saved,output,sizeof output)==0);
    bool present=true;
    CHECK(umi_application_native_discovery_probe(&config,"org.umicom.bank",&present)==UMI_STATUS_OK && !present);
    probe.present=true;probe.status=UMI_STATUS_PERMISSION_DENIED;
    CHECK(umi_application_native_discovery_resolve(&config,"org.umicom.bank",output,sizeof output)==UMI_STATUS_PERMISSION_DENIED);
    CHECK(memcmp(saved,output,sizeof output)==0);
    probe.status=UMI_STATUS_OK;
    const char *invalid[]={"", "relative/bin",ROOT "/../other",ROOT "/./other","//server/share","/bad\nroot"};
    calls=probe.calls;
    for(size_t i=0;i<sizeof invalid/sizeof invalid[0];++i){
        config.executable_root=invalid[i];
        CHECK(umi_application_native_discovery_resolve(&config,"org.umicom.bank",output,sizeof output)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(saved,output,sizeof output)==0 && probe.calls==calls);
    }
    config.executable_root=ROOT;config.executable_suffix=".cmd";
    CHECK(umi_application_native_discovery_resolve(&config,"org.umicom.bank",output,sizeof output)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_application_native_discovery_resolve(NULL,"org.umicom.bank",output,sizeof output)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(saved,output,sizeof output)==0);
    config=umi_application_native_discovery_config_default();
    config.executable_root=ROOT;config.probe=inspect;config.probe_context=&probe;
    for(size_t i=0;i<umi_application_portfolio_count();++i){
        const UmiApplicationDefinition *app=umi_application_portfolio_at(i);
        const char *name=umi_application_portfolio_gui_executable(app->application_id);
        if(name==NULL)continue;
        CHECK(umi_application_native_discovery_resolve(&config,app->application_id,output,sizeof output)==UMI_STATUS_OK);
        char expected[sizeof output];
        (void)snprintf(expected,sizeof expected,"%s/%s%s",ROOT,name,config.executable_suffix);
        CHECK(strcmp(output,expected)==0);
    }
    puts("Explicit GUI roots, all canonical names and transactional refusals passed.");
    return 0;
}
