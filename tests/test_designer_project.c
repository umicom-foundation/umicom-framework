/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_designer_project.c
 *
 * PURPOSE:
 *   Verify one project can own several independent semantic designer documents.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This test exercises the public contract rather than relying on private state,
 * making the behaviour useful as an executable example for new developers.
 */
#include "umicom/umicom.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    UmiDesignerProject *project=NULL;UmiDesignerDocument *document=NULL,*found=NULL;
    assert(umi_designer_project_create("studio.designs",&project)==UMI_STATUS_OK);
    assert(umi_designer_document_create("org.umicom.one",&document)==UMI_STATUS_OK);
    assert(umi_designer_project_add(project,"main",document,1)==UMI_STATUS_OK);
    assert(umi_designer_project_find(project,"main",&found)==UMI_STATUS_OK);
    assert(found==document&&umi_designer_project_count(project)==1U);
    umi_designer_project_destroy(project);
    return EXIT_SUCCESS;
}
