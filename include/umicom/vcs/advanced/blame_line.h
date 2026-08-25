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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_VCS_ADVANCED_BLAME_LINE_H
#define UMICOM_VCS_ADVANCED_BLAME_LINE_H

#include "umicom/vcs/advanced/types.h"

#ifdef __cplusplus
extern "C" {
#endif

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

void umi_vcs_advanced_blame_line_init(UmiVcsAdvancedBlameLine *value);
UmiStatus umi_vcs_advanced_blame_line_validate(const UmiVcsAdvancedBlameLine *value);
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
