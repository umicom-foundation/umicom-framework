/* Umicom Framework | CPython discovery request test | Sammy Hegab | MIT */
#include <assert.h>
#include "umicom/toolchain/python_runtime.h"

int main(void)
{
    UmiPythonRuntimeRequest request;
    UmiPythonRuntimeSnapshot snapshot;
    umi_python_runtime_request_init(&request);
    assert(request.api_version == UMI_PYTHON_RUNTIME_API_VERSION);
    request.preferred_executable = "__umicom_python_that_does_not_exist__";
    request.explicit_root = "__umicom_root_that_does_not_exist__";
    request.validate_version = 0;
    (void)umi_python_runtime_discover(&request, &snapshot);
    return 0;
}
