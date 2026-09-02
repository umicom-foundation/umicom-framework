/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/workbench/cloud_object.h
 *
 * PURPOSE:
 *   Model provider-neutral cloud object-storage inventory and safe operation
 *   plans. Credentials stay referenced through Umicom Security.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_WEB_WORKBENCH_CLOUD_OBJECT_H
#define UMICOM_WEB_WORKBENCH_CLOUD_OBJECT_H

#include "umicom/web/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the web workbench cloud profile data shared with callers of this public
 * contract.
 */
typedef struct UmiWebWorkbenchCloudProfile {
    char profile_id[UMI_WEB_WORKBENCH_ID_CAPACITY];
    char name[UMI_WEB_WORKBENCH_NAME_CAPACITY];
    UmiWebWorkbenchCloudProvider provider;
    char region[UMI_WEB_WORKBENCH_ID_CAPACITY];
    char endpoint[UMI_WEB_WORKBENCH_URL_CAPACITY];
    char secret_reference[UMI_WEB_WORKBENCH_ID_CAPACITY];
    bool verify_tls;
    bool enabled;
} UmiWebWorkbenchCloudProfile;

/**
 * Represent the web workbench cloud object data shared with callers of this public
 * contract.
 */
typedef struct UmiWebWorkbenchCloudObject {
    char bucket[UMI_WEB_WORKBENCH_NAME_CAPACITY];
    char key[UMI_WEB_WORKBENCH_URL_CAPACITY];
    char content_type[UMI_WEB_HEADER_VALUE_CAPACITY];
    char etag[UMI_WEB_WORKBENCH_ID_CAPACITY];
    uint64_t size_bytes;
    uint64_t modified_timestamp;
    bool versioned;
} UmiWebWorkbenchCloudObject;

/**
 * Represent the web workbench cloud object model data shared with callers of this public
 * contract.
 */
typedef struct UmiWebWorkbenchCloudObjectModel {
    UmiWebWorkbenchCloudProfile profile;
    UmiWebWorkbenchCloudObject objects[UMI_WEB_WORKBENCH_MAX_CLOUD_ITEMS];
    size_t object_count;
    size_t omitted_count;
    uint64_t revision;
} UmiWebWorkbenchCloudObjectModel;

/**
 * Initialise web workbench cloud profile from caller-provided values so later operations
 * receive a known state.
 */
void umi_web_workbench_cloud_profile_init(
    UmiWebWorkbenchCloudProfile *profile,
    const char *profile_id,
    const char *name,
    UmiWebWorkbenchCloudProvider provider);
/**
 * Check that web workbench cloud profile satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_web_workbench_cloud_profile_validate(
    const UmiWebWorkbenchCloudProfile *profile);
/**
 * Initialise web workbench cloud object model from caller-provided values so later
 * operations receive a known state.
 */
void umi_web_workbench_cloud_object_model_init(
    UmiWebWorkbenchCloudObjectModel *model,
    const UmiWebWorkbenchCloudProfile *profile);
/**
 * Provide the web workbench cloud object upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_web_workbench_cloud_object_upsert(
    UmiWebWorkbenchCloudObjectModel *model,
    const UmiWebWorkbenchCloudObject *object);
/**
 * Remove web workbench cloud object while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_web_workbench_cloud_object_remove(
    UmiWebWorkbenchCloudObjectModel *model,
    const char *bucket,
    const char *key);
/**
 * Provide the web workbench cloud object query operation used by this module and its
 * client applications.
 */
size_t umi_web_workbench_cloud_object_query(
    const UmiWebWorkbenchCloudObjectModel *model,
    const char *bucket,
    const char *text,
    const UmiWebWorkbenchCloudObject **out_objects,
    size_t capacity);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_WEB_WORKBENCH_CLOUD_OBJECT_H */
