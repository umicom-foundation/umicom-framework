/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/server_config.h
 *
 * PURPOSE:
 *   Validate portable web server configuration.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This module has one narrow responsibility. Keeping the pieces separate makes the web platform easier to test and lets Studio, Trader and TMS reuse the same implementation.
 */

#ifndef UMICOM_WEB_SERVER_CONFIG_H
#define UMICOM_WEB_SERVER_CONFIG_H
#include <stdint.h>
#include "umicom/base/value_archive.h"
#include "umicom/web/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the web server config data shared with callers of this public contract.
 */
typedef struct UmiWebServerConfig { char bind_address[64]; uint16_t port; size_t max_request_bytes; int loopback_only; } UmiWebServerConfig;
/**
 * Provide the web server config default operation used by this module and its client
 * applications.
 */
UmiWebServerConfig umi_web_server_config_default(void);
/**
 * Check that web server config satisfies its contract before another service relies on it.
 */
UmiStatus umi_web_server_config_validate(const UmiWebServerConfig *config);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_web_server_config_archive_encode(const UmiWebServerConfig *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_web_server_config_archive_decode(const void *bytes, size_t byte_count,
    UmiWebServerConfig *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_web_server_config_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiWebServerConfig *expected, UmiWebServerConfig *value);


#ifdef __cplusplus
}
#endif
#endif
