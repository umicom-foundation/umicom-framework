/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/workspace_state.c
 *
 * PURPOSE:
 *   Implement versioned developer workspace-state persistence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer/workspace_state.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/platform/atomic_file.h"
#include "umicom/platform/filesystem.h"

/* Provide the append escape operation used by this module and its client applications. */
static UmiStatus append_escape(
    char *output, size_t capacity, size_t *position, const char *text)
{
    static const char hex[] = "0123456789ABCDEF";
    const unsigned char *cursor = (const unsigned char *)(text != NULL ? text : "");
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != 0U) {
        unsigned char value = *cursor++;
        /* Apply this branch only when its contract condition is satisfied. */
        if (value == (unsigned char)'%' || value == (unsigned char)'|') {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (*position + 3U >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
            output[(*position)++] = '%';
            output[(*position)++] = hex[(value >> 4U) & 0x0FU];
            output[(*position)++] = hex[value & 0x0FU];
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (*position + 1U >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
            output[(*position)++] = (char)value;
        }
    }
    output[*position] = '\0';
    return UMI_STATUS_OK;
}

/* Provide the hex value operation used by this module and its client applications. */
static int hex_value(char value)
{
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (value >= '0' && value <= '9') return value - '0';
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (value >= 'A' && value <= 'F') return value - 'A' + 10;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (value >= 'a' && value <= 'f') return value - 'a' + 10;
    return -1;
}

/* Provide the decode field operation used by this module and its client applications. */
static UmiStatus decode_field(
    const char *begin, size_t length, char *output, size_t capacity)
{
    size_t input = 0U, position = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (output == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (input < length) {
        unsigned char value = (unsigned char)begin[input++];
        /* Apply this branch only when its contract condition is satisfied. */
        if (value == (unsigned char)'%') {
            int high, low;
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (input + 1U >= length) return UMI_STATUS_PARSE_ERROR;
            high = hex_value(begin[input++]);
            low = hex_value(begin[input++]);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (high < 0 || low < 0) return UMI_STATUS_PARSE_ERROR;
            value = (unsigned char)((high << 4) | low);
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (position + 1U >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
        output[position++] = (char)value;
    }
    output[position] = '\0';
    return UMI_STATUS_OK;
}

/* Provide the next field operation used by this module and its client applications. */
static UmiStatus next_field(
    const char **cursor, const char **out_begin, size_t *out_length)
{
    const char *separator;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (cursor == NULL || *cursor == NULL || out_begin == NULL || out_length == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_begin = *cursor;
    separator = strchr(*cursor, '|');
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (separator == NULL) {
        *out_length = strlen(*cursor);
        *cursor += *out_length;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        *out_length = (size_t)(separator - *cursor);
        *cursor = separator + 1;
    }
    return UMI_STATUS_OK;
}

/*
 * Initialise developer workspace state from caller-provided values so later operations
 * receive a known state.
 */
void umi_developer_workspace_state_init(
    UmiDeveloperWorkspaceStateSnapshot *state)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (state == NULL) return;
    memset(state, 0, sizeof(*state));
    state->struct_size = (uint32_t)sizeof(*state);
    state->api_version = UMI_DEVELOPER_WORKSPACE_STATE_API_VERSION;
    state->context.struct_size = (uint32_t)sizeof(state->context);
    state->context.api_version = UMI_DEVELOPER_CONTEXT_API_VERSION;
    state->context.revision = 1U;
    state->revision = 1U;
}

/*
 * Write developer workspace state in its stable representation and report capacity or
 * input failures to the caller.
 */
UmiStatus umi_developer_workspace_state_encode(
    const UmiDeveloperWorkspaceStateSnapshot *state,
    char *out_text,
    size_t capacity,
    size_t *out_length)
{
    const char *fields[10];
    char numbers[4][32];
    size_t position = 0U, index;
    int written;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (state == NULL || out_text == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    written = snprintf(numbers[0], sizeof(numbers[0]), "%d", state->configure_before_build != 0);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (written < 0) return UMI_STATUS_INTERNAL_ERROR;
    written = snprintf(numbers[1], sizeof(numbers[1]), "%d", state->build_before_run != 0);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (written < 0) return UMI_STATUS_INTERNAL_ERROR;
    written = snprintf(numbers[2], sizeof(numbers[2]), "%d", state->test_after_build != 0);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (written < 0) return UMI_STATUS_INTERNAL_ERROR;
    written = snprintf(numbers[3], sizeof(numbers[3]), "%llu",
                       (unsigned long long)state->revision);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (written < 0) return UMI_STATUS_INTERNAL_ERROR;

    fields[0] = state->context.project_id;
    fields[1] = state->context.configuration_id;
    fields[2] = state->context.target_id;
    fields[3] = state->context.launch_profile_id;
    fields[4] = state->context.repository_id;
    fields[5] = state->context.test_item_id;
    fields[6] = state->context.active_document;
    fields[7] = state->context.workspace_directory;
    fields[8] = state->last_task_id;
    fields[9] = state->last_launch_profile_id;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strlen(UMI_DEVELOPER_WORKSPACE_STATE_CODEC_PREFIX) + 1U >= capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    memcpy(out_text, UMI_DEVELOPER_WORKSPACE_STATE_CODEC_PREFIX,
           strlen(UMI_DEVELOPER_WORKSPACE_STATE_CODEC_PREFIX));
    position = strlen(UMI_DEVELOPER_WORKSPACE_STATE_CODEC_PREFIX);
    out_text[position++] = '|';
    out_text[position] = '\0';

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < 10U; ++index) {
        status = append_escape(out_text, capacity, &position, fields[index]);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (position + 1U >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
        out_text[position++] = '|';
        out_text[position] = '\0';
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < 4U; ++index) {
        status = append_escape(out_text, capacity, &position, numbers[index]);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (index + 1U < 4U) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (position + 1U >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
            out_text[position++] = '|';
            out_text[position] = '\0';
        }
    }

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_length != NULL) *out_length = position;
    return UMI_STATUS_OK;
}

/*
 * Read developer workspace state into validated module state and return a status when
 * input cannot be used.
 */
UmiStatus umi_developer_workspace_state_decode(
    const char *text,
    UmiDeveloperWorkspaceStateSnapshot *out_state)
{
    UmiDeveloperWorkspaceStateSnapshot state;
    const char *cursor, *begin;
    size_t length, index;
    char *destinations[10];
    size_t capacities[10];
    char number[32];
    char *end = NULL;
    unsigned long long revision;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || out_state == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strncmp(text, UMI_DEVELOPER_WORKSPACE_STATE_CODEC_PREFIX "|",
                strlen(UMI_DEVELOPER_WORKSPACE_STATE_CODEC_PREFIX) + 1U) != 0) {
        return UMI_STATUS_PARSE_ERROR;
    }

    umi_developer_workspace_state_init(&state);
    cursor = text + strlen(UMI_DEVELOPER_WORKSPACE_STATE_CODEC_PREFIX) + 1U;
    destinations[0]=state.context.project_id; capacities[0]=sizeof(state.context.project_id);
    destinations[1]=state.context.configuration_id; capacities[1]=sizeof(state.context.configuration_id);
    destinations[2]=state.context.target_id; capacities[2]=sizeof(state.context.target_id);
    destinations[3]=state.context.launch_profile_id; capacities[3]=sizeof(state.context.launch_profile_id);
    destinations[4]=state.context.repository_id; capacities[4]=sizeof(state.context.repository_id);
    destinations[5]=state.context.test_item_id; capacities[5]=sizeof(state.context.test_item_id);
    destinations[6]=state.context.active_document; capacities[6]=sizeof(state.context.active_document);
    destinations[7]=state.context.workspace_directory; capacities[7]=sizeof(state.context.workspace_directory);
    destinations[8]=state.last_task_id; capacities[8]=sizeof(state.last_task_id);
    destinations[9]=state.last_launch_profile_id; capacities[9]=sizeof(state.last_launch_profile_id);

    /* Visit each bounded item once so every record receives the same rule. */
    for (index=0U; index<10U; ++index) {
        status=next_field(&cursor,&begin,&length); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(status!=UMI_STATUS_OK)return status;
        status=decode_field(begin,length,destinations[index],capacities[index]);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if(status!=UMI_STATUS_OK)return status;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (*cursor == '\0' && index + 1U < 10U) return UMI_STATUS_PARSE_ERROR;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index=0U; index<4U; ++index) {
        status=next_field(&cursor,&begin,&length); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(status!=UMI_STATUS_OK)return status;
        status=decode_field(begin,length,number,sizeof(number)); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(status!=UMI_STATUS_OK)return status;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (index < 3U) {
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (strcmp(number,"0")!=0 && strcmp(number,"1")!=0) return UMI_STATUS_PARSE_ERROR;
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (index==0U) state.configure_before_build = number[0]=='1';
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (index==1U) state.build_before_run = number[0]=='1';
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (index==2U) state.test_after_build = number[0]=='1';
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            revision = strtoull(number,&end,10);
            /*
             * Protect caller-owned memory by checking that required state is available before it is
             * used.
             */
            if (number[0]=='\0' || end==NULL || *end!='\0') return UMI_STATUS_PARSE_ERROR;
            state.revision = (uint64_t)revision;
        }
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (*cursor != '\0') return UMI_STATUS_PARSE_ERROR;
    state.context.revision = state.revision;
    *out_state = state;
    return UMI_STATUS_OK;
}

/*
 * Write developer workspace state in its stable representation and report capacity or
 * input failures to the caller.
 */
UmiStatus umi_developer_workspace_state_save(
    const char *path,
    const UmiDeveloperWorkspaceStateSnapshot *state)
{
    char text[UMI_DEVELOPER_WORKSPACE_STATE_TEXT_CAPACITY];
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (path == NULL || state == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_developer_workspace_state_encode(state, text, sizeof(text), NULL);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_atomic_file_write_text(path, text);
}

/*
 * Read developer workspace state into validated module state and return a status when
 * input cannot be used.
 */
UmiStatus umi_developer_workspace_state_load(
    const char *path,
    UmiDeveloperWorkspaceStateSnapshot *out_state)
{
    char *text = NULL;
    size_t size = 0U;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (path == NULL || out_state == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_fs_read_text(path, &text, &size);
    (void)size;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_developer_workspace_state_decode(text, out_state);
    umi_fs_free_text(text);
    return status;
}
