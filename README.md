OpenMM ANI Plugin
============================

This is a plugin for [OpenMM](http://openmm.org) that allows the [ANI](https://github.com/isayev/ASE_ANI)
Neural Net Potential developed by [Smith et al.](http://pubs.rsc.org/en/content/articlelanding/2017/sc/c6sc05720a) to be used to compute forces and energies. 

This code is released under the [MIT license](License.txt).

Installation
============

At present this plugin must be compiled from source.  It uses CMake as its build
system.  

1. Download the [ASE_ANI](https://github.com/isayev/ASE_ANI) package for
   access to the neural network definitions.

2. Acquire modified shared libraries from the ANI authors. 
   The library will soon be avialable directly from the [ASE_ANI](https://github.com/isayev/ASE_ANI) repository.

3. Create a directory in which to build the plugin.

4. Run the CMake GUI or ccmake, specifying your new directory as the build directory and the top
level directory of this project as the source directory.

5. Press "Configure".

6. Set OPENMM_DIR to point to the directory where OpenMM is installed.  This is needed to locate
the OpenMM header files and libraries.

7. Set CMAKE_INSTALL_PREFIX to the directory where the plugin should be installed.  Usually,
this will be the same as OPENMM_DIR, so the plugin will be added to your OpenMM installation.

8. Make sure that CUDA_TOOLKIT_ROOT_DIR is set correctly and that NN_BUILD_CUDA_LIB is selected.

9. Press "Configure" again if necessary, then press "Generate".
Alternatively, to steps 3-9 you can issue command line statements similar to the following:
```bash
mkdir build
cd build
\rm -rf ../build/*

ASE_ANI_DIR=<your path to ASE_ANI installation>
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:$ASE_ANI_DIR/lib

condaEnv=~/.conda/envs/openMM2
CUDA_DIR=/local/CUDA/9.0.176
export OPENMM_CUDA_COMPILER=$CUDA_DIR/bin/nvcc

cmake -DOPENMM_DIR=$condaEnv \
      -DCMAKE_INSTALL_PREFIX=$condaEnv \
      -DASE_ANI_DIR=$ASE_ANI_DIR \
      -DCUDA_TOOLKIT_ROOT_DIR=$CUDA_DIR \
      -DNN_BUILD_CUDA_LIB=1 \
       <your path to the openmm-ani source root>
```

10. Use the build system you selected to build and install the plugin.  For example, if you
selected Unix Makefiles, type `make install` to install the plugin, and `make PythonInstall` to
install the Python wrapper.

Usage
=====
You should now be able to run a minimization of a water molecule by running the command line below:
```bash
cd demo
min_ani.py -netDir $ASE_ANI_DIR/ani_models/ani-1ccx_8x -in H20.pdb -out H20.min.pdb
```

This runs a very simple minimization of water using OpenMM and the ANI neural Net potential.
Pleae note that when starting from a strongly distorted water conformation the minimization might
not converge to the expected minimum conformation. This is due to the optimizer taking big steps and landing in regions of the chemical wpace in which the ANI network was not trained. The result is that a wrong local minimum might be found.

Acknowledgments
===============

Big parts of the is code were derived from the [openmm-nn](https://github.com/pandegroup/openmm-nn)
plugin by Peter Eastman. I would like to thank Peter for a great introduction to his plugin and
OpenMM development as a whole!

The integration with [ANI](https://github.com/isayev/ASE_ANI) was done with lots of input from
Justin S. Smith. I would like to thank him for all his input and for providing me with a modified
version of the ANI shared libraries.



Literature:
===========

Smith, J. S.; Isayev, O.; Roitberg, A. E. [ANI-1: An Extensible Neural Network Potential with DFT Accuracy at Force Field](https://doi.org/10.1039/C6SC05720A) Computational Cost. Chem. Sci. 2017, 8 (4), 3192–3203.

Smith, J. S.; Nebgen, B.; Lubbers, N.; Isayev, O.; Roitberg, A. E. [Less Is More: Sampling Chemical Space with Active Learning](https://doi.org/10.1063/1.5023802). The Journal of Chemical Physics 2018, 148 (24), 241733.

Smith, J. s.; Nebgen, B. T.; Zubatyuk, R.; Lubbers, N.; Devereux, C.; Barros, K.; Tretiak, S.; Isayev, O.; Roitberg, A. [Outsmarting Quantum Chemistry Through Transfer Learning](https://doi.org/10.26434/chemrxiv.6744440.v1). figshare 2018.

Eastman, P.; Swails, J.; Chodera, J. D.; McGibbon, R. T.; Zhao, Y.; Beauchamp, K. A.; Wang, L.-P.; Simmonett, A. C.; Harrigan, M. P.; Stern, C. D.; et al. [OpenMM 7: Rapid Development of High Performance Algorithms for Molecular Dynamics](https://doi.org/10.1371/journal.pcbi.1005659). PLOS Computational Biology 2017, 13 (7), e1005659.


License
=======
```
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
```

