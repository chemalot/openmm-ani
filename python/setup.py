###############################################################################
## The MIT License
##
## SPDX short identifier: MIT
##
## Copyright 2019 Genentech Inc. South San Francisco
##
## Permission is hereby granted, free of charge, to any person obtaining a
## copy of this software and associated documentation files (the "Software"),
## to deal in the Software without restriction, including without limitation
## the rights to use, copy, modify, merge, publish, distribute, sublicense,
## and/or sell copies of the Software, and to permit persons to whom the
## Software is furnished to do so, subject to the following conditions:
##
## The above copyright notice and this permission notice shall be included
## in all copies or substantial portions of the Software.
##
## THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
## OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
## FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
## AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
## LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
## FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
## DEALINGS IN THE SOFTWARE.
###############################################################################
###############################################################################
## Portions of this software were derived from code originally developed
## by Peter Eastman and copyrighted by Stanford University and the Authors
###############################################################################

from distutils.core import setup
from distutils.extension import Extension
import os
import sys
import platform

openmm_dir = '@OPENMM_DIR@'
nn_plugin_header_dir = '@NN_PLUGIN_HEADER_DIR@'
nn_plugin_library_dir = '@NN_PLUGIN_LIBRARY_DIR@'

# setup extra compile and link arguments on Mac
extra_compile_args = []
extra_link_args = []

if platform.system() == 'Darwin':
    extra_compile_args += ['-stdlib=libc++', '-mmacosx-version-min=10.7']
    extra_link_args += ['-stdlib=libc++', '-mmacosx-version-min=10.7', '-Wl', '-rpath', openmm_dir+'/lib']

extension = Extension(name='_openmmani',
                      sources=['ANIPluginWrapper.cpp'],
                      libraries=['OpenMM', 'OpenMMANI'],
                      include_dirs=[os.path.join(openmm_dir, 'include'), nn_plugin_header_dir],
                      library_dirs=[os.path.join(openmm_dir, 'lib'), nn_plugin_library_dir],
                      runtime_library_dirs=[os.path.join(openmm_dir, 'lib')],
                      extra_compile_args=extra_compile_args,
                      extra_link_args=extra_link_args
                     )

setup(name='openmmani',
      version='1.0',
      py_modules=['openmmani'],
      ext_modules=[extension],
     )
