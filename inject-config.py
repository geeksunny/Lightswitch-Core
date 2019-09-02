"""
This script parses values from config.json for use in the build environment as value macros.
"""
import json


def parseConfig(d, prefix=''):
    if prefix is not '':
        prefix += '_'
    result = []
    for key in d:
        t = type(d[key])
        if t is dict:
            result.extend(parseConfig(d[key], key))
            continue
        elif t in (str, unicode):
            if d[key] == "":
                result.append(str(prefix+key))
                continue
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
    cfg.extend(parseConfig(json.load(f)))


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
