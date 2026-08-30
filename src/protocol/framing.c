/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/protocol/framing.c
 *
 * PURPOSE:
 *   Implement strict Content-Length framing with partial-buffer detection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/protocol/framing.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

UmiStatus umi_protocol_frame_encode(const char *content,
                                    char *out_frame,
                                    size_t capacity,
                                    size_t *out_length)
{
    size_t content_length;
    int header_length;
    if (content == NULL || out_frame == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    content_length = strlen(content);
    header_length = snprintf(out_frame,
                             capacity,
                             "Content-Length: %zu\r\n\r\n",
                             content_length);
    if (header_length < 0 ||
        (size_t)header_length + content_length + 1U > capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(out_frame + (size_t)header_length,
                 content,
                 content_length + 1U);
    if (out_length != NULL) {
        *out_length = (size_t)header_length + content_length;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_protocol_frame_parse(const char *buffer,
                                   size_t buffer_length,
                                   UmiProtocolFrame *out_frame)
{
    const char *header_end;
    const char *length_start;
    char number[64];
    size_t number_length;
    unsigned long long parsed;
    char *parse_end = NULL;
    size_t header_length;

    if (buffer == NULL || out_frame == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_frame, 0, sizeof(*out_frame));
    header_end = strstr(buffer, "\r\n\r\n");
    if (header_end == NULL ||
        (size_t)(header_end - buffer) + 4U > buffer_length) {
        return UMI_STATUS_BUSY;
    }
    length_start = strstr(buffer, "Content-Length:");
    if (length_start == NULL || length_start > header_end) {
        return UMI_STATUS_PARSE_ERROR;
    }
    length_start += strlen("Content-Length:");
    while (length_start < header_end &&
           isspace((unsigned char)*length_start)) {
        length_start += 1;
    }
    number_length = 0U;
    while (length_start + number_length < header_end &&
           isdigit((unsigned char)length_start[number_length]) &&
           number_length + 1U < sizeof(number)) {
        number[number_length] = length_start[number_length];
        number_length += 1U;
    }
    if (number_length == 0U) {
        return UMI_STATUS_PARSE_ERROR;
    }
    number[number_length] = '\0';
    parsed = strtoull(number, &parse_end, 10);
    if (parse_end == number || *parse_end != '\0' ||
        parsed > UMI_PROTOCOL_MESSAGE_CAPACITY) {
        return UMI_STATUS_PARSE_ERROR;
    }
    header_length = (size_t)(header_end - buffer) + 4U;
    if (header_length + (size_t)parsed > buffer_length) {
        return UMI_STATUS_BUSY;
    }
    out_frame->content_length = (size_t)parsed;
    out_frame->header_length = header_length;
    out_frame->total_length = header_length + (size_t)parsed;
    out_frame->content = buffer + header_length;
    return UMI_STATUS_OK;
}

UmiStatus umi_protocol_frame_copy_content(const UmiProtocolFrame *frame,
                                          char *out_content,
                                          size_t capacity)
{
    if (frame == NULL || out_content == NULL || frame->content == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (frame->content_length + 1U > capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(out_content, frame->content, frame->content_length);
    out_content[frame->content_length] = '\0';
    return UMI_STATUS_OK;
}
