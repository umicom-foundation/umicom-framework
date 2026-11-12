/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_compiler_manifest_admission.c
 *
 * PURPOSE:
 *   Verify bounded manifest records, aliased construction and failure-preserving unit publication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "support/assertions.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "umicom/compiler/manifest.h"

static UmiCompilerProjectManifest manifest, saved;
static UmiCompilerUnitManifest unit;
typedef struct Field { size_t offset, capacity; } Field;
static const Field unit_fields[] = {
    {offsetof(UmiCompilerUnitManifest,unit_id),sizeof(unit.unit_id)},
    {offsetof(UmiCompilerUnitManifest,source),sizeof(unit.source)},
    {offsetof(UmiCompilerUnitManifest,output),sizeof(unit.output)},
    {offsetof(UmiCompilerUnitManifest,provider_id),sizeof(unit.provider_id)},
    {offsetof(UmiCompilerUnitManifest,exports_abi),sizeof(unit.exports_abi)}
};
static const Field manifest_fields[] = {
    {offsetof(UmiCompilerProjectManifest,project_id),sizeof(manifest.project_id)},
    {offsetof(UmiCompilerProjectManifest,name),sizeof(manifest.name)},
    {offsetof(UmiCompilerProjectManifest,target),sizeof(manifest.target)}
};

static void MakeUnit(size_t number)
{
    (void)memset(&unit,0,sizeof(unit));
    (void)snprintf(unit.unit_id,sizeof(unit.unit_id),"unit-%zu",number);
    (void)snprintf(unit.source,sizeof(unit.source),"source-%zu.c",number);
    (void)snprintf(unit.output,sizeof(unit.output),"output-%zu.o",number);
    unit.language = UMI_COMPILER_LANGUAGE_C;
}

static void Seed(void)
{
    assert(umi_compiler_manifest_init(&manifest,"project","Example","target") == UMI_STATUS_OK);
    MakeUnit(0U);
    assert(umi_compiler_manifest_add_unit(&manifest,&unit) == UMI_STATUS_OK);
    MakeUnit(1U);
    assert(umi_compiler_manifest_add_unit(&manifest,&unit) == UMI_STATUS_OK);
}

static void Save(void) { (void)memcpy(&saved,&manifest,sizeof(manifest)); }
static void Same(void) { assert(memcmp(&saved,&manifest,sizeof(manifest)) == 0); }

static void Construction(void)
{
    Seed();
    assert(umi_compiler_manifest_init(&manifest,manifest.project_id,manifest.name,manifest.target) == UMI_STATUS_OK);
    assert(strcmp(manifest.project_id,"project") == 0 && strcmp(manifest.name,"Example") == 0);
    assert(strcmp(manifest.target,"target") == 0 && manifest.unit_count == 0U && manifest.revision == 1U);
    char long_id[UMI_COMPILER_ID_CAPACITY], long_name[UMI_COMPILER_NAME_CAPACITY];
    (void)memset(long_id,'i',sizeof(long_id)); (void)memset(long_name,'n',sizeof(long_name));
    Save();
    assert(umi_compiler_manifest_init(&manifest,long_id,"Name","target") == UMI_STATUS_CAPACITY_EXCEEDED); Same();
    assert(umi_compiler_manifest_init(&manifest,"id",long_name,"target") == UMI_STATUS_CAPACITY_EXCEEDED); Same();
    assert(umi_compiler_manifest_init(&manifest,"id","Name",long_name) == UMI_STATUS_CAPACITY_EXCEEDED); Same();
    assert(umi_compiler_manifest_init(&manifest,"","Name","target") == UMI_STATUS_INVALID_ARGUMENT); Same();
    assert(umi_compiler_manifest_init(&manifest,"id",NULL,"target") == UMI_STATUS_INVALID_ARGUMENT); Same();
    assert(umi_compiler_manifest_storage_valid(&manifest));
    assert(umi_compiler_manifest_validate(&manifest,NULL,0U) == UMI_STATUS_INVALID_STATE);
}

static void MalformedStorage(void)
{
    const size_t bad_counts[] = {UMI_COMPILER_MAX_UNITS + 1U,SIZE_MAX};
    for (size_t i = 0U; i < sizeof(bad_counts)/sizeof(bad_counts[0]); ++i) {
        Seed(); manifest.unit_count = bad_counts[i]; MakeUnit(2U); Save();
        assert(!umi_compiler_manifest_storage_valid(&manifest));
        assert(umi_compiler_manifest_find_unit(&manifest,"unit-0") == NULL);
        assert(umi_compiler_manifest_add_unit(&manifest,&unit) == UMI_STATUS_INVALID_STATE);
        assert(umi_compiler_manifest_validate(&manifest,NULL,0U) == UMI_STATUS_INVALID_STATE); Same();
    }
    for (size_t i = 0U; i < sizeof(manifest_fields)/sizeof(manifest_fields[0]); ++i) {
        Seed(); (void)memset((char *)&manifest+manifest_fields[i].offset,'x',manifest_fields[i].capacity);
        MakeUnit(2U); Save();
        assert(umi_compiler_manifest_find_unit(&manifest,"unit-0") == NULL);
        assert(umi_compiler_manifest_add_unit(&manifest,&unit) == UMI_STATUS_INVALID_STATE);
        assert(umi_compiler_manifest_validate(&manifest,NULL,0U) == UMI_STATUS_INVALID_STATE); Same();
    }
    for (size_t i = 0U; i < sizeof(unit_fields)/sizeof(unit_fields[0]); ++i) {
        Seed(); (void)memset((char *)&manifest.units[1]+unit_fields[i].offset,'x',unit_fields[i].capacity);
        MakeUnit(2U); Save();
        assert(umi_compiler_manifest_find_unit(&manifest,"unit-0") == NULL);
        assert(umi_compiler_manifest_add_unit(&manifest,&unit) == UMI_STATUS_INVALID_STATE);
        assert(umi_compiler_manifest_validate(&manifest,NULL,0U) == UMI_STATUS_INVALID_STATE); Same();
        Seed(); MakeUnit(2U); (void)memset((char *)&unit+unit_fields[i].offset,'x',unit_fields[i].capacity); Save();
        assert(!umi_compiler_unit_manifest_valid(&unit));
        assert(umi_compiler_manifest_add_unit(&manifest,&unit) == UMI_STATUS_INVALID_ARGUMENT); Same();
    }
    Seed(); MakeUnit(2U); unit.language = (UmiCompilerLanguage)(UMI_COMPILER_LANGUAGE_UAI+1); Save();
    assert(umi_compiler_manifest_add_unit(&manifest,&unit) == UMI_STATUS_INVALID_ARGUMENT); Same();
    unit.language = UMI_COMPILER_LANGUAGE_UNKNOWN;
    assert(umi_compiler_manifest_add_unit(&manifest,&unit) == UMI_STATUS_INVALID_ARGUMENT); Same();
    char lookup[UMI_COMPILER_ID_CAPACITY]; (void)memset(lookup,'x',sizeof(lookup));
    assert(umi_compiler_manifest_find_unit(&manifest,lookup) == NULL); Same();
}

static void Publication(void)
{
    Seed(); MakeUnit(2U); manifest.revision = UINT64_MAX; Save();
    assert(umi_compiler_manifest_add_unit(&manifest,&unit) == UMI_STATUS_CAPACITY_EXCEEDED); Same();
    assert(umi_compiler_manifest_find_unit(&manifest,"unit-0") == &manifest.units[0]);
    assert(umi_compiler_manifest_add_unit(&manifest,&manifest.units[0]) == UMI_STATUS_ALREADY_EXISTS); Same();
    Seed(); MakeUnit(2U); manifest.units[2] = unit;
    assert(umi_compiler_manifest_add_unit(&manifest,&manifest.units[2]) == UMI_STATUS_OK);
    assert(manifest.unit_count == 3U && strcmp(manifest.units[2].unit_id,"unit-2") == 0);
    assert(umi_compiler_manifest_init(&manifest,"project","Example","target") == UMI_STATUS_OK);
    for (size_t i = 0U; i < UMI_COMPILER_MAX_UNITS; ++i) {
        MakeUnit(i); assert(umi_compiler_manifest_add_unit(&manifest,&unit) == UMI_STATUS_OK);
    }
    Save(); MakeUnit(UMI_COMPILER_MAX_UNITS);
    assert(umi_compiler_manifest_add_unit(&manifest,&unit) == UMI_STATUS_CAPACITY_EXCEEDED); Same();
    assert(umi_compiler_manifest_add_unit(&manifest,&manifest.units[0]) == UMI_STATUS_ALREADY_EXISTS); Same();
}

static void Semantics(void)
{
    Seed();
    char reason[2] = {'x','x'};
    assert(umi_compiler_manifest_validate(&manifest,reason,sizeof(reason)) == UMI_STATUS_OK);
    assert(reason[0] == 'o' && reason[1] == '\0');
    manifest.units[0].entry_point = true;
    manifest.units[1].entry_point = true;
    Save();
    assert(umi_compiler_manifest_storage_valid(&manifest));
    assert(umi_compiler_manifest_validate(&manifest,NULL,0U) == UMI_STATUS_INVALID_STATE); Same();
    /* Optional provider and ABI fields remain empty, and unused rows are ignored. */
    Seed(); (void)memset(&manifest.units[2],0xa5,sizeof(manifest.units[2]));
    assert(umi_compiler_manifest_validate(&manifest,NULL,0U) == UMI_STATUS_OK);
}

int main(void)
{
    assert(!umi_compiler_unit_manifest_valid(NULL));
    assert(!umi_compiler_manifest_storage_valid(NULL));
    Construction(); MalformedStorage(); Publication(); Semantics();
    return 0;
}
