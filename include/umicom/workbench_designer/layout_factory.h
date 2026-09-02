/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/layout_factory.h
 *
 * PURPOSE:
 *   Create complete semantic layouts for blank, coding, debugging, trading and
 *   operations workbenches from registered panel identities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_LAYOUT_FACTORY_H
#define UMICOM_WORKBENCH_DESIGNER_LAYOUT_FACTORY_H

#include "umicom/workbench_designer/palette.h"
#include "umicom/workbench_layout/document.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * List the named workbench designer layout factory profile values accepted by this public
 * contract.
 */
typedef enum UmiWorkbenchDesignerLayoutFactoryProfile {
    UMI_WORKBENCH_DESIGNER_LAYOUT_BLANK = 1,
    UMI_WORKBENCH_DESIGNER_LAYOUT_MOSAIC = 2,
    UMI_WORKBENCH_DESIGNER_LAYOUT_CODING = 3,
    UMI_WORKBENCH_DESIGNER_LAYOUT_DEBUGGING = 4,
    UMI_WORKBENCH_DESIGNER_LAYOUT_TRADING = 5,
    UMI_WORKBENCH_DESIGNER_LAYOUT_OPERATIONS = 6
} UmiWorkbenchDesignerLayoutFactoryProfile;

/**
 * Represent the workbench designer layout factory request data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchDesignerLayoutFactoryRequest {
    UmiWorkbenchDesignerLayoutFactoryProfile profile;
    char layout_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char name[UMI_WORKBENCH_DESIGNER_LABEL_CAPACITY];
    char owner_user_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char owner_application_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char workspace_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char monitor_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    uint64_t timestamp_ms;
} UmiWorkbenchDesignerLayoutFactoryRequest;

/**
 * Provide the workbench designer layout factory request default operation used by this
 * module and its client applications.
 */
UmiWorkbenchDesignerLayoutFactoryRequest umi_workbench_designer_layout_factory_request_default(void);
/**
 * Initialise workbench designer layout factory from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_workbench_designer_layout_factory_create(
    const UmiWorkbenchDesignerLayoutFactoryRequest *request,
    const UmiWorkbenchDesignerPalette *palette,
    UmiWorkbenchLayoutDocument *out_document);

#ifdef __cplusplus
}
#endif

#endif
