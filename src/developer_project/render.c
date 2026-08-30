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

static UmiStatus append_text(char *out_text,
                             size_t capacity,
                             size_t *used,
                             const char *text,
                             size_t length)
{
    if (out_text == NULL || used == NULL || text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (*used + length + 1U > capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    if (length > 0U) {
        (void)memcpy(out_text + *used, text, length);
        *used += length;
    }
    out_text[*used] = '\0';
    return UMI_STATUS_OK;
}

UmiStatus umi_developer_project_render_text(
    const char *template_text,
    const UmiDeveloperProjectVariableSet *variables,
    char *out_text,
    size_t capacity)
{
    const char *cursor;
    size_t used = 0U;

    if (template_text == NULL || variables == NULL ||
        out_text == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    out_text[0] = '\0';
    cursor = template_text;

    while (*cursor != '\0') {
        const char *open = strstr(cursor, "{{");

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
            if (status != UMI_STATUS_OK) return status;
        }

        {
            const char *close = strstr(open + 2, "}}");
            char name[UMI_DEVELOPER_PROJECT_VARIABLE_NAME_CAPACITY];
            size_t name_length;
            const char *value;

            if (close == NULL) return UMI_STATUS_PARSE_ERROR;

            name_length = (size_t)(close - (open + 2));
            if (name_length == 0U || name_length >= sizeof(name)) {
                return UMI_STATUS_PARSE_ERROR;
            }

            (void)memcpy(name, open + 2, name_length);
            name[name_length] = '\0';

            value = umi_developer_project_variable_get(variables, name);
            if (value == NULL) return UMI_STATUS_NOT_FOUND;

            {
                UmiStatus status = append_text(
                    out_text,
                    capacity,
                    &used,
                    value,
                    strlen(value));
                if (status != UMI_STATUS_OK) return status;
            }

            cursor = close + 2;
        }
    }

    return UMI_STATUS_OK;
}
