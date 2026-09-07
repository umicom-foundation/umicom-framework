/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/application_manifest.h
 *
 * PURPOSE:
 *   Parse and validate the stable subset of application.umicom.yaml used to
 *   describe Umicom application identity, versions, frontends, aliases, and
 *   required Framework capabilities without introducing a YAML dependency.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_APPLICATION_MANIFEST_H
#define UMICOM_RUNTIME_APPLICATION_MANIFEST_H

#include <stddef.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_MANIFEST_TEXT_CAPACITY 256U
#define UMI_MANIFEST_MAX_ALIASES 16U
#define UMI_MANIFEST_MAX_CAPABILITIES 64U

/**
 * List the named application frontend values accepted by this public contract.
 */
typedef enum UmiApplicationFrontend {
    UMI_FRONTEND_NONE = 0U,
    UMI_FRONTEND_CONSOLE = 1U << 0,
    UMI_FRONTEND_GTK4 = 1U << 1,
    UMI_FRONTEND_WEB = 1U << 2,
    UMI_FRONTEND_QT = 1U << 3,
    UMI_FRONTEND_WT = 1U << 4,
    UMI_FRONTEND_MOBILE = 1U << 5
} UmiApplicationFrontend;

/**
 * Represent the application manifest data shared with callers of this public contract.
 */
typedef struct UmiApplicationManifest {
    char schema[UMI_MANIFEST_TEXT_CAPACITY];
    char id[UMI_MANIFEST_TEXT_CAPACITY];
    char name[UMI_MANIFEST_TEXT_CAPACITY];
    char short_name[UMI_MANIFEST_TEXT_CAPACITY];
    char version[UMI_MANIFEST_TEXT_CAPACITY];
    char executable[UMI_MANIFEST_TEXT_CAPACITY];
    char minimum_framework_version[UMI_MANIFEST_TEXT_CAPACITY];
    unsigned frontends;
    char aliases[UMI_MANIFEST_MAX_ALIASES][UMI_MANIFEST_TEXT_CAPACITY];
    size_t alias_count;
    char capabilities[UMI_MANIFEST_MAX_CAPABILITIES]
                     [UMI_MANIFEST_TEXT_CAPACITY];
    size_t capability_count;
} UmiApplicationManifest;

/** Explicit frontend names accompany the legacy manifest without changing its
 * ABI. Empty names mean undeclared, never a guessed or console GUI fallback.
 * These are executable basenames, not paths, arguments or shell commands. */
typedef struct UmiApplicationLaunchSpec {
    size_t structure_size;
    char native_executable[UMI_MANIFEST_TEXT_CAPACITY];
    char console_executable[UMI_MANIFEST_TEXT_CAPACITY];
} UmiApplicationLaunchSpec;

/** Initialise an empty explicit launch specification. */
void umi_application_launch_spec_init(UmiApplicationLaunchSpec *launch_spec);

/** Validate bounded native/console declarations against advertised frontends.
 * Native currently denotes GTK4. No executable existence or readiness is
 * inferred. Legacy executable text remains a separate compatibility field. */
UmiStatus umi_application_launch_spec_validate(
    const UmiApplicationManifest *manifest,
    const UmiApplicationLaunchSpec *launch_spec);

/** Parse the supported YAML subset and explicit launch declarations together.
 * Input is borrowed NUL-terminated text. Outputs must be distinct, caller-owned
 * storage; they are cleared on failure. Duplicate executable declarations,
 * truncation, paths, whitespace and shell syntax in explicit names are rejected.
 * Legacy manifests without the companion fields remain valid with empty names.
 * Canonical nested umicom.application.v1 and existing generated flat
 * umicom.application/v1 are accepted separately; their spelling is preserved.
 * Schema must be the first non-comment field. Nested application executable
 * declarations must be direct two-space children. Mixing nested application/
 * framework blocks into the flat shape is rejected. */
UmiStatus umi_application_manifest_parse_with_launch_spec(
    const char *text,
    UmiApplicationManifest *out_manifest,
    UmiApplicationLaunchSpec *out_launch_spec);

/** Load the same manifest/launch contract from a file, without launching it.
 * Ownership and failure behaviour match parse_with_launch_spec. */
UmiStatus umi_application_manifest_load_with_launch_spec(
    const char *path,
    UmiApplicationManifest *out_manifest,
    UmiApplicationLaunchSpec *out_launch_spec);

/**
 * Initialise application manifest from caller-provided values so later operations receive
 * a known state.
 */
void umi_application_manifest_init(UmiApplicationManifest *manifest);
/**
 * Read application manifest into validated module state and return a status when input
 * cannot be used.
 */
UmiStatus umi_application_manifest_load(
    const char *path,
    UmiApplicationManifest *out_manifest
);
/**
 * Check that application manifest satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_application_manifest_validate(
    const UmiApplicationManifest *manifest,
    char *out_message,
    size_t capacity
);
/**
 * Provide the application manifest has capability operation used by this module and its
 * client applications.
 */
int umi_application_manifest_has_capability(
    const UmiApplicationManifest *manifest,
    const char *capability
);
/**
 * Provide the application frontend text operation used by this module and its client
 * applications.
 */
const char *umi_application_frontend_text(unsigned frontend_flag);

#ifdef __cplusplus
}
#endif

#endif
