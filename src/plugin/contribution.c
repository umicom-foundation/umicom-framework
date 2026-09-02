/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/contribution.c
 *
 * PURPOSE:
 *   Implement stable ordered plug-in contribution metadata without coupling the
 *   registry to GTK or Studio.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/contribution.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/platform/threading.h"
struct UmiPluginContributionRegistry { UmiPluginContribution items[UMI_PLUGIN_CONTRIBUTION_MAX]; size_t count; UmiMutex *mutex; };
/*
 * Initialise plugin contribution registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_plugin_contribution_registry_create(UmiPluginContributionRegistry **out_registry)
{
    UmiPluginContributionRegistry *r; UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL; r = (UmiPluginContributionRegistry *)calloc(1U, sizeof(*r)); /* Protect caller-owned memory by checking that required state is available before it is used. */ if (r == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_mutex_create(&r->mutex); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) { free(r); return status; }
    *out_registry = r; return UMI_STATUS_OK;
}
/*
 * Release or reset state held by plugin contribution registry so the same storage can be
 * reused safely.
 */
void umi_plugin_contribution_registry_destroy(UmiPluginContributionRegistry *r) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (r != NULL) { umi_mutex_destroy(r->mutex); free(r); } }
/*
 * Add plugin contribution registry only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_plugin_contribution_registry_add(UmiPluginContributionRegistry *r, const UmiPluginContribution *c)
{
    size_t i; UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (r == NULL || c == NULL || c->plugin_id[0] == '\0' || c->contribution_type[0] == '\0' || c->contribution_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_mutex_lock(r->mutex); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < r->count; ++i) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(r->items[i].contribution_id, c->contribution_id) == 0) { (void)umi_mutex_unlock(r->mutex); return UMI_STATUS_ALREADY_EXISTS; }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (r->count >= UMI_PLUGIN_CONTRIBUTION_MAX) { (void)umi_mutex_unlock(r->mutex); return UMI_STATUS_CAPACITY_EXCEEDED; }
    r->items[r->count++] = *c; (void)umi_mutex_unlock(r->mutex); return UMI_STATUS_OK;
}
/*
 * Return the number of records represented by plugin contribution registry without
 * changing their state.
 */
size_t umi_plugin_contribution_registry_count(const UmiPluginContributionRegistry *r)
{ size_t n = 0U; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (r != NULL && umi_mutex_lock(r->mutex) == UMI_STATUS_OK) { n = r->count; (void)umi_mutex_unlock(r->mutex); } return n; }
/*
 * Find plugin contribution registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_plugin_contribution_registry_at(const UmiPluginContributionRegistry *r, size_t index, UmiPluginContribution *out)
{
    UmiStatus status; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (r == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_mutex_lock(r->mutex); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= r->count) { (void)umi_mutex_unlock(r->mutex); return UMI_STATUS_NOT_FOUND; }
    *out = r->items[index]; (void)umi_mutex_unlock(r->mutex); return UMI_STATUS_OK;
}
/*
 * Provide the plugin contribution registry count type operation used by this module and
 * its client applications.
 */
size_t umi_plugin_contribution_registry_count_type(const UmiPluginContributionRegistry *r, const char *type)
{ size_t i, n = 0U; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (r == NULL || type == NULL || umi_mutex_lock(r->mutex) != UMI_STATUS_OK) return 0U; /* Visit each bounded item once so every record receives the same rule. */ for (i = 0U; i < r->count; ++i) /* Protect caller-owned memory by checking that required state is available before it is used. */ if (strcmp(r->items[i].contribution_type, type) == 0) ++n; (void)umi_mutex_unlock(r->mutex); return n; }
