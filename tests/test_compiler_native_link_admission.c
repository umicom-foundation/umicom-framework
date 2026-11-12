/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_compiler_native_link_admission.c
 *
 * PURPOSE:
 *   Check native link-plan extents, search-root identity and shared overlap-safe text copying.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "support/assertions.h"
#include "umicom/compiler/native/link_plan.h"
#include <assert.h>
#include <string.h>

static UmiNativeLinkPlan owner, before;

static void Initialize(void)
{
    assert(umi_nc_link_plan_init(&owner,"app",UMI_NC_LINK_EXECUTABLE) == UMI_STATUS_OK);
}

static void Snapshot(void) { (void)memcpy(&before,&owner,sizeof(before)); }
static void Unchanged(void) { assert(memcmp(&owner,&before,sizeof(owner)) == 0); }

static void TextCopies(void)
{
    char text[16] = "umicom", snapshot[16];
    assert(umi_nc_copy_text(text,sizeof(text),text) == UMI_STATUS_OK);
    assert(strcmp(text,"umicom") == 0);
    assert(umi_nc_copy_text(text,sizeof(text),text + 3) == UMI_STATUS_OK);
    assert(strcmp(text,"com") == 0);
    assert(umi_nc_copy_text(text + 1,sizeof(text) - 1U,text) == UMI_STATUS_OK);
    assert(strcmp(text,"ccom") == 0);
    (void)memcpy(snapshot,text,sizeof(text));
    char unclosed[16]; (void)memset(unclosed,'x',sizeof(unclosed));
    assert(umi_nc_copy_text(text,sizeof(text),unclosed) == UMI_STATUS_CAPACITY_EXCEEDED);
    assert(memcmp(text,snapshot,sizeof(text)) == 0);
    assert(umi_nc_copy_text(text,0U,"x") == UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_nc_copy_text(text,sizeof(text),NULL) == UMI_STATUS_INVALID_ARGUMENT);
    assert(memcmp(text,snapshot,sizeof(text)) == 0);
    assert(umi_nc_copy_text(NULL,1U,"") == UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_nc_copy_text(text,1U,"") == UMI_STATUS_OK && text[0] == '\0');
    char exact[16]; (void)memset(exact,'x',sizeof(exact) - 1U); exact[sizeof(exact) - 1U] = '\0';
    assert(umi_nc_copy_text(text,sizeof(text),exact) == UMI_STATUS_OK);
    assert(strcmp(text,exact) == 0);
}

static void Construction(void)
{
    Initialize(); Snapshot();
    assert(umi_nc_link_plan_init(&owner,NULL,UMI_NC_LINK_EXECUTABLE) == UMI_STATUS_INVALID_ARGUMENT); Unchanged();
    assert(umi_nc_link_plan_init(&owner,"",UMI_NC_LINK_EXECUTABLE) == UMI_STATUS_INVALID_ARGUMENT); Unchanged();
    const UmiNativeLinkOutputKind bad[] = {(UmiNativeLinkOutputKind)0,(UmiNativeLinkOutputKind)-1,(UmiNativeLinkOutputKind)4};
    for (size_t index = 0U; index < sizeof(bad)/sizeof(bad[0]); ++index) {
        assert(umi_nc_link_plan_init(&owner,"app",bad[index]) == UMI_STATUS_INVALID_ARGUMENT); Unchanged();
    }
    char unclosed[UMI_NC_PATH_CAPACITY]; (void)memset(unclosed,'x',sizeof(unclosed));
    assert(umi_nc_link_plan_init(&owner,unclosed,UMI_NC_LINK_EXECUTABLE) == UMI_STATUS_CAPACITY_EXCEEDED); Unchanged();
    assert(umi_nc_link_plan_init(&owner,owner.output,UMI_NC_LINK_SHARED) == UMI_STATUS_OK);
    assert(strcmp(owner.output,"app") == 0 && owner.kind == UMI_NC_LINK_SHARED);
    assert(umi_nc_link_plan_init(NULL,"app",UMI_NC_LINK_STATIC) == UMI_STATUS_INVALID_ARGUMENT);
}

static void RejectMalformed(void)
{
    Snapshot();
    assert(!umi_nc_link_plan_valid(&owner));
    assert(umi_nc_link_plan_fingerprint(&owner) == 0U);
    assert(umi_nc_link_plan_add_input(&owner,"main.o") == UMI_STATUS_INVALID_STATE);
    assert(umi_nc_link_plan_add_library(&owner,"m") == UMI_STATUS_INVALID_STATE);
    assert(umi_nc_link_plan_add_search_path(&owner,"lib") == UMI_STATUS_INVALID_STATE);
    Unchanged();
}

static void Storage(void)
{
    assert(!umi_nc_link_plan_valid(NULL) && umi_nc_link_plan_fingerprint(NULL) == 0U);
    for (unsigned field = 0U; field < 11U; ++field) {
        Initialize();
        switch (field) {
        case 0U: (void)memset(owner.output,'x',sizeof(owner.output)); break;
        case 1U: owner.output[0] = '\0'; break;
        case 2U: owner.input_count = 1U; (void)memset(owner.inputs[0],'x',sizeof(owner.inputs[0])); break;
        case 3U: owner.library_count = 1U; (void)memset(owner.libraries[0],'x',sizeof(owner.libraries[0])); break;
        case 4U: owner.search_path_count = 1U; (void)memset(owner.search_paths[0],'x',sizeof(owner.search_paths[0])); break;
        case 5U: owner.input_count = 1U; break;
        case 6U: owner.library_count = 1U; break;
        case 7U: owner.search_path_count = 1U; break;
        case 8U: owner.kind = (UmiNativeLinkOutputKind)0; break;
        case 9U: owner.kind = (UmiNativeLinkOutputKind)-1; break;
        case 10U: owner.kind = (UmiNativeLinkOutputKind)4; break;
        default: assert(false); break;
        }
        RejectMalformed();
    }
    const size_t limits[] = {UMI_NC_MAX_LINK_INPUTS,UMI_NC_MAX_LINK_LIBRARIES,UMI_NC_MAX_LINK_SEARCH_PATHS};
    for (size_t field = 0U; field < sizeof(limits)/sizeof(limits[0]); ++field) {
        const size_t bad[] = {limits[field] + 1U,SIZE_MAX};
        for (size_t index = 0U; index < sizeof(bad)/sizeof(bad[0]); ++index) {
            Initialize();
            if (field == 0U) owner.input_count = bad[index];
            else if (field == 1U) owner.library_count = bad[index];
            else owner.search_path_count = bad[index];
            RejectMalformed();
        }
    }
    Initialize();
    (void)memset(owner.inputs[UMI_NC_MAX_LINK_INPUTS - 1U],'x',sizeof(owner.inputs[0]));
    (void)memset(owner.libraries[UMI_NC_MAX_LINK_LIBRARIES - 1U],'x',sizeof(owner.libraries[0]));
    (void)memset(owner.search_paths[UMI_NC_MAX_LINK_SEARCH_PATHS - 1U],'x',sizeof(owner.search_paths[0]));
    assert(umi_nc_link_plan_valid(&owner));
}

typedef UmiStatus (*AddEntry)(UmiNativeLinkPlan *,const char *);

static void Additions(void)
{
    const AddEntry add[] = {umi_nc_link_plan_add_input,umi_nc_link_plan_add_library,umi_nc_link_plan_add_search_path};
    const size_t limits[] = {UMI_NC_MAX_LINK_INPUTS,UMI_NC_MAX_LINK_LIBRARIES,UMI_NC_MAX_LINK_SEARCH_PATHS};
    for (size_t field = 0U; field < sizeof(add)/sizeof(add[0]); ++field) {
        Initialize(); Snapshot();
        char unclosed[UMI_NC_PATH_CAPACITY]; (void)memset(unclosed,'x',sizeof(unclosed));
        assert(add[field](&owner,unclosed) == UMI_STATUS_CAPACITY_EXCEEDED); Unchanged();
        assert(add[field](&owner,NULL) == UMI_STATUS_INVALID_ARGUMENT); Unchanged();
        assert(add[field](&owner,"") == UMI_STATUS_INVALID_ARGUMENT); Unchanged();
        assert(add[field](NULL,"item") == UMI_STATUS_INVALID_ARGUMENT);
        assert(add[field](&owner,"first") == UMI_STATUS_OK);
        const char *borrowed = field == 0U ? owner.inputs[0] : field == 1U ? owner.libraries[0] : owner.search_paths[0];
        assert(add[field](&owner,borrowed) == UMI_STATUS_OK);
        assert(strcmp(field == 0U ? owner.inputs[1] : field == 1U ? owner.libraries[1] : owner.search_paths[1],"first") == 0);
        for (size_t index = 2U; index < limits[field]; ++index)
            assert(add[field](&owner,"item") == UMI_STATUS_OK);
        Snapshot();
        assert(add[field](&owner,"overflow") == UMI_STATUS_CAPACITY_EXCEEDED); Unchanged();
        assert(umi_nc_link_plan_valid(&owner));
    }
}

static void Fingerprints(void)
{
    Initialize();
    assert(umi_nc_link_plan_add_input(&owner,"main.o") == UMI_STATUS_OK);
    assert(umi_nc_link_plan_add_library(&owner,"m") == UMI_STATUS_OK);
    /* Golden identity from the established no-search-root calculation. */
    const uint64_t legacy = UINT64_C(0xdd3892749af64379);
    assert(umi_nc_link_plan_fingerprint(&owner) == legacy);
    assert(umi_nc_link_plan_add_search_path(&owner,"first/lib") == UMI_STATUS_OK);
    const uint64_t first = umi_nc_link_plan_fingerprint(&owner);
    assert(first != legacy);
    assert(umi_nc_link_plan_add_search_path(&owner,"second/lib") == UMI_STATUS_OK);
    const uint64_t ordered = umi_nc_link_plan_fingerprint(&owner);
    assert(ordered != first);
    char temporary[UMI_NC_PATH_CAPACITY];
    (void)memcpy(temporary,owner.search_paths[0],sizeof(temporary));
    (void)memcpy(owner.search_paths[0],owner.search_paths[1],sizeof(temporary));
    (void)memcpy(owner.search_paths[1],temporary,sizeof(temporary));
    assert(umi_nc_link_plan_fingerprint(&owner) != ordered);
    Snapshot();
    const uint64_t stable = umi_nc_link_plan_fingerprint(&owner);
    assert(umi_nc_link_plan_fingerprint(&owner) == stable);
    Unchanged();
}

int main(void)
{
    TextCopies();
    Construction();
    Storage();
    Additions();
    Fingerprints();
    return 0;
}
