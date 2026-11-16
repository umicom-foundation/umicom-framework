/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/local_models/test_session_admission.c
 *
 * PURPOSE:
 *   Prove real role and digest refusals precede supervisor creation and preserve session/lease ownership.
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
#include "../build_log/fixture.h"
#include "../model_formats/gguf_fixture.h"
#include "umicom/local_models/manager.h"
#include "umicom/platform/file_lease.h"
#include "umicom/platform/filesystem.h"
static const char *mode;
static unsigned acquired,released,created,submitted,closed;
static unsigned char lease_marker,supervisor_marker;
static char *fake_environment(const char *name) { (void)name;return NULL; }
static int fake_executable(const char *path) { assert(path!=NULL);return 1; }
static UmiStatus fake_acquire(const char *path,UmiFileLease **out)
{
    assert(path!=NULL && out!=NULL && *out==NULL);++acquired;
    if(strcmp(mode,"lease-busy")==0) return UMI_STATUS_BUSY;
    *out=(UmiFileLease *)&lease_marker;return UMI_STATUS_OK;
}
static UmiStatus fake_release(UmiFileLease **owner)
{
    assert(owner!=NULL && *owner==(UmiFileLease *)&lease_marker);++released;*owner=NULL;
    return strcmp(mode,"lease-close-failure")==0?UMI_STATUS_IO_ERROR:UMI_STATUS_OK;
}
static UmiStatus fake_create(const UmiProcessSupervisorConfig *config,UmiProcessSupervisor **out)
{
    assert(config!=NULL && config->capacity==1U && out!=NULL && *out==NULL);++created;
    if(strcmp(mode,"supervisor-failure")==0) return UMI_STATUS_OUT_OF_MEMORY;
    *out=(UmiProcessSupervisor *)&supervisor_marker;return UMI_STATUS_OK;
}
static UmiStatus fake_submit(UmiProcessSupervisor *owner,const char *label,
    const UmiProcessRequest *request,UmiProcessLifetime lifetime,UmiProcessJobId *id)
{
    assert(owner==(UmiProcessSupervisor *)&supervisor_marker && label!=NULL && request!=NULL);
    assert(lifetime==UMI_PROCESS_LIFETIME_TREE && request->argument_count==UMI_MODEL_RUN_ARGUMENTS);
    ++submitted;*id=1U;return UMI_STATUS_OK;
}
static UmiStatus fake_close(UmiProcessSupervisor **owner)
{
    assert(owner!=NULL && *owner==(UmiProcessSupervisor *)&supervisor_marker);
    ++closed;*owner=NULL;return UMI_STATUS_OK;
}
/* Only OS ownership boundaries are substituted. The selected file, metadata
 * parser, full digest and run-admission implementation remain real. No process
 * starts and no fake pointer reaches an actual supervisor or lease function. */
#define getenv fake_environment
#define umi_fs_is_file fake_executable
#define UmiFileLeaseTryAcquire fake_acquire
#define UmiFileLeaseRelease fake_release
#define umi_process_supervisor_create fake_create
#define UmiProcessSupervisorSubmitWithLifetime fake_submit
#define UmiProcessSupervisorRelease fake_close
#include "../../src/local_models/session.c"
#undef UmiProcessSupervisorRelease
#undef UmiProcessSupervisorSubmitWithLifetime
#undef umi_process_supervisor_create
#undef UmiFileLeaseRelease
#undef UmiFileLeaseTryAcquire
#undef umi_fs_is_file
#undef getenv
int main(int argc,char **argv)
{
    assert(argc==2);mode=argv[1];
    char root[UMI_PATH_CAPACITY],path[UMI_PATH_CAPACITY],server[UMI_PATH_CAPACITY];
    FixtureDirectory(root);FixturePath(path,root,"selected.gguf");FixturePath(server,root,"fixture-server.exe");
    GgufFixture fixture;fixture_model(&fixture);
    UmiStatus wanted=UMI_STATUS_OK;
    if(strcmp(mode,"auxiliary")==0 || strcmp(mode,"lease-close-failure")==0) {
        fixture_start(&fixture);fixture_text(&fixture,"general.architecture","controlvector");
        wanted=strcmp(mode,"auxiliary")==0?UMI_STATUS_NOT_IMPLEMENTED:UMI_STATUS_IO_ERROR;
    } else if(strcmp(mode,"adapter")==0) { fixture_text(&fixture,"general.type","adapter");wanted=UMI_STATUS_NOT_IMPLEMENTED; }
    else if(strcmp(mode,"unknown")==0) { fixture_start(&fixture);wanted=UMI_STATUS_NOT_IMPLEMENTED; }
    else if(strcmp(mode,"vocabulary")==0) { fixture_number_at(fixture.bytes+8U,0U,8U);wanted=UMI_STATUS_PARSE_ERROR; }
    else if(strcmp(mode,"hash-mismatch")==0) wanted=UMI_STATUS_INVALID_STATE;
    else if(strcmp(mode,"read-error")==0) wanted=UMI_STATUS_NOT_FOUND;
    else if(strcmp(mode,"lease-busy")==0 || strcmp(mode,"occupied")==0) wanted=UMI_STATUS_BUSY;
    else if(strcmp(mode,"denied")==0) wanted=UMI_STATUS_PERMISSION_DENIED;
    else if(strcmp(mode,"supervisor-failure")==0) wanted=UMI_STATUS_OUT_OF_MEMORY;
    else assert(strcmp(mode,"candidate")==0);
    unsigned char payload[64]={0};fixture_bytes(&fixture,payload,sizeof(payload));
    char digest[65];assert(UmiSha256Buffer(fixture.bytes,fixture.size,digest)==UMI_STATUS_OK);
    if(strcmp(mode,"read-error")!=0) {
        UmiOutputFile *file=NULL;assert(UmiOutputFileCreate(path,&file)==UMI_STATUS_OK);
        assert(UmiOutputFileWrite(file,fixture.bytes,fixture.size)==UMI_STATUS_OK);
        assert(UmiOutputFileClose(file)==UMI_STATUS_OK);UmiOutputFileDestroy(file);
    }
    if(strcmp(mode,"hash-mismatch")==0) digest[0]=digest[0]=='0'?'1':'0';
    UmiModelRunPlan plan;
    assert(UmiModelRunPlanCreate(server,path,digest,"fixture",18080U,512U,1U,&plan)==UMI_STATUS_OK);
    UmiModelSession retained={0},*owner=strcmp(mode,"occupied")==0?&retained:NULL;
    assert(UmiModelSessionStart(&plan,strcmp(mode,"denied")!=0,&owner)==wanted);
    if(strcmp(mode,"occupied")==0 || strcmp(mode,"denied")==0) {
        assert(owner==(strcmp(mode,"occupied")==0?&retained:NULL));
        assert(acquired==0U && released==0U && created==0U && submitted==0U);
    } else if(wanted==UMI_STATUS_OK) {
        assert(owner!=NULL && acquired==1U && released==0U && created==1U && submitted==1U);
        assert(UmiModelSessionStop(&owner)==UMI_STATUS_OK && owner==NULL);
        assert(released==1U && closed==1U);
    } else {
        assert(owner==NULL && acquired==1U && submitted==0U && closed==0U);
        assert(released==(strcmp(mode,"lease-busy")==0?0U:1U));
        assert(created==(strcmp(mode,"supervisor-failure")==0?1U:0U));
    }
    return 0;
}
