#
# Copyright 2008,2009 Free Software Foundation, Inc.
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

# The presence of this file turns this directory into a Python package

'''
This is the GNU Radio HSDEC module. Place your Python package
description here (python/__init__.py).
'''
import os

# import pybind11 generated symbols into the hsdec namespace
try:
    # this might fail if the module is python-only
    from .hsdec_python import *
except ModuleNotFoundError:
    pass

# import any pure python here
#
