/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/component/types.h
 *
 * PURPOSE:
 *   Define stable version, frontend, lifecycle and placement primitives for
 *   reusable application components and toolkit adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_COMPONENT_TYPES_H
#define UMICOM_APPLICATION_COMPONENT_TYPES_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/application/component_catalogue.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_APPLICATION_COMPONENT_MODEL_API_VERSION 1U
#define UMI_APPLICATION_COMPONENT_REGISTRY_CAPACITY 256U
#define UMI_APPLICATION_COMPONENT_FACTORY_CAPACITY 32U
#define UMI_APPLICATION_COMPONENT_SELECTION_CAPACITY 64U
#define UMI_APPLICATION_COMPONENT_LAYOUT_CAPACITY 64U
#define UMI_APPLICATION_COMPONENT_ID_CAPACITY 192U
#define UMI_APPLICATION_COMPONENT_TITLE_CAPACITY 256U

typedef uint32_t UmiApplicationComponentFrontendMask;

typedef enum UmiApplicationComponentFrontend {
  UMI_APPLICATION_COMPONENT_FRONTEND_NONE = 0U,
  UMI_APPLICATION_COMPONENT_FRONTEND_HEADLESS = 1U << 0,
  UMI_APPLICATION_COMPONENT_FRONTEND_GTK4 = 1U << 1,
  UMI_APPLICATION_COMPONENT_FRONTEND_QT6 = 1U << 2,
  UMI_APPLICATION_COMPONENT_FRONTEND_WEB = 1U << 3
} UmiApplicationComponentFrontend;

typedef enum UmiApplicationComponentLifecycle {
  UMI_APPLICATION_COMPONENT_REGISTERED = 1,
  UMI_APPLICATION_COMPONENT_CREATED = 2,
  UMI_APPLICATION_COMPONENT_ACTIVE = 3,
  UMI_APPLICATION_COMPONENT_SUSPENDED = 4,
  UMI_APPLICATION_COMPONENT_CLOSED = 5,
  UMI_APPLICATION_COMPONENT_FAILED = 6
} UmiApplicationComponentLifecycle;

typedef enum UmiApplicationComponentRegion {
  UMI_APPLICATION_COMPONENT_REGION_PRIMARY = 1,
  UMI_APPLICATION_COMPONENT_REGION_LEFT = 2,
  UMI_APPLICATION_COMPONENT_REGION_RIGHT = 3,
  UMI_APPLICATION_COMPONENT_REGION_BOTTOM = 4,
  UMI_APPLICATION_COMPONENT_REGION_FLOATING = 5
} UmiApplicationComponentRegion;

typedef struct UmiApplicationComponentVersion {
  uint16_t major;
  uint16_t minor;
  uint16_t patch;
} UmiApplicationComponentVersion;

typedef struct UmiApplicationComponentContract {
  uint32_t struct_size;
  uint32_t api_version;
  const UmiApplicationComponentDefinition *definition;
  UmiApplicationComponentVersion version;
  const char *factory_id;
  UmiApplicationComponentFrontendMask frontend_mask;
  UmiApplicationComponentRegion default_region;
  uint32_t minimum_width;
  uint32_t minimum_height;
} UmiApplicationComponentContract;

const char *umi_application_component_frontend_text(
    UmiApplicationComponentFrontend frontend);
int umi_application_component_frontend_valid(
    UmiApplicationComponentFrontend frontend);
const char *umi_application_component_lifecycle_text(
    UmiApplicationComponentLifecycle lifecycle);
const char *umi_application_component_region_text(
    UmiApplicationComponentRegion region);

#ifdef __cplusplus
}
#endif

#endif
