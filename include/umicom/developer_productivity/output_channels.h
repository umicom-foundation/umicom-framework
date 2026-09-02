/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/output_channels.h
 *
 * PURPOSE:
 *   Retain named build/test/run/source-control/AI output channels independently
 *   from the Studio Output panel.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PRODUCTIVITY_OUTPUT_CHANNELS_H
#define UMICOM_DEVELOPER_PRODUCTIVITY_OUTPUT_CHANNELS_H

#include "umicom/developer_productivity/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_OUTPUT_CHANNEL_CAPACITY 16U
#define UMI_DEVELOPER_OUTPUT_TEXT_CAPACITY 8192U

/**
 * Represent the developer output channel data shared with callers of this public contract.
 */
typedef struct UmiDeveloperOutputChannel {
    char channel_id[UMI_DEVELOPER_PRODUCTIVITY_ID_CAPACITY];
    char title[UMI_DEVELOPER_PRODUCTIVITY_NAME_CAPACITY];
    char text[UMI_DEVELOPER_OUTPUT_TEXT_CAPACITY];
    size_t length;
    uint64_t revision;
} UmiDeveloperOutputChannel;

/**
 * Represent the developer output channels data shared with callers of this public
 * contract.
 */
typedef struct UmiDeveloperOutputChannels {
    UmiDeveloperOutputChannel
        channels[UMI_DEVELOPER_OUTPUT_CHANNEL_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiDeveloperOutputChannels;

/**
 * Initialise developer output channels from caller-provided values so later operations
 * receive a known state.
 */
void umi_developer_output_channels_init(
    UmiDeveloperOutputChannels *channels);

/**
 * Add developer output channel only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_developer_output_channel_append(
    UmiDeveloperOutputChannels *channels,
    const char *channel_id,
    const char *title,
    const char *text);

/**
 * Provide the developer output channels clear all operation used by this module and its
 * client applications.
 */
void umi_developer_output_channels_clear_all(
    UmiDeveloperOutputChannels *channels);

/**
 * Release or reset state held by developer output channel so the same storage can be
 * reused safely.
 */
UmiStatus umi_developer_output_channel_clear(
    UmiDeveloperOutputChannels *channels,
    const char *channel_id);

/**
 * Find developer output channel while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_developer_output_channel_find(
    const UmiDeveloperOutputChannels *channels,
    const char *channel_id,
    UmiDeveloperOutputChannel *out_channel);

#ifdef __cplusplus
}
#endif

#endif
