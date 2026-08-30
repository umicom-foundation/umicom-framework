/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/checksum.h
 *
 * PURPOSE:
 *   Calculate deterministic file fingerprints for plug-in inventory and change
 *   detection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLUGIN_CHECKSUM_H
#define UMICOM_PLUGIN_CHECKSUM_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_PLUGIN_CHECKSUM_TEXT_CAPACITY 17U
UmiStatus umi_plugin_checksum_file(const char *path,
                                   uint64_t *out_checksum,
                                   uint64_t *out_size);
UmiStatus umi_plugin_checksum_text(uint64_t checksum,
                                   char *out_text,
                                   size_t capacity);
#ifdef __cplusplus
}
#endif

#endif
