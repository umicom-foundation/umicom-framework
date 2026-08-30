/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_label_catalogue.c
 *
 * PURPOSE:
 *   Verify the label catalogue runtime contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/test_runtime/label_catalogue.h"
int main(void)
{
    UmiTestRuntimeLabelCatalogue value,same;
    uint64_t r;
    umi_test_runtime_label_catalogue_init(&value,"test-runtime.label_catalogue");
    assert(umi_test_runtime_label_catalogue_validate(&value)==UMI_STATUS_OK);
    r=value.revision;
    assert(umi_test_runtime_label_catalogue_set_category(&value,"regression")==UMI_STATUS_OK);
    assert(umi_test_runtime_label_catalogue_set_detail(&value,"evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_label_catalogue_set_label_count(&value,13U)==UMI_STATUS_OK);
    assert(umi_test_runtime_label_catalogue_set_generation(&value,21U)==UMI_STATUS_OK);
    assert(umi_test_runtime_label_catalogue_set_active(&value,false)==UMI_STATUS_OK);
    assert(value.revision>r);
    assert(value.label_count==13U&&value.generation==21U);
    umi_test_runtime_label_catalogue_init(&same,"test-runtime.label_catalogue");
    assert(umi_test_runtime_label_catalogue_same_identity(&value,&same));
    return 0;
    }
