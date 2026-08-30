/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/integration.h
 *
 * PURPOSE:
 *   Provide the aggregate public include for the Suite and Inter-Application Runtime Foundation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file keeps one part of the public runtime small and explicit. Product
 * code uses these contracts instead of reaching into another application's
 * private state or private headers.
 */

#ifndef UMICOM_INTEGRATION_INTEGRATION_H
#define UMICOM_INTEGRATION_INTEGRATION_H

#include "umicom/integration/types.h"
#include "umicom/integration/application.h"
#include "umicom/integration/registry.h"
#include "umicom/integration/dependency.h"
#include "umicom/integration/resolver.h"
#include "umicom/integration/presence.h"
#include "umicom/integration/endpoint.h"
#include "umicom/integration/route.h"
#include "umicom/integration/router.h"
#include "umicom/integration/envelope.h"
#include "umicom/integration/dispatcher.h"
#include "umicom/integration/session.h"
#include "umicom/integration/suite.h"
#include "umicom/integration/launch_plan.h"
#include "umicom/integration/suite_runtime.h"
#include "umicom/integration/topology.h"
#include "umicom/integration/contract.h"
#include "umicom/integration/health.h"
#include "umicom/integration/bridge.h"
#include "umicom/integration/catalogue.h"
#include "umicom/integration/designer.h"

#endif
