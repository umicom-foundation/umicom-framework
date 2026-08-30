/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/dependency_graph.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Dependency Graph developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_DEPENDENCY_GRAPH_H
#define UMICOM_DEVELOPER_CAPABILITIES_DEPENDENCY_GRAPH_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_DEPENDENCY_GRAPH "umicom.developer.configuration.dependency_graph"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_dependency_graph(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_DEPENDENCY_GRAPH_H */
