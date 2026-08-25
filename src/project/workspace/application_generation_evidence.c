#include "umicom/project/workspace/application_generation_evidence.h"
uint64_t umi_project_workspace_application_generation_evidence_fingerprint(const char*text) {
    uint64_t h=UINT64_C(1469598103934665603);
    const unsigned char*p=(const unsigned char*)text;
    if(text==NULL)return 0U;
    while(*p!='\0') {
        h^=(uint64_t)*p++;
        h*=UINT64_C(1099511628211);
    }
    return h;
}
