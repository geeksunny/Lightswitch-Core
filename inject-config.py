"""
This script parses values from config.json for use in the build environment as value macros.
The order of values in the source json file is expected to be maintained.
"""
import json
import re
import sys
from collections import OrderedDict


# Python 2/3 compatibility
STR_TYPE = bytes if (sys.version_info > (3, 0)) else unicode


def parseConfig(d, prefix=''):
    if prefix != '':
        prefix += '_'
    result = []
    for key in d:
        t = type(d[key])
        if t in (dict, OrderedDict):
            result.extend(parseConfig(d[key], key))
            continue
        elif t in (str, STR_TYPE):
            if d[key] == "":
                result.append(str(prefix+key))
                continue
            elif d[key][0] == '`' and d[key][:1] == '`':
                val = re.escape(d[key][1:-1])
            else:
                val = "\\\"%s\\\"" % d[key]
        elif t is bool:
            val = str(d[key]).lower()
        else:
            val = d[key]
        result.append((str(prefix+key), val))
    return result


cfg = []
with open('config.json') as f:
    cfg.extend(parseConfig(json.load(f, object_pairs_hook=OrderedDict)))


if cfg:
    output = []
    for item in cfg:
        t = type(item)
        if t is tuple:
            output.append("-D%s=%s" % item)
        elif t is str:
            output.append("-D%s" % item)
        else:
            pass
    print(' '.join(output))
