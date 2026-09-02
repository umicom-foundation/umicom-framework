/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/config.h
 *
 * PURPOSE:
 *   Publish the public config contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLATFORM_CONFIG_H
#define UMICOM_PLATFORM_CONFIG_H

#include <stddef.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the config data shared with callers of this public contract.
 */
typedef struct UmiConfig UmiConfig;
/**
 * Initialise config from caller-provided values so later operations receive a known state.
 */
UmiStatus umi_config_create(UmiConfig **out_config);
/**
 * Release or reset state held by config so the same storage can be reused safely.
 */
void umi_config_destroy(UmiConfig *config);
/**
 * Copy config into module-owned storage so callers keep ownership of their input values.
 */
UmiStatus umi_config_set(UmiConfig *config, const char *key, const char *value);
/**
 * Provide the config get operation used by this module and its client applications.
 */
const char *umi_config_get(const UmiConfig *config, const char *key, const char *fallback);
/**
 * Provide the config load file operation used by this module and its client applications.
 */
UmiStatus umi_config_load_file(UmiConfig *config, const char *path);
/**
 * Return the number of records represented by config without changing their state.
 */
size_t umi_config_count(const UmiConfig *config);

#ifdef __cplusplus
}
#endif

#endif
