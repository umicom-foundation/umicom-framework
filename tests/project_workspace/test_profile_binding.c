#include "umicom/project/workspace/profile_binding.h"
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
int main(void) {
    UmiProjectWorkspaceProfileBinding v;
    CHECK(umi_project_workspace_profile_binding_init(&v,"project","profile")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_profile_binding_matches(&v,"project","profile"));
    CHECK(umi_project_workspace_profile_binding_retarget(&v,"profile2")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_profile_binding_matches(&v,"project","profile2"));
    return 0;
}
