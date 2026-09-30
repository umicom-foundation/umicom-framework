/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/local_profile/fixture.h
 * PURPOSE: Provide an isolated non-cryptographic test double; never used by production profiles.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LOCAL_PROFILE_TEST_FIXTURE_H
#define UMICOM_LOCAL_PROFILE_TEST_FIXTURE_H
#include "umicom/security/local_profile.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x); exit(1); } } while (0)
typedef struct ProfileFixture {
    char name[49]; UmiLocalProfileRecord record; int present;
    UmiStatus read_error, random_error, derive_error;
    int conflict, writes, removals, derivations;
} ProfileFixture;
static UmiStatus FixtureRead(void *data,const char *name,UmiLocalProfileRecord *out)
{
    ProfileFixture *f=data;
    if(f->read_error!=UMI_STATUS_OK) return f->read_error;
    if(!f->present || strcmp(name,f->name)!=0) return UMI_STATUS_NOT_FOUND;
    *out=f->record; return UMI_STATUS_OK;
}
static UmiStatus FixtureCreate(void *data,const char *name,const UmiLocalProfileRecord *record)
{
    ProfileFixture *f=data;
    if(f->present || f->conflict) return UMI_STATUS_ALREADY_EXISTS;
    strcpy(f->name,name); f->record=*record; f->present=1; ++f->writes; return UMI_STATUS_OK;
}
static UmiStatus FixtureRemove(void *data,const char *name,const UmiLocalProfileRecord *record)
{
    ProfileFixture *f=data;
    if(!f->present || strcmp(name,f->name)!=0) return UMI_STATUS_NOT_FOUND;
    if(f->conflict || memcmp(record,&f->record,sizeof(*record))!=0) return UMI_STATUS_INVALID_STATE;
    f->present=0; ++f->removals; return UMI_STATUS_OK;
}
static UmiStatus FixtureRandom(void *data,unsigned char *bytes,size_t length)
{
    ProfileFixture *f=data;
    if(f->random_error!=UMI_STATUS_OK) return f->random_error;
    for(size_t i=0U;i<length;++i) bytes[i]=(unsigned char)(i+1U);
    return UMI_STATUS_OK;
}
/* Deliberately cheap and not cryptographic. This verifies policy and ownership
 * without writing a real user's credential vault or claiming crypto coverage. */
static UmiStatus FixtureDerive(void *data,const char *password,size_t length,
                              const UmiLocalProfileRecord *record,unsigned char out[32])
{
    ProfileFixture *f=data; ++f->derivations;
    if(f->derive_error!=UMI_STATUS_OK) return f->derive_error;
    for(size_t i=0U;i<32U;++i) out[i]=(unsigned char)((unsigned char)password[i%length]^record->salt[i]);
    return UMI_STATUS_OK;
}
static inline UmiLocalProfileStore *FixtureStore(ProfileFixture *fixture)
{
    UmiLocalProfileBackend backend={fixture,FixtureRead,FixtureCreate,FixtureRemove,FixtureRandom,FixtureDerive,NULL};
    UmiLocalProfileStore *store=NULL; CHECK(UmiLocalProfileStoreCreate(&backend,&store)==UMI_STATUS_OK); return store;
}
#endif
