#ifndef CUDA_ANI_KERNELS_H_
#define CUDA_ANI_KERNELS_H_

/* -------------------------------------------------------------------------- *
 * The MIT License
 *
 * SPDX short identifier: MIT
 *
 * Copyright 2019 Genentech Inc. South San Francisco
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 * -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- *
 * Portions of this software were derived from code originally developed
 * by Peter Eastman and copyrighted by Stanford University and the Authors
 * -------------------------------------------------------------------------- */


#include "ANIKernels.h"
#include "openmm/cuda/CudaContext.h"
#include "openmm/cuda/CudaArray.h"



#define NM_TO_ANGST 10
#define HARTREE_TO_KJ_MOL 2625.50
#define HARTREE_A_TO_KJ_MOL_NM (HARTREE_TO_KJ_MOL * NM_TO_ANGST)



namespace ANIPlugin {

/**
 * This kernel is invoked by ANIForce to calculate the forces acting on the system and the energy of the system.
 */
class CudaCalcANIForceKernel : public CalcANIForceKernel {
public:
    CudaCalcANIForceKernel(std::string name, const OpenMM::Platform& platform, OpenMM::CudaContext& cu) :
            CalcANIForceKernel(name, platform), hasInitializedKernel(false), cu(cu) {
    }

    ~CudaCalcANIForceKernel();

    /**
     * Initialize the kernel.
     * 
     * @param system         the System this kernel will be applied to
     * @param force          the ANIForce this kernel will be used for
     */
    void initialize(const OpenMM::System& system, const ANIForce& force);
    /**
     * Execute the kernel to calculate the forces and/or energy.
     *
     * @param context        the context in which to execute this kernel
     * @param includeForces  true if forces should be calculated
     * @param includeEnergy  true if the energy should be calculated
     * @return the potential energy due to the force
     */
    double execute(OpenMM::ContextImpl& context, bool includeForces, bool includeEnergy);

private:
    bool hasInitializedKernel;
    OpenMM::CudaContext& cu;
    vector<float> aniPositions;
    vector<string> atomicSymbols;
    bool usePeriodic;
    OpenMM::CudaArray networkForces;
    CUfunction addForcesKernel;
};

} // namespace ANIPlugin

#endif /*CUDA_ANI_KERNELS_H_*/
