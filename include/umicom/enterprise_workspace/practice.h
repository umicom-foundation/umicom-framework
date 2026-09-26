/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/enterprise_workspace/practice.h
 * PURPOSE: Compose visibly labelled practice roles without implying authentication.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_ENTERPRISE_WORKSPACE_PRACTICE_H
#define UMICOM_ENTERPRISE_WORKSPACE_PRACTICE_H
#include "umicom/enterprise_workspace/workspace.h"
#ifdef __cplusplus
extern "C" {
#endif
/* This bundle is for a local teaching workspace. It does NOT authenticate a
 * person or secure an untrusted process. A production host supplies its own
 * authenticated actor and a governed authorisation service instead. */
typedef struct UmiEnterprisePracticeAccess UmiEnterprisePracticeAccess;
UmiStatus UmiEnterprisePracticeAccessCreate(UmiEnterprisePracticeAccess **outAccess);
void UmiEnterprisePracticeAccessDestroy(UmiEnterprisePracticeAccess *access);
UmiAuthorisationService *UmiEnterprisePracticeAuthorisation(UmiEnterprisePracticeAccess *access);
UmiEnterpriseActor UmiEnterprisePracticeActor(size_t index);
const char *UmiEnterprisePracticeActorLabel(size_t index);
#define UMI_ENTERPRISE_PRACTICE_ACTORS 5U
#ifdef __cplusplus
}
#endif
#endif
