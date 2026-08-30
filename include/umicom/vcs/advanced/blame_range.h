/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/blame_range.h
 *
 * PURPOSE:
 *   Represent and normalize line ranges requested from blame/history providers.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_VCS_ADVANCED_BLAME_RANGE_H
#define UMICOM_VCS_ADVANCED_BLAME_RANGE_H

#include "umicom/vcs/advanced/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiVcsAdvancedBlameRange {
    uint32_t struct_size;
    uint32_t api_version;
    size_t first_line;
    size_t last_line;
} UmiVcsAdvancedBlameRange;

void umi_vcs_advanced_blame_range_init(UmiVcsAdvancedBlameRange *value);
UmiStatus umi_vcs_advanced_blame_range_validate(const UmiVcsAdvancedBlameRange *value);
UmiStatus umi_vcs_advanced_blame_range_set(UmiVcsAdvancedBlameRange *value,
                                             size_t first_line,
                                             size_t last_line);
size_t umi_vcs_advanced_blame_range_count(const UmiVcsAdvancedBlameRange *value);

#ifdef __cplusplus
}
#endif

#endif
