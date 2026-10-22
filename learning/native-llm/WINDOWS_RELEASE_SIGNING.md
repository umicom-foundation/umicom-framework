# Umicom Windows release signing and antivirus review

**Owner:** Sammy Hegab · **Organisation:** Umicom Foundation · **Implementation policy:** C23/Assembly for reusable runtime services; native Windows SDK for signing · **Licence:** MIT.

## Current security incident is not closed

McAfee quarantined two builds of `umicom-native-llm-experiment.exe` and an Umicom Studio composition-lifecycle test. The presence of `ti!` detection identifiers is not a verdict on their safety. We do not have a vendor determination, a copy of the quarantined binary, its complete hash, or a clean independent analysis. A blocked CTest `BAD_COMMAND` means Windows did not start the test, not that its assertions passed or failed.

Do **not** restore or run the quarantined files merely because they were locally compiled. Do not exclude `C:\umicom` from scanning, turn off malware protection, attempt to modify binaries to avoid heuristic detection, or sign those suspect test binaries. Record McAfee's event identifiers, the exact file hashes if accessible, compiler provenance, build logs, linked DLLs, and whether the same detection occurs on an isolated clean Windows builder. Submit the affected files to McAfee's false-positive review process when the files can be handled safely and lawfully. Re-run the entire native test suite **only after** the blocked executable is cleared and available. Other unaffected tests may be run separately, but the result is a partial qualification.

Official McAfee guidance: https://www.mcafee.com/support/s/article/000001921?id=35533&language=en_US

## What Windows signing will and will not change

Authenticode binds executable bytes to a verified publisher certificate and detects later changes. An RFC 3161 timestamp helps preserve a signature's validity after certificate expiry. A signed executable can still be harmful; security software must inspect it independently. A new signed release can still trigger Windows SmartScreen, and **McAfee's vendor-specific heuristic detection is a separate decision**. Paid EV signing does not automatically confer positive SmartScreen reputation. The usual verified-publisher choices are an organisational OV code-signing certificate or Microsoft Artifact Signing, subject to the publisher's identity, location and eligibility. A self-signed development certificate is **not** suitable as a public trusted release identity.

Microsoft references:
- https://learn.microsoft.com/en-us/windows/win32/seccrypto/signtool
- https://learn.microsoft.com/en-us/windows/win32/seccrypto/time-stamping-authenticode-signatures
- https://learn.microsoft.com/en-us/windows/apps/package-and-deploy/smartscreen-reputation
- https://learn.microsoft.com/en-us/azure/artifact-signing/faq

## Approved release order

1. Review Framework and product source and dependency provenance. Inspect anomalous AV detections, stop on unresolved findings, and **do not sign suspect or quarantined test executables**.
2. Build a clean, reproducible Windows Release configuration in a controlled builder using the reviewed GCC/Clang/MSVC compiler chain. Record compiler versions, the Git commit and submodule revisions. Quarantine and signing are security processes, not modifications to the C23 model.
3. Build and run the complete authorised test suite, including separate clean-host checks; correct all failing or blocked tests before approval. Do not treat Linux-only green tests as Windows qualification.
4. Obtain a publicly trusted code-signing identity or qualifying managed signing service. Keep private keys or signing-service credentials outside the source tree, repositories, ZIPs and CI logs. Restrict signing access to reviewed product release artefacts.
5. **Sign after final compilation and executable modification**, before creating release packages; sign each intended Win32 EXE and native DLL with SHA-256 and an approved RFC 3161 timestamp. The installer is signed separately after packaging. Do not modify signed bytes after signing.
6. Verify Authenticode's policy, signer identity and timestamp, then calculate final signed-file SHA-256 hashes. Publish an independently reviewed release manifest (build provenance, version, commit, signed hashes). Submit any later false positive as a specific signed hash to the security vendor.

## Interactive PowerShell — inspection only

```powershell
Set-Location "C:\umicom\Umicom-Applications"
$exe = ".\build\windows-ucrt64-all-release\bin\umicom-llm-console.exe"
if (-not (Test-Path -LiteralPath $exe -PathType Leaf)) {
    throw "Approved release executable not present; no signature check performed"
}
Get-FileHash -LiteralPath $exe -Algorithm SHA256
Get-AuthenticodeSignature -LiteralPath $exe |
    Format-List Status,StatusMessage,SignerCertificate,TimeStamperCertificate
Get-Command signtool.exe -ErrorAction SilentlyContinue
```

## Manual signing with Windows SDK SignTool

Use this only for an **approved, clean Release executable** and an already-provisioned, trusted publisher identity; the placeholders are not valid production values. Microsoft documents `signtool sign /fd SHA256` and the RFC 3161 pair `/tr` and `/td SHA256`. The timestamp URL must be obtained from the certificate authority or signing provider; it is not hard-coded in Umicom source.

```powershell
$signTool = (Get-Command signtool.exe -ErrorAction Stop).Source
$releaseExecutable = "C:\umicom\Umicom-Applications\build\windows-ucrt64-all-release\bin\umicom-llm-console.exe"
$publisherThumbprint = "<40-digit-thumbprint-of-approved-code-signing-cert>"
$timestampUrl = "<approved-https-rfc3161-timestamp-url>"

# REVIEW FIRST. Never run these commands against quarantined test binaries.
# After certificate verification and vendor clearance:
& $signTool sign /fd SHA256 /sha1 $publisherThumbprint /tr $timestampUrl /td SHA256 $releaseExecutable
if ($LASTEXITCODE -ne 0) { throw "Signing failed" }
& $signTool verify /pa /v $releaseExecutable
if ($LASTEXITCODE -ne 0) { throw "Authenticode verification failed" }
Get-FileHash -LiteralPath $releaseExecutable -Algorithm SHA256
Get-AuthenticodeSignature -LiteralPath $releaseExecutable
```

## Optional Framework CMake helper (explicit approval required)

`framework/cmake/UmicomWindowsReleaseSigning.cmake` provides `umicom_attach_windows_release_signing(target)`. The helper is **not included by any existing application build** and does not change defaults. When explicitly called it refuses non-Windows hosts, non-Release or multi-config builds, unapproved signing, missing signer thumbprints or timestamps, and a missing SignTool. It signs and verifies one approved EXE or DLL after that target's build completes. Do not attach it to CTest executables. Multi-config Visual Studio signing remains a separate engineering task.

Approved example **only after** McAfee case closure and acquisition of a CA identity:

```cmake
include("${CMAKE_SOURCE_DIR}/framework/cmake/UmicomWindowsReleaseSigning.cmake")
# Only an approved Release product target. Never attach to tests.
umicom_attach_windows_release_signing(umicom-llm-console)
```

The caller supplies `UMICOM_WINDOWS_SIGNING_APPROVED=ON`, `UMICOM_WINDOWS_SIGNER_THUMBPRINT`, `UMICOM_WINDOWS_TIMESTAMP_URL` and optionally `UMICOM_WINDOWS_SIGNTOOL` at CMake configure time. A certificate thumbprint is public metadata; signing-service authentication is intentionally absent. No code or documentation in this release disables scanning or suppresses a failed test.

## Distinguish three independently reviewable identities

- **Benchmark checkpoint SHA-256:** identity of the portable model file, not its author or capability.
- **Benchmark reviewed-case SHA-256:** identity of the ordered case text and binary data after canonical parsing, not proof of test independence.
- **Authenticode signature of EXE/DLL:** verified publisher identity and post-signing integrity of the executable bytes, not proof of model quality or malware innocence.

A content hash never substitutes for Authenticode; signing never substitutes for checking independent evaluation, developer approvals or antivirus-vendor findings.
