/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_compiler_database_admission.c
 *
 * PURPOSE:
 *   Verify owned compilation entries, bounded traversal and atomic executable-first JSON export.
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
#include <stdlib.h>
#include <string.h>
#include "umicom/compiler/compilation_database.h"

static UmiPolyglotCompilationDatabase *database, *saved;
static UmiPolyglotCompilationEntry entry;
static char output[8192], previous_output[8192];
typedef struct Field { size_t offset,capacity; } Field;
static const Field fields[] = {
    {offsetof(UmiPolyglotCompilationEntry,unit_id),sizeof(entry.unit_id)},
    {offsetof(UmiPolyglotCompilationEntry,directory),sizeof(entry.directory)},
    {offsetof(UmiPolyglotCompilationEntry,source),sizeof(entry.source)},
    {offsetof(UmiPolyglotCompilationEntry,output),sizeof(entry.output)}
};

static void MakeEntry(size_t id)
{
    (void)memset(&entry,0,sizeof(entry));
    (void)snprintf(entry.unit_id,sizeof(entry.unit_id),"unit-%zu",id);
    (void)snprintf(entry.source,sizeof(entry.source),"source-%zu.c",id);
    (void)strcpy(entry.directory,".");
    (void)strcpy(entry.output,"object.o");
    entry.language = UMI_COMPILER_LANGUAGE_C;
    umi_compiler_plan_init(&entry.plan);
    (void)strcpy(entry.plan.executable,"driver");
    assert(umi_compiler_plan_add_argument(&entry.plan,"-c") == UMI_STATUS_OK);
    assert(umi_compiler_plan_add_argument(&entry.plan,entry.source) == UMI_STATUS_OK);
}

static void Seed(void)
{
    (void)memset(database,0,sizeof(*database));
    database->revision = 1U;
    MakeEntry(0U); assert(umi_polyglot_compilation_database_add(database,&entry) == UMI_STATUS_OK);
    MakeEntry(1U); assert(umi_polyglot_compilation_database_add(database,&entry) == UMI_STATUS_OK);
}
static void Save(void)
{
    (void)memcpy(saved,database,sizeof(*database));
    (void)memset(output,0x5a,sizeof(output));
    (void)memcpy(previous_output,output,sizeof(output));
}
static void Same(void)
{
    assert(memcmp(saved,database,sizeof(*database)) == 0);
    assert(memcmp(previous_output,output,sizeof(output)) == 0);
}

static void OwnedPlans(void)
{
    Seed();
    for (size_t row = 0U; row < database->count; ++row)
        for (size_t i = 0U; i < database->items[row].plan.argument_count; ++i)
            assert(database->items[row].plan.argument_view[i] == database->items[row].plan.arguments[i]);
    (void)strcpy(entry.plan.arguments[0],"caller changed");
    assert(strcmp(database->items[1].plan.argument_view[0],"-c") == 0);
    MakeEntry(2U); database->items[2] = entry;
    assert(umi_polyglot_compilation_database_add(database,&database->items[2]) == UMI_STATUS_OK);
    assert(database->items[2].plan.argument_view[0] == database->items[2].plan.arguments[0]);
}

static void Refusals(void)
{
    const size_t counts[] = {UMI_POLYGLOT_DATABASE_MAX+1U,SIZE_MAX};
    for (size_t i = 0U; i < sizeof(counts)/sizeof(counts[0]); ++i) {
        Seed(); database->count = counts[i]; MakeEntry(2U); Save();
        assert(!umi_polyglot_compilation_database_valid(database));
        assert(umi_polyglot_compilation_database_find(database,"source-0.c") == NULL);
        assert(umi_polyglot_compilation_database_add(database,&entry) == UMI_STATUS_INVALID_STATE);
        assert(umi_polyglot_compilation_database_export_json(database,output,sizeof(output)) == UMI_STATUS_INVALID_STATE); Same();
    }
    for (size_t i = 0U; i < sizeof(fields)/sizeof(fields[0]); ++i) {
        Seed(); (void)memset((char *)&database->items[1]+fields[i].offset,'x',fields[i].capacity);
        MakeEntry(2U); Save();
        assert(umi_polyglot_compilation_database_find(database,"source-0.c") == NULL);
        assert(umi_polyglot_compilation_database_add(database,&entry) == UMI_STATUS_INVALID_STATE);
        assert(umi_polyglot_compilation_database_export_json(database,output,sizeof(output)) == UMI_STATUS_INVALID_STATE); Same();
        Seed(); MakeEntry(2U); (void)memset((char *)&entry+fields[i].offset,'x',fields[i].capacity); Save();
        assert(umi_polyglot_compilation_database_add(database,&entry) == UMI_STATUS_INVALID_ARGUMENT); Same();
    }
    Seed(); database->items[1].plan.argument_count = SIZE_MAX; Save();
    assert(umi_polyglot_compilation_database_find(database,"source-0.c") == NULL);
    assert(umi_polyglot_compilation_database_export_json(database,output,sizeof(output)) == UMI_STATUS_INVALID_STATE); Same();
    Seed(); (void)memset(database->items[1].plan.arguments[1],'x',sizeof(entry.plan.arguments[0])); Save();
    assert(umi_polyglot_compilation_database_export_json(database,output,sizeof(output)) == UMI_STATUS_INVALID_STATE); Same();
    Seed(); MakeEntry(2U); entry.language = UMI_COMPILER_LANGUAGE_UNKNOWN; Save();
    assert(umi_polyglot_compilation_database_add(database,&entry) == UMI_STATUS_INVALID_ARGUMENT); Same();
    Seed(); MakeEntry(2U); database->revision = UINT64_MAX; Save();
    assert(umi_polyglot_compilation_database_add(database,&entry) == UMI_STATUS_CAPACITY_EXCEEDED); Same();
    assert(umi_polyglot_compilation_database_add(database,&database->items[0]) == UMI_STATUS_ALREADY_EXISTS); Same();
    assert(umi_polyglot_compilation_database_find(database,"source-0.c") == &database->items[0]);
}

static void JsonPublication(void)
{
    (void)memset(database,0,sizeof(*database)); Save();
    assert(umi_polyglot_compilation_database_export_json(database,output,sizeof(output)) == UMI_STATUS_OK);
    assert(strcmp(output,"[\n]\n") == 0);
    MakeEntry(0U);
    assert(umi_polyglot_compilation_database_add(database,&entry) == UMI_STATUS_OK);
    assert(umi_polyglot_compilation_database_export_json(database,output,sizeof(output)) == UMI_STATUS_OK);
    const char *expected = "[\n  {\"directory\":\".\",\"file\":\"source-0.c\",\"output\":\"object.o\","
        "\"language\":\"C\",\"arguments\":[\"driver\",\"-c\",\"source-0.c\"]}\n]\n";
    assert(strcmp(output,expected) == 0);
    size_t length = strlen(output); Save();
    assert(umi_polyglot_compilation_database_export_json(database,output,length) == UMI_STATUS_CAPACITY_EXCEEDED); Same();
    assert(umi_polyglot_compilation_database_export_json(database,output,length+1U) == UMI_STATUS_OK);
    assert(strcmp(output,expected) == 0 && (unsigned char)output[length+1U] == 0x5aU);
    Seed(); database->items[1].plan.arguments[1][0] = (char)0xff;
    database->items[1].plan.arguments[1][1] = '\0'; Save();
    assert(umi_polyglot_compilation_database_export_json(database,output,sizeof(output)) == UMI_STATUS_INVALID_ARGUMENT); Same();
    Seed(); database->items[1].directory[0] = '\0'; Save();
    assert(umi_polyglot_compilation_database_valid(database));
    assert(umi_polyglot_compilation_database_export_json(database,output,sizeof(output)) == UMI_STATUS_INVALID_STATE); Same();
    Seed(); database->items[1].plan.executable[0] = '\0'; Save();
    assert(umi_polyglot_compilation_database_export_json(database,output,sizeof(output)) == UMI_STATUS_INVALID_STATE); Same();
    Seed(); database->items[1].plan.argument_count = 0U; Save();
    assert(umi_polyglot_compilation_database_export_json(database,output,sizeof(output)) == UMI_STATUS_INVALID_STATE); Same();
    Seed(); (void)strcpy(database->items[1].plan.arguments[1],"NAME=\"caf\xc3\xa9\"\n");
    (void)strcpy(database->items[1].plan.executable,"C:\\compiler\\driver");
    assert(umi_polyglot_compilation_database_export_json(database,output,sizeof(output)) == UMI_STATUS_OK);
    assert(strstr(output,"NAME=\\\"caf\xc3\xa9\\\"\\n") != NULL);
    assert(strstr(output,"\"C:\\\\compiler\\\\driver\"") != NULL);
}

static void FullDatabase(void)
{
    (void)memset(database,0,sizeof(*database));
    for (size_t i = 0U; i < UMI_POLYGLOT_DATABASE_MAX; ++i) {
        MakeEntry(i); assert(umi_polyglot_compilation_database_add(database,&entry) == UMI_STATUS_OK);
    }
    Save(); MakeEntry(UMI_POLYGLOT_DATABASE_MAX);
    assert(umi_polyglot_compilation_database_add(database,&entry) == UMI_STATUS_CAPACITY_EXCEEDED); Same();
    assert(umi_polyglot_compilation_database_add(database,&database->items[0]) == UMI_STATUS_ALREADY_EXISTS); Same();
}

int main(void)
{
    database = malloc(sizeof(*database)); saved = malloc(sizeof(*saved));
    assert(database != NULL && saved != NULL);
    assert(!umi_polyglot_compilation_entry_valid(NULL));
    assert(!umi_polyglot_compilation_database_valid(NULL));
    OwnedPlans(); Refusals(); JsonPublication(); FullDatabase();
    free(saved); free(database);
    return 0;
}
