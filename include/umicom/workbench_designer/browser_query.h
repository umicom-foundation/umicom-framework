/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/browser_query.h
 *
 * PURPOSE:
 *   Parse Layout Browser search text into explicit semantic filters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_BROWSER_QUERY_H
#define UMICOM_WORKBENCH_DESIGNER_BROWSER_QUERY_H

#include "umicom/workbench_designer/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the workbench designer browser query data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerBrowserQuery {
    char text[UMI_WORKBENCH_DESIGNER_TEXT_CAPACITY];
    char category[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char owner[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char workspace[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char tag[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    bool built_in_only;
    bool user_only;
    bool locked_only;
    bool shared_only;
} UmiWorkbenchDesignerBrowserQuery;

/**
 * Initialise workbench designer browser query from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_designer_browser_query_init(UmiWorkbenchDesignerBrowserQuery *query);
/**
 * Read workbench designer browser query into validated module state and return a status
 * when input cannot be used.
 */
UmiStatus umi_workbench_designer_browser_query_parse(const char *text, UmiWorkbenchDesignerBrowserQuery *out_query);

#ifdef __cplusplus
}
#endif

#endif
