"""Exercise CLI contracts using only Python's standard library."""
import pathlib
import subprocess
import sys
import tempfile

binary = str(pathlib.Path(sys.argv[1]).resolve())
cases = [
    ('# comment\na 1\nb 9 a\n', 0, 'a\nb\n'),
    ('a 1oops\n', 1, ''),
    ('a 999999999999999999999\n', 1, ''),
    ('a\n', 1, ''),
    ('a 0 missing\n', 1, ''),
    ('a 1 b\nb 1 a\n', 1, ''),
    ('independent 9\na 1 b\nb 1 a\n', 1, ''),
    ('\n# empty\n', 0, ''),
]
with tempfile.TemporaryDirectory() as directory:
    path = pathlib.Path(directory) / 'input.tasks'
    for content, code, output in cases:
        path.write_text(content)
        result = subprocess.run([binary, str(path)], capture_output=True, text=True)
        if result.returncode != code or result.stdout != output:
            raise AssertionError((content, result.returncode, result.stdout, result.stderr))
        if code and not result.stderr:
            raise AssertionError('Missing error diagnostic')
    for args in ([], [str(path) + '.missing']):
        result = subprocess.run([binary, *args], capture_output=True, text=True)
        if result.returncode != 2:
            raise AssertionError('Expected usage/open error')
print('10 CLI cases passed')
