/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/compare_side.h
 *
 * PURPOSE:
 *   Describe one immutable or editable side of a professional comparison session.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_VCS_ADVANCED_COMPARE_SIDE_H
#define UMICOM_VCS_ADVANCED_COMPARE_SIDE_H

#include "umicom/vcs/advanced/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiVcsAdvancedCompareSide {
    uint32_t struct_size;
    uint32_t api_version;
    char path[UMI_VCS_ADVANCED_PATH_CAPACITY];
    char revision[UMI_VCS_ADVANCED_OID_CAPACITY];
    char label[UMI_VCS_ADVANCED_LABEL_CAPACITY];
    uint64_t fingerprint;
    int read_only;
    int exists;
} UmiVcsAdvancedCompareSide;

void umi_vcs_advanced_compare_side_init(UmiVcsAdvancedCompareSide *value);
UmiStatus umi_vcs_advanced_compare_side_validate(const UmiVcsAdvancedCompareSide *value);
UmiStatus umi_vcs_advanced_compare_side_set(UmiVcsAdvancedCompareSide *value,
                                              const char *path,
                                              const char *revision,
                                              const char *label,
                                              int read_only);

#ifdef __cplusplus
}
#endif

#endif
