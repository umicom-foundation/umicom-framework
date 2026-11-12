/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_compiler_linker_profile_admission.c
 *
 * PURPOSE:
 *   Exercise linker-profile construction, bounded collections, aliases and exhausted revisions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "support/assertions.h"
#include "umicom/compiler/linker_profile.h"
#include <assert.h>
#include <string.h>

static UmiLinkerProfile owner, before;
static UmiCompilerTarget target;

static void Initialize(void)
{
    assert(umi_compiler_target_parse("x86_64-unknown-linux-gnu",&target) == UMI_STATUS_OK);
    assert(umi_linker_profile_init(&owner,"release","cc",&target) == UMI_STATUS_OK);
}

static void Snapshot(void)
{
    (void)memcpy(&before,&owner,sizeof(before));
}

static void Unchanged(void)
{
    assert(memcmp(&owner,&before,sizeof(owner)) == 0);
}

static void Construction(void)
{
    Initialize();
    assert(owner.revision == 1U && owner.library_count == 0U && owner.library_path_count == 0U);
    Snapshot();
    assert(umi_linker_profile_init(&owner,NULL,"cc",&target) == UMI_STATUS_INVALID_ARGUMENT); Unchanged();
    assert(umi_linker_profile_init(&owner,"","cc",&target) == UMI_STATUS_INVALID_ARGUMENT); Unchanged();
    assert(umi_linker_profile_init(&owner,"release",NULL,&target) == UMI_STATUS_INVALID_ARGUMENT); Unchanged();
    assert(umi_linker_profile_init(&owner,"release","",&target) == UMI_STATUS_INVALID_ARGUMENT); Unchanged();
    assert(umi_linker_profile_init(&owner,"release","cc",NULL) == UMI_STATUS_INVALID_ARGUMENT); Unchanged();
    char unclosed[UMI_COMPILER_PATH_CAPACITY];
    (void)memset(unclosed,'x',sizeof(unclosed));
    assert(umi_linker_profile_init(&owner,unclosed,"cc",&target) == UMI_STATUS_CAPACITY_EXCEEDED); Unchanged();
    assert(umi_linker_profile_init(&owner,"release",unclosed,&target) == UMI_STATUS_CAPACITY_EXCEEDED); Unchanged();
    UmiCompilerTarget bad = target;
    (void)memset(bad.triple,'x',sizeof(bad.triple));
    assert(umi_linker_profile_init(&owner,"release","cc",&bad) == UMI_STATUS_INVALID_ARGUMENT); Unchanged();
    assert(umi_linker_profile_init(&owner,owner.profile_id,owner.driver,&owner.target) == UMI_STATUS_OK);
    assert(strcmp(owner.profile_id,"release") == 0 && strcmp(owner.driver,"cc") == 0);
    assert(strcmp(owner.target.triple,target.triple) == 0 && owner.revision == 1U);
    assert(umi_linker_profile_init(NULL,"release","cc",&target) == UMI_STATUS_INVALID_ARGUMENT);
}

static void Corrupt(unsigned field)
{
    switch (field) {
    case 0U: (void)memset(owner.profile_id,'x',sizeof(owner.profile_id)); break;
    case 1U: (void)memset(owner.driver,'x',sizeof(owner.driver)); break;
    case 2U: (void)memset(owner.target.triple,'x',sizeof(owner.target.triple)); break;
    case 3U: (void)memset(owner.target.architecture,'x',sizeof(owner.target.architecture)); break;
    case 4U: (void)memset(owner.target.vendor,'x',sizeof(owner.target.vendor)); break;
    case 5U: (void)memset(owner.target.operating_system,'x',sizeof(owner.target.operating_system)); break;
    case 6U: (void)memset(owner.target.environment,'x',sizeof(owner.target.environment)); break;
    case 7U: owner.library_path_count = 1U; (void)memset(owner.library_paths[0],'x',sizeof(owner.library_paths[0])); break;
    case 8U: owner.library_count = 1U; (void)memset(owner.libraries[0],'x',sizeof(owner.libraries[0])); break;
    case 9U: owner.library_path_count = 1U; break;
    case 10U: owner.library_count = 1U; break;
    default: assert(false); break;
    }
}

static void RejectMalformed(void)
{
    char reason[UMI_COMPILER_TEXT_CAPACITY];
    Snapshot();
    assert(!umi_linker_profile_storage_valid(&owner));
    assert(umi_linker_profile_add_path(&owner,"lib") == UMI_STATUS_INVALID_STATE);
    assert(umi_linker_profile_add_library(&owner,"m") == UMI_STATUS_INVALID_STATE);
    assert(umi_linker_profile_validate(&owner,reason,sizeof(reason)) == UMI_STATUS_INVALID_STATE);
    Unchanged();
}

static void Storage(void)
{
    assert(!umi_linker_profile_storage_valid(NULL));
    for (unsigned field = 0U; field < 11U; ++field) {
        Initialize(); Corrupt(field); RejectMalformed();
    }
    for (unsigned field = 0U; field < 2U; ++field) {
        const size_t limit = field == 0U ? UMI_LINKER_MAX_PATHS : UMI_LINKER_MAX_LIBRARIES;
        const size_t bad_counts[] = {limit + 1U,SIZE_MAX};
        for (size_t index = 0U; index < sizeof(bad_counts)/sizeof(bad_counts[0]); ++index) {
            Initialize();
            if (field == 0U) owner.library_path_count = bad_counts[index];
            else owner.library_count = bad_counts[index];
            RejectMalformed();
        }
    }
    Initialize();
    /* Inactive bytes are not part of the active storage contract. */
    (void)memset(owner.library_paths[UMI_LINKER_MAX_PATHS - 1U],'x',sizeof(owner.library_paths[0]));
    (void)memset(owner.libraries[UMI_LINKER_MAX_LIBRARIES - 1U],'x',sizeof(owner.libraries[0]));
    assert(umi_linker_profile_storage_valid(&owner));
    char reason[UMI_COMPILER_TEXT_CAPACITY];
    assert(umi_linker_profile_validate(&owner,reason,sizeof(reason)) == UMI_STATUS_OK);
    owner.driver[0] = '\0';
    assert(umi_linker_profile_storage_valid(&owner));
    assert(umi_linker_profile_validate(&owner,reason,sizeof(reason)) == UMI_STATUS_INVALID_STATE);
    assert(umi_linker_profile_validate(NULL,reason,sizeof(reason)) == UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_linker_profile_validate(&owner,NULL,sizeof(reason)) == UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_linker_profile_validate(&owner,reason,0U) == UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_linker_profile_validate(&owner,reason,1U) == UMI_STATUS_CAPACITY_EXCEEDED);
}

typedef UmiStatus (*AddEntry)(UmiLinkerProfile *,const char *);

static void Additions(void)
{
    const AddEntry add[] = {umi_linker_profile_add_path,umi_linker_profile_add_library};
    for (size_t field = 0U; field < sizeof(add)/sizeof(add[0]); ++field) {
        const size_t limit = field == 0U ? UMI_LINKER_MAX_PATHS : UMI_LINKER_MAX_LIBRARIES;
        Initialize();
        char unclosed[UMI_COMPILER_PATH_CAPACITY];
        (void)memset(unclosed,'x',sizeof(unclosed));
        Snapshot();
        assert(add[field](&owner,unclosed) == UMI_STATUS_CAPACITY_EXCEEDED); Unchanged();
        assert(add[field](&owner,NULL) == UMI_STATUS_INVALID_ARGUMENT); Unchanged();
        assert(add[field](&owner,"") == UMI_STATUS_INVALID_ARGUMENT); Unchanged();
        assert(add[field](NULL,"item") == UMI_STATUS_INVALID_ARGUMENT);
        assert(add[field](&owner,"first") == UMI_STATUS_OK);
        const char *borrowed = field == 0U ? owner.library_paths[0] : owner.libraries[0];
        assert(add[field](&owner,borrowed) == UMI_STATUS_OK);
        assert(strcmp(field == 0U ? owner.library_paths[1] : owner.libraries[1],"first") == 0);
        assert(owner.revision == 3U);
        for (size_t index = 2U; index < limit; ++index)
            assert(add[field](&owner,"item") == UMI_STATUS_OK);
        Snapshot();
        assert(add[field](&owner,"overflow") == UMI_STATUS_CAPACITY_EXCEEDED); Unchanged();
        Initialize(); owner.revision = UINT64_MAX - 1U;
        assert(add[field](&owner,"last") == UMI_STATUS_OK);
        assert(owner.revision == UINT64_MAX);
        Snapshot();
        assert(add[field](&owner,"exhausted") == UMI_STATUS_CAPACITY_EXCEEDED); Unchanged();
        char reason[UMI_COMPILER_TEXT_CAPACITY];
        assert(umi_linker_profile_storage_valid(&owner));
        assert(umi_linker_profile_validate(&owner,reason,sizeof(reason)) == UMI_STATUS_OK);
        Unchanged();
    }
}

int main(void)
{
    Construction();
    Storage();
    Additions();
    return 0;
}
