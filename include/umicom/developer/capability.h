/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capability.h
 *
 * PURPOSE:
 *   Define the stable metadata contract for reusable developer capabilities
 *   owned by Umicom Framework and consumed by every Umicom application.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITY_H
#define UMICOM_DEVELOPER_CAPABILITY_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_API_VERSION 1U
#define UMI_DEVELOPER_CAPABILITY_MESSAGE_CAPACITY 512U

typedef enum UmiDeveloperCapabilityCategory {
    UMI_DEVELOPER_CAPABILITY_CATEGORY_CONFIGURATION = 1,
    UMI_DEVELOPER_CAPABILITY_CATEGORY_BUILD = 2,
    UMI_DEVELOPER_CAPABILITY_CATEGORY_TESTING = 3,
    UMI_DEVELOPER_CAPABILITY_CATEGORY_EXECUTION = 4,
    UMI_DEVELOPER_CAPABILITY_CATEGORY_TERMINAL = 5,
    UMI_DEVELOPER_CAPABILITY_CATEGORY_DELIVERY = 6,
    UMI_DEVELOPER_CAPABILITY_CATEGORY_DIAGNOSTICS = 7,
    UMI_DEVELOPER_CAPABILITY_CATEGORY_SECURITY = 8,
    UMI_DEVELOPER_CAPABILITY_CATEGORY_PERFORMANCE = 9,
    UMI_DEVELOPER_CAPABILITY_CATEGORY_INTEGRATION = 10,
    UMI_DEVELOPER_CAPABILITY_CATEGORY_EXPERIENCE = 11,
    UMI_DEVELOPER_CAPABILITY_CATEGORY_PORTABILITY = 12
} UmiDeveloperCapabilityCategory;

typedef enum UmiDeveloperCapabilityMaturity {
    UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED = 0,
    UMI_DEVELOPER_CAPABILITY_MATURITY_EXPERIMENTAL = 1,
    UMI_DEVELOPER_CAPABILITY_MATURITY_STABLE = 2
} UmiDeveloperCapabilityMaturity;

enum {
    UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS = 1U << 0,
    UMI_DEVELOPER_CAPABILITY_FLAG_GUI = 1U << 1,
    UMI_DEVELOPER_CAPABILITY_FLAG_IMPLEMENTED = 1U << 2,
    UMI_DEVELOPER_CAPABILITY_FLAG_SECURITY_SENSITIVE = 1U << 3
};

typedef struct UmiDeveloperCapabilityDescriptor {
    size_t struct_size;
    uint32_t api_version;
    const char *capability_id;
    const char *title;
    UmiDeveloperCapabilityCategory category;
    UmiDeveloperCapabilityMaturity maturity;
    const char *summary;
    const char *provider_role;
    uint32_t flags;
    int priority;
} UmiDeveloperCapabilityDescriptor;

const char *umi_developer_capability_category_text(
    UmiDeveloperCapabilityCategory category);
const char *umi_developer_capability_maturity_text(
    UmiDeveloperCapabilityMaturity maturity);
UmiStatus umi_developer_capability_descriptor_validate(
    const UmiDeveloperCapabilityDescriptor *descriptor,
    char *out_message,
    size_t message_capacity);
size_t umi_developer_capability_builtin_count(void);
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_builtin_at(size_t index);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITY_H */
