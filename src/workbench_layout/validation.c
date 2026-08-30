/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout/validation.c
 *
 * PURPOSE:
 *   Implement comprehensive semantic layout validation and actionable diagnostics for persistence, import, recovery and quality gates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_layout/validation.h"
#include "umicom/workbench_layout/migration.h"

#include <stdio.h>
#include <string.h>

#include "internal.h"

static bool severity_is_error(
    UmiWorkbenchLayoutSeverity severity,
    const UmiWorkbenchLayoutValidationOptions *options)
{
    return severity == UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_ERROR ||
           (severity == UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_WARNING &&
            options != NULL && options->warnings_as_errors);
}

static void finalise_report(
    UmiWorkbenchLayoutValidationReport *report,
    const UmiWorkbenchLayoutValidationOptions *options)
{
    size_t index;
    bool valid = true;

    if (report == NULL) {
        return;
    }
    for (index = 0U; index < report->diagnostic_count; ++index) {
        if (severity_is_error(
                report->diagnostics[index].severity, options)) {
            valid = false;
            break;
        }
    }
    report->valid = valid;
}

static UmiStatus make_node_path(
    const UmiWorkbenchLayoutDocument *document,
    size_t node_index,
    char *buffer,
    size_t capacity)
{
    size_t ancestors[UMI_WORKBENCH_LAYOUT_MAX_NODES];
    size_t count = 0U;
    size_t current = node_index;
    size_t index;
    size_t used = 0U;

    if (document == NULL || buffer == NULL || capacity == 0U ||
        !umi_workbench_layout_index_valid(
            node_index, document->node_count)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    while (current != UMI_WORKBENCH_LAYOUT_INDEX_NONE &&
           count < UMI_WORKBENCH_LAYOUT_MAX_NODES) {
        ancestors[count++] = current;
        if (!umi_workbench_layout_index_valid(
                current, document->node_count)) {
            break;
        }
        current = document->nodes[current].parent_index;
    }

    buffer[0] = '\0';
    for (index = count; index > 0U; --index) {
        const char *node_id =
            document->nodes[ancestors[index - 1U]].node_id;
        int written = snprintf(
            buffer + used,
            capacity - used,
            "%s%s",
            used > 0U ? "/" : "",
            node_id);
        if (written < 0) {
            return UMI_STATUS_INTERNAL_ERROR;
        }
        if ((size_t)written >= capacity - used) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        used += (size_t)written;
    }
    return UMI_STATUS_OK;
}

static UmiStatus add_node_diagnostic(
    const UmiWorkbenchLayoutDocument *document,
    size_t node_index,
    UmiWorkbenchLayoutValidationReport *report,
    UmiWorkbenchLayoutSeverity severity,
    UmiWorkbenchLayoutDiagnosticCode code,
    const char *message,
    const char *remediation)
{
    char path[UMI_WORKBENCH_LAYOUT_PATH_CAPACITY];
    const char *node_id = "";

    path[0] = '\0';
    if (document != NULL &&
        umi_workbench_layout_index_valid(
            node_index, document->node_count)) {
        node_id = document->nodes[node_index].node_id;
        (void)make_node_path(document, node_index, path, sizeof(path));
    }

    return umi_workbench_layout_validation_report_add(
        report,
        severity,
        code,
        node_id,
        path,
        message,
        remediation);
}

static bool detect_cycle_from(
    const UmiWorkbenchLayoutDocument *document,
    size_t node_index,
    unsigned char *state)
{
    const UmiWorkbenchLayoutNode *node;
    size_t child;

    if (state[node_index] == 1U) {
        return true;
    }
    if (state[node_index] == 2U) {
        return false;
    }

    state[node_index] = 1U;
    node = &document->nodes[node_index];
    for (child = 0U; child < node->child_count; ++child) {
        size_t child_index = node->child_indices[child];
        if (umi_workbench_layout_index_valid(
                child_index, document->node_count) &&
            detect_cycle_from(document, child_index, state)) {
            return true;
        }
    }
    state[node_index] = 2U;
    return false;
}

static void mark_reachable(
    const UmiWorkbenchLayoutDocument *document,
    size_t node_index,
    bool *reachable)
{
    const UmiWorkbenchLayoutNode *node;
    size_t child;

    if (!umi_workbench_layout_index_valid(
            node_index, document->node_count) ||
        reachable[node_index]) {
        return;
    }

    reachable[node_index] = true;
    node = &document->nodes[node_index];
    for (child = 0U; child < node->child_count; ++child) {
        mark_reachable(document, node->child_indices[child], reachable);
    }
}

static void validate_document_metadata(
    const UmiWorkbenchLayoutDocument *document,
    UmiWorkbenchLayoutValidationReport *report)
{
    size_t index;

    if (!umi_workbench_layout_text_present(
            document->identity.layout_id)) {
        (void)umi_workbench_layout_validation_report_add(
            report,
            UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_ERROR,
            UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_MISSING_LAYOUT_ID,
            "",
            "",
            "The layout document has no stable layout identifier.",
            "Assign a non-empty layout_id before saving or sharing.");
    }
    if (!umi_workbench_layout_text_present(document->name)) {
        (void)umi_workbench_layout_validation_report_add(
            report,
            UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_ERROR,
            UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_MISSING_LAYOUT_NAME,
            "",
            "",
            "The layout document has no display name.",
            "Assign a user-facing name before saving the layout.");
    }
    if (!umi_workbench_layout_schema_is_supported(
            document->version.schema_version)) {
        (void)umi_workbench_layout_validation_report_add(
            report,
            UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_ERROR,
            UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_UNSUPPORTED_SCHEMA,
            "",
            "",
            "The layout schema version is not supported by this Framework.",
            "Migrate the document through the layout migration service.");
    }
    if (document->node_count == 0U ||
        document->root_index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        (void)umi_workbench_layout_validation_report_add(
            report,
            UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_ERROR,
            UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_MISSING_ROOT,
            "",
            "",
            "The layout document does not contain a root node.",
            "Create a root window or empty node before activation.");
    } else if (!umi_workbench_layout_index_valid(
                   document->root_index, document->node_count)) {
        (void)umi_workbench_layout_validation_report_add(
            report,
            UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_ERROR,
            UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_INVALID_ROOT,
            "",
            "",
            "The root index points outside the document node array.",
            "Repair the root index or rebuild the document from a template.");
    }
    if (umi_workbench_layout_document_has_flag(
            document, UMI_WORKBENCH_LAYOUT_DOCUMENT_LOCKED) &&
        umi_workbench_layout_document_has_flag(
            document, UMI_WORKBENCH_LAYOUT_DOCUMENT_DIRTY)) {
        (void)umi_workbench_layout_validation_report_add(
            report,
            UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_WARNING,
            UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_LOCKED_DIRTY_LAYOUT,
            "",
            "",
            "The layout is locked but still marked dirty.",
            "Save the final locked revision or clear the dirty state.");
    }

    for (index = 0U; index < document->tag_count; ++index) {
        if (!umi_workbench_layout_text_present(
                document->tags[index].value)) {
            (void)umi_workbench_layout_validation_report_add(
                report,
                UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_WARNING,
                UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_INVALID_TAG,
                "",
                "",
                "The layout contains an empty tag.",
                "Remove the tag or replace it with a meaningful value.");
        }
    }
}

static void validate_duplicate_node_ids(
    const UmiWorkbenchLayoutDocument *document,
    UmiWorkbenchLayoutValidationReport *report)
{
    size_t index;
    for (index = 0U; index < document->node_count; ++index) {
        size_t other;
        for (other = index + 1U; other < document->node_count; ++other) {
            if (umi_workbench_layout_text_equal(
                    document->nodes[index].node_id,
                    document->nodes[other].node_id)) {
                (void)add_node_diagnostic(
                    document,
                    other,
                    report,
                    UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_ERROR,
                    UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_DUPLICATE_NODE_ID,
                    "Two layout nodes use the same stable identifier.",
                    "Assign a unique node_id to every semantic node.");
            }
        }
    }
}

static void validate_node_structure(
    const UmiWorkbenchLayoutDocument *document,
    size_t node_index,
    const UmiWorkbenchLayoutValidationOptions *options,
    UmiWorkbenchLayoutValidationReport *report)
{
    const UmiWorkbenchLayoutNode *node = &document->nodes[node_index];
    size_t child;
    UmiStatus status = umi_workbench_layout_node_validate(node);

    if (status != UMI_STATUS_OK) {
        (void)add_node_diagnostic(
            document,
            node_index,
            report,
            UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_ERROR,
            UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_INVALID_CHILD_KIND,
            "The node violates one or more public node invariants.",
            "Inspect node kind, children, split ratio, geometry and component "
            "ownership.");
    }

    if (node_index == document->root_index &&
        node->parent_index != UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        (void)add_node_diagnostic(
            document,
            node_index,
            report,
            UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_ERROR,
            UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_INVALID_ROOT,
            "The root node has a parent.",
            "Detach the root from its parent before activation.");
    }
    if (node_index != document->root_index &&
        node->parent_index == UMI_WORKBENCH_LAYOUT_INDEX_NONE &&
        options->require_connected_tree) {
        (void)add_node_diagnostic(
            document,
            node_index,
            report,
            UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_ERROR,
            UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_INVALID_PARENT,
            "A non-root node has no parent.",
            "Attach the node to a container or remove it from the document.");
    }
    if (node->parent_index != UMI_WORKBENCH_LAYOUT_INDEX_NONE &&
        !umi_workbench_layout_index_valid(
            node->parent_index, document->node_count)) {
        (void)add_node_diagnostic(
            document,
            node_index,
            report,
            UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_ERROR,
            UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_INVALID_PARENT,
            "The node parent index is outside the document.",
            "Repair the parent relation before loading the layout.");
    }

    if (umi_workbench_layout_node_is_container(node) &&
        node->child_count == 0U &&
        options->require_non_empty_containers) {
        (void)add_node_diagnostic(
            document,
            node_index,
            report,
            UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_WARNING,
            UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_EMPTY_CONTAINER,
            "The container has no semantic children.",
            "Add a panel, editor group or explicit empty placeholder.");
    }

    if (node->kind == UMI_WORKBENCH_LAYOUT_NODE_SPLIT &&
        (node->split_ratio < 0.05 || node->split_ratio > 0.95)) {
        (void)add_node_diagnostic(
            document,
            node_index,
            report,
            UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_ERROR,
            UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_INVALID_SPLIT_RATIO,
            "The split ratio is outside the supported range.",
            "Use a split ratio between 0.05 and 0.95.");
    }

    if (options->require_components_for_leaf_nodes &&
        (node->kind == UMI_WORKBENCH_LAYOUT_NODE_PANEL ||
         node->kind == UMI_WORKBENCH_LAYOUT_NODE_EDITOR_GROUP) &&
        !umi_workbench_layout_text_present(node->component_id)) {
        (void)add_node_diagnostic(
            document,
            node_index,
            report,
            UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_ERROR,
            UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_MISSING_COMPONENT,
            "The leaf node does not identify a Framework component.",
            "Assign a stable component_id provided by a registered panel.");
    }

    if (options->require_owner_for_components &&
        umi_workbench_layout_text_present(node->component_id) &&
        !umi_workbench_layout_text_present(
            node->owner_application_id)) {
        (void)add_node_diagnostic(
            document,
            node_index,
            report,
            UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_ERROR,
            UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_MISSING_OWNER,
            "The component node does not identify its owning application.",
            "Assign owner_application_id from the application manifest.");
    }

    if (options->validate_geometry &&
        !umi_workbench_layout_rect_is_valid(&node->bounds)) {
        (void)add_node_diagnostic(
            document,
            node_index,
            report,
            UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_ERROR,
            UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_INVALID_BOUNDS,
            "The node contains negative width or height.",
            "Normalise geometry before saving the layout.");
    }

    if (node->active_child_index != UMI_WORKBENCH_LAYOUT_INDEX_NONE &&
        node->active_child_index >= node->child_count) {
        (void)add_node_diagnostic(
            document,
            node_index,
            report,
            UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_ERROR,
            UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_INVALID_ACTIVE_CHILD,
            "The active child position is outside the child list.",
            "Select an existing child or clear the active child.");
    }

    for (child = 0U; child < node->child_count; ++child) {
        const size_t child_index = node->child_indices[child];
        if (!umi_workbench_layout_index_valid(
                child_index, document->node_count)) {
            (void)add_node_diagnostic(
                document,
                node_index,
                report,
                UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_ERROR,
                UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_INVALID_PARENT,
                "The child index points outside the document.",
                "Remove the invalid child reference or restore the node.");
            continue;
        }
        if (document->nodes[child_index].parent_index != node_index) {
            (void)add_node_diagnostic(
                document,
                child_index,
                report,
                UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_ERROR,
                UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_CHILD_NOT_RECIPROCAL,
                "The parent and child relations disagree.",
                "Repair both sides of the topology in one transaction.");
        }
        if (!umi_workbench_layout_node_accepts_child_kind(
                node, document->nodes[child_index].kind)) {
            (void)add_node_diagnostic(
                document,
                child_index,
                report,
                UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_ERROR,
                UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_INVALID_CHILD_KIND,
                "The container does not accept this child node kind.",
                "Move the node to a compatible container.");
        }
    }
}

static void validate_cycles_and_reachability(
    const UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchLayoutValidationOptions *options,
    UmiWorkbenchLayoutValidationReport *report)
{
    unsigned char state[UMI_WORKBENCH_LAYOUT_MAX_NODES];
    bool reachable[UMI_WORKBENCH_LAYOUT_MAX_NODES];
    size_t index;

    (void)memset(state, 0, sizeof(state));
    (void)memset(reachable, 0, sizeof(reachable));

    for (index = 0U; index < document->node_count; ++index) {
        if (state[index] == 0U &&
            detect_cycle_from(document, index, state)) {
            (void)add_node_diagnostic(
                document,
                index,
                report,
                UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_ERROR,
                UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_CYCLE,
                "The semantic layout topology contains a cycle.",
                "Detach the node that points back to one of its ancestors.");
            break;
        }
    }

    if (umi_workbench_layout_index_valid(
            document->root_index, document->node_count)) {
        mark_reachable(document, document->root_index, reachable);
    }

    if (options->require_connected_tree) {
        for (index = 0U; index < document->node_count; ++index) {
            if (!reachable[index]) {
                (void)add_node_diagnostic(
                    document,
                    index,
                    report,
                    UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_ERROR,
                    UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_UNREACHABLE_NODE,
                    "The node cannot be reached from the layout root.",
                    "Attach the node to the root tree or remove it.");
            }
        }
    }
}

static void validate_hash(
    const UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchLayoutValidationOptions *options,
    UmiWorkbenchLayoutValidationReport *report)
{
    uint64_t calculated;
    if (!options->validate_hash || document->content_hash == 0U) {
        return;
    }
    calculated =
        umi_workbench_layout_document_calculate_hash(document);
    if (calculated != document->content_hash) {
        (void)umi_workbench_layout_validation_report_add(
            report,
            UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_ERROR,
            UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_HASH_MISMATCH,
            "",
            "",
            "The persisted layout hash does not match its semantic content.",
            "Reject the document or rebuild it from an audited revision.");
    }
}

UmiWorkbenchLayoutValidationOptions
umi_workbench_layout_validation_options_default(void)
{
    UmiWorkbenchLayoutValidationOptions options;
    (void)memset(&options, 0, sizeof(options));
    options.structure_size = sizeof(options);
    options.require_connected_tree = true;
    options.require_non_empty_containers = false;
    options.require_components_for_leaf_nodes = true;
    options.require_owner_for_components = true;
    options.validate_geometry = true;
    options.validate_hash = true;
    options.warnings_as_errors = false;
    return options;
}

void umi_workbench_layout_validation_report_init(
    UmiWorkbenchLayoutValidationReport *report)
{
    if (report == NULL) {
        return;
    }
    (void)memset(report, 0, sizeof(*report));
    report->structure_size = sizeof(*report);
    report->valid = true;
}

UmiStatus umi_workbench_layout_validate(
    const UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchLayoutValidationOptions *options,
    UmiWorkbenchLayoutValidationReport *out_report)
{
    UmiWorkbenchLayoutValidationOptions effective;
    size_t index;

    if (document == NULL || out_report == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    effective = options != NULL
        ? *options
        : umi_workbench_layout_validation_options_default();
    if (effective.structure_size < sizeof(effective)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_workbench_layout_validation_report_init(out_report);
    validate_document_metadata(document, out_report);
    validate_duplicate_node_ids(document, out_report);

    for (index = 0U; index < document->node_count; ++index) {
        validate_node_structure(
            document, index, &effective, out_report);
    }
    validate_cycles_and_reachability(
        document, &effective, out_report);
    validate_hash(document, &effective, out_report);
    finalise_report(out_report, &effective);

    return out_report->valid
        ? UMI_STATUS_OK
        : UMI_STATUS_INVALID_STATE;
}

UmiStatus umi_workbench_layout_validation_report_add(
    UmiWorkbenchLayoutValidationReport *report,
    UmiWorkbenchLayoutSeverity severity,
    UmiWorkbenchLayoutDiagnosticCode code,
    const char *node_id,
    const char *path,
    const char *message,
    const char *remediation)
{
    UmiWorkbenchLayoutDiagnostic *diagnostic;
    UmiStatus status;

    if (report == NULL || message == NULL || remediation == NULL ||
        severity < UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_INFO ||
        severity > UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_ERROR) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (report->diagnostic_count >=
        UMI_WORKBENCH_LAYOUT_MAX_DIAGNOSTICS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    diagnostic = &report->diagnostics[report->diagnostic_count];
    (void)memset(diagnostic, 0, sizeof(*diagnostic));
    diagnostic->structure_size = sizeof(*diagnostic);
    diagnostic->severity = severity;
    diagnostic->code = code;

    status = umi_workbench_layout_copy_text(
        diagnostic->node_id,
        sizeof(diagnostic->node_id),
        node_id != NULL ? node_id : "",
        true);
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_copy_text(
            diagnostic->path,
            sizeof(diagnostic->path),
            path != NULL ? path : "",
            true);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_copy_text(
            diagnostic->message,
            sizeof(diagnostic->message),
            message,
            false);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_copy_text(
            diagnostic->remediation,
            sizeof(diagnostic->remediation),
            remediation,
            true);
    }
    if (status != UMI_STATUS_OK) {
        return status;
    }

    report->diagnostic_count += 1U;
    switch (severity) {
    case UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_INFO:
        report->info_count += 1U;
        break;
    case UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_WARNING:
        report->warning_count += 1U;
        break;
    case UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_ERROR:
        report->error_count += 1U;
        report->valid = false;
        break;
    default:
        break;
    }
    return UMI_STATUS_OK;
}

const char *umi_workbench_layout_diagnostic_code_text(
    UmiWorkbenchLayoutDiagnosticCode code)
{
    switch (code) {
    case UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_NONE: return "none";
    case UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_MISSING_LAYOUT_ID:
        return "missing-layout-id";
    case UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_MISSING_LAYOUT_NAME:
        return "missing-layout-name";
    case UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_UNSUPPORTED_SCHEMA:
        return "unsupported-schema";
    case UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_MISSING_ROOT:
        return "missing-root";
    case UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_INVALID_ROOT:
        return "invalid-root";
    case UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_DUPLICATE_NODE_ID:
        return "duplicate-node-id";
    case UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_INVALID_PARENT:
        return "invalid-parent";
    case UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_CHILD_NOT_RECIPROCAL:
        return "child-not-reciprocal";
    case UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_CYCLE: return "cycle";
    case UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_UNREACHABLE_NODE:
        return "unreachable-node";
    case UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_INVALID_CHILD_KIND:
        return "invalid-child-kind";
    case UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_TOO_MANY_CHILDREN:
        return "too-many-children";
    case UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_INVALID_SPLIT_RATIO:
        return "invalid-split-ratio";
    case UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_MISSING_COMPONENT:
        return "missing-component";
    case UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_MISSING_OWNER:
        return "missing-owner";
    case UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_INVALID_BOUNDS:
        return "invalid-bounds";
    case UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_INVALID_ACTIVE_CHILD:
        return "invalid-active-child";
    case UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_INVALID_MONITOR:
        return "invalid-monitor";
    case UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_EMPTY_CONTAINER:
        return "empty-container";
    case UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_LOCKED_DIRTY_LAYOUT:
        return "locked-dirty-layout";
    case UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_INVALID_TAG:
        return "invalid-tag";
    case UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_HASH_MISMATCH:
        return "hash-mismatch";
    default: return "invalid";
    }
}

bool umi_workbench_layout_validation_report_has_code(
    const UmiWorkbenchLayoutValidationReport *report,
    UmiWorkbenchLayoutDiagnosticCode code)
{
    size_t index;
    if (report == NULL) {
        return false;
    }
    for (index = 0U; index < report->diagnostic_count; ++index) {
        if (report->diagnostics[index].code == code) {
            return true;
        }
    }
    return false;
}
