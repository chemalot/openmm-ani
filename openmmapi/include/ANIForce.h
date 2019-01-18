#ifndef OPENMM_ANIFORCE_H_
#define OPENMM_ANIFORCE_H_

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


#include "openmm/Context.h"
#include "openmm/Force.h"
#include <string>
#include <vector>
#include "internal/windowsExportANI.h"

using namespace std;


namespace ANIPlugin {

/**
 * This class implements forces that are defined by the ANI shared object */

class OPENMM_EXPORT_NN ANIForce : public OpenMM::Force {
public:
    /**
     * Create a ANIForce.  The network parameters are given in a txt file
     *
     * @param aniInfoFile   the path to the file containing ani info
     * @param atomSymbols of atoms in system
     */
    ANIForce(const string& aniInfoFile, vector<string> atomSymbols);

    /**
     * String containing arguments needed to load and initialize ANI network
     */
    const string& getInfoFile() const;

    /**
     * Return a vector with the atomic numbers of the atoms in topology order.
     */
    const vector<string> getAtomSymbols() const;

    /**
     * Set whether this force makes use of periodic boundary conditions.  If this is set
     * to true, the TensorFlow graph must include a 3x3 tensor called "boxvectors", which
     * is set to the current periodic box vectors.
     */
    void setUsesPeriodicBoundaryConditions(bool periodic);

    /**
     * Get whether this force makes use of periodic boundary conditions.
     */
    bool usesPeriodicBoundaryConditions() const;

protected:
    OpenMM::ForceImpl* createImpl() const;

private:
    string aniInfoFile;
    bool usePeriodic;
    const vector<string> atomSymbols;
};

} // namespace NNPlugin

#endif /*OPENMM_ANIFORCE_H_*/
