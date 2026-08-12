/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_developer_context.c
 *
 * PURPOSE:
 *   Verify monotonic developer-context revisions and lossless codec behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include "umicom/developer/context.h"

int main(void)
{
    UmiDeveloperContext *context = NULL;
    UmiDeveloperContextSnapshot input;
    UmiDeveloperContextSnapshot snapshot;
    UmiDeveloperContextSnapshot decoded;
    char encoded[8192];
    size_t encoded_length = 0U;

    assert(umi_developer_context_create(&context) == UMI_STATUS_OK);
    assert(context != NULL);
    assert(umi_developer_context_snapshot(context, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.revision == 1U);

    memset(&input, 0, sizeof(input));
    strcpy(input.project_id, "framework|studio");
    strcpy(input.configuration_id, "Debug%ASan");
    strcpy(input.target_id, "umicom-studio-ide");
    strcpy(input.launch_profile_id, "local");
    strcpy(input.repository_id, "main");
    strcpy(input.test_item_id, "all");
    strcpy(input.active_document, "C:\\Dev\\Umicom|Studio\\main.c");
    strcpy(input.workspace_directory, "C:\\Dev\\Umicom%Workspace");
    input.revision = 999U;

    assert(umi_developer_context_set(context, &input) == UMI_STATUS_OK);
    assert(umi_developer_context_snapshot(context, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.revision == 2U);
    assert(strcmp(snapshot.project_id, input.project_id) == 0);


    {
        UmiDeveloperContextPatch patch;

        memset(&patch, 0, sizeof(patch));
        patch.struct_size = (uint32_t)sizeof(patch);
        patch.api_version = UMI_DEVELOPER_CONTEXT_PATCH_API_VERSION;
        patch.field_mask = UMI_DEVELOPER_CONTEXT_PATCH_REPOSITORY |
                           UMI_DEVELOPER_CONTEXT_PATCH_TEST_ITEM;
        patch.repository_id = "framework-studio.git";
        patch.test_item_id = "suite.project-import";

        assert(umi_developer_context_patch(
            context, &patch, &snapshot) == UMI_STATUS_OK);
        assert(snapshot.revision == 3U);
        assert(strcmp(snapshot.project_id, input.project_id) == 0);
        assert(strcmp(snapshot.configuration_id, input.configuration_id) == 0);
        assert(strcmp(snapshot.repository_id, "framework-studio.git") == 0);
        assert(strcmp(snapshot.test_item_id, "suite.project-import") == 0);
        assert(strcmp(snapshot.active_document, input.active_document) == 0);
        assert(strcmp(snapshot.workspace_directory,
                      input.workspace_directory) == 0);
    }

    assert(umi_developer_context_encode(
        context, encoded, sizeof(encoded), &encoded_length) == UMI_STATUS_OK);
    assert(encoded_length == strlen(encoded));
    assert(strncmp(encoded, "UDC1|", 5U) == 0);
    assert(strstr(encoded, "%7C") != NULL);
    assert(strstr(encoded, "%25") != NULL);

    assert(umi_developer_context_decode(encoded, &decoded) == UMI_STATUS_OK);
    assert(strcmp(decoded.project_id, input.project_id) == 0);
    assert(strcmp(decoded.configuration_id, input.configuration_id) == 0);
    assert(strcmp(decoded.active_document, input.active_document) == 0);
    assert(strcmp(decoded.workspace_directory, input.workspace_directory) == 0);
    assert(decoded.revision == 1U);

    assert(umi_developer_context_decode("BAD|one", &decoded) ==
           UMI_STATUS_PARSE_ERROR);

    umi_developer_context_destroy(context);
    return 0;
}
