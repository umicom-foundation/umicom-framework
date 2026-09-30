/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/local_profile/test_local_profile.c
 * PURPOSE: Regress local profile validation, failure atomicity, verification and credential removal.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
static const char password[]="local test phrase";
static void Names(void)
{
    char output[49]; memset(output,'X',sizeof(output));
    CHECK(UmiLocalProfileName("../../other",output)==UMI_STATUS_INVALID_ARGUMENT && output[0]=='X');
    CHECK(UmiLocalProfileName("2name",output)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiLocalProfileName("ab",output)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiLocalProfileName("Some_User-123",output)==UMI_STATUS_OK && strcmp(output,"some_user-123")==0);
    char long_name[50]; memset(long_name,'a',sizeof(long_name)); long_name[49]='\0';
    CHECK(UmiLocalProfileName(long_name,output)==UMI_STATUS_INVALID_ARGUMENT);
    long_name[48]='\0'; CHECK(UmiLocalProfileName(long_name,output)==UMI_STATUS_OK);
}
static void Credentials(void)
{
    ProfileFixture f={0}; UmiLocalProfileStore *store=FixtureStore(&f);
    CHECK(UmiLocalProfileRegister(store,"Alice",password)==UMI_STATUS_OK);
    CHECK(strcmp(f.name,"alice")==0 && f.writes==1);
    UmiLocalProfileRecord saved=f.record;
    CHECK(UmiLocalProfileRegister(store,"ALICE","another password")==UMI_STATUS_ALREADY_EXISTS);
    CHECK(f.writes==1 && memcmp(&saved,&f.record,sizeof(saved))==0);
    CHECK(UmiLocalProfileVerify(store,"ALICE",password)==UMI_STATUS_OK);
    CHECK(UmiLocalProfileVerify(store,"alice","wrong test phrase")==UMI_STATUS_PERMISSION_DENIED);
    CHECK(UmiLocalProfileVerify(store,"missing",password)==UMI_STATUS_PERMISSION_DENIED);
    CHECK(UmiLocalProfileRemove(store,"alice","wrong test phrase")==UMI_STATUS_PERMISSION_DENIED && f.present);
    f.conflict=1;
    CHECK(UmiLocalProfileRemove(store,"alice",password)==UMI_STATUS_INVALID_STATE && f.present);
    f.conflict=0;
    CHECK(UmiLocalProfileRemove(store,"Alice",password)==UMI_STATUS_OK && !f.present && f.removals==1);
    CHECK(UmiLocalProfileVerify(store,"alice",password)==UMI_STATUS_PERMISSION_DENIED);
    UmiLocalProfileStoreRetain(store); UmiLocalProfileStoreRelease(store); UmiLocalProfileStoreRelease(store);
}
static void Failures(void)
{
    ProfileFixture f={0}; UmiLocalProfileStore *store=FixtureStore(&f);
    CHECK(UmiLocalProfileRegister(store,"alice","short")==UMI_STATUS_INVALID_ARGUMENT && f.writes==0);
    f.read_error=UMI_STATUS_IO_ERROR;
    CHECK(UmiLocalProfileRegister(store,"alice",password)==UMI_STATUS_IO_ERROR && f.writes==0);
    f.read_error=UMI_STATUS_OK; f.random_error=UMI_STATUS_UNAVAILABLE;
    CHECK(UmiLocalProfileRegister(store,"alice",password)==UMI_STATUS_UNAVAILABLE && f.writes==0);
    f.random_error=UMI_STATUS_OK; f.derive_error=UMI_STATUS_UNAVAILABLE;
    CHECK(UmiLocalProfileRegister(store,"alice",password)==UMI_STATUS_UNAVAILABLE && f.writes==0);
    f.derive_error=UMI_STATUS_OK; f.conflict=1;
    CHECK(UmiLocalProfileRegister(store,"alice",password)==UMI_STATUS_ALREADY_EXISTS && f.writes==0);
    f.conflict=0; CHECK(UmiLocalProfileRegister(store,"alice",password)==UMI_STATUS_OK);
    f.record.iterations=1U; int before=f.derivations;
    CHECK(UmiLocalProfileVerify(store,"alice",password)==UMI_STATUS_PARSE_ERROR && f.derivations==before);
    f.record.iterations=UMI_LOCAL_PROFILE_ITERATIONS; f.record.version=2U;
    CHECK(UmiLocalProfileRemove(store,"alice",password)==UMI_STATUS_PARSE_ERROR && f.present);
    UmiLocalProfileStoreRelease(store);
}
int main(int argc,char **argv)
{
    if(argc!=2) return 2;
    if(strcmp(argv[1],"names")==0) Names();
    else if(strcmp(argv[1],"credentials")==0) Credentials();
    else if(strcmp(argv[1],"failures")==0) Failures();
    else return 2;
    return 0;
}
