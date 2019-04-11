#!/usr/bin/env python
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


from sys import stdout, exit
from math import sqrt
import glob
from simtk.openmm import app, KcalPerKJ
import simtk.openmm as mm
from simtk.openmm import CustomNonbondedForce
from simtk import unit as u
from openmmani import *



def saveANIInfo(oFileName, netDir, netParamFile, atomFitFile, nEnsambles):
    with open(oFileName, "wt") as oaf:
       print(netDir, file=oaf)
       print(netParamFile, file=oaf)
       print(atomFitFile, file=oaf)
       print(nEnsambles, file=oaf)


def createSystem(topology):
    # initil version taken from: simtk/openmm/app/forcefield.py
    sys = mm.System()
    sys.setDefaultPeriodicBoxVectors([2, 0, 0], [0, 3, 0], [0, 0, 4])
    for atom in topology.atoms():
        #if atom not in data.atomType:
        #    raise Exception("Could not identify atom type for atom '%s'." % str(atom))
        #typename = data.atomType[atom]

        # Look up the type name in the list of registered atom types, returning a helpful error message if it cannot be found.
        #if typename not in self._atomTypes:
        #    msg  = "Could not find typename '%s' for atom '%s' in list of known atom types.\n" % (typename, str(atom))
        #    msg += "Known atom types are: %s" % str(self._atomTypes.keys())
        #    raise Exception(msg)

        # Add the particle to the OpenMM system.
        #mass = self._atomTypes[typename].mass
        mass = atom.element.mass
        sys.addParticle(mass)

    # Adjust hydrogen masses if requested.

    #if hydrogenMass is not None:
    #    if not unit.is_quantity(hydrogenMass):
    #        hydrogenMass *= unit.dalton
    #    for atom1, atom2 in topology.bonds():
    #        if atom1.element is elem.hydrogen:
    #            (atom1, atom2) = (atom2, atom1)
    #        if atom2.element is elem.hydrogen and atom1.element not in (elem.hydrogen, None):
    #            transferMass = hydrogenMass-sys.getParticleMass(atom2.index)
    #            sys.setParticleMass(atom2.index, hydrogenMass)
    #            sys.setParticleMass(atom1.index, sys.getParticleMass(atom1.index)-transferMass)

    # Set periodic boundary conditions.

    boxVectors = topology.getPeriodicBoxVectors()
    if boxVectors is not None:
        sys.setDefaultPeriodicBoxVectors(boxVectors[0], boxVectors[1], boxVectors[2])
    #elif nonbondedMethod not in [NoCutoff, CutoffNonPeriodic]:
    #   raise ValueError('Requested periodic boundary conditions for a Topology that does not specify periodic box dimensions')

    return sys


def Minimize(simulation, outFile, iters=0):
    simulation.minimizeEnergy(tolerance=0.001, maxIterations=iters)
    position = simulation.context.getState(getPositions=True).getPositions()
    energy = simulation.context.getState(getEnergy=True).getPotentialEnergy()
    with open(outFile, 'w') as outF:
        app.PDBFile.writeFile(simulation.topology, position, outF)
    print( 'Energy at Minima is %3.3f kcal/mol' % (energy._value * KcalPerKJ))
    return simulation


def warn(*argv):
    # write to stderr
    print(*argv, file=sys.stderr, flush=True)




if __name__ == '__main__':

    import argparse

    parser = argparse.ArgumentParser(formatter_class=argparse.RawTextHelpFormatter,
                                     description="")

    parser.add_argument('-netDir', help='directory with ANI network definitions e.g. ASE_ANI/ani_models/ani-1ccx_8x',
                        metavar='netDir' ,  type=str, required=True)

    parser.add_argument('-paramFile', help='name of ANI parameter file default=netDir/*.params',
                        metavar='paramF' ,  type=str)

    parser.add_argument('-atFitFile', help='File with atomization energies default=netDir/*.dat',
                        metavar='atFitFile' ,  type=str)

    parser.add_argument('-in', help='Molecule to minimize',
                        dest='inFile', metavar='pdb' ,  type=str, required=True)

    parser.add_argument('-out', help='optimized molecule output',
                        dest='outFile', metavar='pdb' ,  type=str, required=True)

    args = parser.parse_args()

    # make parsed parameter local variables
    locals().update(args.__dict__)

    if paramFile is None:
        paramFile = glob.glob1(netDir,"*.params")
        if len(paramFile) != 1: 
            warn("paramFile does not exist or is not uique: %s" %(paramFile))
            sys.exit(1)
        paramFile = paramFile[0]

    if atFitFile is None:
        atFitFile = glob.glob1(netDir,"*.dat")
        if len(atFitFile) != 1:
            warn("atFitFile does not exist or is not uique: %s" %(atFitFile))
            sys.exit(1)
        atFitFile = atFitFile[0]

    nEnsambles = len(glob.glob1(netDir,"train[0-9]") + glob.glob1(netDir,"train[1-9][0-9]"))
    if nEnsambles <= 0:
        warn("No train* directories found in " + netDir)
        sys.exit(1)


    temperature = 298.15 * u.kelvin
    pdb = app.PDBFile(inFile)
    
    modeller = app.Modeller(pdb.topology, pdb.positions)
    
    topo = modeller.topology
    system = createSystem( modeller.topology )
    atomSym = []
    for atom in topo.atoms():
        atomSym.append(atom.element.symbol)
    print(atomSym)
    
    saveANIInfo("aniInfo.txt", netDir, paramFile, atFitFile, nEnsambles)
    
    #################################################
    # add ANI force to system
    f = ANIForce("aniInfo.txt", atomSym)
    f.setUsesPeriodicBoundaryConditions(True)
    system.addForce(f)
    #################################################

    
    integrator = mm.LangevinIntegrator(
        temperature, 1 / u.picosecond,  0.0005 * u.picoseconds)
    simulation = app.Simulation(modeller.topology, system, integrator)
    simulation.context.setPositions(modeller.positions)
    simulation = Minimize(simulation,outFile,1000)
