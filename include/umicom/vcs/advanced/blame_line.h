/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/blame_line.h
 *
 * PURPOSE:
 *   Describe line-level provenance without leaking Git parser details into applications.
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
#ifndef UMICOM_VCS_ADVANCED_BLAME_LINE_H
#define UMICOM_VCS_ADVANCED_BLAME_LINE_H

#include "umicom/vcs/advanced/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the vcs advanced blame line data shared with callers of this public contract.
 */
typedef struct UmiVcsAdvancedBlameLine {
    uint32_t struct_size;
    uint32_t api_version;
    size_t line_number;
    size_t original_line_number;
    char commit_oid[UMI_VCS_ADVANCED_OID_CAPACITY];
    char author[UMI_VCS_ADVANCED_LABEL_CAPACITY];
    char source_path[UMI_VCS_ADVANCED_PATH_CAPACITY];
    uint64_t timestamp_seconds;
    uint64_t content_fingerprint;
} UmiVcsAdvancedBlameLine;

/**
 * Initialise vcs advanced blame line from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_blame_line_init(UmiVcsAdvancedBlameLine *value);
/**
 * Check that vcs advanced blame line satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_vcs_advanced_blame_line_validate(const UmiVcsAdvancedBlameLine *value);
/**
 * Copy vcs advanced blame line into module-owned storage so callers keep ownership of
 * their input values.
 */
UmiStatus umi_vcs_advanced_blame_line_set(UmiVcsAdvancedBlameLine *value,
                                            size_t line_number,
                                            const char *commit_oid,
                                            const char *author,
                                            const char *source_path,
                                            const char *content);

#ifdef __cplusplus
}
#endif

#endif
