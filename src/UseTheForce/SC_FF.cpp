/*
 * Copyright (c) 2005 The University of Notre Dame. All Rights Reserved.
 *
 * The University of Notre Dame grants you ("Licensee") a
 * non-exclusive, royalty free, license to use, modify and
 * redistribute this software in source and binary code form, provided
 * that the following conditions are met:
 *
 * 1. Acknowledgement of the program authors must be made in any
 *    publication of scientific results based in part on use of the
 *    program.  An acceptable form of acknowledgement is citation of
 *    the article in which the program was described (Matthew
 *    A. Meineke, Charles F. Vardeman II, Teng Lin, Christopher
 *    J. Fennell and J. Daniel Gezelter, "OOPSE: An Object-Oriented
 *    Parallel Simulation Engine for Molecular Dynamics,"
 *    J. Comput. Chem. 26, pp. 252-271 (2005))
 *
 * 2. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 3. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 * This software is provided "AS IS," without a warranty of any
 * kind. All express or implied conditions, representations and
 * warranties, including any implied warranty of merchantability,
 * fitness for a particular purpose or non-infringement, are hereby
 * excluded.  The University of Notre Dame and its licensors shall not
 * be liable for any damages suffered by licensee as a result of
 * using, modifying or distributing the software or its
 * derivatives. In no event will the University of Notre Dame or its
 * licensors be liable for any lost revenue, profit or data, or for
 * direct, indirect, special, consequential, incidental or punitive
 * damages, however caused and regardless of the theory of liability,
 * arising out of the use of or inability to use software, even if the
 * University of Notre Dame has been advised of the possibility of
 * such damages.
 *
 *
 *  SC_FF.cpp
 *  OOPSE-2.0
 *
 *  Created by Charles F. Vardeman II on 11/9/05.
 *  @author  Charles F. Vardeman II 
 *  @version $Id: SC_FF.cpp,v 1.2 2005-11-15 16:18:36 chuckv Exp $
 *
 */

#include "UseTheForce/SC_FF.hpp"
#include "UseTheForce/DarkSide/eam_interface.h"
#include "UseTheForce/DarkSide/lj_interface.h"
#include "UseTheForce/DarkSide/sticky_interface.h"
#include "UseTheForce/DarkSide/suttonchen_interface.h"
#include "UseTheForce/ForceFieldFactory.hpp"
#include "io/DirectionalAtomTypesSectionParser.hpp"
#include "io/AtomTypesSectionParser.hpp"
#include "io/LennardJonesAtomTypesSectionParser.hpp"
#include "io/ChargeAtomTypesSectionParser.hpp"
#include "io/MultipoleAtomTypesSectionParser.hpp"
#include "io/EAMAtomTypesSectionParser.hpp"
#include "io/StickyAtomTypesSectionParser.hpp"
#include "io/BondTypesSectionParser.hpp"
#include "io/BendTypesSectionParser.hpp"
#include "io/TorsionTypesSectionParser.hpp"
#include "io/SCAtomTypesSectionParser.hpp"
#include "UseTheForce/ForceFieldCreator.hpp"
#include "utils/simError.h"
namespace oopse {
  
  SC_FF::SC_FF(){
    
    //set default force field filename
    setForceFieldFileName("SuttonChen.QSC.frc");
    
    //the order of adding section parsers are important
    //DirectionalAtomTypesSectionParser should be added before AtomTypesSectionParser Since
    //These two section parsers will actually create "real" AtomTypes (AtomTypesSectionParser will create
    //AtomType and DirectionalAtomTypesSectionParser will creat DirectionalAtomType which is a subclass
    //of AtomType, therefore it should come first). Other AtomTypes Section Parser will not create the 
    //"real" AtomType, they only add and set some attribute of the AtomType. Thus their order are not
    //important. AtomTypesSectionParser should be added before other atom type section parsers.
    //Make sure they are added after DirectionalAtomTypesSectionParser and AtomTypesSectionParser. 
    //The order of BondTypesSectionParser, BendTypesSectionParser and TorsionTypesSectionParser are
    //not important.
    spMan_.push_back(new DirectionalAtomTypesSectionParser());
    spMan_.push_back(new AtomTypesSectionParser());
    spMan_.push_back(new LennardJonesAtomTypesSectionParser());
    spMan_.push_back(new ChargeAtomTypesSectionParser());
    spMan_.push_back(new MultipoleAtomTypesSectionParser());
    spMan_.push_back(new EAMAtomTypesSectionParser());
    spMan_.push_back(new StickyAtomTypesSectionParser());
    spMan_.push_back(new BondTypesSectionParser());
    spMan_.push_back(new BendTypesSectionParser());
    spMan_.push_back(new TorsionTypesSectionParser());
    spMan_.push_back(new SCAtomTypesSectionParser());
    
  }
  
  void SC_FF::parse(const std::string& filename) {
    ifstrstream* ffStream;
    ffStream = openForceFieldFile(filename);
    
    spMan_.parse(*ffStream, *this);
    
    ForceField::AtomTypeContainer::MapTypeIterator i;
    AtomType* at;
    
    for (at = atomTypeCont_.beginType(i); at != NULL; at = atomTypeCont_.nextType(i)) {
      at->makeFortranAtomType();
    }
    
    for (at = atomTypeCont_.beginType(i); at != NULL; at = atomTypeCont_.nextType(i)) {
      at->complete();
    }
    
    delete ffStream;
  }
  
    
  SC_FF::~SC_FF(){
    // We need to clean up the fortran side so we don't have bad things happen if
    // we try to create a second SC force field.
    destroySCTypes();
  }
} //end namespace oopse
