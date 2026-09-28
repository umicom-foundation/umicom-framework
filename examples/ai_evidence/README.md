# AI source selection and citation inspection

This C23 example uses the canonical AI Workspace, AI runtime and Data Server.
It is a dependency-subset host, not a replacement complete Framework SDK.
`umicom-ai-evidence --self-test` and `demo` run the labelled extractive preview in memory.
They neither start a model nor access files or a network. `demo` also prints the captured report.

The explicit `local --port 8080 --model local-model --acknowledge-local` route sends the
built-in fictional Notes passages and prompt once through the existing loopback HTTP provider.
It requires an already running compatible server; no model or server is bundled or downloaded.
The CLI deadline is ten seconds, and there is no automatic retry. The returned prose is not
proof that the model obeyed its sources. A non-zero result must be investigated; 77 is unavailable.

## Focused build

```powershell
Set-Location "C:\umicom\Umicom-Applications"
$env:Path = "C:\msys64\ucrt64\bin;$env:Path"
cmake -S ".\framework\examples\ai_evidence" -B ".\build\ai-evidence" -G Ninja `
    -DCMAKE_BUILD_TYPE=Release -DUMICOM_AI_WORKSPACE_LOCAL_HTTP=AUTO
if ($LASTEXITCODE -ne 0) { throw "Configure failed." }
cmake --build ".\build\ai-evidence" --parallel 2
if ($LASTEXITCODE -ne 0) { throw "Build failed." }
ctest --test-dir ".\build\ai-evidence" --parallel 2 --no-tests=error --output-on-failure
if ($LASTEXITCODE -ne 0) { throw "Tests failed." }
& ".\build\ai-evidence\bin\umicom-ai-evidence.exe" --self-test
if ($LASTEXITCODE -ne 0) { throw "Lesson failed." }
```

`UMICOM_AI_WORKSPACE_LOCAL_HTTP=ON` requires libcurl and json-c via pkg-config;
`OFF` deliberately excludes network transport. `UMICOM_AI_EVIDENCE_SQLITE=OFF` deliberately
excludes the SQLite backend. Neither mode reports unavailable checks as successful execution.
`UMICOM_AI_EVIDENCE_SANITIZERS=ON` instruments supported GCC/Clang builds with ASan and UBSan.

The new loopback fixture is C on Windows/Linux. The older Python fixtures remain unchanged
as separate alternatives. Disable Python package discovery with
`-DCMAKE_DISABLE_FIND_PACKAGE_Python3=TRUE` to verify the native route independently.

`lesson.c` is the complete example. `installed_client` uses only public headers and
`Umicom::ai_workspace` from an installed SDK. The full public guide is
`docs/learning/ai-evidence.html`; development contracts are in `docs/development/ai-evidence.md`.
