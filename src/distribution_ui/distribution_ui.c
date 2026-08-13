/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution_ui/distribution_ui.c
 *
 * PURPOSE:
 *   Build toolkit-neutral Product, Marketplace and Update Centre projections
 *   over the reusable Framework distribution service.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/
#include "umicom/distribution_ui/distribution_ui.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "umicom/ui/command_view.h"

static UmiStatus set_string(UmiUiViewModel *view,
                            const char *key,
                            const char *text)
{
    UmiUiValue value;
    UmiStatus status = umi_ui_value_set_string(&value, text != NULL ? text : "");
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value) : status;
}

static UmiStatus set_integer(UmiUiViewModel *view,
                             const char *key,
                             int64_t number)
{
    UmiUiValue value;
    UmiStatus status = umi_ui_value_set_integer(&value, number);
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value) : status;
}

static UmiStatus set_action(UmiUiViewModel *view,
                            size_t index,
                            const char *action_id,
                            const char *label,
                            const char *tooltip)
{
    UmiUiCommandViewAction action;
    (void)memset(&action, 0, sizeof(action));
    (void)snprintf(action.action_id, sizeof(action.action_id), "%s", action_id);
    (void)snprintf(action.label, sizeof(action.label), "%s", label);
    (void)snprintf(action.tooltip, sizeof(action.tooltip), "%s", tooltip);
    action.enabled = 1;
    return umi_ui_command_view_set_action(view, index, &action);
}

static UmiStatus base_view(const char *view_id,
                           const char *kind,
                           const char *title,
                           const char *summary,
                           UmiUiViewModel **out_view)
{
    UmiStatus status;
    if (out_view == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_view = NULL;
    status = umi_ui_view_model_create(
        view_id, "umicom.distribution-ui", UMI_UI_ROLE_PANE, out_view);
    if (status == UMI_STATUS_OK) status = set_string(*out_view, "umicom.view-kind", kind);
    if (status == UMI_STATUS_OK) status = set_string(*out_view, "title", title);
    if (status == UMI_STATUS_OK) status = set_string(*out_view, "summary", summary);
    if (status != UMI_STATUS_OK && *out_view != NULL) {
        umi_ui_view_model_destroy(*out_view);
        *out_view = NULL;
    }
    return status;
}

UmiStatus umi_distribution_ui_marketplace_view_create(
    const char *view_id,
    UmiDistributionService *service,
    UmiUiViewModel **out_view)
{
    UmiDistributionRepository *repository;
    size_t count;
    size_t index;
    UmiStatus status;
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = base_view(
        view_id, "product-marketplace", "Product Marketplace",
        "Trusted applications, extensions, runtimes and templates from configured repositories.",
        out_view);
    if (status != UMI_STATUS_OK) return status;
    repository = umi_distribution_service_repository(service);
    count = umi_distribution_repository_count(repository);
    if (count > UMI_DISTRIBUTION_UI_VISIBLE_ROWS) {
        count = UMI_DISTRIBUTION_UI_VISIBLE_ROWS;
    }
    status = set_integer(*out_view, "marketplace.row-count", (int64_t)count);
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        UmiDistributionPackage package;
        char key[96];
        char text[768];
        status = umi_distribution_repository_at(repository, index, &package);
        if (status != UMI_STATUS_OK) break;
        (void)snprintf(key, sizeof(key), "marketplace.row.%zu", index);
        (void)snprintf(text, sizeof(text),
                       "%s %u.%u.%u | %s | %s | %s",
                       package.display_name,
                       (unsigned int)package.version.major,
                       (unsigned int)package.version.minor,
                       (unsigned int)package.version.patch,
                       umi_distribution_package_kind_text(package.kind),
                       umi_release_channel_text(package.channel),
                       package.trusted ? "trusted" : "untrusted");
        status = set_string(*out_view, key, text);
    }
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.marketplace.check-updates",
        "Check for Updates", "Evaluate installed products against verified repository releases");
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 1U, "studio.action.pane.products-installed",
        "Installed", "Open installed product state");
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 2U, "studio.action.pane.product-updates",
        "Updates", "Open update notifications");
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 3U, "studio.action.pane.product-evidence",
        "Evidence", "Inspect checksums, signatures, SBOMs, licences and provenance");
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 4U, "studio.action.marketplace.plan-update",
        "Plan Release", "Plan an install, update or rollback by release ID");
    return status;
}

UmiStatus umi_distribution_ui_installed_view_create(
    const char *view_id,
    UmiDistributionService *service,
    UmiUiViewModel **out_view)
{
    UmiProductInstallationRegistry *installations;
    size_t count;
    size_t index;
    UmiStatus status;
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = base_view(
        view_id, "products-installed", "Installed Products",
        "Verified installation generations, update channels and rollback readiness.",
        out_view);
    if (status != UMI_STATUS_OK) return status;
    installations = umi_product_centre_installations(
        umi_distribution_service_products(service));
    count = umi_product_installation_state_registry_count(installations);
    if (count > UMI_DISTRIBUTION_UI_VISIBLE_ROWS) count = UMI_DISTRIBUTION_UI_VISIBLE_ROWS;
    status = set_integer(*out_view, "products-installed.row-count", (int64_t)count);
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        UmiProductInstallationSnapshot installation;
        char key[96];
        char text[768];
        status = umi_product_installation_state_registry_at(
            installations, index, &installation);
        if (status != UMI_STATUS_OK) break;
        (void)snprintf(key, sizeof(key), "products-installed.row.%zu", index);
        (void)snprintf(text, sizeof(text), "%s %s | %s | %s",
                       installation.product_id, installation.version,
                       installation.channel,
                       installation.rollback_available ? "rollback ready" : "no rollback");
        status = set_string(*out_view, key, text);
    }
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.pane.product-marketplace",
        "Marketplace", "Browse compatible product releases");
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 1U, "studio.action.pane.product-updates",
        "Updates", "Open available update notifications");
    return status;
}

UmiStatus umi_distribution_ui_updates_view_create(
    const char *view_id,
    UmiDistributionService *service,
    UmiUiViewModel **out_view)
{
    UmiDistributionServiceSnapshot snapshot;
    UmiDistributionNotificationCentre *notifications;
    size_t count;
    size_t index;
    UmiStatus status;
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = base_view(
        view_id, "product-updates", "Product Updates",
        "Actionable updates that passed release-channel and supply-chain policy.",
        out_view);
    if (status != UMI_STATUS_OK) return status;
    status = umi_distribution_service_snapshot(service, &snapshot);
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "product-updates.available", (int64_t)snapshot.available_updates);
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "product-updates.unread", (int64_t)snapshot.unread_notifications);
    notifications = umi_distribution_service_notifications(service);
    count = umi_distribution_notification_centre_count(notifications);
    if (count > UMI_DISTRIBUTION_UI_VISIBLE_ROWS) count = UMI_DISTRIBUTION_UI_VISIBLE_ROWS;
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "product-updates.row-count", (int64_t)count);
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        UmiDistributionNotification notification;
        char key[96];
        char text[768];
        status = umi_distribution_notification_centre_at(
            notifications, index, &notification);
        if (status != UMI_STATUS_OK) break;
        (void)snprintf(key, sizeof(key), "product-updates.row.%zu", index);
        (void)snprintf(text, sizeof(text), "%s | %s | %s",
                       notification.title, notification.product_id,
                       notification.read ? "read" : "unread");
        status = set_string(*out_view, key, text);
    }
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.marketplace.check-updates",
        "Check Again", "Refresh verified update decisions");
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 1U, "studio.action.pane.product-transactions",
        "Transactions", "Inspect planned install, update and rollback operations");
    return status;
}

UmiStatus umi_distribution_ui_transactions_view_create(
    const char *view_id,
    UmiDistributionService *service,
    UmiUiViewModel **out_view)
{
    UmiDistributionTransactionLog *transactions;
    size_t count;
    size_t index;
    UmiStatus status;
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = base_view(
        view_id, "product-transactions", "Distribution Transactions",
        "Inspectable install, update and rollback plans with recoverable state.",
        out_view);
    if (status != UMI_STATUS_OK) return status;
    transactions = umi_distribution_service_transactions(service);
    count = umi_distribution_transaction_log_count(transactions);
    if (count > UMI_DISTRIBUTION_UI_VISIBLE_ROWS) count = UMI_DISTRIBUTION_UI_VISIBLE_ROWS;
    status = set_integer(*out_view, "product-transactions.row-count", (int64_t)count);
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        UmiDistributionTransaction transaction;
        char key[96];
        char text[768];
        status = umi_distribution_transaction_log_at(
            transactions, index, &transaction);
        if (status != UMI_STATUS_OK) break;
        (void)snprintf(key, sizeof(key), "product-transactions.row.%zu", index);
        (void)snprintf(text, sizeof(text), "%s | %s | %s | %zu/%zu steps",
                       transaction.product_id,
                       umi_distribution_transaction_action_text(transaction.action),
                       umi_distribution_transaction_state_text(transaction.state),
                       transaction.current_step, transaction.step_count);
        status = set_string(*out_view, key, text);
    }
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.pane.product-updates",
        "Updates", "Return to update notifications");
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 1U, "studio.action.pane.product-evidence",
        "Evidence", "Inspect release verification evidence");
    return status;
}

UmiStatus umi_distribution_ui_evidence_view_create(
    const char *view_id,
    UmiDistributionService *service,
    UmiUiViewModel **out_view)
{
    UmiDistributionEvidenceRegistry *evidence_registry;
    size_t count;
    size_t index;
    UmiStatus status;
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = base_view(
        view_id, "product-evidence", "Supply-chain Evidence",
        "Checksum, signature, SBOM, licence and provenance verification results.",
        out_view);
    if (status != UMI_STATUS_OK) return status;
    evidence_registry = umi_distribution_service_evidence(service);
    count = umi_distribution_evidence_registry_count(evidence_registry);
    if (count > UMI_DISTRIBUTION_UI_VISIBLE_ROWS) count = UMI_DISTRIBUTION_UI_VISIBLE_ROWS;
    status = set_integer(*out_view, "product-evidence.row-count", (int64_t)count);
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        UmiDistributionEvidence evidence;
        char key[96];
        char text[768];
        status = umi_distribution_evidence_registry_at(
            evidence_registry, index, &evidence);
        if (status != UMI_STATUS_OK) break;
        (void)snprintf(key, sizeof(key), "product-evidence.row.%zu", index);
        (void)snprintf(text, sizeof(text),
                       "%s | checksum %s | signature %s | SBOM %zu | licences %zu | provenance %s",
                       evidence.release_id,
                       evidence.checksum_verified ? "verified" : "missing",
                       evidence.signature_verified ? "verified" : "missing",
                       evidence.sbom_components, evidence.licence_entries,
                       evidence.provenance_verified ? "verified" : "missing");
        status = set_string(*out_view, key, text);
    }
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.pane.product-marketplace",
        "Marketplace", "Return to product releases");
    return status;
}
