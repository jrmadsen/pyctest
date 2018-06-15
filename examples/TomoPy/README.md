# TomoPy Example

## Overview

- Downloads Miniconda in `PyCTestPreInit.cmake`
- Installs conda packages in `PyCTestPreInit.cmake`
- Checkouts source code of Python package via `git clone ...` in `tomopy.py`
- Build source code via `python setup.py install` when running CTest
  - Warnings are logged in "Build" section of dashboard
- Generates a test around the `nosetests` unit testing
- Generates tests around several algorithms by calling `run_tomopy.py`
- Submits to CDash dashboard at `cdash.nersc.gov`
- Timing and memory plots provided via TiMemory
- Attaches CTest notes (e.g. ASCII results)
- Attaches images from reconstruction (e.g. Dart measurement files)

Results from running the TomoPy example can be found at the [TomoPy CDash Testing Dashboard @ NERSC](https://cdash.nersc.gov/index.php?project=TomoPy)

### Dependencies
- Python packages
  - gitpython (optional)

### Optional setup
$ git clone https://github.com/tomopy/tomopy.git tomopy-src

### Run and submit to dashboard
```bash
$ ./tomopy.py
```
