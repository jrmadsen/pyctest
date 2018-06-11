#!/usr/bin/env python

import os
import sys

def echo_dart_tag(name, filepath, filetype):
    """
    Printing this string will upload the results to CDash when running CTest
    """
    print('<DartMeasurementFile name="{}" '.format(name) +
    'type="{}">{}</DartMeasurementFile>'.format(filetype, filepath))


if __name__ == "__main__":
    try:
        args = sys.argv[1:]
        for i in range(0, len(args), 3):
            if i+2 < len(args):
                echo_dart_tag(args[i], args[i+1], args[i+2])
    except Exception as e:
        print("{}".format(e))
