/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/context.c
 *
 * PURPOSE:
 *   Implement the developer context and its versioned persistence codec.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer/context.h"

#include <stdlib.h>
#include <string.h>

struct UmiDeveloperContext {
    UmiDeveloperContextSnapshot state;
};

static void normalise(UmiDeveloperContextSnapshot *state)
{
    if (state == NULL) {
        return;
    }

    state->struct_size = (uint32_t)sizeof(*state);
    state->api_version = UMI_DEVELOPER_CONTEXT_API_VERSION;
    state->project_id[UMI_DEVELOPER_ID_CAPACITY - 1U] = '\0';
    state->configuration_id[UMI_DEVELOPER_ID_CAPACITY - 1U] = '\0';
    state->target_id[UMI_DEVELOPER_ID_CAPACITY - 1U] = '\0';
    state->launch_profile_id[UMI_DEVELOPER_ID_CAPACITY - 1U] = '\0';
    state->repository_id[UMI_DEVELOPER_ID_CAPACITY - 1U] = '\0';
    state->test_item_id[UMI_DEVELOPER_ID_CAPACITY - 1U] = '\0';
    state->active_document[UMI_DEVELOPER_PATH_CAPACITY - 1U] = '\0';
    state->workspace_directory[UMI_DEVELOPER_PATH_CAPACITY - 1U] = '\0';
}

UmiStatus umi_developer_context_create(UmiDeveloperContext **out_context)
{
    UmiDeveloperContext *context;

    if (out_context == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_context = NULL;
    context = (UmiDeveloperContext *)calloc(1U, sizeof(*context));
    if (context == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }

    context->state.revision = 1U;
    normalise(&context->state);
    *out_context = context;
    return UMI_STATUS_OK;
}

void umi_developer_context_destroy(UmiDeveloperContext *context)
{
    free(context);
}

UmiStatus umi_developer_context_set(
    UmiDeveloperContext *context,
    const UmiDeveloperContextSnapshot *snapshot)
{
    uint64_t next_revision;

    if (context == NULL || snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    next_revision = context->state.revision + 1U;
    context->state = *snapshot;
    normalise(&context->state);
    context->state.revision = next_revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_developer_context_snapshot(
    const UmiDeveloperContext *context,
    UmiDeveloperContextSnapshot *out_snapshot)
{
    if (context == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_snapshot = context->state;
    return UMI_STATUS_OK;
}

static int append_char(char *out_text, size_t capacity, size_t *position, char value)
{
    if (out_text == NULL || position == NULL || *position + 1U >= capacity) {
        return 0;
    }

    out_text[*position] = value;
    *position += 1U;
    out_text[*position] = '\0';
    return 1;
}

static int append_escape(
    char *out_text,
    size_t capacity,
    size_t *position,
    const char *text)
{
    const char hex[] = "0123456789ABCDEF";
    const unsigned char *cursor = (const unsigned char *)text;

    if (text == NULL) {
        text = "";
        cursor = (const unsigned char *)text;
    }

    while (*cursor != 0U) {
        unsigned char value = *cursor;
        if (value == (unsigned char)'%' ||
            value == (unsigned char)'|' ||
            value == (unsigned char)'\n' ||
            value == (unsigned char)'\r') {
            if (!append_char(out_text, capacity, position, '%') ||
                !append_char(out_text, capacity, position, hex[(value >> 4U) & 0x0FU]) ||
                !append_char(out_text, capacity, position, hex[value & 0x0FU])) {
                return 0;
            }
        } else if (!append_char(out_text, capacity, position, (char)value)) {
            return 0;
        }
        cursor += 1U;
    }

    return 1;
}

UmiStatus umi_developer_context_encode(
    const UmiDeveloperContext *context,
    char *out_text,
    size_t capacity,
    size_t *out_length)
{
    const char *fields[8];
    size_t position = 0U;
    size_t index;

    if (context == NULL || out_text == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    out_text[0] = '\0';
    fields[0] = context->state.project_id;
    fields[1] = context->state.configuration_id;
    fields[2] = context->state.target_id;
    fields[3] = context->state.launch_profile_id;
    fields[4] = context->state.repository_id;
    fields[5] = context->state.test_item_id;
    fields[6] = context->state.active_document;
    fields[7] = context->state.workspace_directory;

    if (!append_escape(out_text, capacity, &position, UMI_DEVELOPER_CONTEXT_CODEC_PREFIX)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    for (index = 0U; index < 8U; ++index) {
        if (!append_char(out_text, capacity, &position, '|') ||
            !append_escape(out_text, capacity, &position, fields[index])) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
    }

    if (out_length != NULL) {
        *out_length = position;
    }
    return UMI_STATUS_OK;
}

static int hex_value(char value)
{
    if (value >= '0' && value <= '9') {
        return value - '0';
    }
    if (value >= 'A' && value <= 'F') {
        return value - 'A' + 10;
    }
    if (value >= 'a' && value <= 'f') {
        return value - 'a' + 10;
    }
    return -1;
}

static UmiStatus decode_field(
    const char **cursor,
    char *destination,
    size_t capacity,
    int expect_separator)
{
    size_t length = 0U;

    if (cursor == NULL || *cursor == NULL ||
        destination == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    while (**cursor != '\0' && **cursor != '|') {
        char value = **cursor;

        if (value == '%') {
            int high;
            int low;

            if ((*cursor)[1] == '\0' || (*cursor)[2] == '\0') {
                return UMI_STATUS_PARSE_ERROR;
            }
            high = hex_value((*cursor)[1]);
            low = hex_value((*cursor)[2]);
            if (high < 0 || low < 0) {
                return UMI_STATUS_PARSE_ERROR;
            }
            value = (char)((high << 4) | low);
            *cursor += 3;
        } else {
            *cursor += 1;
        }

        if (length + 1U >= capacity) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        destination[length++] = value;
    }

    destination[length] = '\0';

    if (expect_separator) {
        if (**cursor != '|') {
            return UMI_STATUS_PARSE_ERROR;
        }
        *cursor += 1;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_developer_context_decode(
    const char *text,
    UmiDeveloperContextSnapshot *out_snapshot)
{
    const char *cursor;
    char prefix[8];
    UmiStatus status;

    if (text == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    memset(out_snapshot, 0, sizeof(*out_snapshot));
    cursor = text;

    status = decode_field(&cursor, prefix, sizeof(prefix), 1);
    if (status != UMI_STATUS_OK ||
        strcmp(prefix, UMI_DEVELOPER_CONTEXT_CODEC_PREFIX) != 0) {
        return UMI_STATUS_PARSE_ERROR;
    }

#define UMI_DECODE_CONTEXT_FIELD(field_name, separator_required)                  \
    do {                                                                          \
        status = decode_field(&cursor, out_snapshot->field_name,                  \
                              sizeof(out_snapshot->field_name),                    \
                              (separator_required));                               \
        if (status != UMI_STATUS_OK) {                                             \
            return status;                                                         \
        }                                                                          \
    } while (0)

    UMI_DECODE_CONTEXT_FIELD(project_id, 1);
    UMI_DECODE_CONTEXT_FIELD(configuration_id, 1);
    UMI_DECODE_CONTEXT_FIELD(target_id, 1);
    UMI_DECODE_CONTEXT_FIELD(launch_profile_id, 1);
    UMI_DECODE_CONTEXT_FIELD(repository_id, 1);
    UMI_DECODE_CONTEXT_FIELD(test_item_id, 1);
    UMI_DECODE_CONTEXT_FIELD(active_document, 1);
    UMI_DECODE_CONTEXT_FIELD(workspace_directory, 0);

#undef UMI_DECODE_CONTEXT_FIELD

    if (*cursor != '\0') {
        return UMI_STATUS_PARSE_ERROR;
    }

    out_snapshot->revision = 1U;
    normalise(out_snapshot);
    return UMI_STATUS_OK;
}


static UmiStatus patch_copy_text(char *destination,
                                 size_t capacity,
                                 const char *source)
{
    size_t length;

    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    if (length >= capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (length > 0U) {
        memcpy(destination, source, length);
    }
    destination[length] = '\0';
    return UMI_STATUS_OK;
}

UmiStatus umi_developer_context_patch(
    UmiDeveloperContext *context,
    const UmiDeveloperContextPatch *patch,
    UmiDeveloperContextSnapshot *out_snapshot)
{
    UmiDeveloperContextSnapshot next;
    UmiStatus status;

    if (context == NULL || patch == NULL ||
        patch->struct_size < sizeof(*patch) ||
        patch->api_version != UMI_DEVELOPER_CONTEXT_PATCH_API_VERSION ||
#if defined(UMI_DEVELOPER_CONTEXT_USE_LEGACY_SIGNED_MASK)
        (patch->field_mask & ~UMI_DEVELOPER_CONTEXT_PATCH_ALL) != 0U) {
#else
        (patch->field_mask & ~(uint32_t)UMI_DEVELOPER_CONTEXT_PATCH_ALL) != 0U) {
#endif
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_context_snapshot(context, &next);
    if (status != UMI_STATUS_OK) {
        return status;
    }

#define UMI_PATCH_CONTEXT_TEXT(mask_value, member_name)                           \
    do {                                                                           \
        if ((patch->field_mask & (mask_value)) != 0U) {                            \
            status = patch_copy_text(next.member_name,                             \
                                     sizeof(next.member_name),                     \
                                     patch->member_name);                          \
            if (status != UMI_STATUS_OK) {                                         \
                return status;                                                     \
            }                                                                      \
        }                                                                          \
    } while (0)

    UMI_PATCH_CONTEXT_TEXT(UMI_DEVELOPER_CONTEXT_PATCH_PROJECT, project_id);
    UMI_PATCH_CONTEXT_TEXT(UMI_DEVELOPER_CONTEXT_PATCH_CONFIGURATION,
                           configuration_id);
    UMI_PATCH_CONTEXT_TEXT(UMI_DEVELOPER_CONTEXT_PATCH_TARGET, target_id);
    UMI_PATCH_CONTEXT_TEXT(UMI_DEVELOPER_CONTEXT_PATCH_LAUNCH_PROFILE,
                           launch_profile_id);
    UMI_PATCH_CONTEXT_TEXT(UMI_DEVELOPER_CONTEXT_PATCH_REPOSITORY, repository_id);
    UMI_PATCH_CONTEXT_TEXT(UMI_DEVELOPER_CONTEXT_PATCH_TEST_ITEM, test_item_id);
    UMI_PATCH_CONTEXT_TEXT(UMI_DEVELOPER_CONTEXT_PATCH_ACTIVE_DOCUMENT,
                           active_document);
    UMI_PATCH_CONTEXT_TEXT(UMI_DEVELOPER_CONTEXT_PATCH_WORKSPACE_DIRECTORY,
                           workspace_directory);

#undef UMI_PATCH_CONTEXT_TEXT

    status = umi_developer_context_set(context, &next);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    if (out_snapshot != NULL) {
        return umi_developer_context_snapshot(context, out_snapshot);
    }
    return UMI_STATUS_OK;
}
