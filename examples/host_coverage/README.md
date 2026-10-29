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
3. The original four checks use synthetic evidence for passed, skipped, mismatched-source and malformed inputs. Each creates a fresh directory beneath the consumer build tree and retains its input files and log. The checks run from a different working directory to exercise manifest-relative report lookup.
4. Review your own plan with:

   ```powershell
   & ".\build\host-coverage-consumer\host-coverage-consumer.exe" "C:\umicom\_local\logs\testing\coverage.csv" "source-fingerprint-from-the-run"
   $LASTEXITCODE
   ```

The example prints pages of up to 64 requirements. Exit 0 means all explicitly listed requirements passed for the supplied identity, 1 means outstanding or empty coverage, and 2 means an input or output error. A saved identity is caller-supplied metadata, not proof of which source produced a report.

If configuration cannot find the SDK, check that the selected directory contains the Framework package, libraries and headers. Do not add a source-tree include path to bypass an incomplete installation. If an input is refused, inspect the retained log and the [host-coverage guide](../../docs/guides/test-host-coverage.html). This example neither runs the tests listed in the plan nor modifies their evidence.

## Focus on outstanding coverage

Keep the same manifest and source arguments, then add a display option:

```powershell
& ".\build\host-coverage-consumer\host-coverage-consumer.exe" "C:\umicom\_local\logs\testing\coverage.csv" "source-fingerprint-from-the-run" --outstanding
$LASTEXITCODE
& ".\build\host-coverage-consumer\host-coverage-consumer.exe" "C:\umicom\_local\logs\testing\coverage.csv" "source-fingerprint-from-the-run" --outstanding "editor"
$LASTEXITCODE
```

The optional last argument matches literal test-name or capability text, folding ASCII case. Use `--passed` to display saved passes. Every page includes the accepted filter and the original whole-review counts. The exit status still evaluates **all** listed requirements, even when the filtered display contains only passes or no rows.

The additional filtered-passed, filtered-empty and filtered-text checks expect exit 1 from synthetic mixed evidence. This checks that presentation cannot conceal an outstanding qualification requirement. These documented results are expected outcomes, not a claim that a particular SDK build has passed qualification.

## Read result messages without changing the outcome

Observed rows now include the saved result message. A skip return code identifies the outcome rule; it does not explain which prerequisite was missing. Read the original testcase output for that detail. A missing observation or a source mismatch does not borrow a message from another host. An observed result without a message says **not recorded**.

The additional `not-run`, `disabled`, `skip-pattern` and `message-controls` scenarios exercise the same public consumer against synthetic reports. They expect exit 1 because coverage remains outstanding. `message-controls` checks that a newline inside an imported message is displayed as `\x0A`, so its text cannot create a forged result row. The original evidence files must remain byte-for-byte unchanged. These checks are separate from executing the application tests named by a real coverage plan.

## Paths in different languages

The native entry point captures Windows Unicode arguments and converts them to
UTF-8 before passing paths to Framework. It leaves the process locale and working
directory unchanged. The unicode-path check places evidence under a directory
containing accented and Chinese characters, then reads it from another working
directory. Its expected exit status is zero; it does not run the tests described
by the saved report. Keep this check when adapting the example into another tool.
