# Compare completed reports using the installed SDK

This C23 example reads two completed CTest JUnit files and prints their saved outcome changes. It uses the installed public comparison service without GTK or Studio.

1. Build and install a matching Framework SDK before configuring this example. Use a fresh installation prefix to avoid mixing old headers and libraries.
2. From the Framework checkout, configure the independent consumer with that absolute prefix:

   ```powershell
   $env:Path = "C:\msys64\ucrt64\bin;$env:Path"
   cmake -S ".\examples\report_comparison" -B ".\build\report-comparison-consumer" -G Ninja -DCMAKE_C_COMPILER=C:/msys64/ucrt64/bin/gcc.exe -DUMICOM_SDK_PREFIX=C:/umicom/installed/applications -DBUILD_TESTING=ON
   cmake --build ".\build\report-comparison-consumer" --parallel 2
   ctest --test-dir ".\build\report-comparison-consumer" --output-on-failure --no-tests=error
   ```

   Run each command only after the previous one succeeds. The SDK guard checks imported targets and installed header fingerprints; do not add source-tree include paths to bypass a missing package.
3. The six synthetic checks cover recovery, regression, removal, unchanged failure, empty reports and malformed input. They retain complete input files and a consumer log in a fresh directory beneath the build tree. Running from a different working directory exercises absolute-path handling.
4. To inspect your own completed files:

   ```powershell
   & ".\build\report-comparison-consumer\report-comparison-consumer.exe" "C:\umicom\_local\logs\testing\before.xml" "C:\umicom\_local\logs\testing\after.xml"
   $LASTEXITCODE
   ```

Exit 0 means this example read and printed the comparison; it does **not** mean tests passed. Exit 2 reports an argument, input or output error. The separate `umicom test-report --compare` command has its own qualification-oriented exit policy. Do not substitute this display example for a test gate.

Only exact decoded test names and outcomes are compared. A removed failure is not recovered, and an unchanged report may still contain failures. Both files must be complete and stable while reading. Neither source identities nor host capabilities are verified by this example.

Pages contain at most 64 changed records, with totals and explicit limitations. Each report is bounded by the shared reader's 64 MiB limit. For input errors, inspect the retained log and confirm that the inputs are CTest JUnit XML, not console transcripts. See the [saved-report guide](../../docs/guides/saved-test-report-browser.html) for native integration and SDK ownership rules.

The output begins with each input's byte count and SHA-256 digest. These are
calculated from the exact buffer used to parse that report, including whitespace
and line endings. The example checks compare them with CMake's independent file
hash. Different formatting can produce the same outcome comparison with different
digests. A digest identifies content; it does not identify a trusted author,
source revision, or the machine that ran the tests. Rejected pairs publish no
accepted identity.
