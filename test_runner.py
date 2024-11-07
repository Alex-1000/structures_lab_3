#!/bin/env python3

import shlex
import sys
import subprocess

command, *arg = shlex.split("".join(sys.argv[1:]))
arg = "".join(arg)
output = bytes(
    "".join(c for c in arg if not c.isnumeric()).replace("+", "++").replace("-", "--"),
    encoding="UTF-8",
)
result = subprocess.run([command, arg], capture_output=True)
if output not in result.stdout:
    print(
        "Test has not passed. Program output:",
        result.stdout,
        "Error log:",
        result.stderr,
        sep="\n",
    )
    sys.exit(1)
sys.exit(0)
