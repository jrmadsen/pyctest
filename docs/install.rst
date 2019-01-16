============
Installation
============

This section covers the basics of how to download and install PyCTest.

.. contents:: Contents:
   :local:

Supported Environments
======================

PyCTest is tested, built, and distributed for python 2.7, 3.6, and 3.7 on Linux/macOS through conda-forge.
Windows support is possible but Anaconda compiler issues within conda-forge with respect to ``std::unique_ptr``
are currently causing issues.

Installing from Conda (Recommended)
===================================

First, you must have `Conda <http://continuum.io/downloads>`_ installed,
then open a terminal or a command prompt window and run::

    $ conda install -c conda-forge pyctest

This will install PyCTest and all the dependencies from the conda-forge channel.

Updating the installation
-------------------------

PyCTest is an active project, so we suggest you update your installation
frequently. To update the installation run::

    $ conda update -c conda-forge pyctest

For some more information about using Conda, please refer to the
`docs <http://conda.pydata.org/docs>`__.

Installing from source
======================

Sometimes an adventurous user may want to get the source code, which is
always more up-to-date than the one provided by Conda (with more bugs of
course!).

For this you need to get the source from the
`PyCTest repository <https://github.com/jrmadsen/pyctest>`_ on GitHub.
Download the source to your local computer using git by opening a
terminal and running::

    $ git clone https://github.com/jrmadsen/pyctest.git

in the folder where you want the source code. This will create a folder called
`pyctest` which contains a copy of the source code.

Source installation is also available through `PyPi <https://pypi.org/project/pyctest/>`_::

    $ pip install -vvv pyctest

Installing dependencies
-----------------------

You will need a C compiler, C++ compiler, CMake, Git, OpenSSL, and Curl
on your system. Generally, these packages already exist on your system. The C++
compiler requires support for C++11, in particular it needs to support lambdas and
``std::unique_ptr``.

After navigating to inside the `pyctest` directory, you can install PyCTest by
building/compiling the shared libraries and either of the following standard Python
installation commands:

.. code:: shell

    $ pip install -vvv .
    $ python setup.py install

Common issues
-------------

- Lack of full C++11 support, particularly `std::unique_ptr`

Importing PyCTest
=================

In general, the base module is not utilized directly. The following import scheme is generally simplest:

   .. code:: python

        import pyctest as _pyctest
        import pyctest.pyctest as pyctest
        import pyctest.pycmake as pycmake
        import pyctest.helpers as helpers
        from pyctest.cmake import CMake
        from pyctest.ctest import CTest
        from pyctest.cpack import CPack

        print(_pyctest.version)
        CMake('--version')
        CTest('--version')
        CPack('--version')

