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


#include "ANIForceProxy.h"
#include "ANIForce.h"
#include "openmm/serialization/SerializationNode.h"
#include <string>
#include <vector>
#include <ostream>
#include <istream>
#include <fstream>
#include <iostream>

using namespace ANIPlugin;
using namespace OpenMM;
using namespace std;

ANIForceProxy::ANIForceProxy() : SerializationProxy("ANIForce") {
}

void ANIForceProxy::serialize(const void* object, SerializationNode& node) const {
    node.setIntProperty("version", 1);
    const ANIForce& force = *reinterpret_cast<const ANIForce*>(object);
    node.setStringProperty("aniSerFile", ANI_SERIALIZATION_FILE);
    
    string aniInfoFile = force.getInfoFile();
    vector<string> atomTypes = force.getAtomSymbols();

    ofstream ofs{ANI_SERIALIZATION_FILE};
    if( ! ofs ) runtime_error("could not open " + ANI_SERIALIZATION_FILE);
    ofs << aniInfoFile << endl;
    for(string at:atomTypes)
        ofs << at << endl;
}

void* ANIForceProxy::deserialize(const SerializationNode& node) const {
    if (node.getIntProperty("version") != 1)
        throw OpenMMException("Unsupported version number");

    string aniInfoFile;

    string serFile = node.getStringProperty("aniSerFile");
    ifstream ifs{serFile};

    getline(ifs, aniInfoFile);

    vector<string> atomTypes;
    for( string at; ifs >> at; )
        atomTypes.push_back(at);
    ANIForce* force = new ANIForce(aniInfoFile, atomTypes);
    return force;
}
