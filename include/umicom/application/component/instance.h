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

/**
 * Represent the application component instance request data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationComponentInstanceRequest {
  const char *component_id;
  const char *instance_id;
  UmiApplicationComponentFrontend frontend;
} UmiApplicationComponentInstanceRequest;

/**
 * Represent the application component instance data shared with callers of this public
 * contract.
 */
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

/**
 * Initialise application component instance from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_application_component_instance_create(
    const UmiApplicationComponentRegistry *components,
    const UmiApplicationComponentFactoryRegistry *factories,
    const UmiApplicationComponentInstanceRequest *request,
    UmiApplicationComponentInstance *out_instance);
/**
 * Provide the application component instance activate operation used by this module and
 * its client applications.
 */
UmiStatus umi_application_component_instance_activate(
    UmiApplicationComponentInstance *instance);
/**
 * Provide the application component instance suspend operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_component_instance_suspend(
    UmiApplicationComponentInstance *instance);
/**
 * Provide the application component instance close operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_component_instance_close(
    UmiApplicationComponentInstance *instance);

#ifdef __cplusplus
}
#endif

#endif
