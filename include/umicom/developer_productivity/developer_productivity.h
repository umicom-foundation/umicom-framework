/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/developer_productivity.h
 *
 * PURPOSE:
 *   Aggregate the reusable Source Control, diff/merge, Terminal, Problems/output
 *   and navigation platform used by Umicom Studio and future developer tools.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PRODUCTIVITY_DEVELOPER_PRODUCTIVITY_H
#define UMICOM_DEVELOPER_PRODUCTIVITY_DEVELOPER_PRODUCTIVITY_H

#include "umicom/developer_productivity/types.h"
#include "umicom/developer_productivity/source_control.h"
#include "umicom/developer_productivity/source_control_operation.h"
#include "umicom/developer_productivity/source_control_operations/operations.h"
#include "umicom/developer_productivity/builtin_source_control_operations.h"
#include "umicom/developer_productivity/diff.h"
#include "umicom/developer_productivity/diff_hunk.h"
#include "umicom/developer_productivity/diff_navigation.h"
#include "umicom/developer_productivity/merge.h"
#include "umicom/developer_productivity/unified_diff.h"
#include "umicom/developer_productivity/directory_compare.h"
#include "umicom/developer_productivity/terminal_workspace.h"
#include "umicom/developer_productivity/terminal_tabs.h"
#include "umicom/developer_productivity/terminal_split.h"
#include "umicom/developer_productivity/terminal_search.h"
#include "umicom/developer_productivity/terminal_profile.h"
#include "umicom/developer_productivity/terminal_profiles/profiles.h"
#include "umicom/developer_productivity/builtin_terminal_profiles.h"
#include "umicom/developer_productivity/problem.h"
#include "umicom/developer_productivity/problem_store.h"
#include "umicom/developer_productivity/diagnostic_parser.h"
#include "umicom/developer_productivity/diagnostic_parser_support.h"
#include "umicom/developer_productivity/diagnostic_registry.h"
#include "umicom/developer_productivity/diagnostic_parsers/parsers.h"
#include "umicom/developer_productivity/builtin_diagnostics.h"
#include "umicom/developer_productivity/diagnostic_pipeline.h"
#include "umicom/developer_productivity/problem_navigation.h"
#include "umicom/developer_productivity/output_channels.h"
#include "umicom/developer_productivity/navigation_history.h"
#include "umicom/developer_productivity/bookmarks.h"
#include "umicom/developer_productivity/symbols.h"
#include "umicom/developer_productivity/references.h"
#include "umicom/developer_productivity/outline.h"
#include "umicom/developer_productivity/location_link.h"
#include "umicom/developer_productivity/navigation_service.h"
#include "umicom/developer_productivity/workbench_bridge.h"
#include "umicom/developer_productivity/platform.h"

#endif
