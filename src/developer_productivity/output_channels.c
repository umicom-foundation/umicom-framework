/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/output_channels.c
 *
 * PURPOSE:
 *   Implement bounded named output channels.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/output_channels.h"

#include <stdio.h>
#include <string.h>

static size_t find_index(
    const UmiDeveloperOutputChannels *channels,
    const char *channel_id)
{
    size_t index;

    if (channels == NULL || channel_id == NULL) return (size_t)-1;

    for (index = 0U; index < channels->count; ++index) {
        if (strcmp(channels->channels[index].channel_id, channel_id) == 0) {
            return index;
        }
    }

    return (size_t)-1;
}

void umi_developer_output_channels_init(
    UmiDeveloperOutputChannels *channels)
{
    if (channels == NULL) return;
    (void)memset(channels, 0, sizeof(*channels));
    channels->revision = 1U;
}

UmiStatus umi_developer_output_channel_append(
    UmiDeveloperOutputChannels *channels,
    const char *channel_id,
    const char *title,
    const char *text)
{
    size_t index;
    UmiDeveloperOutputChannel *channel;
    size_t length;

    if (channels == NULL || channel_id == NULL ||
        title == NULL || text == NULL || channel_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    index = find_index(channels, channel_id);
    if (index == (size_t)-1) {
        if (channels->count >= UMI_DEVELOPER_OUTPUT_CHANNEL_CAPACITY) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        index = channels->count++;
        channel = &channels->channels[index];
        (void)memset(channel, 0, sizeof(*channel));
        (void)snprintf(channel->channel_id,
                       sizeof(channel->channel_id),
                       "%s",
                       channel_id);
        (void)snprintf(channel->title,
                       sizeof(channel->title),
                       "%s",
                       title);
    }

    channel = &channels->channels[index];
    length = strlen(text);

    if (channel->length + length + 2U > sizeof(channel->text)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)memcpy(channel->text + channel->length, text, length);
    channel->length += length;
    channel->text[channel->length++] = '\n';
    channel->text[channel->length] = '\0';
    channel->revision += 1U;
    channels->revision += 1U;
    return UMI_STATUS_OK;
}

void umi_developer_output_channels_clear_all(
    UmiDeveloperOutputChannels *channels)
{
    size_t index;

    if (channels == NULL) return;

    for (index = 0U; index < channels->count; ++index) {
        channels->channels[index].text[0] = '\0';
        channels->channels[index].length = 0U;
        channels->channels[index].revision += 1U;
    }

    channels->revision += 1U;
}

UmiStatus umi_developer_output_channel_clear(
    UmiDeveloperOutputChannels *channels,
    const char *channel_id)
{
    size_t index;

    if (channels == NULL || channel_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    index = find_index(channels, channel_id);
    if (index == (size_t)-1) return UMI_STATUS_NOT_FOUND;

    channels->channels[index].text[0] = '\0';
    channels->channels[index].length = 0U;
    channels->channels[index].revision += 1U;
    channels->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_developer_output_channel_find(
    const UmiDeveloperOutputChannels *channels,
    const char *channel_id,
    UmiDeveloperOutputChannel *out_channel)
{
    size_t index;

    if (channels == NULL || channel_id == NULL || out_channel == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    index = find_index(channels, channel_id);
    if (index == (size_t)-1) return UMI_STATUS_NOT_FOUND;

    *out_channel = channels->channels[index];
    return UMI_STATUS_OK;
}
