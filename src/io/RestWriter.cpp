/*
 * Copyright (c) 2009 The University of Notre Dame. All Rights Reserved.
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
 */


#include <iostream>

#include "io/RestWriter.hpp"
#include "utils/simError.h"
#include "brains/SnapshotManager.hpp"
#ifdef IS_MPI
#include <mpi.h>
#endif

namespace oopse {
  RestWriter::RestWriter(SimInfo* info, const std::string& filename, 
                         std::vector<Restraint*> restraints ) : 
    info_(info){

    //use master - slave mode, only master node writes to disk
#ifdef IS_MPI
    if(worldRank == 0){
#endif
      
      output_.open(filename.c_str());
      
      if(!output_){
        sprintf( painCave.errMsg,
                 "Could not open %s for restraint output.\n", 
                 filename.c_str());
        painCave.isFatal = 1;
        simError();
      }

      output_ << "#time\t";
      
      // TODO:  get Restraint info from slave nodes:
      std::vector<Restraint*>::const_iterator resti;
      for(resti=restraints.begin(); resti != restraints.end(); ++resti){

        if ((*resti)->getPrintRestraint()) {
          std::string myName = (*resti)->getRestraintName();
          int myType = (*resti)->getRestraintType();
          
          output_ << myName << ":";
          
          if (myType & Restraint::rtDisplacement)
            output_ << "\tPosition(angstroms)\tEnergy(kcal/mol)";
          
          if (myType & Restraint::rtTwist)
            output_ << "\tTwistAngle(radians)\tEnergy(kcal/mol)";
          
          if (myType & Restraint::rtSwingX)
            output_ << "\tSwingXAngle(radians)\tEnergy(kcal/mol)";
          
          if (myType & Restraint::rtSwingY)
            output_ << "\tSwingYAngle(radians)\tEnergy(kcal/mol)";
          
        }
      }
      output_ << "\n";
#ifdef IS_MPI
    }
#endif      
  }
  
  RestWriter::~RestWriter() {
#ifdef IS_MPI
    if(worldRank == 0 ){
#endif  
      output_.close();  
#ifdef IS_MPI  
    }
#endif
  }
  
  void RestWriter::writeRest(std::vector<std::map<int, Restraint::RealPair> > restInfo){
      
    
    output_ << info_->getSnapshotManager()->getCurrentSnapshot()->getTime();
        
    // output some information about the molecules
    std::vector<std::map<int, Restraint::RealPair> >::const_iterator i;
    std::map<int, Restraint::RealPair>::const_iterator j;
    for( i = restInfo.begin(); i != restInfo.end(); ++i){
      for(j = (*i).begin(); j != (*i).end(); ++j){                
        output_ << "\t" << (j->second).first << "\t" << (j->second).second;
      }
      output_ << std::endl;
    }
  }
  
}// end oopse
  
