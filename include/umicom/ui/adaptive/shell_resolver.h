/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/adaptive/shell_resolver.h
 *
 * PURPOSE:
 *   Resolve a shell profile into concrete region presentations for one viewport class.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_ADAPTIVE_SHELL_RESOLVER_H
#define UMICOM_UI_ADAPTIVE_SHELL_RESOLVER_H
#include "umicom/ui/adaptive/shell_profile.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAdaptiveResolvedShell {
    UmiAdaptiveNavigationPattern navigation;
    UmiDesignDensity density;
    size_t inline_regions;
    size_t overlay_regions;
    size_t hidden_regions;
} UmiAdaptiveResolvedShell;
/* Resolve shell-region presentation counts for the active canonical size class. */
UmiStatus umi_adaptive_shell_resolver_resolve(const UmiAdaptiveShellProfile *profile,
                                              UmiDesignSizeClass size_class,
                                              UmiAdaptiveResolvedShell *out_shell);

#ifdef __cplusplus
}
#endif
#endif
