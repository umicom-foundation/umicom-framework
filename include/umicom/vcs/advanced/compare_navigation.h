/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/compare_navigation.h
 *
 * PURPOSE:
 *   Track deterministic next/previous change navigation in comparison sessions.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_VCS_ADVANCED_COMPARE_NAVIGATION_H
#define UMICOM_VCS_ADVANCED_COMPARE_NAVIGATION_H

#include "umicom/vcs/advanced/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiVcsAdvancedCompareNavigation {
    uint32_t struct_size;
    uint32_t api_version;
    size_t change_count;
    size_t current_index;
    int wrap;
} UmiVcsAdvancedCompareNavigation;

void umi_vcs_advanced_compare_navigation_init(UmiVcsAdvancedCompareNavigation *value);
UmiStatus umi_vcs_advanced_compare_navigation_validate(const UmiVcsAdvancedCompareNavigation *value);
int umi_vcs_advanced_compare_navigation_next(UmiVcsAdvancedCompareNavigation *value);
int umi_vcs_advanced_compare_navigation_previous(UmiVcsAdvancedCompareNavigation *value);

#ifdef __cplusplus
}
#endif

#endif
