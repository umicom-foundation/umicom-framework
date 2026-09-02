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
/* Umicom Framework | Polyglot compilation database | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/compiler/compilation_database.h"
#include "umicom/compiler/language.h"
#include <stdio.h>
#include <string.h>

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
/*
 * Add polyglot compilation database only after its inputs and available capacity have been
 * checked.
 */
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
/*
 * Find polyglot compilation database while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiPolyglotCompilationEntry *umi_polyglot_compilation_database_find(const UmiPolyglotCompilationDatabase *database,const char *source)
{ size_t index; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (database == NULL || source == NULL) return NULL; /* Visit each bounded item once so every record receives the same rule. */ for (index = 0U; index < database->count; ++index) /* Protect caller-owned memory by checking that required state is available before it is used. */ if (strcmp(database->items[index].source,source) == 0) return &database->items[index]; return NULL; }
/*
 * Provide the polyglot compilation database export json operation used by this module and
 * its client applications.
 */
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
