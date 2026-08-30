/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/navigation_provider.h
 *
 * PURPOSE:
 *   Define the stable, provider-neutral boundary used by native parsers,
 *   compiler indexes, Language Server Protocol clients, remote development
 *   hosts and governed AI tools to contribute navigation information.
 *
 * OWNERSHIP AND THREADING:
 *   Registries copy descriptors and function tables by value.  Provider
 *   instances remain owned by the registering module.  Requests, results and
 *   sink callbacks are valid only for the duration of a provider call unless a
 *   provider explicitly documents a stronger lifetime.  Providers may stream
 *   results synchronously through the supplied sink and must observe the
 *   cancellation contract when they advertise cancellable operation.
 *
 * ARCHITECTURE:
 *   This contract contains no GTK, Qt, Wt, LSP transport or vendor-specific
 *   type.  Umicom Studio and every future application consume the same stable
 *   C ABI and contribute only product placement and adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_NAVIGATION_PROVIDER_H
#define UMICOM_EDITOR_NAVIGATION_PROVIDER_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/editor/navigation_result.h"
#include "umicom/editor/symbol_index.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_NAVIGATION_PROVIDER_API_VERSION 1U
#define UMI_EDITOR_NAVIGATION_PROVIDER_ID_CAPACITY 128U
#define UMI_EDITOR_NAVIGATION_PROVIDER_LABEL_CAPACITY 256U
#define UMI_EDITOR_NAVIGATION_PROVIDER_LANGUAGE_CAPACITY 256U
#define UMI_EDITOR_NAVIGATION_REQUEST_TEXT_CAPACITY 512U
#define UMI_EDITOR_NAVIGATION_WORKSPACE_ID_CAPACITY 128U
#define UMI_EDITOR_NAVIGATION_DOCUMENT_ID_CAPACITY 128U
#define UMI_EDITOR_NAVIGATION_LANGUAGE_ID_CAPACITY 64U
#define UMI_EDITOR_NAVIGATION_HIERARCHY_NODE_ID_CAPACITY 256U
#define UMI_EDITOR_NAVIGATION_HIERARCHY_DETAIL_CAPACITY 512U
#define UMI_EDITOR_NAVIGATION_PREVIEW_CONTENT_CAPACITY 8192U
#define UMI_EDITOR_NAVIGATION_REPORT_MESSAGE_CAPACITY 512U
#define UMI_EDITOR_NAVIGATION_PROVIDER_DEFAULT_CAPACITY 16U
#define UMI_EDITOR_NAVIGATION_PROVIDER_MAXIMUM_CAPACITY 256U
#define UMI_EDITOR_NAVIGATION_DEFAULT_MAXIMUM_RESULTS 256U
#define UMI_EDITOR_NAVIGATION_MAXIMUM_RESULTS 8192U
#define UMI_EDITOR_NAVIGATION_DEFAULT_MAXIMUM_HIERARCHY_NODES 512U
#define UMI_EDITOR_NAVIGATION_MAXIMUM_HIERARCHY_NODES 8192U

typedef uint32_t UmiEditorNavigationProviderCapabilities;
enum {
    UMI_EDITOR_NAVIGATION_CAPABILITY_DEFINITION = 1U << 0,
    UMI_EDITOR_NAVIGATION_CAPABILITY_DECLARATION = 1U << 1,
    UMI_EDITOR_NAVIGATION_CAPABILITY_TYPE_DEFINITION = 1U << 2,
    UMI_EDITOR_NAVIGATION_CAPABILITY_IMPLEMENTATION = 1U << 3,
    UMI_EDITOR_NAVIGATION_CAPABILITY_REFERENCES = 1U << 4,
    UMI_EDITOR_NAVIGATION_CAPABILITY_DOCUMENT_SYMBOLS = 1U << 5,
    UMI_EDITOR_NAVIGATION_CAPABILITY_WORKSPACE_SYMBOLS = 1U << 6,
    UMI_EDITOR_NAVIGATION_CAPABILITY_CALL_HIERARCHY = 1U << 7,
    UMI_EDITOR_NAVIGATION_CAPABILITY_TYPE_HIERARCHY = 1U << 8,
    UMI_EDITOR_NAVIGATION_CAPABILITY_SOURCE_PREVIEW = 1U << 9,
    UMI_EDITOR_NAVIGATION_CAPABILITY_ALL = (1U << 10) - 1U
};

typedef uint32_t UmiEditorNavigationProviderFlags;
enum {
    UMI_EDITOR_NAVIGATION_PROVIDER_ENABLED = 1U << 0,
    UMI_EDITOR_NAVIGATION_PROVIDER_REMOTE = 1U << 1,
    UMI_EDITOR_NAVIGATION_PROVIDER_REQUIRES_TRUST = 1U << 2,
    UMI_EDITOR_NAVIGATION_PROVIDER_STREAMING = 1U << 3,
    UMI_EDITOR_NAVIGATION_PROVIDER_CANCELLABLE = 1U << 4,
    UMI_EDITOR_NAVIGATION_PROVIDER_PARTIAL_RESULTS = 1U << 5,
    UMI_EDITOR_NAVIGATION_PROVIDER_ALL_FLAGS = (1U << 6) - 1U
};

typedef uint32_t UmiEditorNavigationRequestFlags;
enum {
    UMI_EDITOR_NAVIGATION_REQUEST_INCLUDE_DECLARATIONS = 1U << 0,
    UMI_EDITOR_NAVIGATION_REQUEST_INCLUDE_EXTERNAL = 1U << 1,
    UMI_EDITOR_NAVIGATION_REQUEST_INCLUDE_GENERATED = 1U << 2,
    UMI_EDITOR_NAVIGATION_REQUEST_ALLOW_STALE = 1U << 3,
    UMI_EDITOR_NAVIGATION_REQUEST_PREFER_LOCAL = 1U << 4,
    UMI_EDITOR_NAVIGATION_REQUEST_EXACT_SYMBOL = 1U << 5,
    UMI_EDITOR_NAVIGATION_REQUEST_ALL_FLAGS = (1U << 6) - 1U
};

typedef enum UmiEditorNavigationHierarchyKind {
    UMI_EDITOR_NAVIGATION_HIERARCHY_CALL_INCOMING = 1,
    UMI_EDITOR_NAVIGATION_HIERARCHY_CALL_OUTGOING = 2,
    UMI_EDITOR_NAVIGATION_HIERARCHY_TYPE_SUPERTYPES = 3,
    UMI_EDITOR_NAVIGATION_HIERARCHY_TYPE_SUBTYPES = 4
} UmiEditorNavigationHierarchyKind;

typedef struct UmiEditorNavigationCancellation {
    uint32_t struct_size;
    uint32_t api_version;
    int (*is_cancelled)(void *user_data);
    void *user_data;
} UmiEditorNavigationCancellation;

typedef struct UmiEditorNavigationRequest {
    uint32_t struct_size;
    uint32_t api_version;
    uint64_t request_id;
    UmiEditorNavigationQueryKind query_kind;
    UmiEditorSourceLocation origin;
    uint64_t document_revision;
    char workspace_id[UMI_EDITOR_NAVIGATION_WORKSPACE_ID_CAPACITY];
    char document_id[UMI_EDITOR_NAVIGATION_DOCUMENT_ID_CAPACITY];
    char language_id[UMI_EDITOR_NAVIGATION_LANGUAGE_ID_CAPACITY];
    char symbol_id[UMI_EDITOR_SYMBOL_ID_CAPACITY];
    char text[UMI_EDITOR_NAVIGATION_REQUEST_TEXT_CAPACITY];
    size_t maximum_results;
    UmiEditorNavigationRequestFlags flags;
} UmiEditorNavigationRequest;

typedef struct UmiEditorNavigationHierarchyRequest {
    uint32_t struct_size;
    uint32_t api_version;
    uint64_t request_id;
    UmiEditorNavigationHierarchyKind kind;
    char root_symbol_id[UMI_EDITOR_SYMBOL_ID_CAPACITY];
    char expand_node_id[UMI_EDITOR_NAVIGATION_HIERARCHY_NODE_ID_CAPACITY];
    char language_id[UMI_EDITOR_NAVIGATION_LANGUAGE_ID_CAPACITY];
    size_t maximum_depth;
    size_t maximum_nodes;
    int include_indirect;
} UmiEditorNavigationHierarchyRequest;

typedef struct UmiEditorNavigationHierarchyNode {
    uint32_t struct_size;
    uint32_t api_version;
    char node_id[UMI_EDITOR_NAVIGATION_HIERARCHY_NODE_ID_CAPACITY];
    char parent_node_id[UMI_EDITOR_NAVIGATION_HIERARCHY_NODE_ID_CAPACITY];
    char provider_id[UMI_EDITOR_NAVIGATION_PROVIDER_ID_CAPACITY];
    char symbol_id[UMI_EDITOR_SYMBOL_ID_CAPACITY];
    char label[UMI_EDITOR_SYMBOL_NAME_CAPACITY];
    char detail[UMI_EDITOR_NAVIGATION_HIERARCHY_DETAIL_CAPACITY];
    UmiEditorSourceLocation location;
    size_t depth;
    size_t child_count;
    int has_children;
    int expanded;
    int cycle;
} UmiEditorNavigationHierarchyNode;

typedef struct UmiEditorNavigationPreviewRequest {
    uint32_t struct_size;
    uint32_t api_version;
    uint64_t request_id;
    char uri[UMI_EDITOR_SOURCE_URI_CAPACITY];
    char language_id[UMI_EDITOR_NAVIGATION_LANGUAGE_ID_CAPACITY];
    uint64_t document_revision;
    uint64_t start_line;
    uint64_t end_line;
    size_t maximum_bytes;
} UmiEditorNavigationPreviewRequest;

typedef struct UmiEditorNavigationSourcePreview {
    uint32_t struct_size;
    uint32_t api_version;
    char provider_id[UMI_EDITOR_NAVIGATION_PROVIDER_ID_CAPACITY];
    char uri[UMI_EDITOR_SOURCE_URI_CAPACITY];
    char language_id[UMI_EDITOR_NAVIGATION_LANGUAGE_ID_CAPACITY];
    uint64_t document_revision;
    uint64_t start_line;
    uint64_t end_line;
    uint64_t sequence;
    size_t content_length;
    int complete;
    int stale;
    char content[UMI_EDITOR_NAVIGATION_PREVIEW_CONTENT_CAPACITY];
} UmiEditorNavigationSourcePreview;

typedef struct UmiEditorNavigationProviderReport {
    uint32_t struct_size;
    uint32_t api_version;
    char provider_id[UMI_EDITOR_NAVIGATION_PROVIDER_ID_CAPACITY];
    UmiStatus status;
    size_t emitted_count;
    uint64_t sequence;
    int cancelled;
    int truncated;
    char message[UMI_EDITOR_NAVIGATION_REPORT_MESSAGE_CAPACITY];
} UmiEditorNavigationProviderReport;

typedef UmiStatus (*UmiEditorNavigationResultSink)(
    const UmiEditorNavigationResult *result,
    void *user_data);
typedef UmiStatus (*UmiEditorNavigationSymbolSink)(
    const UmiEditorIndexedSymbol *symbol,
    void *user_data);
typedef UmiStatus (*UmiEditorNavigationHierarchySink)(
    const UmiEditorNavigationHierarchyNode *node,
    void *user_data);

typedef struct UmiEditorNavigationProviderFunctions {
    uint32_t struct_size;
    uint32_t api_version;
    UmiStatus (*query)(
        void *instance,
        const UmiEditorNavigationRequest *request,
        const UmiEditorNavigationCancellation *cancellation,
        UmiEditorNavigationResultSink sink,
        void *sink_user_data,
        UmiEditorNavigationProviderReport *out_report);
    UmiStatus (*symbols)(
        void *instance,
        const UmiEditorNavigationRequest *request,
        const UmiEditorNavigationCancellation *cancellation,
        UmiEditorNavigationSymbolSink sink,
        void *sink_user_data,
        UmiEditorNavigationProviderReport *out_report);
    UmiStatus (*hierarchy)(
        void *instance,
        const UmiEditorNavigationHierarchyRequest *request,
        const UmiEditorNavigationCancellation *cancellation,
        UmiEditorNavigationHierarchySink sink,
        void *sink_user_data,
        UmiEditorNavigationProviderReport *out_report);
    UmiStatus (*preview)(
        void *instance,
        const UmiEditorNavigationPreviewRequest *request,
        const UmiEditorNavigationCancellation *cancellation,
        UmiEditorNavigationSourcePreview *out_preview);
    UmiStatus (*cancel)(void *instance, uint64_t request_id);
} UmiEditorNavigationProviderFunctions;

typedef struct UmiEditorNavigationProviderDescriptor {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_EDITOR_NAVIGATION_PROVIDER_ID_CAPACITY];
    char label[UMI_EDITOR_NAVIGATION_PROVIDER_LABEL_CAPACITY];
    char language_selector[UMI_EDITOR_NAVIGATION_PROVIDER_LANGUAGE_CAPACITY];
    int32_t priority;
    UmiEditorNavigationProviderCapabilities capabilities;
    UmiEditorNavigationProviderFlags flags;
} UmiEditorNavigationProviderDescriptor;

typedef struct UmiEditorNavigationProviderRegistration {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorNavigationProviderDescriptor descriptor;
    UmiEditorNavigationProviderFunctions functions;
    void *instance;
} UmiEditorNavigationProviderRegistration;

typedef struct UmiEditorNavigationProviderRegistrySnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t provider_count;
    size_t enabled_count;
    size_t remote_count;
    size_t cancellable_count;
    uint64_t revision;
} UmiEditorNavigationProviderRegistrySnapshot;

typedef struct UmiEditorNavigationProviderRegistry
    UmiEditorNavigationProviderRegistry;

UmiStatus umi_editor_navigation_provider_descriptor_initialize(
    UmiEditorNavigationProviderDescriptor *descriptor,
    const char *provider_id,
    const char *label,
    const char *language_selector,
    UmiEditorNavigationProviderCapabilities capabilities,
    int32_t priority);
UmiStatus umi_editor_navigation_request_initialize(
    UmiEditorNavigationRequest *request,
    uint64_t request_id,
    UmiEditorNavigationQueryKind query_kind,
    const UmiEditorSourceLocation *origin,
    const char *language_id);
UmiStatus umi_editor_navigation_hierarchy_request_initialize(
    UmiEditorNavigationHierarchyRequest *request,
    uint64_t request_id,
    UmiEditorNavigationHierarchyKind kind,
    const char *root_symbol_id,
    const char *language_id);
UmiStatus umi_editor_navigation_preview_request_initialize(
    UmiEditorNavigationPreviewRequest *request,
    uint64_t request_id,
    const char *uri,
    const char *language_id,
    uint64_t document_revision,
    uint64_t start_line,
    uint64_t end_line);

UmiStatus umi_editor_navigation_provider_registry_create(
    UmiEditorNavigationProviderRegistry **out_registry);
void umi_editor_navigation_provider_registry_destroy(
    UmiEditorNavigationProviderRegistry *registry);
UmiStatus umi_editor_navigation_provider_registry_clear(
    UmiEditorNavigationProviderRegistry *registry);
UmiStatus umi_editor_navigation_provider_registry_upsert(
    UmiEditorNavigationProviderRegistry *registry,
    const UmiEditorNavigationProviderRegistration *registration);
UmiStatus umi_editor_navigation_provider_registry_remove(
    UmiEditorNavigationProviderRegistry *registry,
    const char *provider_id);
UmiStatus umi_editor_navigation_provider_registry_find(
    const UmiEditorNavigationProviderRegistry *registry,
    const char *provider_id,
    UmiEditorNavigationProviderRegistration *out_registration);
UmiStatus umi_editor_navigation_provider_registry_at(
    const UmiEditorNavigationProviderRegistry *registry,
    size_t position,
    UmiEditorNavigationProviderRegistration *out_registration);
UmiStatus umi_editor_navigation_provider_registry_snapshot(
    const UmiEditorNavigationProviderRegistry *registry,
    UmiEditorNavigationProviderRegistrySnapshot *out_snapshot);
size_t umi_editor_navigation_provider_registry_count(
    const UmiEditorNavigationProviderRegistry *registry);
uint64_t umi_editor_navigation_provider_registry_revision(
    const UmiEditorNavigationProviderRegistry *registry);

UmiEditorNavigationProviderCapabilities
umi_editor_navigation_capability_for_query_kind(
    UmiEditorNavigationQueryKind query_kind);
int umi_editor_navigation_provider_language_matches(
    const UmiEditorNavigationProviderDescriptor *descriptor,
    const char *language_id);
int umi_editor_navigation_provider_supports_request(
    const UmiEditorNavigationProviderRegistration *registration,
    const UmiEditorNavigationRequest *request);
int umi_editor_navigation_provider_supports_hierarchy(
    const UmiEditorNavigationProviderRegistration *registration,
    const UmiEditorNavigationHierarchyRequest *request);
int umi_editor_navigation_provider_supports_preview(
    const UmiEditorNavigationProviderRegistration *registration,
    const UmiEditorNavigationPreviewRequest *request);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_NAVIGATION_PROVIDER_H */
