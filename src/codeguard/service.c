/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/service.c
 *
 * PURPOSE:
 *   Keep a reusable scan root and retained result for Studio, Helix and CI
 *   hosts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file is intentionally small and focused. CodeGuard separates scanning
 * rules, analysis engines and reports so new developers can understand one
 * responsibility at a time and test it independently.
 */

#include "umicom/codeguard/service.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct UmiCodeGuardService { char root[1024]; UmiCodeGuardResult *result; };
/*
 * Initialise codeguard service from caller-provided values so later operations receive a
 * known state.
 */
/* Reject an unrepresentable scan root instead of silently changing the requested workspace through truncation. The previous implementation remains below for review. */
#if 0
UmiStatus umi_codeguard_service_create(const char *root,UmiCodeGuardService **out){UmiCodeGuardService *s;UmiStatus st;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(root==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;s=(UmiCodeGuardService*)calloc(1U,sizeof(*s));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return UMI_STATUS_OUT_OF_MEMORY;(void)snprintf(s->root,sizeof(s->root),"%s",root);st=umi_codeguard_result_create(256U,&s->result);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK){free(s);return st;}*out=s;return UMI_STATUS_OK;}
#endif
UmiStatus umi_codeguard_service_create(
    const char *root, UmiCodeGuardService **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (root == NULL || root[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = 0U;
    while (length < sizeof ((UmiCodeGuardService *)0)->root && root[length] != '\0')
        ++length;
    if (length == sizeof ((UmiCodeGuardService *)0)->root)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    UmiCodeGuardService *service = calloc(1U, sizeof *service);
    if (service == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(service->root, root, length + 1U);
    UmiStatus status = umi_codeguard_result_create(256U, &service->result);
    if (status != UMI_STATUS_OK) {
        free(service);
        return status;
    }
    *out = service;
    return UMI_STATUS_OK;
}
/*
 * Release or reset state held by codeguard service so the same storage can be reused
 * safely.
 */
void umi_codeguard_service_destroy(UmiCodeGuardService *s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=NULL){umi_codeguard_result_destroy(s->result);free(s);}}
/*
 * Provide the codeguard service scan operation used by this module and its client
 * applications.
 */
/* Publish complete findings only after a successful scan, keeping the retained result object and its previous evidence on failure. The previous implementation remains below for review. */
#if 0
UmiStatus umi_codeguard_service_scan(UmiCodeGuardService *s,const UmiCodeGuardProfile *p){UmiCodeGuardConfig c;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT;umi_codeguard_result_clear(s->result);c=umi_codeguard_config_default(s->root);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p!=NULL)c.profile=*p;return umi_codeguard_scan(&c,s->result);}
#endif
/* Observe cancellation before allocating and before publishing the staged report; failed attempts preserve retained evidence. The previous implementation remains for review. */
#if 0
UmiStatus umi_codeguard_service_scan(
    UmiCodeGuardService *service, const UmiCodeGuardProfile *profile)
{
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiCodeGuardConfig config = umi_codeguard_config_default(service->root);
    if (profile != NULL) config.profile = *profile;

    /* Scan into a separate owned candidate. Failed traversal, allocation or
     * analysis must not publish a partial replacement as the current report. */
    UmiCodeGuardResult *candidate = NULL;
    UmiStatus status = umi_codeguard_result_create(256U, &candidate);
    if (status == UMI_STATUS_OK) status = umi_codeguard_scan(&config, candidate);
    if (status == UMI_STATUS_OK)
        status = umi_codeguard_result_swap(service->result, candidate);
    umi_codeguard_result_destroy(candidate);
    return status;
}
#endif
UmiStatus umi_codeguard_service_scan_cancellable(
    UmiCodeGuardService *service, const UmiCodeGuardProfile *profile,
    const UmiCancellationToken *cancellation)
{
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_cancellation_token_is_requested(cancellation))
        return UMI_STATUS_CANCELLED;
    UmiCodeGuardConfig config = umi_codeguard_config_default(service->root);
    if (profile != NULL) config.profile = *profile;

    /* Scan into a separate owned candidate. Failed traversal, allocation or
     * analysis must not publish a partial replacement as the current report. */
    UmiCodeGuardResult *candidate = NULL;
    UmiStatus status = umi_codeguard_result_create(256U, &candidate);
    if (status == UMI_STATUS_OK) status = umi_codeguard_scan_cancellable(&config, candidate, cancellation);
    /* A request arriving after the scanner returns must still prevent a
     * partial attempt from replacing the last complete report. */
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancellation))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
        status = umi_codeguard_result_swap(service->result, candidate);
    umi_codeguard_result_destroy(candidate);
    return status;
}

/* Retain the established service API and its default-profile behavior. */
UmiStatus umi_codeguard_service_scan(UmiCodeGuardService *service,
    const UmiCodeGuardProfile *profile)
{
    return umi_codeguard_service_scan_cancellable(service, profile, NULL);
}
/*
 * Provide the codeguard service result operation used by this module and its client
 * applications.
 */
const UmiCodeGuardResult *umi_codeguard_service_result(const UmiCodeGuardService *s){return s!=NULL?s->result:NULL;}
/*
 * Provide the codeguard service summary operation used by this module and its client
 * applications.
 */
UmiCodeGuardSummary umi_codeguard_service_summary(const UmiCodeGuardService *s){return umi_codeguard_summary_build(s!=NULL?s->result:NULL);}

/* A job owns its root and profile. It does not keep this retained service alive
 * or touch the service on the worker; the caller owns both lifetimes separately. */
UmiStatus umi_codeguard_service_create_job(UmiCodeGuardService *service,
    const UmiCodeGuardProfile *profile, UmiCodeGuardJob **out_job)
{
    if (out_job == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_job = NULL;
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return UmiCodeGuardJobCreate(service->root, profile, out_job);
}

/* Admit identity before consuming the one-shot result. A stale workspace must
 * retain both its prior report and the job's unconsumed completed candidate. */
UmiStatus umi_codeguard_service_adopt_job(UmiCodeGuardService *service, UmiCodeGuardJob *job)
{
    if (service == NULL || job == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    int matches = 0;
    UmiStatus status = UmiCodeGuardJobMatchRoot(job, service->root, &matches);
    if (status != UMI_STATUS_OK) return status;
    if (!matches) return UMI_STATUS_INVALID_STATE;
    UmiCodeGuardResult *candidate = NULL;
    status = UmiCodeGuardJobTake(job, &candidate);
    if (status != UMI_STATUS_OK) return status;
    /* Both result objects are valid and owned, so swap needs no allocation.
     * Keep the retained object address stable; dispose its old storage only
     * after publication. Successful adoption invalidates old finding borrows. */
    status = umi_codeguard_result_swap(service->result, candidate);
    umi_codeguard_result_destroy(candidate);
    return status;
}
