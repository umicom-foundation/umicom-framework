/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/codeguard/architecture_policy.h
 *
 * PURPOSE:
 *   Publish the public architecture policy contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_CODEGUARD_ARCHITECTURE_POLICY_H
#define UMICOM_CODEGUARD_ARCHITECTURE_POLICY_H
#include "umicom/codeguard/evidence.h"
typedef enum UmiCodeGuardArchitectureLayer {
    UMI_CODEGUARD_LAYER_FRAMEWORK_PUBLIC = 1,
    UMI_CODEGUARD_LAYER_FRAMEWORK_PRIVATE,
    UMI_CODEGUARD_LAYER_STUDIO,
    UMI_CODEGUARD_LAYER_APPLICATION,
    UMI_CODEGUARD_LAYER_PLUGIN,
    UMI_CODEGUARD_LAYER_UNKNOWN
} UmiCodeGuardArchitectureLayer;
typedef struct UmiCodeGuardArchitectureRule {
    UmiCodeGuardArchitectureLayer source;
    UmiCodeGuardArchitectureLayer target;
    bool allowed;
    char reason[UMI_CODEGUARD_QUALITY_TEXT_CAPACITY];
} UmiCodeGuardArchitectureRule;
typedef struct UmiCodeGuardArchitecturePolicy {
    UmiCodeGuardArchitectureRule rules[UMI_CODEGUARD_QUALITY_MAX_POLICIES];
    size_t count;
} UmiCodeGuardArchitecturePolicy;
void umi_codeguard_architecture_policy_default(UmiCodeGuardArchitecturePolicy *policy);
UmiCodeGuardArchitectureLayer umi_codeguard_architecture_classify(const char *path);
bool umi_codeguard_architecture_is_private_header(const char *path);
UmiStatus umi_codeguard_architecture_check_dependency(const UmiCodeGuardArchitecturePolicy *policy,const char *source_path,const char *target_path,UmiCodeGuardEvidenceStore *evidence);
#endif
