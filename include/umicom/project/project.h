/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/project.h
 *
 * PURPOSE:
 *   Aggregate the complete reusable Umicom project-system API.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This module uses a small, explicit C API and bounded storage.  The public
 * contract does not expose toolkit objects, C++ types, or private structures.
 */
#ifndef FRAMEWORK_INCLUDE_UMICOM_PROJECT_PROJECT_H
#define FRAMEWORK_INCLUDE_UMICOM_PROJECT_PROJECT_H

#include "umicom/project/descriptor.h"
#include "umicom/project/target.h"
#include "umicom/project/configuration.h"
#include "umicom/project/dependency.h"
#include "umicom/project/file_set.h"
#include "umicom/project/task.h"
#include "umicom/project/launch_profile.h"
#include "umicom/project/environment.h"
#include "umicom/project/reference.h"
#include "umicom/project/template.h"
#include "umicom/project/variable.h"
#include "umicom/project/capability.h"
#include "umicom/project/build_node.h"
#include "umicom/project/workspace.h"
#include "umicom/project/workspace_query.h"
#include "umicom/project/workspace_validation.h"
#include "umicom/project/workspace_import.h"
#include "umicom/project/workspace_model.h"
#include "umicom/project/workspace_group.h"
#include "umicom/project/workspace_setting.h"
#include "umicom/project/workspace_exclusion.h"
#include "umicom/project/workspace_discovery.h"
#include "umicom/project/workspace_refresh.h"
#include "umicom/project/language_matrix.h"

#endif
