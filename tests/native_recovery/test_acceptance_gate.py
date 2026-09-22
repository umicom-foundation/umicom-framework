# -----------------------------------------------------------------------------
# Umicom Framework
# File: tests/native_recovery/test_acceptance_gate.py
# PURPOSE: Test the evidence gate with hostile/incomplete reports and real CTest.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
# -----------------------------------------------------------------------------
"""These are evidence-utility tests, not evidence that native GTK cases passed."""
from __future__ import annotations
import importlib.util
import json
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[2]
SPEC = importlib.util.spec_from_file_location("native_acceptance", ROOT / "tools/testing/native_acceptance.py")
assert SPEC is not None and SPEC.loader is not None
GATE = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(GATE)


class ReportTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(prefix="umicom-evidence-unit-")
        self.addCleanup(self.temp.cleanup)
        self.path = Path(self.temp.name) / "report.xml"

    def verify(self, body, expected=False, code=0, markers=None):
        self.path.write_text(body, encoding="utf-8")
        actual, _ = GATE.check_report(self.path, "notes.save[1]+", code, markers)
        self.assertEqual(actual, expected)

    def test_accept(self):
        self.verify('<testsuite tests="1" failures="0"><testcase name="notes.save[1]+" status="run"/></testsuite>', True)

    def test_empty(self): self.verify('')
    def test_malformed(self): self.verify('<testsuite>')
    def test_wrong_identity(self): self.verify('<testsuite><testcase name="other"/></testsuite>')
    def test_missing_case(self): self.verify('<testsuite tests="0"/>')
    def test_duplicate(self): self.verify('<testsuite><testcase name="notes.save[1]+"/><testcase name="notes.save[1]+"/></testsuite>')
    def test_failure(self): self.verify('<testsuite><testcase name="notes.save[1]+"><failure/></testcase></testsuite>')
    def test_error(self): self.verify('<testsuite><testcase name="notes.save[1]+"><error/></testcase></testsuite>')
    def test_skip(self): self.verify('<testsuite><testcase name="notes.save[1]+"><skipped/></testcase></testsuite>')
    def test_disabled(self): self.verify('<testsuite><testcase name="notes.save[1]+" status="disabled"/></testsuite>')
    def test_nonzero_process(self): self.verify('<testsuite><testcase name="notes.save[1]+"/></testsuite>', code=8)
    def test_counter(self): self.verify('<testsuite failures="1"><testcase name="notes.save[1]+"/></testsuite>')
    def test_bad_counter(self): self.verify('<testsuite failures="bad"><testcase name="notes.save[1]+"/></testsuite>')
    def test_wrong_count(self): self.verify('<testsuite tests="3"><testcase name="notes.save[1]+"/></testsuite>')
    def test_declaration(self): self.verify('<!DOCTYPE testsuite [<!ENTITY x "value">]><testsuite><testcase name="notes.save[1]+"/></testsuite>')
    def test_nul(self): self.verify('<testsuite>\x00</testsuite>')
    def test_output_skip(self): self.verify('<testsuite><testcase name="notes.save[1]+"><system-out>SKIP: GTK unavailable</system-out></testcase></testsuite>', markers=['SKIP:'])
    def test_missing_file(self): self.assertFalse(GATE.check_report(self.path, "notes.save[1]+", 0)[0])
    def test_limit(self):
        self.path.write_bytes(b'x' * (GATE.MAX_REPORT_BYTES + 1))
        self.assertFalse(GATE.check_report(self.path, "notes.save[1]+", 0)[0])
    def test_valid_plan(self):
        self.path.write_text(json.dumps({'tests':[{'name':'notes.save[1]+'}]}))
        self.assertEqual(len(GATE.read_plan(self.path)), 1)
    def test_empty_plan(self):
        self.path.write_text('{"tests":[]}')
        with self.assertRaises(ValueError): GATE.read_plan(self.path)
    def test_duplicate_plan(self):
        self.path.write_text('{"tests":[{"name":"same"},{"name":"same"}]}')
        with self.assertRaises(ValueError): GATE.read_plan(self.path)
    def test_control_name(self):
        self.path.write_text(json.dumps({'tests':[{'name':'test\nname'}]}))
        with self.assertRaises(ValueError): GATE.read_plan(self.path)


class RealCTestTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.ctest = shutil.which("ctest")
        cls.cmake = shutil.which("cmake")
        if cls.ctest is None or cls.cmake is None:
            raise RuntimeError("CTest and CMake are required; these integration checks must not silently skip")
        cls.temp = tempfile.TemporaryDirectory(prefix="umicom real ctest ")
        cls.addClassCleanup(cls.temp.cleanup)
        cls.root = Path(cls.temp.name)
        source = cls.root / "source"
        source.mkdir()
        (source / "CMakeLists.txt").write_text('''cmake_minimum_required(VERSION 3.24)
project(UmicomAcceptanceFixture NONE)
enable_testing()
add_test(NAME "notes.save[1]+" COMMAND "${CMAKE_COMMAND}" -E true)
add_test(NAME "notes.save1" COMMAND "${CMAKE_COMMAND}" -E false)
add_test(NAME "notes.failure" COMMAND "${CMAKE_COMMAND}" -E false)
add_test(NAME "notes.disabled" COMMAND "${CMAKE_COMMAND}" -E true)
set_tests_properties("notes.disabled" PROPERTIES DISABLED TRUE)
add_test(NAME "notes.fake-pass" COMMAND "${CMAKE_COMMAND}" -E echo "SKIP: no display")
''', encoding="utf-8")
        cls.build = cls.root / "build"
        subprocess.run([cls.cmake, "-S", str(source), "-B", str(cls.build)],
                       check=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)

    def run_case(self, name, expected):
        directory = GATE.run_acceptance(self.ctest, self.build, self.root / "evidence",
            [{'name':name, 'reject_output':['SKIP:']}], 'Debug')
        data = json.loads((directory / 'summary.json').read_text())
        self.assertEqual(data['accepted'], expected)
        self.assertEqual(len(data['results']), 1)
        self.assertTrue((directory / 'summary.html').is_file())
        return directory

    def test_literal_selection(self): self.run_case('notes.save[1]+', True)
    def test_actual_failure(self): self.run_case('notes.failure', False)
    def test_actual_disabled(self): self.run_case('notes.disabled', False)
    def test_zero_match(self): self.run_case('notes.absent', False)
    def test_textual_false_pass(self): self.run_case('notes.fake-pass', False)
    def test_no_stale_reuse(self):
        first = self.run_case('notes.save[1]+', True)
        second = self.run_case('notes.save[1]+', True)
        self.assertNotEqual(first, second)
        self.assertTrue((first / '001.xml').is_file())


if __name__ == '__main__':
    unittest.main(verbosity=2)
