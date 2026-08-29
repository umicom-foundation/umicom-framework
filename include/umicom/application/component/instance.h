/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/component/instance.h
 *
 * PURPOSE:
 *   Manage component-instance identity and lifecycle independently of GTK4,
 *   Qt6, web or headless frontend implementation details.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_COMPONENT_INSTANCE_H
#define UMICOM_APPLICATION_COMPONENT_INSTANCE_H

#include "umicom/application/component/factory.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiApplicationComponentInstanceRequest {
  const char *component_id;
  const char *instance_id;
  UmiApplicationComponentFrontend frontend;
} UmiApplicationComponentInstanceRequest;

typedef struct UmiApplicationComponentInstance {
  char component_id[UMI_APPLICATION_COMPONENT_ID_CAPACITY];
  char instance_id[UMI_APPLICATION_COMPONENT_ID_CAPACITY];
  UmiApplicationComponentFrontend frontend;
  UmiApplicationComponentLifecycle lifecycle;
  const UmiApplicationComponentContract *contract;
  const UmiApplicationComponentFactoryDescriptor *factory;
  void *handle;
  uint64_t revision;
} UmiApplicationComponentInstance;

UmiStatus umi_application_component_instance_create(
    const UmiApplicationComponentRegistry *components,
    const UmiApplicationComponentFactoryRegistry *factories,
    const UmiApplicationComponentInstanceRequest *request,
    UmiApplicationComponentInstance *out_instance);
UmiStatus umi_application_component_instance_activate(
    UmiApplicationComponentInstance *instance);
UmiStatus umi_application_component_instance_suspend(
    UmiApplicationComponentInstance *instance);
UmiStatus umi_application_component_instance_close(
    UmiApplicationComponentInstance *instance);

#ifdef __cplusplus
}
#endif

#endif
