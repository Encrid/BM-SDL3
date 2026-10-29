# Copyright (C) 2026 Enkrid
#
# Under the GNU Affero General Public License, version 3 only: see license.
import sys

root, dist, out = sys.argv[1:4]
sys.path.insert(0, root + '/.toolchain/tools')
sys.argv = ['build.py', '--root', root]

import build
import shared

config, values = build.prepared(shared.arguments())
values.update(dist=dist, out=out)
build.composed(config, values)
