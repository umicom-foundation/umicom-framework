# Review saved coverage using the installed SDK

This C23 example reads a CSV plan and complete CTest JUnit reports through public Framework APIs. It demonstrates how another application can review required hosts without depending on Framework source files.

1. Build and install the Framework SDK before configuring this consumer. Use a fresh installation so headers and libraries come from the same build.
2. Configure this directory with an absolute installed prefix. For example, from the Framework checkout in PowerShell:

   ```powershell
   $env:Path = "C:\msys64\ucrt64\bin;$env:Path"
   cmake -S ".\examples\host_coverage" -B ".\build\host-coverage-consumer" -G Ninja -DCMAKE_C_COMPILER=C:/msys64/ucrt64/bin/gcc.exe -DUMICOM_SDK_PREFIX=C:/umicom/installed/applications -DBUILD_TESTING=ON
   cmake --build ".\build\host-coverage-consumer" --parallel 2
   ctest --test-dir ".\build\host-coverage-consumer" --output-on-failure --no-tests=error
   ```

   Run each command only after the previous one succeeds. The configure step rejects source-built targets, a package outside the selected prefix and mismatched installed headers.
3. The four checks use synthetic evidence for passed, skipped, mismatched-source and malformed inputs. Each creates a fresh directory beneath the consumer build tree and retains its input files and log. The checks run from a different working directory to exercise manifest-relative report lookup.
4. Review your own plan with:

   ```powershell
   & ".\build\host-coverage-consumer\host-coverage-consumer.exe" "C:\umicom\_local\logs\testing\coverage.csv" "source-fingerprint-from-the-run"
   $LASTEXITCODE
   ```

The example prints pages of up to 64 requirements. Exit 0 means all explicitly listed requirements passed for the supplied identity, 1 means outstanding or empty coverage, and 2 means an input or output error. A saved identity is caller-supplied metadata, not proof of which source produced a report.

If configuration cannot find the SDK, check that the selected directory contains the Framework package, libraries and headers. Do not add a source-tree include path to bypass an incomplete installation. If an input is refused, inspect the retained log and the [host-coverage guide](../../docs/guides/test-host-coverage.html). This example neither runs the tests listed in the plan nor modifies their evidence.
