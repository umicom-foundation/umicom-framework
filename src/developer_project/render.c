/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/render.c
 *
 * PURPOSE:
 *   Implement simple non-executable project-template variable substitution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_project/render.h"

#include <string.h>

/* Provide the append text operation used by this module and its client applications. */
static UmiStatus append_text(char *out_text,
                             size_t capacity,
                             size_t *used,
                             const char *text,
                             size_t length)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_text == NULL || used == NULL || text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (*used + length + 1U > capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length > 0U) {
        (void)memcpy(out_text + *used, text, length);
        *used += length;
    }
    out_text[*used] = '\0';
    return UMI_STATUS_OK;
}

/*
 * Provide the developer project render text operation used by this module and its client
 * applications.
 */
UmiStatus umi_developer_project_render_text(
    const char *template_text,
    const UmiDeveloperProjectVariableSet *variables,
    char *out_text,
    size_t capacity)
{
    const char *cursor;
    size_t used = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (template_text == NULL || variables == NULL ||
        out_text == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    out_text[0] = '\0';
    cursor = template_text;

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != '\0') {
        const char *open = strstr(cursor, "{{");

        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (open == NULL) {
            return append_text(
                out_text, capacity, &used, cursor, strlen(cursor));
        }

        {
            UmiStatus status = append_text(
                out_text,
                capacity,
                &used,
                cursor,
                (size_t)(open - cursor));
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) return status;
        }

        {
            const char *close = strstr(open + 2, "}}");
            char name[UMI_DEVELOPER_PROJECT_VARIABLE_NAME_CAPACITY];
            size_t name_length;
            const char *value;

            /*
             * Protect caller-owned memory by checking that required state is available before it is
             * used.
             */
            if (close == NULL) return UMI_STATUS_PARSE_ERROR;

            name_length = (size_t)(close - (open + 2));
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (name_length == 0U || name_length >= sizeof(name)) {
                return UMI_STATUS_PARSE_ERROR;
            }

            (void)memcpy(name, open + 2, name_length);
            name[name_length] = '\0';

            value = umi_developer_project_variable_get(variables, name);
            /*
             * Protect caller-owned memory by checking that required state is available before it is
             * used.
             */
            if (value == NULL) return UMI_STATUS_NOT_FOUND;

            {
                UmiStatus status = append_text(
                    out_text,
                    capacity,
                    &used,
                    value,
                    strlen(value));
                /* Preserve the original failure result so the caller can respond to the correct cause. */
                if (status != UMI_STATUS_OK) return status;
            }

            cursor = close + 2;
        }
    }

    return UMI_STATUS_OK;
}
