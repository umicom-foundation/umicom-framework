#include "umicom/project/workspace/cmake_cache_entry.h"
#include <string.h>
#define CHECK(x) do{if(!(x))return __LINE__;}while(0)
int main(void) {
    UmiProjectWorkspaceCmakeCacheEntry e;
    CHECK(umi_project_workspace_cmake_cache_entry_parse("CMAKE_BUILD_TYPE:STRING=Release",&e)==UMI_STATUS_OK);
    CHECK(strcmp(e.key,"CMAKE_BUILD_TYPE")==0);
    CHECK(strcmp(e.value,"Release")==0);
    return 0;
}
