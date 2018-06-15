#!/usr/bin/env python

import os
import sys


#==============================================================================#
def message(key, val_is, val_should_be, err_code):
    """
    Print message about test and exit if non-zero error code
    """
    if err_code != 0:
        print("Environment variable {} is \n\t'{}'\nshould be\n\t'{}'".format(
              key, val_is, val_should_be))
        sys.exit(err_code)
    else:
        print("EXAMPLE_ENV_{} is '{}' ".format(key, val_is))


#==============================================================================#
if __name__ == "__main__":

    _test_env_a_correct = "Successfully set example environment variable A"
    _test_env_b_correct = "Successfully set example environment variable B"

    _test_env_a = os.environ.get("EXAMPLE_ENV_A")
    _test_env_b = os.environ.get("EXAMPLE_ENV_B")

    if _test_env_a is None:
        message("A", "not set (None)", _test_env_a_correct, 1)
    elif _test_env_a != _test_env_a_correct:
        message("A", _test_env_a, _test_env_a_correct, 2)
    else:
        message("A", _test_env_a, _test_env_a_correct, 0)

    if _test_env_b is None:
        message("B", "not set (None)", _test_env_b_correct, 3)
    elif _test_env_b != _test_env_b_correct:
        message("B", _test_env_b, _test_env_b_correct, 4)
    else:
        message("B", _test_env_b, _test_env_b_correct, 0)

    sys.exit(0)
