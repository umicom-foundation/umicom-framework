/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/development_workflow/test_delivery_contracts.c
 * PURPOSE: Check command ownership, delivery phase order and refusal boundaries.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/build/cpack_provider.h"
#include "umicom/build/project_session.h"
#include "umicom/platform/threading.h"
#include <stdio.h>
#include <string.h>
#define CHECK(c) do { if(!(c)){fprintf(stderr,"line %d: %s\n",__LINE__,#c);return 1;} }while(0)
typedef struct Context { UmiBuildPhase phases[4]; size_t count; int cancel; int fail; int targetCleared; } Context;
static UmiStatus Execute(const UmiBuildProfile *profile,UmiBuildPhase phase,
    UmiCancellationToken *token,UmiBuildResult *result,void *data)
{
    Context *context=data;
    if(context->count >= 4U)return UMI_STATUS_CAPACITY_EXCEEDED;
    context->phases[context->count++]=phase;
    if(profile->build_target[0]=='\0')context->targetCleared=1;
    UmiStatus status=context->fail && phase==UMI_BUILD_PHASE_TEST?UMI_STATUS_IO_ERROR:UMI_STATUS_OK;
    if(context->cancel)umi_cancellation_token_request(token);
    umi_build_result_finish(result,status,status==UMI_STATUS_OK?0:7,0U);
    return status;
}
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *mode=argv[1];
    UmiBuildProfile profile;umi_build_profile_init(&profile);
    CHECK(umi_build_profile_set(&profile,"notes","/tmp/Notes with spaces","build dir")==UMI_STATUS_OK);
    strcpy(profile.install_directory,"install dir");strcpy(profile.build_target,"only-app");
    UmiBuildProvider provider=UmiBuildCPackProvider();UmiBuildCommand command;
    if(strcmp(mode,"provider-argv")==0){
        CHECK(umi_build_provider_create_command(&provider,&profile,UMI_BUILD_PHASE_PACKAGE,&command)==UMI_STATUS_OK);
        CHECK(strcmp(command.program,"cpack")==0 && command.argument_count==10U);
        CHECK(strcmp(command.arguments[1],"build dir/CPackConfig.cmake")==0);
        CHECK(strcmp(command.arguments[7],"build dir/packages")==0);
        CHECK(strcmp(command.arguments[9],"CPACK_PACKAGE_CHECKSUM=SHA256")==0);
        CHECK(strcmp(command.working_directory,profile.source_directory)==0);return 0;
    }
    if(strcmp(mode,"provider-invalid")==0){
        memset(&command,0x5A,sizeof(command));UmiBuildCommand original=command;
        CHECK(provider.create_command(NULL,UMI_BUILD_PHASE_PACKAGE,&command)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(provider.create_command(&profile,UMI_BUILD_PHASE_BUILD,&command)==UMI_STATUS_NOT_IMPLEMENTED);
        memset(profile.build_directory,'x',sizeof(profile.build_directory));
        CHECK(provider.create_command(&profile,UMI_BUILD_PHASE_PACKAGE,&command)!=UMI_STATUS_OK);
        CHECK(memcmp(&command,&original,sizeof(command))==0);return 0;
    }
    Context context={0};UmiBuildProjectSessionConfig config={0};config.execute=Execute;config.context=&context;
    UmiBuildProjectSession *session=NULL;CHECK(umi_build_project_session_create(&config,&session)==UMI_STATUS_OK);
    if(strcmp(mode,"tests-disabled")==0){profile.build_testing=0;
        CHECK(umi_build_project_session_submit(session,&profile,UMI_BUILD_PHASE_PACKAGE,true)==UMI_STATUS_INVALID_STATE);
        CHECK(umi_build_project_session_submit(session,&profile,UMI_BUILD_PHASE_DEPLOY,true)==UMI_STATUS_INVALID_STATE);
        CHECK(context.count==0U);umi_build_project_session_destroy(session);return 0;
    }
    context.cancel=strcmp(mode,"cancel")==0;context.fail=strcmp(mode,"test-refusal")==0;
    UmiBuildPhase phase=strcmp(mode,"rebuild-plan")==0?UMI_BUILD_PHASE_REBUILD:
        strcmp(mode,"deploy-plan")==0?UMI_BUILD_PHASE_DEPLOY:UMI_BUILD_PHASE_PACKAGE;
    CHECK(umi_build_project_session_submit(session,&profile,phase,false)==UMI_STATUS_PERMISSION_DENIED);
    CHECK(umi_build_project_session_submit(session,&profile,phase,true)==UMI_STATUS_OK);
    strcpy(profile.profile_id,"caller changed later");
    UmiBuildProjectSessionSnapshot progress={0};
    for(unsigned i=0;i<1000U;++i){CHECK(umi_build_project_session_snapshot(session,&progress)==UMI_STATUS_OK);if(!progress.active)break;umi_thread_sleep_ms(1U);}
    CHECK(!progress.active);
    CHECK(context.phases[0]==UMI_BUILD_PHASE_CONFIGURE);
    if(context.cancel){CHECK(progress.status==UMI_STATUS_CANCELLED && context.count==1U);}
    else if(context.fail){CHECK(progress.status==UMI_STATUS_IO_ERROR && context.count==3U);}
    else if(phase==UMI_BUILD_PHASE_REBUILD){CHECK(context.count==3U && context.phases[1]==UMI_BUILD_PHASE_CLEAN && context.phases[2]==UMI_BUILD_PHASE_BUILD);}
    else {CHECK(context.count==4U && context.phases[1]==UMI_BUILD_PHASE_BUILD && context.phases[2]==UMI_BUILD_PHASE_TEST && context.phases[3]==phase);CHECK(context.targetCleared);}
    UmiBuildResult *result=NULL;CHECK(umi_build_result_create(&result)==UMI_STATUS_OK);
    CHECK(umi_build_project_session_result_at(session,0U,result)==UMI_STATUS_OK);
    CHECK(strcmp(result->profile_id,"notes")==0);
    umi_build_result_destroy(result);umi_build_project_session_destroy(session);return 0;
}
