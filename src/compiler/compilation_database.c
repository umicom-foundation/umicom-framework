/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/compilation_database.c
 *
 * PURPOSE:
 *   Implement the compilation database behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/compilation_database.h"
#include "umicom/compiler/language.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "catalogue_validation.h"
#include "umicom/protocol/json_writer.h"

/* Protocol now owns UTF-8 validation and JSON escaping. Retain the previous
 * incremental helpers for review of the old output behavior. */
#if 0
/* Add json only after its inputs and available capacity have been checked. */
static UmiStatus json_append(char *output,size_t capacity,size_t *length,const char *text)
{ size_t count = strlen(text); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (*length + count + 1U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED; (void)memcpy(output + *length,text,count + 1U); *length += count; return UMI_STATUS_OK; }
/* Provide the json string operation used by this module and its client applications. */
static UmiStatus json_string(char *output,size_t capacity,size_t *length,const char *text)
{
    const unsigned char *cursor; UmiStatus status = json_append(output,capacity,length,"\"");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Visit each bounded item once so every record receives the same rule. */
    for (cursor = (const unsigned char *)text; *cursor != 0U; ++cursor) {
        char encoded[8U] = {0};
        /* Apply this branch only when its contract condition is satisfied. */
        if (*cursor == '"' || *cursor == '\\') { encoded[0] = '\\'; encoded[1] = (char)*cursor; }
        else /* Apply this branch only when its contract condition is satisfied. */ if (*cursor == '\n') { encoded[0] = '\\'; encoded[1] = 'n'; }
        else /* Apply this branch only when its contract condition is satisfied. */ if (*cursor < 32U) (void)snprintf(encoded,sizeof(encoded),"\\u%04x",(unsigned int)*cursor);
        /* Use this fallback path when the earlier condition does not apply. */
        else encoded[0] = (char)*cursor;
        status = json_append(output,capacity,length,encoded); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (status != UMI_STATUS_OK) return status;
    }
    return json_append(output,capacity,length,"\"");
}
#endif

bool umi_polyglot_compilation_entry_valid(const UmiPolyglotCompilationEntry *entry)
{
    return entry != NULL &&
        UmiCompilerCatalogueText(entry->unit_id,sizeof(entry->unit_id),true) &&
        UmiCompilerCatalogueText(entry->directory,sizeof(entry->directory),false) &&
        UmiCompilerCatalogueText(entry->source,sizeof(entry->source),true) &&
        UmiCompilerCatalogueText(entry->output,sizeof(entry->output),false) &&
        entry->language > UMI_COMPILER_LANGUAGE_UNKNOWN && entry->language <= UMI_COMPILER_LANGUAGE_UAI &&
        umi_compiler_plan_storage_valid(&entry->plan);
}

bool umi_polyglot_compilation_database_valid(const UmiPolyglotCompilationDatabase *database)
{
    if (database == NULL || database->count > UMI_POLYGLOT_DATABASE_MAX) return false;
    for (size_t i = 0U; i < database->count; ++i)
        if (!umi_polyglot_compilation_entry_valid(&database->items[i])) return false;
    return true;
}

/*
 * Add polyglot compilation database only after its inputs and available capacity have been
 * checked.
 */
/* Admit complete entries and owner storage before duplicate/capacity checks, then rebase stored plan views into their receiving owner. Refusal leaves all database bytes unchanged. The previous implementation is retained for review. */
#if 0
UmiStatus umi_polyglot_compilation_database_add(UmiPolyglotCompilationDatabase *database,const UmiPolyglotCompilationEntry *entry)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (database == NULL || entry == NULL || entry->unit_id[0] == '\0' || entry->source[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < database->count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(database->items[index].source,entry->source) == 0) return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (database->count >= UMI_POLYGLOT_DATABASE_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    database->items[database->count++] = *entry; database->revision += 1U; return UMI_STATUS_OK;
}
#endif
UmiStatus umi_polyglot_compilation_database_add(UmiPolyglotCompilationDatabase *database,const UmiPolyglotCompilationEntry *entry)
{
    if (database == NULL || !umi_polyglot_compilation_entry_valid(entry)) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_polyglot_compilation_database_valid(database)) return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < database->count; ++i)
        if (strcmp(database->items[i].source,entry->source) == 0) return UMI_STATUS_ALREADY_EXISTS;
    if (database->count == UMI_POLYGLOT_DATABASE_MAX || database->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiPolyglotCompilationEntry *destination = &database->items[database->count];
    (void)memmove(destination,entry,sizeof(*entry));
    /* The copied plan's argument pointers belonged to the incoming owner.
     * Full storage admission above makes rebasing an infallible publication step. */
    (void)umi_compiler_plan_arguments(&destination->plan);
    database->count += 1U;
    database->revision += 1U;
    return UMI_STATUS_OK;
}
/*
 * Find polyglot compilation database while leaving the underlying catalogue or model owned
 * by this module.
 */
/* Bound lookup input and every active entry before returning a borrowed record; malformed late rows cannot hide behind an early match. The previous implementation is retained for review. */
#if 0
const UmiPolyglotCompilationEntry *umi_polyglot_compilation_database_find(const UmiPolyglotCompilationDatabase *database,const char *source)
{ size_t index; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (database == NULL || source == NULL) return NULL; /* Visit each bounded item once so every record receives the same rule. */ for (index = 0U; index < database->count; ++index) /* Protect caller-owned memory by checking that required state is available before it is used. */ if (strcmp(database->items[index].source,source) == 0) return &database->items[index]; return NULL; }
#endif
const UmiPolyglotCompilationEntry *umi_polyglot_compilation_database_find(const UmiPolyglotCompilationDatabase *database,const char *source)
{
    if (!UmiCompilerCatalogueText(source,UMI_COMPILER_PATH_CAPACITY,true) ||
        !umi_polyglot_compilation_database_valid(database)) return NULL;
    for (size_t i = 0U; i < database->count; ++i)
        if (strcmp(database->items[i].source,source) == 0) return &database->items[i];
    return NULL;
}
/*
 * Provide the polyglot compilation database export json operation used by this module and
 * its client applications.
 */
/* Use the shared UTF-8 JSON writer and private scratch output, publishing only a complete document. Export the executable first as required by the compilation database format; draft entries without a command remain readable but cannot be exported. The previous implementation is retained for review. */
#if 0
UmiStatus umi_polyglot_compilation_database_export_json(const UmiPolyglotCompilationDatabase *database,char *out_json,size_t capacity)
{
    size_t item,arg,length = 0U; UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (database == NULL || out_json == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    out_json[0] = '\0'; status = json_append(out_json,capacity,&length,"[\n"); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (status != UMI_STATUS_OK) return status;
    /* Visit each bounded item once so every record receives the same rule. */
    for (item = 0U; item < database->count; ++item) {
        const UmiPolyglotCompilationEntry *entry = &database->items[item];
        status = json_append(out_json,capacity,&length,"  {\"directory\":"); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (status == UMI_STATUS_OK) status = json_string(out_json,capacity,&length,entry->directory);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (status == UMI_STATUS_OK) status = json_append(out_json,capacity,&length,",\"file\":");
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (status == UMI_STATUS_OK) status = json_string(out_json,capacity,&length,entry->source);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (status == UMI_STATUS_OK) status = json_append(out_json,capacity,&length,",\"output\":");
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (status == UMI_STATUS_OK) status = json_string(out_json,capacity,&length,entry->output);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (status == UMI_STATUS_OK) status = json_append(out_json,capacity,&length,",\"language\":");
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (status == UMI_STATUS_OK) status = json_string(out_json,capacity,&length,umi_compiler_language_name(entry->language));
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (status == UMI_STATUS_OK) status = json_append(out_json,capacity,&length,",\"arguments\":[");
        /* Visit each bounded item once so every record receives the same rule. */
        for (arg = 0U; status == UMI_STATUS_OK && arg < entry->plan.argument_count; ++arg) { /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (arg > 0U) status = json_append(out_json,capacity,&length,","); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (status == UMI_STATUS_OK) status = json_string(out_json,capacity,&length,entry->plan.arguments[arg]); }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (status == UMI_STATUS_OK) status = json_append(out_json,capacity,&length,item + 1U < database->count ? "]},\n" : "]}\n");
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    return json_append(out_json,capacity,&length,"]\n");
}
#endif
UmiStatus umi_polyglot_compilation_database_export_json(const UmiPolyglotCompilationDatabase *database,char *out_json,size_t capacity)
{
    if (database == NULL || out_json == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_polyglot_compilation_database_valid(database)) return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < database->count; ++i) {
        const UmiPolyglotCompilationEntry *entry = &database->items[i];
        if (entry->directory[0] == '\0' || entry->plan.executable[0] == '\0' ||
            entry->plan.argument_count == 0U) return UMI_STATUS_INVALID_STATE;
    }
    /* Count is bounded above. Size the scratch allocation to the smaller of
     * caller capacity and the worst escaped document for these active entries. */
    const size_t per_entry = 256U + 6U * ((size_t)4U * UMI_COMPILER_PATH_CAPACITY +
                                        (size_t)UMI_COMPILER_MAX_ARGUMENTS * UMI_COMPILER_TEXT_CAPACITY);
    const size_t bound = 16U + database->count * per_entry;
    char *candidate = malloc(capacity < bound ? capacity : bound);
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiJsonWriter writer;
    UmiJsonWriterInit(&writer,candidate,capacity < bound ? capacity : bound);
    (void)UmiJsonWriterRaw(&writer,"[\n");
    for (size_t i = 0U; i < database->count; ++i) {
        const UmiPolyglotCompilationEntry *entry = &database->items[i];
        (void)UmiJsonWriterRaw(&writer,"  {\"directory\":");
        (void)UmiJsonWriterString(&writer,entry->directory);
        (void)UmiJsonWriterRaw(&writer,",\"file\":");
        (void)UmiJsonWriterString(&writer,entry->source);
        (void)UmiJsonWriterRaw(&writer,",\"output\":");
        (void)UmiJsonWriterString(&writer,entry->output);
        (void)UmiJsonWriterRaw(&writer,",\"language\":");
        (void)UmiJsonWriterString(&writer,umi_compiler_language_name(entry->language));
        (void)UmiJsonWriterRaw(&writer,",\"arguments\":[");
        /* Compilation databases contain the executable as argv[0]. Framework
         * plans keep it separate, so prepend it without shell escaping. */
        (void)UmiJsonWriterString(&writer,entry->plan.executable);
        for (size_t argument = 0U; argument < entry->plan.argument_count; ++argument) {
            (void)UmiJsonWriterRaw(&writer,",");
            (void)UmiJsonWriterString(&writer,entry->plan.arguments[argument]);
        }
        (void)UmiJsonWriterRaw(&writer,i + 1U < database->count ? "]},\n" : "]}\n");
    }
    (void)UmiJsonWriterRaw(&writer,"]\n");
    UmiStatus status = writer.status;
    if (status == UMI_STATUS_OK) (void)memcpy(out_json,candidate,writer.length + 1U);
    free(candidate);
    return status;
}

/* Explicit replacement is separate from add-only admission. Callers such as the
 * compiler centre can refresh a command without silently retaining stale options. */
UmiStatus umi_polyglot_compilation_database_upsert(UmiPolyglotCompilationDatabase *database,const UmiPolyglotCompilationEntry *entry)
{
    UmiStatus status = umi_polyglot_compilation_database_add(database,entry);
    if (status != UMI_STATUS_ALREADY_EXISTS) return status;
    /* Add has admitted the complete owner and incoming record without mutation.
     * Existing-key replacement needs no free slot, but still consumes revision. */
    if (database->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t index = 0U; index < database->count; ++index) {
        if (strcmp(database->items[index].source,entry->source) != 0) continue;
        (void)memmove(&database->items[index],entry,sizeof(*entry));
        (void)umi_compiler_plan_arguments(&database->items[index].plan);
        database->revision += 1U;
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_INVALID_STATE;
}
