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


#include "internal/ANIForceImpl.h"
#include "ANIKernels.h"
#include "openmm/OpenMMException.h"
#include "openmm/internal/ContextImpl.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "neurochemcpp_iface.h"

using namespace ANIPlugin;
using namespace OpenMM;
using namespace std;

ANIForceImpl::ANIForceImpl(const ANIForce& owner) : owner(owner) {
}

ANIForceImpl::~ANIForceImpl() {
    // Cleanup instances (required to shut the classes down before the driver shuts down)
    neurochem::molecule_instances.clear();
}


string ANIForceImpl::compileError(string varName, string fileName) {
    ostringstream oss;
    oss << "InfoFile " << fileName << " has no line for " << varName;
    return oss.str();
}

void ANIForceImpl::initialize(ContextImpl& context) {
    // Load the graph from the file.


    string infoFile = owner.getInfoFile();
    ifstream infile( infoFile );

    string netWorkDir;
    if( ! getline(infile, netWorkDir) )
        throw OpenMMException(compileError("netWorkDir",infoFile));

    string paramFile;
    if( ! getline(infile, paramFile) )
        throw OpenMMException(compileError("paramFile",infoFile));

    string atomFitFile;
    if( ! getline(infile, atomFitFile) )
        throw OpenMMException(compileError("atomFitFile",infoFile));

    string dummy;
    if( ! getline(infile, dummy) )
        throw OpenMMException(compileError("ensamples",infoFile));
    int nEnsambles = stoi(dummy);

    cerr << "initilized ANI with args=" << infoFile << " paramFile:" << paramFile
         << " atomFitFile:"<<atomFitFile << " netWorkDir:"<<netWorkDir
         << " nEnsambles:"<<nEnsambles << endl;

    // Initialize ANI Network as ensamble of multiple networks
    neurochem::instantiate_ani_ensemble(paramFile,atomFitFile,netWorkDir,nEnsambles);

    // Create the kernel.
    kernel = context.getPlatform().createKernel(CalcANIForceKernel::Name(), context);
    kernel.getAs<CalcANIForceKernel>().initialize(context.getSystem(), owner);
}

double ANIForceImpl::calcForcesAndEnergy(ContextImpl& context, bool includeForces, bool includeEnergy, int groups) {
    if ((groups&(1<<owner.getForceGroup())) != 0)
        return kernel.getAs<CalcANIForceKernel>().execute(context, includeForces, includeEnergy);
    return 0.0;
}

vector<string> ANIForceImpl::getKernelNames() {
    vector<string> names;
    names.push_back(CalcANIForceKernel::Name());
    return names;
}
