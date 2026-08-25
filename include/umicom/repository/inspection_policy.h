/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/inspection_policy.h
 *
 * PURPOSE:
 *   Define additive inspection checks layered on top of the existing repository doctor policy.
 *
 * ARCHITECTURE:
 *   Extend the existing Framework-owned repository control and maintenance
 *   capabilities. This module does not duplicate VCS, source-control, lock,
 *   maintenance or doctor state already present in Umicom Framework.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_REPOSITORY_INSPECTION_POLICY_H
#define INCLUDE_UMICOM_REPOSITORY_INSPECTION_POLICY_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Add only structural checks that are not already represented by
 * UmiRepositoryDoctorPolicy. Dirty state, remotes, upstreams, divergence and
 * submodule health remain exclusively owned by the existing doctor subsystem.
 */
typedef struct UmiRepositoryInspectionPolicy {
    int require_fetch_capability;
    int require_gitlink_alignment;
    int require_gitignore;
    int require_gitattributes;
} UmiRepositoryInspectionPolicy;

/* Initialise conservative structural checks for an Umicom source repository. */
void umi_repository_inspection_policy_init(
    UmiRepositoryInspectionPolicy *policy);

#ifdef __cplusplus
}
#endif
#endif
