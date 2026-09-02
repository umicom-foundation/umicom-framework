/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/response_file.c
 *
 * PURPOSE:
 *   Implement the response file behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Compiler response files | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/compiler/response_file.h"
#include <stdio.h>
#include <string.h>

/* Provide the append text operation used by this module and its client applications. */
static UmiStatus append_text(char *output,size_t capacity,size_t *length,const char *text)
{
    size_t count;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (output == NULL || length == NULL || text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    count = strlen(text);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (*length + count + 1U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(output + *length,text,count + 1U); *length += count; return UMI_STATUS_OK;
}

/* Provide the append quoted operation used by this module and its client applications. */
static UmiStatus append_quoted(char *output,size_t capacity,size_t *length,const char *argument)
{
    const char *cursor; UmiStatus status; bool quote;
    quote = argument[0] == '\0' || strpbrk(argument," \t\r\n\"") != NULL;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (!quote) return append_text(output,capacity,length,argument);
    status = append_text(output,capacity,length,"\""); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (status != UMI_STATUS_OK) return status;
    /* Visit each bounded item once so every record receives the same rule. */
    for (cursor = argument; *cursor != '\0'; ++cursor) {
        char encoded[3U] = {0};
        /* Apply this branch only when its contract condition is satisfied. */
        if (*cursor == '\\' || *cursor == '"') { encoded[0] = '\\'; encoded[1] = *cursor; }
        /* Use this fallback path when the earlier condition does not apply. */
        else encoded[0] = *cursor;
        status = append_text(output,capacity,length,encoded); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (status != UMI_STATUS_OK) return status;
    }
    return append_text(output,capacity,length,"\"");
}

/*
 * Write compiler response file in its stable representation and report capacity or input
 * failures to the caller.
 */
UmiStatus umi_compiler_response_file_encode(const UmiCompilerPlan *plan,char *out_text,size_t capacity)
{
    size_t index,length = 0U; UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || out_text == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    out_text[0] = '\0';
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < plan->argument_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (index > 0U) { status = append_text(out_text,capacity,&length,"\n"); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (status != UMI_STATUS_OK) return status; }
        status = append_quoted(out_text,capacity,&length,plan->arguments[index]); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the compiler response file argument operation used by this module and its client
 * applications.
 */
UmiStatus umi_compiler_response_file_argument(const char *path,char *out_argument,size_t capacity)
{
    int written;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (path == NULL || path[0] == '\0' || out_argument == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    written = snprintf(out_argument,capacity,"@%s",path);
    return written < 0 || (size_t)written >= capacity ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}
