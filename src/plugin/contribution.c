/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/contribution.c
 *
 * PURPOSE:
 *   Implement stable ordered plug-in contribution metadata without coupling the
 *   registry to GTK or Studio.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/contribution.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/platform/threading.h"
struct UmiPluginContributionRegistry { UmiPluginContribution items[UMI_PLUGIN_CONTRIBUTION_MAX]; size_t count; UmiMutex *mutex; };
UmiStatus umi_plugin_contribution_registry_create(UmiPluginContributionRegistry **out_registry)
{
    UmiPluginContributionRegistry *r; UmiStatus status;
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL; r = (UmiPluginContributionRegistry *)calloc(1U, sizeof(*r)); if (r == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_mutex_create(&r->mutex); if (status != UMI_STATUS_OK) { free(r); return status; }
    *out_registry = r; return UMI_STATUS_OK;
}
void umi_plugin_contribution_registry_destroy(UmiPluginContributionRegistry *r) { if (r != NULL) { umi_mutex_destroy(r->mutex); free(r); } }
UmiStatus umi_plugin_contribution_registry_add(UmiPluginContributionRegistry *r, const UmiPluginContribution *c)
{
    size_t i; UmiStatus status;
    if (r == NULL || c == NULL || c->plugin_id[0] == '\0' || c->contribution_type[0] == '\0' || c->contribution_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_mutex_lock(r->mutex); if (status != UMI_STATUS_OK) return status;
    for (i = 0U; i < r->count; ++i) if (strcmp(r->items[i].contribution_id, c->contribution_id) == 0) { (void)umi_mutex_unlock(r->mutex); return UMI_STATUS_ALREADY_EXISTS; }
    if (r->count >= UMI_PLUGIN_CONTRIBUTION_MAX) { (void)umi_mutex_unlock(r->mutex); return UMI_STATUS_CAPACITY_EXCEEDED; }
    r->items[r->count++] = *c; (void)umi_mutex_unlock(r->mutex); return UMI_STATUS_OK;
}
size_t umi_plugin_contribution_registry_count(const UmiPluginContributionRegistry *r)
{ size_t n = 0U; if (r != NULL && umi_mutex_lock(r->mutex) == UMI_STATUS_OK) { n = r->count; (void)umi_mutex_unlock(r->mutex); } return n; }
UmiStatus umi_plugin_contribution_registry_at(const UmiPluginContributionRegistry *r, size_t index, UmiPluginContribution *out)
{
    UmiStatus status; if (r == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_mutex_lock(r->mutex); if (status != UMI_STATUS_OK) return status;
    if (index >= r->count) { (void)umi_mutex_unlock(r->mutex); return UMI_STATUS_NOT_FOUND; }
    *out = r->items[index]; (void)umi_mutex_unlock(r->mutex); return UMI_STATUS_OK;
}
size_t umi_plugin_contribution_registry_count_type(const UmiPluginContributionRegistry *r, const char *type)
{ size_t i, n = 0U; if (r == NULL || type == NULL || umi_mutex_lock(r->mutex) != UMI_STATUS_OK) return 0U; for (i = 0U; i < r->count; ++i) if (strcmp(r->items[i].contribution_type, type) == 0) ++n; (void)umi_mutex_unlock(r->mutex); return n; }
