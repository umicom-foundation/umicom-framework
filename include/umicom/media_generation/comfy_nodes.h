/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/media_generation/comfy_nodes.h
 *
 * PURPOSE:
 *   Expose bounded, connection-bound ComfyUI node metadata and borrowed schema spans without executing custom nodes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_MEDIA_GENERATION_COMFY_NODES_H
#define UMICOM_MEDIA_GENERATION_COMFY_NODES_H
#include "umicom/provider_connections/connections.h"
#include "umicom/platform/cancellation.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_COMFY_NODES_MAX_BYTES (4U * 1024U * 1024U)
#define UMI_COMFY_NODES_LIMIT 4096U
#define UMI_COMFY_NODE_INPUT_LIMIT 256U
#define UMI_COMFY_NODE_OUTPUT_LIMIT 64U
#define UMI_COMFY_NODE_OUTPUT_NODE 1U
#define UMI_COMFY_NODE_DEPRECATED 2U
#define UMI_COMFY_NODE_EXPERIMENTAL 4U
#define UMI_COMFY_NODE_API_NODE 8U
typedef struct UmiComfyNodeInfo {
    char class_type[129], display_name[257], category[257];
    unsigned known_flags, enabled_flags;
    size_t required_count, optional_count, output_count;
} UmiComfyNodeInfo;
typedef enum UmiComfyInputGroup {
    UMI_COMFY_INPUT_REQUIRED = 1, UMI_COMFY_INPUT_OPTIONAL = 2
} UmiComfyInputGroup;
typedef struct UmiComfyNodeInput {
    char name[129];
    /* Original validated JSON spelling; not NUL-terminated. Borrowed until
     * catalogue destruction. Descriptor formats are deliberately opaque. */
    const char *schema_json;
    size_t schema_bytes;
} UmiComfyNodeInput;
typedef struct UmiComfyNodeCatalogue UmiComfyNodeCatalogue;
typedef struct UmiComfyNodeCatalogueReview {
    uint64_t connection_revision;
    unsigned char connection_sha256[UMI_SHA256_BYTES];
    /* Exact host-supplied response bytes, including whitespace. This identifies
     * the observation and is not a server signature or freshness guarantee. */
    unsigned char response_sha256[UMI_SHA256_BYTES];
} UmiComfyNodeCatalogueReview;
UmiStatus UmiComfyNodeCatalogueInspect(const UmiComfyNodeCatalogue *catalogue,
    UmiComfyNodeCatalogueReview *out);
/* Decode one /object_info response supplied by the host. Connection must be
 * enabled ComfyUI at an exact http://127.0.0.1:<port> origin; revision nonzero.
 * These values label host-supplied bytes, not authenticated server provenance.
 * Verify the real request/response binding before calling. No I/O occurs.
 *
 * Preserve class/input/output source order. Require each class value to be an
 * object with input object and output array of bounded type strings. Optional
 * name must equal its class key; optional display_name/category are text.
 * Missing flags stay unknown, rather than false. Required/optional input groups
 * are bounded objects with unique names, also unique across the two groups.
 * Their descriptors and other metadata remain opaque validated JSON; hidden
 * inputs, widgets, output shapes and custom-node contracts are not interpreted.
 *
 * Limits: 4 MiB, 65536 JSON nodes, depth 32, 4096 classes, 256 inputs per group,
 * 64 outputs; class/input/type names 1..128 UTF-8 bytes, display/category up to
 * 256. Every JSON object requires unique decoded keys. Empty catalogue allowed.
 * Initialize *out=NULL. Failure leaves it unchanged; cancellation cooperative.
 * Raw response memory is owned and cleared on destroy. No Python import, model
 * installation, server capability validation or generation readiness is claimed. */
UmiStatus UmiComfyNodeCatalogueCreate(const UmiProviderConnection *connection,
    uint64_t connection_revision, const void *bytes, size_t length,
    const UmiCancellationToken *cancel, UmiComfyNodeCatalogue **out);
void UmiComfyNodeCatalogueDestroy(UmiComfyNodeCatalogue *catalogue);
size_t UmiComfyNodeCatalogueCount(const UmiComfyNodeCatalogue *catalogue);
/* Serialize reads/destruction. Copy outputs are caller-owned; all outputs are
 * unchanged on refusal. Returned spans are immutable untrusted data, not markup
 * to render or code to execute. Inputs/outputs must not overlap owned storage. */
UmiStatus UmiComfyNodeCatalogueAt(const UmiComfyNodeCatalogue *catalogue, size_t index,
    UmiComfyNodeInfo *out);
UmiStatus UmiComfyNodeCatalogueFind(const UmiComfyNodeCatalogue *catalogue, const char *class_type,
    size_t *out_index);
UmiStatus UmiComfyNodeCatalogueInputAt(const UmiComfyNodeCatalogue *catalogue, size_t node_index,
    UmiComfyInputGroup group, size_t input_index, UmiComfyNodeInput *out);
UmiStatus UmiComfyNodeCatalogueOutputAt(const UmiComfyNodeCatalogue *catalogue, size_t node_index,
    size_t output_index, char *out_type, size_t capacity);
UmiStatus UmiComfyNodeCatalogueSchema(const UmiComfyNodeCatalogue *catalogue, size_t node_index,
    const char **out_json, size_t *out_length);
/* Compare canonical saved settings and revision. BUSY means a different snapshot;
 * it is not permission to retarget a reviewed workflow. A matching connection
 * does not prove freshness after a server restart or custom-node change. */
UmiStatus UmiComfyNodeCatalogueCheckConnection(const UmiComfyNodeCatalogue *catalogue,
    const UmiProviderConnection *current, uint64_t current_revision);
#ifdef __cplusplus
}
#endif
#endif
