/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/developer.h
 *
 * PURPOSE:
 *   Aggregate the reusable Umicom developer-runtime API.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEVELOPER_DEVELOPER_H
#define UMICOM_DEVELOPER_DEVELOPER_H

#include "umicom/developer/types.h"
#include "umicom/developer/capability.h"
#include "umicom/developer/capabilities.h"
#include "umicom/developer/spin_hint.h"
#include "umicom/developer/context.h"
#include "umicom/developer/operation.h"
#include "umicom/developer/pipeline.h"
#include "umicom/developer/journal.h"
#include "umicom/developer/executor.h"
#include "umicom/developer/cmake_plan.h"
#include "umicom/developer/command_line.h"
#include "umicom/developer/task_plan.h"
#include "umicom/developer/launch_plan.h"
#include "umicom/developer/workflow.h"
#include "umicom/developer/project_workflow.h"
#include "umicom/developer/project_bootstrap.h"
#include "umicom/developer/batch_execution.h"
#include "umicom/developer/workspace_state.h"
#include "umicom/developer/toolchain_binding.h"
#include "umicom/developer/universal_model.h"
#include "umicom/developer/runtime.h"

/*
 * Professional IDE/workbench composition builds on the runtime above. Keeping
 * it in a separate aggregate header preserves the existing developer contracts
 * while making the new command/search/perspective platform available to every
 * Framework application.
 */
#include "umicom/developer_workbench/developer_workbench.h"

#endif
