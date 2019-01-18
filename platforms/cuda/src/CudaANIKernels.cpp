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


#include "CudaANIKernels.h"
#include "CudaANIKernelSources.h"
#include "openmm/internal/ContextImpl.h"
#include <map>
#include <iostream>
#include "neurochemcpp_iface.h"

#define PRINT_RESULTS 1
#define OPENMM_FLOAT 4



using namespace ANIPlugin;
using namespace OpenMM;
using namespace std;

CudaCalcANIForceKernel::~CudaCalcANIForceKernel() {
}

   
void CudaCalcANIForceKernel::initialize(const System& system, const ANIForce& force) {

    // cu is OpenMM::CudaContext&
    cu.setAsCurrent();
    usePeriodic = force.usesPeriodicBoundaryConditions();
    atomicSymbols = force.getAtomSymbols();
    int numParticles = system.getNumParticles();

    // Construct input tensors.

    aniPositions.resize(numParticles*3);
     
    //if (usePeriodic) {
    //    int64_t boxVectorsDims[] = {3, 3};
    //    boxVectorsTensor = TF_AllocateTensor(boxType, boxVectorsDims, 2, 9*TF_DataTypeSize(boxType));
    //}

    // Inititalize CUDA objects.
    // networkForces is OpenMM::CudaArray
    networkForces.initialize(cu, 3*numParticles, OPENMM_FLOAT, "networkForces");
    map<string, string> defines;
    defines["FORCES_TYPE"] = "float";
    CUmodule module = cu.createModule(CudaANIKernelSources::aniForce, defines);
    addForcesKernel = cu.getKernel(module, "addForces");
}


/**
 * This is where the actual energy and forces are computed by calling
 * neurochem::compute_ensemble_energy() and neurochem::compute_ensemble_force()
 *
 */
double CudaCalcANIForceKernel::execute(ContextImpl& context, bool includeForces, bool includeEnergy) {

    vector<Vec3> pos;
    context.getPositions(pos);
    int numParticles = cu.getNumAtoms();

    for (int i = 0; i < numParticles; i++) {
        // libANI.so coordinates are in A, OpenMM in NM
        aniPositions[3*i] = pos[i][0]   * NM_TO_ANGST;
        aniPositions[3*i+1] = pos[i][1] * NM_TO_ANGST;
        aniPositions[3*i+2] = pos[i][2] * NM_TO_ANGST;
    }

    if( PRINT_RESULTS ) {
        for( string at : atomicSymbols ) cerr << at << " ";
        for(float p : aniPositions) cerr << p << " ";
        cerr << aniPositions.size() << endl;
    }


    /*
    if (usePeriodic) {
        Vec3 box[3];
        cu.getPeriodicBoxVectors(box[0], box[1], box[2]);
        if (boxType == TF_FLOAT) {
            float* boxVectors = reinterpret_cast<float*>(TF_TensorData(boxVectorsTensor));
            for (int i = 0; i < 3; i++)
                for (int j = 0; j < 3; j++)
                    boxVectors[3*i+j] = box[i][j];
        }
    }
    */

    // Compute energies for the ensemble, libANI ennergies are in Hartree's
    double energy = neurochem::compute_ensemble_energy(aniPositions, atomicSymbols);
    if( PRINT_RESULTS ) cerr << "ANI Energy [H]     =" << energy << endl;
    energy *= HARTREE_TO_KJ_MOL;
    if( PRINT_RESULTS ) cerr << "ANI Energy [KJ/Mol]=" << energy << endl;

    if (includeForces) {
        // Compute forces for the ensemble
        // libANI forces are in Hartree/A, OpenMM Forces are in KJ/Mol/nM
        vector<float> forces = neurochem::compute_ensemble_force(numParticles);

        if( PRINT_RESULTS ) {
            cerr << "   ANI Forces [H/A      ]: ";
            for( float f : forces )
                cerr << f << " ";
            cerr << endl;
        }

        for( int i=0; i<forces.size(); i++ )
            forces[i] *= HARTREE_A_TO_KJ_MOL_NM;

        if( PRINT_RESULTS ) {
            cerr << "   ANI Forces [KJ/Mol/nm]: ";
            for( float f : forces )
                cerr << f << " ";
            cerr << endl;
        }

        // Use cuda Kernel to upload forces to GPU
        networkForces.upload(forces.data());
        int paddedNumAtoms = cu.getPaddedNumAtoms();
        void* args[] = {&networkForces.getDevicePointer(), &cu.getForce().getDevicePointer(), 
                        &cu.getAtomIndexArray().getDevicePointer(), &numParticles, &paddedNumAtoms};
        cu.executeKernel(addForcesKernel, args, numParticles);

    }
    return energy;
}
