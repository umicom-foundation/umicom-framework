#!/usr/bin/env python3
# -----------------------------------------------------------------------------
# Umicom Framework
# File: tools/testing/native_acceptance.py
# PURPOSE: Record exact CTest acceptance evidence without accepting skips,
#          absent tests, stale XML or unsuccessful CTest invocations as passes.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
# -----------------------------------------------------------------------------
"""Development evidence utility over the existing CTest runner, not a test engine.

Use only a trusted configured build. CTest reads its scripts and runs native
executables. This utility never edits source, creates commits or publishes code.
Each invocation owns a new evidence directory. Existing evidence is never reused.
"""
from __future__ import annotations

import argparse
from datetime import datetime, timezone
import hashlib
import html
import json
from pathlib import Path
import platform
import re
import shutil
import subprocess
import sys
from typing import Any
import uuid
import xml.etree.ElementTree as ET

MAX_REPORT_BYTES = 16 * 1024 * 1024
MAX_PLAN_BYTES = 1024 * 1024
MAX_REQUIRED_TESTS = 128


def read_plan(path: Path) -> list[dict[str, Any]]:
    """Validate all selectors before launching any test or creating evidence."""
    if path.stat().st_size > MAX_PLAN_BYTES:
        raise ValueError("The acceptance plan exceeds 1 MiB")
    plan = json.loads(path.read_text(encoding="utf-8"))
    entries = plan.get("tests") if isinstance(plan, dict) else None
    if not isinstance(entries, list) or not 1 <= len(entries) <= MAX_REQUIRED_TESTS:
        raise ValueError("The plan must contain between 1 and 128 tests")
    names: set[str] = set()
    for entry in entries:
        if not isinstance(entry, dict):
            raise ValueError("Every test entry must be an object")
        name = entry.get("name")
        if (not isinstance(name, str) or not name or len(name) > 1024
                or any(ord(c) < 32 or ord(c) == 127 for c in name)
                or name in names):
            raise ValueError("Test names must be nonempty, unique and free of controls")
        names.add(name)
        rejects = entry.get("reject_output", [])
        if (not isinstance(rejects, list) or len(rejects) > 32
                or any(not isinstance(s, str) or not s or len(s) > 1024 for s in rejects)):
            raise ValueError("reject_output must contain bounded nonempty literal strings")
    return entries


def check_report(path: Path, expected: str, returncode: int,
                 reject_output: list[str] | None = None) -> tuple[bool, str]:
    """Accept exactly one matching, completed case with a successful invocation."""
    if returncode != 0:
        return False, f"CTest returned {returncode}; the invocation was not accepted"
    try:
        if not path.is_file() or path.is_symlink():
            return False, "The fresh JUnit report is missing or is not a regular file"
        with path.open("rb") as stream:
            raw = stream.read(MAX_REPORT_BYTES + 1)
        if not raw or len(raw) > MAX_REPORT_BYTES:
            return False, "JUnit is empty or exceeds the 16 MiB limit"
        # CTest emits UTF-8. Reject alternate encodings/NULs and declarations
        # before parsing, rather than allowing entity expansion in evidence.
        text = raw.decode("utf-8-sig")
        if "\x00" in text or "<!doctype" in text.lower() or "<!entity" in text.lower():
            return False, "JUnit contains a forbidden declaration or encoding"
        root = ET.fromstring(text)
    except (OSError, UnicodeError, ET.ParseError) as error:
        return False, f"JUnit could not be read: {error}"
    if root.tag not in {"testsuite", "testsuites"}:
        return False, "Unexpected JUnit root"
    cases = list(root.iter("testcase"))
    if len(cases) != 1 or cases[0].get("name") != expected:
        return False, "JUnit does not contain exactly the requested test identity"
    case = cases[0]
    if case.get("status", "run").lower() not in {"run", "passed", "completed"}:
        return False, "The requested test was not run to completion"
    if any(node.tag in {"failure", "error", "skipped"} for node in root.iter()):
        return False, "JUnit reports failure, error or a skipped test"
    for suite in root.iter("testsuite"):
        for attribute in ("failures", "errors", "skipped", "disabled"):
            try:
                if int(suite.get(attribute, "0")) != 0:
                    return False, f"JUnit suite reports {attribute}"
            except ValueError:
                return False, "JUnit suite has an invalid outcome counter"
        if suite.get("tests") is not None and suite.get("tests") != "1":
            return False, "JUnit suite test count disagrees with the expected single case"
    output = "\n".join("".join(node.itertext()) for node in case
                       if node.tag in {"system-out", "system-err"})
    for marker in reject_output or []:
        if marker.lower() in output.lower():
            return False, f"Test output contains the refusal marker: {marker}"
    return True, "Exactly one requested test completed without a skip or failure"


def digest(path: Path) -> str:
    checksum = hashlib.sha256()
    with path.open("rb") as stream:
        for block in iter(lambda: stream.read(65536), b""):
            checksum.update(block)
    return checksum.hexdigest()


def write_summary(directory: Path, report: dict[str, Any]) -> None:
    """Publish a complete JSON record and a local, escaped HTML view."""
    candidate = directory / "summary.json.tmp"
    candidate.write_text(json.dumps(report, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    candidate.replace(directory / "summary.json")
    rows = "".join(
        "<tr><td>" + html.escape(item["name"]) + "</td><td>"
        + ("ACCEPTED" if item["accepted"] else "NOT ACCEPTED") + "</td><td>"
        + html.escape(item["reason"]) + "</td></tr>" for item in report["results"])
    (directory / "summary.html").write_text(
        '<!doctype html><html lang="en-GB"><meta charset="utf-8">'
        '<title>Umicom native acceptance evidence</title><style>'
        'body{font:17px/1.6 system-ui;max-width:1100px;margin:40px auto;padding:20px}'
        'table{border-collapse:collapse;width:100%}td,th{border:1px solid #bbb;padding:12px;text-align:left}'
        '</style><h1>Umicom native acceptance evidence</h1><p>'
        + ("ACCEPTED" if report["accepted"] else "NOT ACCEPTED")
        + ' — this result applies only to the named cases in this run.</p><p>Started: '
        + html.escape(report["started_utc"]) + '</p><table><tr><th>Test</th><th>Outcome</th>'
        '<th>Evidence</th></tr>' + rows + '</table><p>See summary.json and the numbered '
        'CTest logs and JUnit reports for invocation details and checksums.</p></html>', encoding="utf-8")


def run_acceptance(ctest: str, build_dir: Path, evidence_root: Path,
                   tests: list[dict[str, Any]], configuration: str) -> Path:
    """Run separate anchored selections. CTest retains ownership of test timeouts."""
    if not tests or len(tests) > MAX_REQUIRED_TESTS:
        raise ValueError("A nonempty bounded acceptance plan is required")
    if not (build_dir / "CTestTestfile.cmake").is_file():
        raise ValueError("The build directory has no CTestTestfile.cmake; configure and build first")
    executable = shutil.which(ctest)
    if executable is None:
        raise ValueError("The specified CTest executable was not found")
    build_dir = build_dir.resolve()
    evidence_root.mkdir(parents=True, exist_ok=True)
    stamp = datetime.now(timezone.utc).strftime("%Y%m%dT%H%M%SZ")
    directory = evidence_root.resolve() / (stamp + "-" + uuid.uuid4().hex[:12])
    directory.mkdir()  # Never overwrite a previous run or a previous JUnit file.
    report: dict[str, Any] = {
        "schema": 1, "started_utc": datetime.now(timezone.utc).isoformat(),
        "build_directory": str(build_dir), "ctest": str(Path(executable).resolve()),
        "configuration": configuration, "host": platform.platform(),
        "required_count": len(tests), "accepted": False, "completed": False, "results": [],
    }
    (directory / "required-tests.json").write_text(
        json.dumps({"tests": tests}, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    write_summary(directory, report)
    for index, entry in enumerate(tests, 1):
        name = entry["name"]
        xml = directory / f"{index:03}.xml"
        log = directory / f"{index:03}.log"
        command = [executable, "--test-dir", str(build_dir), "-C", configuration,
                   "--parallel", "1", "--no-tests=error", "--output-on-failure",
                   "--output-junit", str(xml), "-R", "^" + re.escape(name) + "$"]
        print(f"[{index}/{len(tests)}] {name}", flush=True)
        code = -1
        reason = "CTest did not complete"
        try:
            with log.open("xb") as stream:
                code = subprocess.run(command, stdout=stream, stderr=subprocess.STDOUT,
                                      shell=False, check=False).returncode
            accepted, reason = check_report(xml, name, code, entry.get("reject_output", []))
        except (OSError, KeyboardInterrupt) as error:
            accepted = False
            reason = f"Invocation interrupted or unavailable: {type(error).__name__}: {error}"
        record = {"name": name, "accepted": accepted, "reason": reason,
                  "returncode": code, "command": command, "log": log.name,
                  "junit": xml.name, "log_sha256": digest(log) if log.is_file() else None,
                  "junit_sha256": digest(xml) if xml.is_file() and not xml.is_symlink() else None}
        report["results"].append(record)
        write_summary(directory, report)
        print("  " + ("ACCEPTED: " if accepted else "NOT ACCEPTED: ") + reason, flush=True)
        if code == -1:
            break
    report["completed"] = len(report["results"]) == len(tests)
    report["accepted"] = report["completed"] and all(item["accepted"] for item in report["results"])
    report["finished_utc"] = datetime.now(timezone.utc).isoformat()
    write_summary(directory, report)
    return directory


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--ctest", required=True, help="Exact CTest executable or executable name")
    parser.add_argument("--build-dir", type=Path, required=True)
    parser.add_argument("--evidence-dir", type=Path, required=True)
    parser.add_argument("--tests-file", type=Path, required=True)
    parser.add_argument("--configuration", default="Debug")
    args = parser.parse_args()
    try:
        tests = read_plan(args.tests_file)
        destination = run_acceptance(args.ctest, args.build_dir, args.evidence_dir,
                                     tests, args.configuration)
        print(f"Evidence: {destination}")
        report = json.loads((destination / "summary.json").read_text(encoding="utf-8"))
        return 0 if report["accepted"] else 1
    except (OSError, ValueError) as error:
        print(f"Acceptance was not run: {error}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
