/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/workspace_state.c
 *
 * PURPOSE:
 *   Implement versioned developer workspace-state persistence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer/workspace_state.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/platform/atomic_file.h"
#include "umicom/platform/filesystem.h"

static UmiStatus append_escape(
    char *output, size_t capacity, size_t *position, const char *text)
{
    static const char hex[] = "0123456789ABCDEF";
    const unsigned char *cursor = (const unsigned char *)(text != NULL ? text : "");
    while (*cursor != 0U) {
        unsigned char value = *cursor++;
        if (value == (unsigned char)'%' || value == (unsigned char)'|') {
            if (*position + 3U >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
            output[(*position)++] = '%';
            output[(*position)++] = hex[(value >> 4U) & 0x0FU];
            output[(*position)++] = hex[value & 0x0FU];
        } else {
            if (*position + 1U >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
            output[(*position)++] = (char)value;
        }
    }
    output[*position] = '\0';
    return UMI_STATUS_OK;
}

static int hex_value(char value)
{
    if (value >= '0' && value <= '9') return value - '0';
    if (value >= 'A' && value <= 'F') return value - 'A' + 10;
    if (value >= 'a' && value <= 'f') return value - 'a' + 10;
    return -1;
}

static UmiStatus decode_field(
    const char *begin, size_t length, char *output, size_t capacity)
{
    size_t input = 0U, position = 0U;
    if (output == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    while (input < length) {
        unsigned char value = (unsigned char)begin[input++];
        if (value == (unsigned char)'%') {
            int high, low;
            if (input + 1U >= length) return UMI_STATUS_PARSE_ERROR;
            high = hex_value(begin[input++]);
            low = hex_value(begin[input++]);
            if (high < 0 || low < 0) return UMI_STATUS_PARSE_ERROR;
            value = (unsigned char)((high << 4) | low);
        }
        if (position + 1U >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
        output[position++] = (char)value;
    }
    output[position] = '\0';
    return UMI_STATUS_OK;
}

static UmiStatus next_field(
    const char **cursor, const char **out_begin, size_t *out_length)
{
    const char *separator;
    if (cursor == NULL || *cursor == NULL || out_begin == NULL || out_length == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_begin = *cursor;
    separator = strchr(*cursor, '|');
    if (separator == NULL) {
        *out_length = strlen(*cursor);
        *cursor += *out_length;
    } else {
        *out_length = (size_t)(separator - *cursor);
        *cursor = separator + 1;
    }
    return UMI_STATUS_OK;
}

void umi_developer_workspace_state_init(
    UmiDeveloperWorkspaceStateSnapshot *state)
{
    if (state == NULL) return;
    memset(state, 0, sizeof(*state));
    state->struct_size = (uint32_t)sizeof(*state);
    state->api_version = UMI_DEVELOPER_WORKSPACE_STATE_API_VERSION;
    state->context.struct_size = (uint32_t)sizeof(state->context);
    state->context.api_version = UMI_DEVELOPER_CONTEXT_API_VERSION;
    state->context.revision = 1U;
    state->revision = 1U;
}

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

    if (state == NULL || out_text == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    written = snprintf(numbers[0], sizeof(numbers[0]), "%d", state->configure_before_build != 0);
    if (written < 0) return UMI_STATUS_INTERNAL_ERROR;
    written = snprintf(numbers[1], sizeof(numbers[1]), "%d", state->build_before_run != 0);
    if (written < 0) return UMI_STATUS_INTERNAL_ERROR;
    written = snprintf(numbers[2], sizeof(numbers[2]), "%d", state->test_after_build != 0);
    if (written < 0) return UMI_STATUS_INTERNAL_ERROR;
    written = snprintf(numbers[3], sizeof(numbers[3]), "%llu",
                       (unsigned long long)state->revision);
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

    if (strlen(UMI_DEVELOPER_WORKSPACE_STATE_CODEC_PREFIX) + 1U >= capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    memcpy(out_text, UMI_DEVELOPER_WORKSPACE_STATE_CODEC_PREFIX,
           strlen(UMI_DEVELOPER_WORKSPACE_STATE_CODEC_PREFIX));
    position = strlen(UMI_DEVELOPER_WORKSPACE_STATE_CODEC_PREFIX);
    out_text[position++] = '|';
    out_text[position] = '\0';

    for (index = 0U; index < 10U; ++index) {
        status = append_escape(out_text, capacity, &position, fields[index]);
        if (status != UMI_STATUS_OK) return status;
        if (position + 1U >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
        out_text[position++] = '|';
        out_text[position] = '\0';
    }

    for (index = 0U; index < 4U; ++index) {
        status = append_escape(out_text, capacity, &position, numbers[index]);
        if (status != UMI_STATUS_OK) return status;
        if (index + 1U < 4U) {
            if (position + 1U >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
            out_text[position++] = '|';
            out_text[position] = '\0';
        }
    }

    if (out_length != NULL) *out_length = position;
    return UMI_STATUS_OK;
}

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

    if (text == NULL || out_state == NULL) return UMI_STATUS_INVALID_ARGUMENT;
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

    for (index=0U; index<10U; ++index) {
        status=next_field(&cursor,&begin,&length); if(status!=UMI_STATUS_OK)return status;
        status=decode_field(begin,length,destinations[index],capacities[index]);
        if(status!=UMI_STATUS_OK)return status;
        if (*cursor == '\0' && index + 1U < 10U) return UMI_STATUS_PARSE_ERROR;
    }

    for (index=0U; index<4U; ++index) {
        status=next_field(&cursor,&begin,&length); if(status!=UMI_STATUS_OK)return status;
        status=decode_field(begin,length,number,sizeof(number)); if(status!=UMI_STATUS_OK)return status;
        if (index < 3U) {
            if (strcmp(number,"0")!=0 && strcmp(number,"1")!=0) return UMI_STATUS_PARSE_ERROR;
            if (index==0U) state.configure_before_build = number[0]=='1';
            if (index==1U) state.build_before_run = number[0]=='1';
            if (index==2U) state.test_after_build = number[0]=='1';
        } else {
            revision = strtoull(number,&end,10);
            if (number[0]=='\0' || end==NULL || *end!='\0') return UMI_STATUS_PARSE_ERROR;
            state.revision = (uint64_t)revision;
        }
    }
    if (*cursor != '\0') return UMI_STATUS_PARSE_ERROR;
    state.context.revision = state.revision;
    *out_state = state;
    return UMI_STATUS_OK;
}

UmiStatus umi_developer_workspace_state_save(
    const char *path,
    const UmiDeveloperWorkspaceStateSnapshot *state)
{
    char text[UMI_DEVELOPER_WORKSPACE_STATE_TEXT_CAPACITY];
    UmiStatus status;
    if (path == NULL || state == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_developer_workspace_state_encode(state, text, sizeof(text), NULL);
    if (status != UMI_STATUS_OK) return status;
    return umi_atomic_file_write_text(path, text);
}

UmiStatus umi_developer_workspace_state_load(
    const char *path,
    UmiDeveloperWorkspaceStateSnapshot *out_state)
{
    char *text = NULL;
    size_t size = 0U;
    UmiStatus status;
    if (path == NULL || out_state == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_fs_read_text(path, &text, &size);
    (void)size;
    if (status != UMI_STATUS_OK) return status;
    status = umi_developer_workspace_state_decode(text, out_state);
    umi_fs_free_text(text);
    return status;
}
