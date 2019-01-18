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


#include "ANIForce.h"
#include "openmm/Platform.h"
#include "openmm/internal/AssertionUtilities.h"
#include "openmm/serialization/XmlSerializer.h"
#include <iostream>
#include <sstream>

using namespace ANIPlugin;
using namespace OpenMM;
using namespace std;

extern "C" void registerANISerializationProxies();


void testSerialization() {
    // Create a ANIForce.

    vector<string> dummy = { "O","H","H" };
    ANIForce force("test_aniInfoFile.txt", dummy);

    // Serialize and then deserialize it.

    stringstream buffer;
    XmlSerializer::serialize<ANIForce>(&force, "Force", buffer);
    ANIForce* copy = XmlSerializer::deserialize<ANIForce>(buffer);

    // Compare the two forces to see if they are identical.

    ANIForce& force2 = *copy;
    ASSERT_EQUAL(force.getInfoFile(), force2.getInfoFile());

    vector<string> atT1 = force.getAtomSymbols();
    vector<string> atT2 = force2.getAtomSymbols();
    for( int i=0; i<atT1.size(); i++ ) {
       //cerr << atT1[i] << " " << atT2[i] << endl;
       ASSERT_EQUAL(atT1[i], atT2[i]);
    }
}

int main() {
    try {
        registerANISerializationProxies();
        testSerialization();
    }
    catch(const exception& e) {
        cerr << "exception: " << e.what() << endl;
        return 1;
    }
    cerr << "Done" << endl;
    return 0;
}
