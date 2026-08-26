/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/token_registry.h
 *
 * PURPOSE:
 *   Register theme-mode-specific token sets for application and frontend resolution.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_UI_DESIGN_TOKEN_REGISTRY_H
#define INCLUDE_UMICOM_UI_DESIGN_TOKEN_REGISTRY_H

#include "umicom/ui/design/types.h"
#include "umicom/ui/design/token_set.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/ui/design/token_set.h"
typedef struct UmiDesignTokenRegistryEntry { UmiDesignThemeMode mode; UmiDesignTokenSet set; } UmiDesignTokenRegistryEntry;
typedef struct UmiDesignTokenRegistry { UmiDesignTokenRegistryEntry entries[8]; size_t count; } UmiDesignTokenRegistry;
/* Register or replace a token set for one theme mode. */
UmiStatus umi_design_token_registry_set(UmiDesignTokenRegistry *registry, UmiDesignThemeMode mode, const UmiDesignTokenSet *set);
/* Resolve the token set registered for a theme mode. */
UmiStatus umi_design_token_registry_get(const UmiDesignTokenRegistry *registry, UmiDesignThemeMode mode, UmiDesignTokenSet *out_set);

#ifdef __cplusplus
}
#endif

#endif
