/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/moved_block.h
 *
 * PURPOSE:
 *   Capture identical or near-identical blocks moved within a document.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_VCS_ADVANCED_MOVED_BLOCK_H
#define UMICOM_VCS_ADVANCED_MOVED_BLOCK_H

#include "umicom/vcs/advanced/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiVcsAdvancedMovedBlock {
    uint32_t struct_size;
    uint32_t api_version;
    size_t old_start;
    size_t new_start;
    size_t line_count;
    uint64_t fingerprint;
    uint32_t confidence_percent;
} UmiVcsAdvancedMovedBlock;

void umi_vcs_advanced_moved_block_init(UmiVcsAdvancedMovedBlock *value);
UmiStatus umi_vcs_advanced_moved_block_validate(const UmiVcsAdvancedMovedBlock *value);
int umi_vcs_advanced_moved_block_is_significant(const UmiVcsAdvancedMovedBlock *value,
                                                   size_t minimum_lines,
                                                   uint32_t minimum_confidence);

#ifdef __cplusplus
}
#endif

#endif
