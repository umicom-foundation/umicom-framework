/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/browser_query.h
 *
 * PURPOSE:
 *   Parse Layout Browser search text into explicit semantic filters.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_BROWSER_QUERY_H
#define UMICOM_WORKBENCH_DESIGNER_BROWSER_QUERY_H

#include "umicom/workbench_designer/types.h"

#ifdef __cplusplus
extern "C" {
#endif


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

void umi_workbench_designer_browser_query_init(UmiWorkbenchDesignerBrowserQuery *query);
UmiStatus umi_workbench_designer_browser_query_parse(const char *text, UmiWorkbenchDesignerBrowserQuery *out_query);

#ifdef __cplusplus
}
#endif

#endif
