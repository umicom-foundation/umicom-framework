/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/tag_operation.h
 *
 * PURPOSE:
 *   Plan tag creation, deletion or verification with explicit signing intent.
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
#ifndef UMICOM_VCS_ADVANCED_TAG_OPERATION_H
#define UMICOM_VCS_ADVANCED_TAG_OPERATION_H

#include "umicom/vcs/advanced/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiVcsAdvancedTagOperation {
    uint32_t struct_size;
    uint32_t api_version;
    char tag_name[UMI_VCS_ADVANCED_LABEL_CAPACITY];
    char target[UMI_VCS_ADVANCED_OID_CAPACITY];
    char message[UMI_VCS_ADVANCED_TEXT_CAPACITY];
    int create;
    int delete_tag;
    int annotated;
    int sign;
    int verify;
} UmiVcsAdvancedTagOperation;

void umi_vcs_advanced_tag_operation_init(UmiVcsAdvancedTagOperation *value);
UmiStatus umi_vcs_advanced_tag_operation_validate(const UmiVcsAdvancedTagOperation *value);
UmiStatus umi_vcs_advanced_tag_operation_create(UmiVcsAdvancedTagOperation *value,
                                                  const char *tag_name,
                                                  const char *target,
                                                  const char *message,
                                                  int sign);

#ifdef __cplusplus
}
#endif

#endif
