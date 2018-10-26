/*
 * Copyright (c) 2012 The University of Notre Dame. All Rights Reserved.
 *
 * The University of Notre Dame grants you ("Licensee") a
 * non-exclusive, royalty free, license to use, modify and
 * redistribute this software in source and binary code form, provided
 * that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
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
 * SUPPORT OPEN SCIENCE!  If you use OpenMD or its source code in your
 * research, please cite the appropriate papers when you publish your
 * work.  Good starting points are:
 *
 * [1]  Meineke, et al., J. Comp. Chem. 26, 252-271 (2005).
 * [2]  Fennell & Gezelter, J. Chem. Phys. 124, 234104 (2006).
 * [3]  Sun, Lin & Gezelter, J. Chem. Phys. 128, 234107 (2008).
 * [4]  Kuang & Gezelter,  J. Chem. Phys. 133, 164101 (2010).
 * [5]  Vardeman, Stocker & Gezelter, J. Chem. Theory Comput. 7, 834 (2011).
 */

#include "types/EAMAdapter.hpp"
#include "utils/simError.h"
#include "utils/CaseConversion.hpp"
#include <cstdio>
#include <cmath>

namespace OpenMD {

  bool EAMAdapter::isEAM() {
    return at_->hasProperty(EAMtypeID);
  }

  EAMParameters* EAMAdapter::getEAMParam() {

    if (!isEAM()) {
      sprintf( painCave.errMsg,
               "EAMAdapter::getEAMParam was passed an atomType (%s)\n"
               "\tthat does not appear to be an EAM atom.\n",
               at_->getName().c_str());
      painCave.severity = OPENMD_ERROR;
      painCave.isFatal = 1;
      simError();
    }

    GenericData* data = at_->getPropertyByName(EAMtypeID);
    if (data == NULL) {
      sprintf( painCave.errMsg,
               "EAMAdapter::getEAMParam could not find EAM\n"
               "\tparameters for atomType %s.\n", at_->getName().c_str());
      painCave.severity = OPENMD_ERROR;
      painCave.isFatal = 1;
      simError();
    }

    EAMData* eamData = dynamic_cast<EAMData*>(data);
    if (eamData == NULL) {
      sprintf( painCave.errMsg,
               "EAMAdapter::getEAMParam could not convert\n"
               "\tGenericData to EAMData for atom type %s\n",
               at_->getName().c_str());
      painCave.severity = OPENMD_ERROR;
      painCave.isFatal = 1;
      simError();
    }

    return eamData->getData();
  }

  FuncflParameters* EAMAdapter::getFuncflParam() {

    if (!isEAM()) {
      sprintf( painCave.errMsg,
               "EAMAdapter::getFuncflParam was passed an atomType (%s)\n"
               "\tthat does not appear to be an EAM atom.\n",
               at_->getName().c_str());
      painCave.severity = OPENMD_ERROR;
      painCave.isFatal = 1;
      simError();
    }

    GenericData* data = at_->getPropertyByName(FuncflTypeID);
    if (data == NULL) {
      sprintf( painCave.errMsg,
               "EAMAdapter::getFuncflParam could not find Funcfl\n"
               "\tparameters for atomType %s.\n", at_->getName().c_str());
      painCave.severity = OPENMD_ERROR;
      painCave.isFatal = 1;
      simError();
    }

    FuncflData* funcflData = dynamic_cast<FuncflData*>(data);
    if (funcflData == NULL) {
      sprintf( painCave.errMsg,
               "EAMAdapter::getFuncflParam could not convert\n"
               "\tGenericData to FuncflData for atom type %s\n",
               at_->getName().c_str());
      painCave.severity = OPENMD_ERROR;
      painCave.isFatal = 1;
      simError();
    }

    return funcflData->getData();
  }

  ZhouParameters* EAMAdapter::getZhouParam() {

    if (!isEAM()) {
      sprintf( painCave.errMsg,
               "EAMAdapter::getZhouParam was passed an atomType (%s)\n"
               "\tthat does not appear to be an EAM atom.\n",
               at_->getName().c_str());
      painCave.severity = OPENMD_ERROR;
      painCave.isFatal = 1;
      simError();
    }

    GenericData* data = at_->getPropertyByName(ZhouTypeID);
    if (data == NULL) {
      sprintf( painCave.errMsg,
               "EAMAdapter::getZhou2001Param could not find Zhou\n"
               "\tparameters for atomType %s.\n", at_->getName().c_str());
      painCave.severity = OPENMD_ERROR;
      painCave.isFatal = 1;
      simError();
    }

    ZhouData* zhouData = dynamic_cast<ZhouData*>(data);
    if (zhouData == NULL) {
      sprintf( painCave.errMsg,
               "EAMAdapter::getZhouParam could not convert\n"
               "\tGenericData to ZhouData for atom type %s\n",
               at_->getName().c_str());
      painCave.severity = OPENMD_ERROR;
      painCave.isFatal = 1;
      simError();
    }

    return zhouData->getData();
  }

  EVBParameters* EAMAdapter::getEVBParam() {

    if (!isEAM()) {
      sprintf( painCave.errMsg,
               "EAMAdapter::getEVBParam was passed an atomType (%s)\n"
               "\tthat does not appear to be an EAM atom.\n",
               at_->getName().c_str());
      painCave.severity = OPENMD_ERROR;
      painCave.isFatal = 1;
      simError();
    }

    GenericData* data = at_->getPropertyByName(EVBTypeID);
    if (data == NULL) {
      sprintf( painCave.errMsg,
               "EAMAdapter::getEVBParam could not find EVB\n"
               "\tparameters for atomType %s.\n", at_->getName().c_str());
      painCave.severity = OPENMD_ERROR;
      painCave.isFatal = 1;
      simError();
    }

    EVBData* evbData = dynamic_cast<EVBData*>(data);
    if (evbData == NULL) {
      sprintf( painCave.errMsg,
               "EAMAdapter::getEVBParam could not convert\n"
               "\tGenericData to EVBData for atom type %s\n",
               at_->getName().c_str());
      painCave.severity = OPENMD_ERROR;
      painCave.isFatal = 1;
      simError();
    }
    
    return evbData->getData();
  }
  
  EAMType EAMAdapter::getEAMType() {
    EAMParameters* eamParam = getEAMParam();
    return eamParam->eamType;
  }

  std::string EAMAdapter::getLatticeType() {
    EAMParameters* eamParam = getEAMParam();
    return eamParam->latticeType;
  }

  RealType EAMAdapter::getLatticeConstant() {
    EAMParameters* eamParam = getEAMParam();
    return eamParam->latticeConstant;
  }


  CubicSpline* EAMAdapter::getZSpline() {
    FuncflParameters* funcflParam = getFuncflParam();
    int nr = funcflParam->nr;
    RealType dr = funcflParam->dr;
    std::vector<RealType> rvals;

    for (int i = 0; i < nr; i++) rvals.push_back(RealType(i) * dr);

    CubicSpline* cs = new CubicSpline();
    cs->addPoints(rvals, funcflParam->Z);
    return cs;
  }

  CubicSpline* EAMAdapter::getRhoSpline() {
    FuncflParameters* funcflParam = getFuncflParam();
    int nr = funcflParam->nr;
    RealType dr = funcflParam->dr;
    std::vector<RealType> rvals;

    for (int i = 0; i < nr; i++) rvals.push_back(RealType(i) * dr);

    CubicSpline* cs = new CubicSpline();
    cs->addPoints(rvals, funcflParam->rho);
    return cs;
  }

  CubicSpline* EAMAdapter::getFSpline() {
    FuncflParameters* funcflParam = getFuncflParam();
    int nrho = funcflParam->nrho;
    RealType drho = funcflParam->drho;
    std::vector<RealType> rhovals;

    for (int i = 0; i < nrho; i++) {
      rhovals.push_back(RealType(i) * drho);
    }

    CubicSpline* cs = new CubicSpline();
    cs->addPoints(rhovals, funcflParam->F);
    return cs;
  }

  void EAMAdapter::makeFuncfl(RealType latticeConstant, std::string latticeType,
                              int nrho, RealType drho,
                              int nr, RealType dr, RealType rcut,
                              vector<RealType> Z, vector<RealType> rho,
                              vector<RealType> F) {

    if (isEAM()){
      at_->removeProperty(EAMtypeID);
      at_->removeProperty(FuncflTypeID);
    }

    EAMParameters* eamParam = new EAMParameters();
    FuncflParameters* funcflParam = new FuncflParameters();

    eamParam->eamType = eamFuncfl;
    eamParam->latticeConstant = latticeConstant;
    eamParam->latticeType = latticeType;

    funcflParam->nrho = nrho;
    funcflParam->drho = drho;
    funcflParam->nr = nr;
    funcflParam->dr = dr;
    funcflParam->rcut = rcut;
    funcflParam->Z = Z;
    funcflParam->rho = rho;
    funcflParam->F = F;

    at_->addProperty(new EAMData(EAMtypeID, eamParam));
    at_->addProperty(new FuncflData(FuncflTypeID, funcflParam));
  }

  void EAMAdapter::makeZhou2001(std::string latticeType,
                                RealType re,
                                RealType fe,
                                RealType rhoe,
                                RealType alpha,
                                RealType beta,
                                RealType A,
                                RealType B,
                                RealType kappa,
                                RealType lambda,
                                std::vector<RealType> Fn,
                                std::vector<RealType> F,
                                RealType eta,
                                RealType Fe){

    if (isEAM()){
      at_->removeProperty(EAMtypeID);
      at_->removeProperty(ZhouTypeID);
    }

    EAMParameters* eamParam = new EAMParameters();
    ZhouParameters* zhouParam = new ZhouParameters();

    eamParam->eamType = eamZhou2001;

    toUpper(latticeType);
    eamParam->latticeType = latticeType;
    // default to FCC if we don't specify HCP or BCC:
    if (latticeType == "HCP")
      eamParam->latticeConstant = re;
    else if (latticeType == "BCC")
      eamParam->latticeConstant = 2.0 * re / sqrt(3.0);
    else
      eamParam->latticeConstant = 2.0 * re / sqrt(2.0);

    zhouParam->re = re;
    zhouParam->fe = fe;
    zhouParam->rhoe = rhoe;
    zhouParam->alpha = alpha;
    zhouParam->beta = beta;
    zhouParam->A = A;
    zhouParam->B = B;
    zhouParam->kappa = kappa;
    zhouParam->lambda = lambda;
    zhouParam->Fn = Fn;
    zhouParam->F = F;
    zhouParam->eta = eta;
    zhouParam->Fe = Fe;

    at_->addProperty(new EAMData(EAMtypeID, eamParam));
    at_->addProperty(new ZhouData(ZhouTypeID, zhouParam));

  }

  void EAMAdapter::makeZhou2004(std::string latticeType,
                                RealType re,
                                RealType fe,
                                RealType rhoe,
                                RealType rhos,
                                RealType alpha,
                                RealType beta,
                                RealType A,
                                RealType B,
                                RealType kappa,
                                RealType lambda,
                                std::vector<RealType> Fn,
                                std::vector<RealType> F,
                                RealType eta,
                                RealType Fe,
                                RealType rhol,
                                RealType rhoh) {

    if (isEAM()){
      at_->removeProperty(EAMtypeID);
      at_->removeProperty(ZhouTypeID);
    }

    EAMParameters* eamParam = new EAMParameters();
    ZhouParameters* zhouParam = new ZhouParameters();

    eamParam->eamType = eamZhou2004;

    toUpper(latticeType);
    eamParam->latticeType = latticeType;
    // default to FCC if we don't specify HCP or BCC:
    if (latticeType == "HCP")
      eamParam->latticeConstant = re;
    else if (latticeType == "BCC")
      eamParam->latticeConstant = 2.0 * re / sqrt(3.0);
    else
      eamParam->latticeConstant = 2.0 * re / sqrt(2.0);

    zhouParam->re = re;
    zhouParam->fe = fe;
    zhouParam->rhoe = rhoe;
    zhouParam->alpha = alpha;
    zhouParam->beta = beta;
    zhouParam->A = A;
    zhouParam->B = B;
    zhouParam->kappa = kappa;
    zhouParam->lambda = lambda;
    zhouParam->Fn = Fn;
    zhouParam->F = F;
    zhouParam->eta = eta;
    zhouParam->Fe = Fe;
    zhouParam->rhos = rhos;
    zhouParam->rhol = rhol;
    zhouParam->rhoh = rhoh;

    at_->addProperty(new EAMData(EAMtypeID, eamParam));
    at_->addProperty(new ZhouData(ZhouTypeID, zhouParam));
  }

  void EAMAdapter::makeZhou2005(std::string latticeType,
                                RealType re,
                                RealType fe,
                                RealType rhoe,
                                RealType rhos,
                                RealType alpha,
                                RealType beta,
                                RealType A,
                                RealType B,
                                RealType kappa,
                                RealType lambda,
                                std::vector<RealType> Fn,
                                std::vector<RealType> F,
                                RealType F3plus,
                                RealType F3minus,
                                RealType eta,
                                RealType Fe) {

    if (isEAM()){
      at_->removeProperty(EAMtypeID);
      at_->removeProperty(ZhouTypeID);
    }

    EAMParameters* eamParam = new EAMParameters();
    ZhouParameters* zhouParam = new ZhouParameters();

    eamParam->eamType = eamZhou2005;

    toUpper(latticeType);
    eamParam->latticeType = latticeType;
    // default to FCC if we don't specify HCP or BCC:
    if (latticeType == "HCP")
      eamParam->latticeConstant = re;
    else if (latticeType == "BCC")
      eamParam->latticeConstant = 2.0 * re / sqrt(3.0);
    else
      eamParam->latticeConstant = 2.0 * re / sqrt(2.0);

    zhouParam->re = re;
    zhouParam->fe = fe;
    zhouParam->rhoe = rhoe;
    zhouParam->alpha = alpha;
    zhouParam->beta = beta;
    zhouParam->A = A;
    zhouParam->B = B;
    zhouParam->kappa = kappa;
    zhouParam->lambda = lambda;
    zhouParam->Fn = Fn;
    zhouParam->F = F;
    zhouParam->F3plus = F3plus;
    zhouParam->F3minus = F3minus;
    zhouParam->eta = eta;
    zhouParam->Fe = Fe;
    zhouParam->rhos = rhos;

    at_->addProperty(new EAMData(EAMtypeID, eamParam));
    at_->addProperty(new ZhouData(ZhouTypeID, zhouParam));
  }

  void EAMAdapter::makeZhou2005Oxygen(RealType re,
                                      RealType fe,
                                      RealType alpha,
                                      RealType beta,
                                      RealType A,
                                      RealType B,
                                      RealType kappa,
                                      RealType lambda,
                                      RealType gamma,
                                      RealType nu,
                                      std::vector<RealType> OrhoLimits,
                                      std::vector<RealType> OrhoE,
                                      std::vector<std::vector<RealType> > OF) {

    if (isEAM()){
      at_->removeProperty(EAMtypeID);
      at_->removeProperty(ZhouTypeID);
    }

    EAMParameters* eamParam = new EAMParameters();
    ZhouParameters* zhouParam = new ZhouParameters();

    eamParam->eamType = eamZhou2005Oxygen;

    eamParam->latticeConstant = re;

    zhouParam->re = re;
    zhouParam->fe = fe;
    zhouParam->alpha = alpha;
    zhouParam->beta = beta;
    zhouParam->A = A;
    zhouParam->B = B;
    zhouParam->kappa = kappa;
    zhouParam->lambda = lambda;
    zhouParam->gamma = gamma;
    zhouParam->nu = nu;
    zhouParam->OrhoLimits = OrhoLimits;
    zhouParam->OrhoE = OrhoE;
    zhouParam->OF = OF;

    at_->addProperty(new EAMData(EAMtypeID, eamParam));
    at_->addProperty(new ZhouData(ZhouTypeID, zhouParam));
  }

  void EAMAdapter::makeEVBOxygen(RealType r0,
                                 RealType D0,
                                 RealType beta0,
                                 RealType rc,
                                 RealType c,
                                 RealType sigma,
                                 RealType re,
                                 RealType fe,
                                 RealType alpha,
                                 RealType beta,
                                 RealType A,
                                 RealType B,
                                 RealType kappa,
                                 RealType lambda,
                                 RealType gamma,
                                 RealType nu,
                                 std::vector<RealType> OrhoLimits,
                                 std::vector<RealType> OrhoE,
                                 std::vector<std::vector<RealType> > OF) {
    
    if (isEAM()){
      at_->removeProperty(EAMtypeID);
      at_->removeProperty(ZhouTypeID);
      at_->removeProperty(EVBTypeID);
    }
    
    EAMParameters* eamParam = new EAMParameters();
    ZhouParameters* zhouParam = new ZhouParameters();
    EVBParameters* evbParam = new EVBParameters();

    eamParam->eamType = eamEVBOxygen;

    eamParam->latticeConstant = re;

    evbParam->r0    = r0;
    evbParam->D0    = D0;
    evbParam->beta0 = beta0;
    evbParam->rc    = rc;
    evbParam->c     = c;
    evbParam->sigma = sigma;
    
    zhouParam->re = re;
    zhouParam->fe = fe;
    zhouParam->alpha = alpha;
    zhouParam->beta = beta;
    zhouParam->A = A;
    zhouParam->B = B;
    zhouParam->kappa = kappa;
    zhouParam->lambda = lambda;
    zhouParam->gamma = gamma;
    zhouParam->nu = nu;
    zhouParam->OrhoLimits = OrhoLimits;
    zhouParam->OrhoE = OrhoE;
    zhouParam->OF = OF;

    at_->addProperty(new EAMData(EAMtypeID, eamParam));
    at_->addProperty(new ZhouData(ZhouTypeID, zhouParam));
    at_->addProperty(new EVBData(EAMtypeID, evbParam));
  }

  int EAMAdapter::getNrho() {
    FuncflParameters* funcflParam = getFuncflParam();
    return funcflParam->nrho;
  }

  RealType EAMAdapter::getDrho() {
    FuncflParameters* funcflParam = getFuncflParam();
    return funcflParam->drho;
  }

  int EAMAdapter::getNr() {
    FuncflParameters* funcflParam = getFuncflParam();
    return funcflParam->nr;
  }

  RealType EAMAdapter::getDr() {
    FuncflParameters* funcflParam = getFuncflParam();
    return funcflParam->dr;
  }

  RealType EAMAdapter::getRcut() {
    FuncflParameters* funcflParam = getFuncflParam();
    return funcflParam->rcut;
  }

  RealType EAMAdapter::getRe() {
    ZhouParameters* zhouParam = getZhouParam();
    return zhouParam->re;
  }
  RealType EAMAdapter::get_fe() {
    ZhouParameters* zhouParam = getZhouParam();
    return zhouParam->fe;
  }
  RealType EAMAdapter::getRhoe() {
    ZhouParameters* zhouParam = getZhouParam();
    return zhouParam->rhoe;
  }
  RealType EAMAdapter::getAlpha() {
    ZhouParameters* zhouParam = getZhouParam();
    return zhouParam->alpha;
  }
  RealType EAMAdapter::getBeta() {
    ZhouParameters* zhouParam = getZhouParam();
    return zhouParam->beta;
  }
  RealType EAMAdapter::getA() {
    ZhouParameters* zhouParam = getZhouParam();
    return zhouParam->A;
  }
  RealType EAMAdapter::getB() {
    ZhouParameters* zhouParam = getZhouParam();
    return zhouParam->B;
  }
  RealType EAMAdapter::getKappa() {
    ZhouParameters* zhouParam = getZhouParam();
    return zhouParam->kappa;
  }
  RealType EAMAdapter::getLambda() {
    ZhouParameters* zhouParam = getZhouParam();
    return zhouParam->lambda;
  }
  RealType EAMAdapter::getGamma() {
    if (getEAMType() != eamZhou2005Oxygen) {
      sprintf( painCave.errMsg,
               "EAMAdapter::getGamma was called on atomType %s\n"
               "\twhich is not a Zhou2005Oxygen type\n",
               at_->getName().c_str());
      painCave.severity = OPENMD_ERROR;
      painCave.isFatal = 1;
      simError();
    }

    ZhouParameters* zhouParam = getZhouParam();
    return zhouParam->gamma;
  }
  RealType EAMAdapter::getNu() {
    if (getEAMType() != eamZhou2005Oxygen) {
      sprintf( painCave.errMsg,
               "EAMAdapter::getNu was called on atomType %s\n"
               "\twhich is not a Zhou2005Oxygen type\n",
               at_->getName().c_str());
      painCave.severity = OPENMD_ERROR;
      painCave.isFatal = 1;
      simError();
    }

    ZhouParameters* zhouParam = getZhouParam();
    return zhouParam->nu;
  }
  std::vector<RealType> EAMAdapter::getFn() {
    ZhouParameters* zhouParam = getZhouParam();
    return zhouParam->Fn;
  }
  std::vector<RealType> EAMAdapter::getF() {
    ZhouParameters* zhouParam = getZhouParam();
    return zhouParam->F;
  }
  RealType EAMAdapter::getF3plus() {
    if (getEAMType() != eamZhou2005) {
      sprintf( painCave.errMsg,
               "EAMAdapter::getF3plus was called on atomType %s\n"
               "\twhich is not a Zhou2005 type\n",
               at_->getName().c_str());
      painCave.severity = OPENMD_ERROR;
      painCave.isFatal = 1;
      simError();
    }

    ZhouParameters* zhouParam = getZhouParam();
    return zhouParam->F3plus;
  }
  RealType EAMAdapter::getF3minus() {
    if (getEAMType() != eamZhou2005) {
      sprintf( painCave.errMsg,
               "EAMAdapter::getF3minus was called on atomType %s\n"
               "\twhich is not a Zhou2005 type\n",
               at_->getName().c_str());
      painCave.severity = OPENMD_ERROR;
      painCave.isFatal = 1;
      simError();
    }

    ZhouParameters* zhouParam = getZhouParam();
    return zhouParam->F3minus;
  }

  RealType EAMAdapter::getEta() {
    ZhouParameters* zhouParam = getZhouParam();
    return zhouParam->eta;
  }

  RealType EAMAdapter::getFe() {
    ZhouParameters* zhouParam = getZhouParam();
    return zhouParam->Fe;
  }

  RealType EAMAdapter::getRhos() {
    EAMType et = getEAMType();
    if (et != eamZhou2004 && et != eamZhou2005 ) {
      sprintf( painCave.errMsg,
               "EAMAdapter::getRhos was called on atomType %s\n"
               "\twhich is not a Zhou2004 or Zhou2005 type\n",
               at_->getName().c_str());
      painCave.severity = OPENMD_ERROR;
      painCave.isFatal = 1;
      simError();
    }
    ZhouParameters* zhouParam = getZhouParam();
    return zhouParam->rhos;
  }

  RealType EAMAdapter::getRhol() {
    if (getEAMType() != eamZhou2004) {
      sprintf( painCave.errMsg,
               "EAMAdapter::getRhos was called on atomType %s\n"
               "\twhich is not a Zhou2004 type\n",
               at_->getName().c_str());
      painCave.severity = OPENMD_ERROR;
      painCave.isFatal = 1;
      simError();
    }
    ZhouParameters* zhouParam = getZhouParam();
    return zhouParam->rhol;
  }

  RealType EAMAdapter::getRhoh() {
    if (getEAMType() != eamZhou2004) {
      sprintf( painCave.errMsg,
               "EAMAdapter::getRhos was called on atomType %s\n"
               "\twhich is not a Zhou2004 type\n",
               at_->getName().c_str());
      painCave.severity = OPENMD_ERROR;
      painCave.isFatal = 1;
      simError();
    }
    ZhouParameters* zhouParam = getZhouParam();
    return zhouParam->rhoh;
  }

  std::vector<RealType> EAMAdapter::getOrhoLimits() {
    if (getEAMType() != eamZhou2005Oxygen) {
      sprintf( painCave.errMsg,
               "EAMAdapter::getOrhoLimits was called on atomType %s\n"
               "\twhich is not a Zhou2005Oxygen type\n",
               at_->getName().c_str());
      painCave.severity = OPENMD_ERROR;
      painCave.isFatal = 1;
      simError();
    }

    ZhouParameters* zhouParam = getZhouParam();
    return zhouParam->OrhoLimits;
  }
  std::vector<RealType> EAMAdapter::getOrhoE() {
    if (getEAMType() != eamZhou2005Oxygen) {
      sprintf( painCave.errMsg,
               "EAMAdapter::getOrhoE was called on atomType %s\n"
               "\twhich is not a Zhou2005Oxygen type\n",
               at_->getName().c_str());
      painCave.severity = OPENMD_ERROR;
      painCave.isFatal = 1;
      simError();
    }

    ZhouParameters* zhouParam = getZhouParam();
    return zhouParam->OrhoE;
  }

  std::vector<std::vector<RealType> > EAMAdapter::getOF() {
    if (getEAMType() != eamZhou2005Oxygen) {
      sprintf( painCave.errMsg,
               "EAMAdapter::getOF was called on atomType %s\n"
               "\twhich is not a Zhou2005Oxygen type\n",
               at_->getName().c_str());
      painCave.severity = OPENMD_ERROR;
      painCave.isFatal = 1;
      simError();
    }

    ZhouParameters* zhouParam = getZhouParam();
    return zhouParam->OF;
  }

  RealType EAMAdapter::getRMorse() {
    EVBParameters* evbParam = getEVBParam();
    return evbParam->r0;
  }
  RealType EAMAdapter::getDMorse() {
    EVBParameters* evbParam = getEVBParam();
    return evbParam->D0;
  }
  RealType EAMAdapter::getBetaMorse() {
    EVBParameters* evbParam = getEVBParam();
    return evbParam->beta0;
  }
  RealType EAMAdapter::getCoupling() {
    EVBParameters* evbParam = getEVBParam();
    return evbParam->c;
  }
  RealType EAMAdapter::getRcoupling() {
    EVBParameters* evbParam = getEVBParam();
    return evbParam->rc;
  }
  RealType EAMAdapter::getSigmaCoupling() {
    EVBParameters* evbParam = getEVBParam();
    return evbParam->sigma;
  }
}
