/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/extension_host/trust_store.c
 *
 * PURPOSE:
 *   Maintain bounded trust decisions by extension or publisher identity.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/trust_store.h"
#include <string.h>

/*
 * Copy plugin extension host trust store into module-owned storage so callers keep
 * ownership of their input values.
 */
static void umi_plugin_extension_host_trust_store_copy(char *destination, size_t capacity, const char *source)
{
    size_t i = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U) return;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source != NULL) {
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (i + 1U < capacity && source[i] != '\0') { destination[i] = source[i]; ++i; }
    }
    destination[i] = '\0';
}

/*
 * Initialise plugin extension host trust store from caller-provided values so later
 * operations receive a known state.
 */
void umi_plugin_extension_host_trust_store_init(UmiPluginExtensionHostTrustStore *catalogue) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (catalogue != NULL) memset(catalogue, 0, sizeof(*catalogue)); }
/*
 * Find plugin extension host trust store while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiPluginExtensionHostTrustStoreItem *umi_plugin_extension_host_trust_store_find(const UmiPluginExtensionHostTrustStore *catalogue, const char *id)
{
    size_t i; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (catalogue == NULL || id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i=0U;i<catalogue->count;++i) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(catalogue->items[i].id,id)==0) return &catalogue->items[i];
    return NULL;
}
/*
 * Add plugin extension host trust store only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_plugin_extension_host_trust_store_add(UmiPluginExtensionHostTrustStore *catalogue, const char *id, uint32_t state, uint32_t priority, uint64_t evidence)
{
    size_t i; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (catalogue == NULL || !umi_plugin_extension_host_text_valid(id,UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i=0U;i<catalogue->count;++i) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(catalogue->items[i].id,id)==0) return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (catalogue->count >= UMI_PLUGIN_EXTENSION_HOST_MEDIUM_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    i=catalogue->count++; umi_plugin_extension_host_trust_store_copy(catalogue->items[i].id,sizeof(catalogue->items[i].id),id); catalogue->items[i].state=state; catalogue->items[i].priority=priority; catalogue->items[i].evidence=evidence; ++catalogue->revision; return UMI_STATUS_OK;
}
/*
 * Remove plugin extension host trust store while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_plugin_extension_host_trust_store_remove(UmiPluginExtensionHostTrustStore *catalogue, const char *id)
{
    size_t i,j; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (catalogue==NULL||id==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for(i=0U;i<catalogue->count;++i) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(catalogue->items[i].id,id)==0) { /* Visit each bounded item once so every record receives the same rule. */ for(j=i+1U;j<catalogue->count;++j) catalogue->items[j-1U]=catalogue->items[j]; --catalogue->count; ++catalogue->revision; return UMI_STATUS_OK; }
    return UMI_STATUS_NOT_FOUND;
}
/*
 * Return the number of records represented by plugin extension host trust store without
 * changing their state.
 */
size_t umi_plugin_extension_host_trust_store_count(const UmiPluginExtensionHostTrustStore *catalogue) { return catalogue==NULL?0U:catalogue->count; }
