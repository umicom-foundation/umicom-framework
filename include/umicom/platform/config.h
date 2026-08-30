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

typedef struct UmiConfig UmiConfig;
UmiStatus umi_config_create(UmiConfig **out_config);
void umi_config_destroy(UmiConfig *config);
UmiStatus umi_config_set(UmiConfig *config, const char *key, const char *value);
const char *umi_config_get(const UmiConfig *config, const char *key, const char *fallback);
UmiStatus umi_config_load_file(UmiConfig *config, const char *path);
size_t umi_config_count(const UmiConfig *config);

#ifdef __cplusplus
}
#endif

#endif
