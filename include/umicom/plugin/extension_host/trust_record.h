/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/trust_record.h
 *
 * PURPOSE:
 *   Capture one durable trust decision and its evidence revision.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_TRUST_RECORD_H
#define UMICOM_PLUGIN_EXTENSION_HOST_TRUST_RECORD_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPluginExtensionHostTrustRecord {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY];
    char subject[UMI_PLUGIN_EXTENSION_HOST_TEXT_CAPACITY];
    uint32_t version;
    uint32_t risk;
    uint64_t flags;
    uint64_t revision;
} UmiPluginExtensionHostTrustRecord;

void umi_plugin_extension_host_trust_record_init(UmiPluginExtensionHostTrustRecord *value);
UmiStatus umi_plugin_extension_host_trust_record_configure(UmiPluginExtensionHostTrustRecord *value, const char *id, const char *subject, uint32_t version, uint32_t risk, uint64_t flags);
UmiStatus umi_plugin_extension_host_trust_record_validate(const UmiPluginExtensionHostTrustRecord *value);
uint64_t umi_plugin_extension_host_trust_record_fingerprint(const UmiPluginExtensionHostTrustRecord *value);

#ifdef __cplusplus
}
#endif

#endif
