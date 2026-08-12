/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/source_control/source_control.h
 *
 * PURPOSE:
 *   Aggregate the complete provider-neutral Umicom source-control API.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This module uses a small, explicit C API and bounded storage.  The public
 * contract does not expose toolkit objects, C++ types, or private structures.
 */
#ifndef FRAMEWORK_INCLUDE_UMICOM_SOURCE_CONTROL_SOURCE_CONTROL_H
#define FRAMEWORK_INCLUDE_UMICOM_SOURCE_CONTROL_SOURCE_CONTROL_H

#include "umicom/source_control/repository.h"
#include "umicom/source_control/change.h"
#include "umicom/source_control/change_set.h"
#include "umicom/source_control/staging.h"
#include "umicom/source_control/commit.h"
#include "umicom/source_control/branch.h"
#include "umicom/source_control/tag.h"
#include "umicom/source_control/remote.h"
#include "umicom/source_control/diff_session.h"
#include "umicom/source_control/operation.h"
#include "umicom/source_control/history_entry.h"
#include "umicom/source_control/service.h"

#endif
