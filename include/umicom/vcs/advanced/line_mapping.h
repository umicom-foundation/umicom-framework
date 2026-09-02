/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/line_mapping.h
 *
 * PURPOSE:
 *   Map individual lines between comparison sides with confidence evidence.
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
#ifndef UMICOM_VCS_ADVANCED_LINE_MAPPING_H
#define UMICOM_VCS_ADVANCED_LINE_MAPPING_H

#include "umicom/vcs/advanced/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the vcs advanced line mapping data shared with callers of this public
 * contract.
 */
typedef struct UmiVcsAdvancedLineMapping {
    uint32_t struct_size;
    uint32_t api_version;
    size_t left_line;
    size_t right_line;
    uint32_t confidence_percent;
    UmiVcsDiffKind kind;
} UmiVcsAdvancedLineMapping;

/**
 * Initialise vcs advanced line mapping from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_line_mapping_init(UmiVcsAdvancedLineMapping *value);
/**
 * Check that vcs advanced line mapping satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_vcs_advanced_line_mapping_validate(const UmiVcsAdvancedLineMapping *value);
/**
 * Provide the vcs advanced line mapping exact operation used by this module and its client
 * applications.
 */
int umi_vcs_advanced_line_mapping_exact(const UmiVcsAdvancedLineMapping *value);

#ifdef __cplusplus
}
#endif

#endif
