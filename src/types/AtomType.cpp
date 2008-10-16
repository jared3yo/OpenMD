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
 */
 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>

#include "types/AtomType.hpp"
#include "utils/simError.h"
#define __OOPSE_C
#include "UseTheForce/DarkSide/atype_interface.h"
#include "UseTheForce/DarkSide/lj_interface.h"
#include "UseTheForce/DarkSide/eam_interface.h"
#include "UseTheForce/DarkSide/electrostatic_interface.h"
#include "UseTheForce/DarkSide/suttonchen_interface.h"
namespace oopse {
  AtomType::AtomType(){

    // initially, all atom types are their own base types.
    base_ = this;
    hasBase_ = false;

    // initialize to an error:
    atp.ident = -1;
    
    // and mass_less:
    mass_ = 0.0;
    
    // atom type is a Tabula Rasa:
    atp.is_Directional = 0;
    atp.is_LennardJones = 0;
    atp.is_Charge = 0;
    atp.is_Dipole = 0;
    atp.is_SplitDipole = 0;
    atp.is_Quadrupole = 0;
    atp.is_Sticky = 0;
    atp.is_StickyPower = 0;
    atp.is_GayBerne = 0;
    atp.is_EAM = 0;
    atp.is_Shape = 0;
    atp.is_FLARB = 0;  
    atp.is_SC = 0;
    myResponsibilities_["mass"] = false;
    myResponsibilities_["LennardJones"] = false;
    myResponsibilities_["Charge"] = false;
    myResponsibilities_["EAM"] = false;
    myResponsibilities_["SC"] = false;
    myResponsibilities_["is_Directional"] = false;
    myResponsibilities_["is_LennardJones"] = false;
    myResponsibilities_["is_EAM"] = false;
    myResponsibilities_["is_Charge"] = false;
    myResponsibilities_["is_SC"] = false;
    myResponsibilities_["is_FLARB"] = false;
  }
  
  void AtomType::useBase(AtomType* base) {
    hasBase_=true;
    base_ = base;
    base->addZig(this);

    AtomTypeProperties batp = base->getATP();

    if (!myResponsibilities_["is_Directional"]) 
      atp.is_Directional = batp.is_Directional;

    if (!myResponsibilities_["is_LennardJones"])
      atp.is_LennardJones = batp.is_LennardJones;

    if (!myResponsibilities_["is_Charge"])
      atp.is_Charge = batp.is_Charge;

    atp.is_Dipole = atp.is_Dipole || batp.is_Dipole;
    atp.is_SplitDipole = atp.is_SplitDipole || batp.is_SplitDipole;
    atp.is_Quadrupole = atp.is_Quadrupole || batp.is_Quadrupole;
    atp.is_Sticky = atp.is_Sticky || batp.is_Sticky;
    atp.is_StickyPower = atp.is_StickyPower || batp.is_StickyPower;
    atp.is_GayBerne = atp.is_GayBerne || batp.is_GayBerne;

    if (!myResponsibilities_["is_EAM"])
      atp.is_EAM = batp.is_EAM;

    atp.is_Shape = atp.is_Shape || batp.is_Shape;

    if (!myResponsibilities_["is_FLARB"])
      atp.is_FLARB = batp.is_FLARB;
    
    if (!myResponsibilities_["is_SC"])
      atp.is_SC = batp.is_SC;
  }

  void AtomType::copyAllData(AtomType* orig) {

    // makes an exact replica of another atom type down to the
    // atom ID and any base attachments it may have.  
    // use with caution!
    
    hasBase_=orig->hasBase_;
    base_ = orig->base_;
    mass_ = orig->mass_;    
    name_ = std::string(orig->name_);

    atp = orig->atp;

    std::map< std::string, bool>::iterator i;;
    std::map< std::string, RealType>::iterator j;

    for (i = orig->myResponsibilities_.begin(); i != orig->myResponsibilities_.end(); ++i) {
      myResponsibilities_[(*i).first] = orig->myResponsibilities_[(*i).first];
    }

    for (j = orig->myValues_.begin(); j != orig->myValues_.end(); ++j) {
      myValues_[(*j).first] = orig->myValues_[(*j).first];
    }

    std::vector< GenericData*> oprops = orig->getProperties();
    std::vector< GenericData*>::iterator it;

    for (it = oprops.begin(); it != oprops.end(); ++it) {      
      addProperty(*it);
    }
  }

  void AtomType::makeFortranAtomType() {
    
    int status;
    
    if (name_.empty()) {
      sprintf( painCave.errMsg,
               "Attempting to complete an AtomType without giving "
               "it a name_!\n");
      painCave.severity = OOPSE_ERROR;
      painCave.isFatal = 1;
      simError();
    }
    
    if (atp.ident == -1) {
      sprintf( painCave.errMsg,
               "Attempting to complete AtomType %s without setting the"
               " ident!/n", name_.c_str());
      painCave.severity = OOPSE_ERROR;
      painCave.isFatal = 1;
      simError();          
    }
    
    status = 0;
    
    makeAtype(&atp, &status);   
    
    if (status != 0) {
      sprintf( painCave.errMsg,
               "Fortran rejected AtomType %s!\n", name_.c_str());
      painCave.severity = OOPSE_ERROR;
      painCave.isFatal = 1;
      simError();          
    }
  }
  
  
  void AtomType::complete() {
    int isError;
    GenericData* data;
    
    //notify a new LJtype atom type is created
    if (isLennardJones()) {
      data = getPropertyByName("LennardJones");
      if (data != NULL) {
	LJParamGenericData* ljData = dynamic_cast<LJParamGenericData*>(data);
        
	if (ljData != NULL) {
	  LJParam ljParam = ljData->getData();
          
	  newLJtype(&atp.ident, &ljParam.sigma, &ljParam.epsilon, 
                    &ljParam.soft_pot, &isError);
          
	  if (isError != 0) {
	    sprintf( painCave.errMsg,
		     "Fortran rejected newLJtype\n");
	    painCave.severity = OOPSE_ERROR;
	    painCave.isFatal = 1;
	    simError();          
	  }
          
	} else {
	  sprintf( painCave.errMsg,
		   "Can not cast GenericData to LJParam\n");
	  painCave.severity = OOPSE_ERROR;
	  painCave.isFatal = 1;
	  simError();          
	}            
      } else {
	sprintf( painCave.errMsg, "Can not find Parameters for LennardJones\n");
	painCave.severity = OOPSE_ERROR;
	painCave.isFatal = 1;
	simError();          
      }
    }
    
    if (isElectrostatic()) {
      newElectrostaticType(&atp, &isError);
      if (isError != 0) {
        sprintf( painCave.errMsg,
                 "Fortran rejected newElectrostaticType\n");
        painCave.severity = OOPSE_ERROR;
        painCave.isFatal = 1;
        simError();          
      }
    }
    
    if (isCharge()) {
      data = getPropertyByName("Charge");
      if (data != NULL) {
	DoubleGenericData* doubleData= dynamic_cast<DoubleGenericData*>(data);
        
	if (doubleData != NULL) {
	  RealType charge = doubleData->getData();
	  setCharge(&atp.ident, &charge, &isError);
          
	  if (isError != 0) {
	    sprintf( painCave.errMsg,
		     "Fortran rejected setCharge\n");
	    painCave.severity = OOPSE_ERROR;
	    painCave.isFatal = 1;
	    simError();          
	  }
	} else {
	  sprintf( painCave.errMsg,
		   "Can not cast GenericData to DoubleGenericData\n");
	  painCave.severity = OOPSE_ERROR;
	  painCave.isFatal = 1;
	  simError();          
	}
      } else {
	sprintf( painCave.errMsg, "Can not find Charge Parameters\n");
	painCave.severity = OOPSE_ERROR;
	painCave.isFatal = 1;
	simError();          
      }
    }
    
    if (isEAM()) {
      data = getPropertyByName("EAM");
      if (data != NULL) {
	EAMParamGenericData* eamData = dynamic_cast<EAMParamGenericData*>(data);
        
	if (eamData != NULL) {
          
	  EAMParam eamParam = eamData->getData();
          
          
	  newEAMtype(&eamParam.latticeConstant, &eamParam.nrho, 
                     &eamParam.drho,  &eamParam.nr, &eamParam.dr, 
                     &eamParam.rcut, &eamParam.rvals[0], &eamParam.rhovals[0], 
                     &eamParam.Frhovals[0], &atp.ident, &isError );
          
	  if (isError != 0) {
	    sprintf( painCave.errMsg,
		     "Fortran rejected newEAMtype\n");
	    painCave.severity = OOPSE_ERROR;
	    painCave.isFatal = 1;
	    simError();          
	  }
	} else {
	  sprintf( painCave.errMsg,
		   "Can not cast GenericData to EAMParam\n");
	  painCave.severity = OOPSE_ERROR;
	  painCave.isFatal = 1;
	  simError();          
	}
      } else {
	sprintf( painCave.errMsg, "Can not find EAM Parameters\n");
	painCave.severity = OOPSE_ERROR;
	painCave.isFatal = 1;
	simError();          
      }
    }
    
    
    if (isSC()) {
      data = getPropertyByName("SC");
      if (data != NULL) {
        SCParamGenericData* SCData = dynamic_cast<SCParamGenericData*>(data);
        
        if (SCData != NULL) {
          
          SCParam scParam = SCData->getData();
                    
          newSCtype(&atp.ident, &scParam.c, &scParam.m,  
                    &scParam.n, &scParam.alpha, &scParam.epsilon, 
                    &isError );
          
          if (isError != 0) {
            sprintf( painCave.errMsg,
                     "Fortran rejected newSCtype\n");
            painCave.severity = OOPSE_ERROR;
            painCave.isFatal = 1;
            simError();          
          }
        } else {
          sprintf( painCave.errMsg,
                   "Can not cast GenericData to SCParam\n");
          painCave.severity = OOPSE_ERROR;
          painCave.isFatal = 1;
          simError();          
        }
      } else {
        sprintf( painCave.errMsg, "Can not find SC Parameters\n");
        painCave.severity = OOPSE_ERROR;
        painCave.isFatal = 1;
        simError();          
      }
    }
  }

  void AtomType::addProperty(GenericData* genData) {
    myResponsibilities_[genData->getID()] = true;
    properties_.addProperty(genData);  
  }
  
  void AtomType::removeProperty(const std::string& propName) {
    properties_.removeProperty(propName);  
    myResponsibilities_[propName] = false;
  }
  
  void AtomType::clearProperties() {
    properties_.clearProperties();
    // should loop through them to get rid of my responsibilities, but
    // we'll punt for now.
  }
  
  std::vector<std::string> AtomType::getPropertyNames() {
    return properties_.getPropertyNames();  
  }
  
  std::vector<GenericData*> AtomType::getProperties() { 
    return properties_.getProperties(); 
  }
  
  GenericData* AtomType::getPropertyByName(const std::string& propName) {
    if (hasBase_ && !myResponsibilities_[propName]){
      return base_->getPropertyByName(propName);}
    else
      return properties_.getPropertyByName(propName); 
  }  

  void AtomType::setMass(RealType m) {
    myResponsibilities_["mass"] = true;
    myValues_["mass"] = m;	
  }
  
  RealType AtomType::getMass(void) {
    if (hasBase_ && !myResponsibilities_["mass"]) 
      return base_->getMass();
    else
      return myValues_["mass"];
  }

  void AtomType::setIdent(int id) {
    atp.ident = id;
  }

  int AtomType::getIdent() {
      return atp.ident;
  }

  void AtomType::setName(const std::string&name) {
    name_ = name;
  }

  std::string AtomType::getName() {
      return name_;
  }

  void AtomType::setLennardJones() {
    myResponsibilities_["is_LennardJones"] = true;
    atp.is_LennardJones = 1;
  }
  
  bool AtomType::isLennardJones() {
    if (hasBase_ && !myResponsibilities_["is_LennardJones"]) 
      return base_->isLennardJones();
    else
      return atp.is_LennardJones;
  }

  bool AtomType::isElectrostatic() {
    return isCharge() || isMultipole();
  }

  void AtomType::setEAM() {
    myResponsibilities_["is_EAM"] = true;
    atp.is_EAM = 1;
  }
  
  bool AtomType::isEAM() {
    if (hasBase_ && !myResponsibilities_["is_EAM"]) 
      return base_->isEAM();
    else
      return atp.is_EAM;
  }

  void AtomType::setIsCharge() {
    myResponsibilities_["is_Charge"] = true;
    atp.is_Charge = 1;
  }
  
  bool AtomType::isCharge() {
    if (hasBase_ && !myResponsibilities_["is_Charge"]) 
      return base_->isCharge();
    else
      return atp.is_Charge;
  }

  bool AtomType::isDirectional() {
    if (hasBase_ && !myResponsibilities_["is_Directional"]) 
      return base_->isDirectional();
    else
      return atp.is_Directional;
  }

  bool AtomType::isDipole() {
    if (hasBase_ && !myResponsibilities_["is_Dipole"]) 
      return base_->isDipole();
    else
      return atp.is_Dipole;
  }

  bool AtomType::isSplitDipole() {
    if (hasBase_ && !myResponsibilities_["is_SplitDipole"]) 
      return base_->isSplitDipole();
    else
      return atp.is_SplitDipole;
  }

  bool AtomType::isQuadrupole() {
    if (hasBase_ && !myResponsibilities_["is_Quadrupole"]) 
      return base_->isQuadrupole();
    else
      return atp.is_Quadrupole;
  }

  bool AtomType::isMultipole() {
    return isDipole() || isQuadrupole();
  }

  bool AtomType::isGayBerne() {
    if (hasBase_ && !myResponsibilities_["is_GayBerne"]) 
      return base_->isGayBerne();
    else
      return atp.is_GayBerne;
  }

  bool AtomType::isSticky() {
    if (hasBase_ && !myResponsibilities_["is_Sticky"]) 
      return base_->isSticky();
    else
      return atp.is_Sticky;
  }

  bool AtomType::isStickyPower() {
    if (hasBase_ && !myResponsibilities_["is_StickyPower"]) 
      return base_->isStickyPower();
    else
      return atp.is_StickyPower;
  }

  bool AtomType::isShape() {
    if (hasBase_ && !myResponsibilities_["is_Shape"]) 
      return base_->isShape();
    else
      return atp.is_Shape;
  }
  
  bool AtomType::isSC() {
    if (hasBase_ && !myResponsibilities_["is_SC"]) 
      return base_->isSC();
    else
      return atp.is_SC;
  }

  void AtomType::setSC() {
    myResponsibilities_["is_SC"] = true;
    atp.is_SC = 1;
  }

  bool AtomType::isMetal() {
    return isSC() || isEAM();
  }

  bool AtomType::isFLARB() {
    if (hasBase_ && !myResponsibilities_["is_FLARB"]) 
      return base_->isFLARB();
    else
      return atp.is_FLARB;
  }

  void AtomType::setFLARB() {
    myResponsibilities_["is_FLARB"] = true;
    atp.is_FLARB = 1;
  }


  std::vector<AtomType* > AtomType::allYourBase() {   
    std::vector<AtomType* > myChain;   

    if(hasBase_){
      myChain = base_->allYourBase();
      myChain.insert(myChain.begin(), this);
    } else {
      myChain.push_back(this);
    }
    
    return myChain;
  }

}
