![Umicom](../../resources/brand/umicom-logo.svg)

# Observe a local page with Umicom Automated Testing

This exercise runs Chromium in headless mode, which renders a page without
opening a visible browser window. Chromium writes the resulting document tree
as HTML to standard output. The test compares those bytes with the reviewed
`expected-dom.html` snapshot. The fixture calculates `6 × 7` after loading the
Umicom logo, so the expected total also checks that the local logo loaded.

## Run the exercise on Windows

1. Install a Framework SDK that includes the `testing` component. Use CMake,
   Ninja, a C23 compiler, and an installed Chrome or Chromium executable.
2. Set `$sdk` and `$browser` to your installed paths. Create a disposable folder
   for the build and browser profile, then build the example.

   ```powershell
   $sdk = 'C:\UmicomSDK'
   $browser = 'C:\Program Files\Google\Chrome\Application\chrome.exe'
   $lesson = Join-Path $sdk 'share/umicom/framework/examples/uat_browser'
   $work = Join-Path ([System.IO.Path]::GetTempPath()) ('umicom-browser-' + [guid]::NewGuid().ToString('N'))
   New-Item -ItemType Directory -Path $work | Out-Null
   cmake -S $lesson -B "$work/build" -G Ninja -DCMAKE_PREFIX_PATH="$sdk"
   cmake --build "$work/build"
   ```

3. Run the observation. PowerShell's `Uri` conversion turns the local page
   path into a `file:///` address, including any spaces in the path.

   ```powershell
   $page = ([uri](Join-Path $lesson 'fixture.html')).AbsoluteUri
   & "$work/build/umicom-uat-browser-observation.exe" $browser $work $page "$lesson/expected-dom.html"
   $LASTEXITCODE
   ```

   Expect exit `0`, `"accepted":true`, and the output summary
   `Process exited successfully and stdout matched.` The report records the
   web fixture, session and observation operation.

4. Try a deliberate mismatch. Create a copy of the snapshot in `$work`, change
   the text inside the `total` output from `42` to `43`, save with the same
   line endings and repeat the command with that copy. Expect exit `1` and
   `Process stdout differed from the expected bytes.`

## Investigate a refusal

Exit `2` identifies a missing or invalid fixture input. Verify the executable,
scratch path, local page address and snapshot location. Use a fresh scratch
folder if the browser reports that its profile is in use.

Exit `1` means the check ran without being accepted. Inspect the report's
status, process exit code and timeout. If the process succeeded but the
comparison failed, check the logo path and browser's document serialization.
The example reads the snapshot as text and prepares Chromium's Windows line
endings before the byte comparison. Compare browser versions before reviewing
a changed snapshot. Keep the deliberate
failure check whenever you update the expected output.

The fixture covers local browser observation and the native report. A timed
networked application or a workflow that manipulates controls needs a fixture
driver with the corresponding operations and permission from its owner.
