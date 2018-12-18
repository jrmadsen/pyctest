#!/usr/bin/env python

import os, sys, platform
import pyctest.pyctest as pyctest
import pyctest.helpers as helpers

if __name__ == "__main__":
    cwd = os.getcwd()
    args = helpers.ArgumentParser("PyCTest", cwd, cwd,
                                  vcs_type="git", use_launchers=True,
                                  build_type="MinSizeRel").parse_args()
    pyctest.DROP_SITE = "cdash.nersc.gov"
    pyctest.CONFIGURE_COMMAND = "{} setup.py configure".format(pyctest.PYTHON_EXECUTABLE)
    pyctest.BUILD_COMMAND = "{} setup.py install".format(pyctest.PYTHON_EXECUTABLE)

    test_dir = os.path.join(pyctest.SOURCE_DIRECTORY, "examples", "Basic")
    test = pyctest.test(name="basic", cmd=["python", "basic.py", "--", "-VV"],
                        properties={ "WORKING_DIRECTORY" : test_dir })

    pyctest.run()
