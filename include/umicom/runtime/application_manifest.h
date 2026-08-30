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

typedef enum UmiApplicationFrontend {
    UMI_FRONTEND_NONE = 0U,
    UMI_FRONTEND_CONSOLE = 1U << 0,
    UMI_FRONTEND_GTK4 = 1U << 1,
    UMI_FRONTEND_WEB = 1U << 2,
    UMI_FRONTEND_QT = 1U << 3,
    UMI_FRONTEND_WT = 1U << 4,
    UMI_FRONTEND_MOBILE = 1U << 5
} UmiApplicationFrontend;

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

void umi_application_manifest_init(UmiApplicationManifest *manifest);
UmiStatus umi_application_manifest_load(
    const char *path,
    UmiApplicationManifest *out_manifest
);
UmiStatus umi_application_manifest_validate(
    const UmiApplicationManifest *manifest,
    char *out_message,
    size_t capacity
);
int umi_application_manifest_has_capability(
    const UmiApplicationManifest *manifest,
    const char *capability
);
const char *umi_application_frontend_text(unsigned frontend_flag);

#ifdef __cplusplus
}
#endif

#endif
