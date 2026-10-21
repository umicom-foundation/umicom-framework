# Windows Security Blocks a Native CTest Executable — Investigation Guide

**Author:** Sammy Hegab · **Organisation:** Umicom Foundation · **Licence:** MIT

## The incident

During the native AI laboratory test run on **21 October 2026 at 23:38**,
security software displayed **"We protected your PC by stopping a threat"**
for `umicom-native-llm-experiment.exe` under the `windows-ucrt64-native-llm-debug`
build folder. CTest reported:

```text
framework.ai.training.accumulation_demo  ***Not Run (BAD_COMMAND)
Process not started: umicom-native-llm-experiment.exe
```

The crucial clue is **Process not started**. This does not establish that the
experiment's calculations failed, nor does it establish that the executable is
safe. A security product blocked execution before CTest could execute the test.
The observed wording closely resembles McAfee protection alerts, but the
specific installed security product and detection name must be verified.

## Safety-first response

Do not disable real-time protection, rename the executable to evade detection,
add blanket `C:\umicom` exclusions, automatically restore a quarantined binary,
or suppress the failed CTest as if it passed. Keep the incident visible until
security software has reviewed and cleared the *specific* executable.

The source code for the experiment command is a native C23 educational client:
it loads a reviewed local experiment plan and source manifest; calculates a
small model training experiment; it only saves a checkpoint if an explicit new
path is passed. It contains no deliberate network connectivity or arbitrary
process execution. This source review cannot verify the binary that was
actually compiled on the user's Windows machine.

## Read-only PowerShell triage

Open PowerShell; these commands do not turn off protection or launch the
quarantined executable.

```powershell
Set-Location "C:\umicom\Umicom-Applications"

$exe = Join-Path (Get-Location) `
  "build\windows-ucrt64-native-llm-debug\umicom-native-llm-experiment.exe"

Get-CimInstance -Namespace root/SecurityCenter2 -ClassName AntiVirusProduct |
  Select-Object displayName, productState, pathToSignedProductExe

Test-Path -LiteralPath $exe

if (Test-Path -LiteralPath $exe) {
  Get-FileHash -LiteralPath $exe -Algorithm SHA256
  Get-AuthenticodeSignature -LiteralPath $exe |
    Select-Object Status, StatusMessage, SignerCertificate
  Get-Item -LiteralPath $exe | Select-Object FullName, Length, LastWriteTime
}

try {
  Get-MpThreatDetection | Where-Object {
    $_.Resources -match 'umicom-native-llm-experiment'
  } | Format-List *
} catch {
  Write-Host "Microsoft Defender history unavailable; inspect your installed antivirus history."
}
```

An executable already moved into quarantine may no longer exist at `$exe`.
That does **not** prove it was deleted from the source tree or never built.
Windows Defender's cmdlet reports its own detections, not McAfee's quarantine
history. If McAfee is the active provider, review **McAfee → My Protection →
Quarantined Items / security history** to record the full detection name,
rule/engine version, action and original pathname.

**Evidence to capture:** vendor/product version, exact detection family/name,
file SHA-256 if still available, whether the file was quarantined or only
blocked, the UTC/local time, antivirus definition version, your OS release,
and the precise compiler/build preset used. Avoid uploading secrets or
unreviewed proprietary source data as part of a false-positive report.

## Vendor false-positive review

McAfee provides an official **Dispute a Detection & Allowlisting Data** form:
https://www.mcafee.com/en-us/consumer-support/dispute-detection-allowlisting.html

Submit the *exact detected build* with its hash and engineering context for
classification. Do not submit a renamed replacement. If Defender rather than
McAfee is confirmed, use Microsoft's official file-submission portal:
https://www.microsoft.com/en-us/wdsi/filesubmission

The antivirus provider, not a CMake variable, must determine whether a flagged
binary is a false positive. An updated verified verdict and definitions can
allow testing without weakening protection. Publisher Authenticode signing is
appropriate for reviewed distributed release executables; it can improve
publisher reputation but it never guarantees exemption from malware scanning.
Never commit signing keys or certificates containing private keys.

## Keep test evidence honest

The full test registration remains unchanged: `accumulation_demo` continues
to launch the `umicom-native-llm-experiment` binary. This delivery neither
renames it nor conceals its result. While the antivirus issue is unresolved,
other CTests may be evaluated separately:

```powershell
ctest --test-dir ".\build\windows-ucrt64-native-llm-debug" `
  --output-on-failure `
  -E '^framework\.ai\.training\.accumulation_demo$'
```

**Report the exclusion explicitly** — other tests passing does not mean the
full suite passed. After the vendor has cleared the exact program, restore and
rebuild from trusted source as necessary, verify the hash, then run:

```powershell
ctest --test-dir ".\build\windows-ucrt64-native-llm-debug" `
  --output-on-failure `
  -R '^framework\.ai\.training\.accumulation_demo$'

ctest --test-dir ".\build\windows-ucrt64-native-llm-debug" `
  --output-on-failure --no-tests=error
```

If antivirus blocks another binary, follow the same process. Do not design
renaming or packer changes merely to evade antivirus analysis. A build-time
malware report is a security incident to investigate, not a reason to disable
endpoint protection.

## Additional evidence: repeated detections across two Umicom applications

A subsequent screenshot of the quarantined-items history shows **three
separate detections**, not one. The displayed local timestamps and identifiers
are:

| Local timestamp shown | Quarantined basename | Displayed detection ID |
|---|---|---|
| 19 October 2026, 09:17 | `umicom-studio-composition-lifecycle-test.exe` | `ti!7371548EA557` |
| 21 October 2026, 23:38 | `umicom-native-llm-experiment.exe` | `ti!EFD3C6591343` |
| 22 October 2026, 00:30 | `umicom-native-llm-experiment.exe` | `ti!8B9E296A0D14` |

The first and second occurrences are consistent with the earlier security
notification and CTest `Process not started` result. The third shows a second
quarantined instance of the same experiment executable name. Different IDs do
not, by themselves, prove that any two binaries are identical or unrelated.
The short `ti!` strings are vendor display identifiers; **they are not a
specific named malware family diagnosis**. McAfee Labs has published `Ti!`
identifiers matching the initial twelve digits of a corresponding sample
SHA-256, so compare the actual full digest rather than assuming that a
short display name proves a shared malware family. They must be interpreted in
conjunction with the full SHA-256 digest, detection engine, signature/version,
and the vendor's analysis.

This broadens the scope of review: investigate the provenance of the shared
compiler/toolchain and common Framework dependencies, and the Studio test's
build input, **in addition to** the specific AI executable. Neither success
of the other CTests nor publication of the Umicom source can certify these
quarantined Windows binaries as safe.

### Read-only evidence commands

```powershell
Set-Location 'C:\umicom\Umicom-Applications'
$ai = '.\build\windows-ucrt64-native-llm-debug\umicom-native-llm-experiment.exe'
$studio = '.\build\windows-ucrt64-all-debug\bin\umicom-studio-composition-lifecycle-test.exe'

foreach ($file in @($ai, $studio)) {
    if (Test-Path -LiteralPath $file -PathType Leaf) {
        Get-Item -LiteralPath $file | Select-Object FullName, Length, LastWriteTimeUtc
        Get-FileHash -LiteralPath $file -Algorithm SHA256
        Get-AuthenticodeSignature -LiteralPath $file |
            Select-Object Status, StatusMessage
    } else {
        Write-Host "Missing or quarantined: $file"
    }
}

# Identify the configured build tools without launching suspect test binaries.
Get-Command cmake, ninja, gcc, clang -ErrorAction SilentlyContinue |
    Select-Object Name, Source

# Confirm exactly which executable the retained CTest definition launches.
ctest --test-dir '.\build\windows-ucrt64-native-llm-debug' `
    -N -V -R '^framework\.ai\.training\.accumulation_demo$'
```

A quarantined file may not be available for hashing from PowerShell. Do not
restore it merely to obtain a hash. Ask McAfee for the complete digest or
collect it from the original build evidence before quarantine, if available.
If `ctest -N -V` reports another command, investigate the CMake test
registration before changing or rerunning anything. Reviewing CTest's
registered command does not execute the test itself.

### What to report to the vendor

Submit the above three detection IDs **separately**, with each original path,
local timestamp, precise file hash (where available) and vendor product and
signature versions. Explain that these are locally compiled native C23/GTK
educational and IDE regression-test executables from the Umicom Foundation
repository; never assert they are safe without a verdict. Request an analysis
of each detection and, if confirmed erroneous, the relevant definition or
reputation correction. McAfee's official support process accepts reviewed
files via the Dispute a Detection & Allowlisting Data form. Security tools
should remain enabled during the review.

Do not send entire unreviewed workspaces, API keys, private repositories or
personal datasets to external multi-engine scanning services. Confirm legal
permission before submitting binaries or source code to any vendor.
