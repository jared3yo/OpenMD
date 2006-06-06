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
 
#include "io/MultipoleAtomTypesSectionParser.hpp"
#include "UseTheForce/ForceField.hpp"
#include "utils/NumericConstant.hpp"
#include "utils/simError.h"
namespace oopse {

  MultipoleAtomTypesSectionParser::MultipoleAtomTypesSectionParser(ForceFieldOptions& options) : options_(options){
    setSectionName("MultipoleAtomTypes");
  }

  void MultipoleAtomTypesSectionParser::parseLine(ForceField& ff,const std::string& line, int lineNo){
    StringTokenizer tokenizer(line);
    int nTokens = tokenizer.countTokens();    

    // name multipole_type theta phi psi 
    // "name" must match the name in the AtomTypes section
    // avaliable multipole type is d (dipole), s (split dipole) ,  q (quadrupoles), dq(dipole plus quadrupole)
    // and sq(split dipole plus quadrupole)
    // Directionality for dipoles and quadrupoles is given by three euler angles (phi, theta, psi),
    //because the body-fixed reference frame for directional atoms is determined by the *mass* 
    //distribution and not by the charge distribution.  
    // Dipoles are given in units of Debye  
    // Quadrupoles are given in units of 
    // examples:
    // name d phi theta psi dipole_moment
    // name s phi theta psi dipole_moment splitdipole_distance
    // name q phi theta psi Qxx Qyy Qzz
    // name dq phi theta psi dipole_moment Qxx Qyy Qzz
    // name sq phi theta psi dipole_moment splitdipole_distance Qxx Qyy Qzz
        
    if (nTokens < 5)  {
      sprintf(painCave.errMsg, "MultipoleAtomTypesSectionParser Error: Not enough tokens at line %d\n",
	      lineNo);
      painCave.isFatal = 1;
      simError();
    } else {

      std::string atomTypeName = tokenizer.nextToken();    
      std::string multipoleType = tokenizer.nextToken();
      RealType phi = tokenizer.nextTokenAsDouble() * NumericConstant::PI /180.0;
      RealType theta = tokenizer.nextTokenAsDouble() * NumericConstant::PI /180.0;
      RealType psi = tokenizer.nextTokenAsDouble() * NumericConstant::PI /180.0;        
      nTokens -=  5;

      AtomType* atomType = ff.getAtomType(atomTypeName);
      if (atomType == NULL) {
	sprintf(painCave.errMsg, "MultipoleAtomTypesSectionParser Error: Can not find matched AtomType[%s] at line %d\n",
		atomTypeName.c_str(), lineNo);
	painCave.isFatal = 1;
	simError();
      }
        
      DirectionalAtomType* dAtomType = dynamic_cast<DirectionalAtomType*>(atomType);            
      if (dAtomType == NULL) {
	sprintf(painCave.errMsg, "MultipoleAtomTypesSectionParser Error: Can not Cast Atom to DirectionalAtom at line %d\n", lineNo);
	painCave.isFatal = 1;
	simError();
      }        
        
      RotMat3x3d electroBodyFrame(phi, theta, psi);        
      dAtomType->setElectroBodyFrame(electroBodyFrame);        

      if (multipoleType== "d") {
	parseDipole(tokenizer, dAtomType, lineNo);
      } else if (multipoleType== "s") {
	parseSplitDipole(tokenizer, dAtomType, lineNo);
      } else if (multipoleType== "q") {
	parseQuadruple( tokenizer, dAtomType, lineNo);
      } else if (multipoleType== "dq") {
	parseDipole(tokenizer, dAtomType, lineNo);
	parseQuadruple( tokenizer, dAtomType, lineNo);
      } else if (multipoleType== "sq") {
	parseSplitDipole(tokenizer, dAtomType, lineNo);
	parseQuadruple( tokenizer, dAtomType, lineNo);
      } else {
	sprintf(painCave.errMsg, "MultipoleAtomTypesSectionParser Error: unrecognized multiple type at line %d\n",
		lineNo);
	painCave.isFatal = 1;
	simError();
      }
    }

  }

  void MultipoleAtomTypesSectionParser::parseDipole(StringTokenizer& tokenizer, 
						    DirectionalAtomType* dAtomType, int lineNo) {

    if (tokenizer.hasMoreTokens()) {
      RealType dipole = tokenizer.nextTokenAsDouble();

      dAtomType->addProperty(new DoubleGenericData("Dipole", dipole));
      dAtomType->setDipole();
    } else {
      sprintf(painCave.errMsg, "MultipoleAtomTypesSectionParser Error: Not enough tokens at line %d\n",
	      lineNo);
      painCave.isFatal = 1;
      simError();
    }
  }

  void MultipoleAtomTypesSectionParser::parseSplitDipole(StringTokenizer& tokenizer, 
							 DirectionalAtomType* dAtomType, int lineNo) {

    if (tokenizer.hasMoreTokens()) {
      parseDipole(tokenizer, dAtomType, lineNo);    
      RealType splitDipoleDistance = tokenizer.nextTokenAsDouble();
      dAtomType->addProperty(new DoubleGenericData("SplitDipoleDistance", splitDipoleDistance));
      dAtomType->setSplitDipole();
    } else {
      sprintf(painCave.errMsg, "MultipoleAtomTypesSectionParser Error: Not enough tokens at line %d\n",
	      lineNo);
      painCave.isFatal = 1;
      simError();
    }
  }

  void MultipoleAtomTypesSectionParser::parseQuadruple(StringTokenizer& tokenizer,
						       DirectionalAtomType* dAtomType, int lineNo) {
    int nTokens = tokenizer.countTokens();   
    if (nTokens >= 3) {
      Vector3d Q;
      Q[0] = tokenizer.nextTokenAsDouble();
      Q[1] = tokenizer.nextTokenAsDouble();
      Q[2] = tokenizer.nextTokenAsDouble();

      RealType trace =  Q[0] + Q[1] + Q[2];

      if (fabs(trace) > oopse::epsilon) {
	sprintf(painCave.errMsg, "MultipoleAtomTypesSectionParser Error: the trace of quadrupole moments is not zero at line %d\n",
		lineNo);
	painCave.isFatal = 1;
	simError();
      }

      dAtomType->addProperty(new Vector3dGenericData("QuadrupoleMoments", Q));
      dAtomType->setQuadrupole();
    } else {
      sprintf(painCave.errMsg, "MultipoleAtomTypesSectionParser Error: Not enough tokens at line %d\n",
	      lineNo);
      painCave.isFatal = 1;
      simError();
    }
  }


} //end namespace oopse



