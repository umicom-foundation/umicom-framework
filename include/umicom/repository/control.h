/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/control.h
 *
 * PURPOSE:
 *   Expose the complete native repository-control platform through one Framework header.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable repository-control capability. Applications
 *   remain thin consumers and must not duplicate this policy or state model.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_REPOSITORY_CONTROL_H
#define UMICOM_REPOSITORY_CONTROL_H
#include "umicom/repository/control_types.h"
#include "umicom/repository/submodule_lock.h"
#include "umicom/repository/sha.h"
#include "umicom/repository/ref.h"
#include "umicom/repository/path.h"
#include "umicom/repository/submodule.h"
#include "umicom/repository/inventory.h"
#include "umicom/repository/gitmodules.h"
#include "umicom/repository/gitlink.h"
#include "umicom/repository/head_set.h"
#include "umicom/repository/lock_policy.h"
#include "umicom/repository/lock_plan.h"
#include "umicom/repository/health.h"
#include "umicom/repository/issue.h"
#include "umicom/repository/snapshot.h"
#include "umicom/repository/dependency.h"
#include "umicom/repository/dependency_graph.h"
#include "umicom/repository/transaction.h"
#include "umicom/repository/event.h"
#include "umicom/repository/audit.h"
#include "umicom/repository/controller.h"
#include "umicom/repository/service.h"
#endif
