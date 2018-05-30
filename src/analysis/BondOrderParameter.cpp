/*
 * Copyright (c) 2005 The University of Notre Dame. All Rights Reserved.
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
 * [4] Kuang & Gezelter,  J. Chem. Phys. 133, 164101 (2010).
 * [4] , Stocker & Gezelter, J. Chem. Theory Comput. 7, 834 (2011).
 */

#include <sstream>
#include "analysis/BondOrderParameter.hpp"
#include "utils/simError.h"
#include "utils/Revision.hpp"
#include "io/DumpReader.hpp"
#include "primitives/Molecule.hpp"
#include "utils/Constants.hpp"
#include "math/Wigner3jm.hpp"

using namespace MATPACK;
namespace OpenMD {
  
  BondOrderParameter::BondOrderParameter(SimInfo* info, 
                                         const std::string& sele,
                                         double rCut, int nbins) 
    : NonSpatialStatistics(info, sele, nbins), selectionScript_(sele), seleMan_(info),
      evaluator_(info) {
    
    setAnalysisType("Bond Order Parameters");
    string prefixFileName = info_->getPrefixFileName();
    setOutputName(prefixFileName + ".bo");

    evaluator_.loadScriptString(sele);
    if (!evaluator_.isDynamic()) {
      seleMan_.setSelectionSet(evaluator_.evaluate());
    }

    
    usePeriodicBoundaryConditions_ = info_->getSimParams()->getUsePeriodicBoundaryConditions();
    // Set up cutoff radius and order of the Legendre Polynomial:

    rCut_ = rCut;
    
    std::stringstream params;
    params << " rcut = " << rCut_
           << ", nbins = " << nBins_;
    const std::string paramString = params.str();
    setParameterString( paramString );

    Qcount_.resize(lMax_+1);
    Wcount_.resize(lMax_+1);

    // Q can take values from 0 to 1

    MinQ_ = 0.0;
    MaxQ_ = 1.1;
    deltaQ_ = (MaxQ_ - MinQ_) / nbins;

    // W_6 for icosahedral clusters is 11 / sqrt(4199) = 0.169754, so we'll
    // use values for MinW_ and MaxW_ that are slightly larger than this:

    MinW_ = -1.1;
    MaxW_ = 1.1;
    deltaW_ = (MaxW_ - MinW_) / nbins;

    q_l_.resize(lMax_+1);
    q2_.resize(lMax_+1);
    w_.resize(lMax_+1);
    w_hat_.resize(lMax_+1);

    Q2_.resize(lMax_+1);
    Q_.resize(lMax_+1);
    W_.resize(lMax_+1);
    W_hat_.resize(lMax_+1);
    Nbonds_ = 0;


    // Make arrays for Wigner3jm
    RealType* THRCOF = new RealType[2*lMax_+1];
    // Variables for Wigner routine
    RealType lPass, m1Pass, m2m, m2M;
    int error, mSize;
    mSize = 2*lMax_+1;

    for (int l = 0; l <= lMax_; l++) {
      lPass = (RealType)l;
      for (int m1 = -l; m1 <= l; m1++) {
        m1Pass = (RealType)m1;

        std::pair<int,int> lm = std::make_pair(l, m1);
        
        // Zero work array
        for (int ii = 0; ii < 2*l + 1; ii++){
          THRCOF[ii] = 0.0;
        }

        // Get Wigner coefficients
        Wigner3jm(lPass, lPass, lPass, 
                  m1Pass, m2m, m2M, 
                  THRCOF, mSize, error);
       
        m2Min[lm] = (int)floor(m2m);
        m2Max[lm] = (int)floor(m2M);
        
        for (int mmm = 0; mmm <= (int)(m2M - m2m); mmm++) {
          w3j[lm].push_back(THRCOF[mmm]);
        }
      }
    }
    delete [] THRCOF;
    THRCOF = NULL;
   
  }
  
  BondOrderParameter::~BondOrderParameter() {
    Q_histogram_.clear();
    W_histogram_.clear();
    for (int l = 0; l <= lMax_; l++) {
      for (int m = -l; m <= l; m++) {
        w3j[std::make_pair(l,m)].clear();
      }
    }
    w3j.clear();
    m2Min.clear();
    m2Max.clear();
  }
  
  void BondOrderParameter::initializeHistogram() {
    for (int bin = 0; bin < nBins_; bin++) {
      for (int l = 0; l <= lMax_; l++) {
        Q_histogram_[std::make_pair(bin,l)] = 0;
        W_histogram_[std::make_pair(bin,l)] = 0;
      }
    }
  }

  void BondOrderParameter::processHistogram() {
    
    /* 
       The rest of the analysis (post dump loop) needs to go somewhere general
    */
         
    // Normalize Qbar2
    for (int l = 0; l <= lMax_; l++) {
      for (int m = -l; m <= l; m++){
        QBar_[std::make_pair(l,m)] /= Nbonds_;
      }
    }
    
    // Find second order invariant Q_l
    
    for (int l = 0; l <= lMax_; l++) {
      Q2_[l] = 0.0;
      for (int m = -l; m <= l; m++){
        Q2_[l] += norm(QBar_[std::make_pair(l,m)]);
      }
      Q_[l] = sqrt(Q2_[l] * 4.0 * Constants::PI / (RealType)(2*l + 1));
    }
    
    // Find Third Order Invariant W_l
    
    for (int l = 0; l <= lMax_; l++) {
      W_[l] = 0.0;
      for (int m1 = -l; m1 <= l; m1++) {
        std::pair<int,int> lm = std::make_pair(l, m1);
        for (int mmm = 0; mmm <= (m2Max[lm] - m2Min[lm]); mmm++) {
          int m2 = m2Min[lm] + mmm;
          int m3 = -m1-m2;
          W_[l] += w3j[lm][mmm] * QBar_[lm] * 
            QBar_[std::make_pair(l,m2)] * QBar_[std::make_pair(l,m3)];
        }
      }
      
      W_hat_[l] = W_[l] / pow(Q2_[l], RealType(1.5));
    }

  }

  
  void BondOrderParameter::processFrame(int istep) {
    Molecule* mol;
    Atom* atom;
    int myIndex;
    SimInfo::MoleculeIterator mi;
    Molecule::AtomIterator ai;
    StuntDouble* sd;
    Vector3d vec;
    RealType costheta;
    RealType phi;
    RealType r;
    SphericalHarmonic sphericalHarmonic;
    int i;

      
      if (evaluator_.isDynamic()) {
        seleMan_.setSelectionSet(evaluator_.evaluate());
      }
            
      // outer loop is over the selected StuntDoubles:

      for (sd = seleMan_.beginSelected(i); sd != NULL; 
           sd = seleMan_.nextSelected(i)) {

        myIndex = sd->getGlobalIndex();
        nBonds_ = 0;
        
        for (int l = 0; l <= lMax_; l++) {
          for (int m = -l; m <= l; m++) {
            q_[std::make_pair(l,m)] = 0.0;
          }
        }
        
        // inner loop is over all other atoms in the system:
        
        for (mol = info_->beginMolecule(mi); mol != NULL; 
             mol = info_->nextMolecule(mi)) {
          for (atom = mol->beginAtom(ai); atom != NULL; 
               atom = mol->nextAtom(ai)) {

            if (atom->getGlobalIndex() != myIndex) {

              vec = sd->getPos() - atom->getPos();       

              if (usePeriodicBoundaryConditions_) 
                currentSnapshot_->wrapVector(vec);
              
              // Calculate "bonds" and build Q_lm(r) where 
              //      Q_lm = Y_lm(theta(r),phi(r))                
              // The spherical harmonics are wrt any arbitrary coordinate
              // system, we choose standard spherical coordinates 
              
              r = vec.length();
              
              // Check to see if neighbor is in bond cutoff 
              
              if (r < rCut_) { 
                costheta = vec.z() / r; 
                phi = atan2(vec.y(), vec.x());

                for (int l = 0; l <= lMax_; l++) {
                  sphericalHarmonic.setL(l);
                  for(int m = -l; m <= l; m++){
                    sphericalHarmonic.setM(m);
                    q_[std::make_pair(l,m)] += sphericalHarmonic.getValueAt(costheta, phi);

                  }
                }
                nBonds_++;
              }  
            }
          }
        }
        
        
        for (int l = 0; l <= lMax_; l++) {
          q2_[l] = 0.0;
          for (int m = -l; m <= l; m++){
            q_[std::make_pair(l,m)] /= (RealType)nBonds_; 

            q2_[l] += norm(q_[std::make_pair(l,m)]);
          }
          q_l_[l] = sqrt(q2_[l] * 4.0 * Constants::PI / (RealType)(2*l + 1));
        }
        
        // Find Third Order Invariant W_l
    
        for (int l = 0; l <= lMax_; l++) {
          w_[l] = 0.0;
          for (int m1 = -l; m1 <= l; m1++) {
            std::pair<int,int> lm = std::make_pair(l, m1);
            for (int mmm = 0; mmm <= (m2Max[lm] - m2Min[lm]); mmm++) {
              int m2 = m2Min[lm] + mmm;
              int m3 = -m1-m2;
              w_[l] += w3j[lm][mmm] * q_[lm] * 
                q_[std::make_pair(l,m2)] *  q_[std::make_pair(l,m3)];
            }
          }
          
          w_hat_[l] = w_[l] / pow(q2_[l], RealType(1.5));
        }

        collectHistogram(q_l_, w_hat_);
        
        Nbonds_ += nBonds_;
        for (int l = 0; l <= lMax_;  l++) {
          for (int m = -l; m <= l; m++) {
            QBar_[std::make_pair(l,m)] += (RealType)nBonds_*q_[std::make_pair(l,m)];
          }
        }
      }
  }

  void BondOrderParameter::processStuntDouble(StuntDouble* sd, int bin) {
    // Fill in later
  }
  
  void BondOrderParameter::collectHistogram(std::vector<RealType> q, 
                                            std::vector<ComplexType> what) {

    for (int l = 0; l <= lMax_; l++) {
      if (q[l] >= MinQ_ && q[l] < MaxQ_) {
        int qbin = int((q[l] - MinQ_) / deltaQ_);
        Q_histogram_[std::make_pair(qbin,l)] += 1;
        Qcount_[l]++;      
      } else {
        sprintf( painCave.errMsg,
                 "q_l value outside reasonable range\n");
        painCave.severity = OPENMD_ERROR;
        painCave.isFatal = 1;
        simError();  
      }
    }

    for (int l = 0; l <= lMax_; l++) {
      if (real(what[l]) >= MinW_ && real(what[l]) < MaxW_) {
        int wbin = int((real(what[l]) - MinW_) / deltaW_);
        W_histogram_[std::make_pair(wbin,l)] += 1;
        Wcount_[l]++;      
      } else {
        sprintf( painCave.errMsg,
                 "Re[w_hat] value (%lf) outside reasonable range\n",
                 real(what[l]));
        painCave.severity = OPENMD_ERROR;
        painCave.isFatal = 1;
        simError();  
      }
    }

  }  


  void BondOrderParameter::writeOutput() {
    Revision rev; 
    std::ofstream osq((getOutputFileName() + "q").c_str());
    if (osq.is_open()) {
      osq << "# " << getAnalysisType() << "\n";
      osq << "# OpenMD " << rev.getFullRevision() << "\n";
      osq << "# " << rev.getBuildDate() << "\n";
      osq << "# selection script: \"" << selectionScript_  << "\"\n";
      if (!paramString_.empty())
        osq << "# parameters: " << paramString_ << "\n";
      
      for (int l = 0; l <= lMax_; l++) {
        osq << "# <Q_" << l << ">: " << Q_[l] << "\n";
      }
      // Normalize by number of frames and write it out:
      for (int i = 0; i < nBins_; ++i) {
        RealType Qval = MinQ_ + (i + 0.5) * deltaQ_;               
        osq << Qval;
        for (int l = 0; l <= lMax_; l++) {
          osq << "\t"
              << (RealType)Q_histogram_[std::make_pair(i,l)]/(RealType)Qcount_[l]/deltaQ_;
        }
        osq << "\n";
      }

      osq.close();

    } else {
      sprintf(painCave.errMsg, "BondOrderParameter: unable to open %s\n", 
              (getOutputFileName() + "q").c_str());
      painCave.isFatal = 1;
      simError();  
    }

  
    std::ofstream osw((getOutputFileName() + "w").c_str());

    if (osw.is_open()) {
      osq << "# " << getAnalysisType() << "\n";
      osq << "# OpenMD " << rev.getFullRevision() << "\n";
      osq << "# " << rev.getBuildDate() << "\n";
      osq << "# selection script: \"" << selectionScript_  << "\"\n";
      if (!paramString_.empty())
        osq << "# parameters: " << paramString_ << "\n";

      for (int l = 0; l <= lMax_; l++) {
        osw << "# <W_" << l << ">: " << real(W_hat_[l]) << "\t"
            << imag(W_hat_[l]) << "\n";
      }
      // Normalize by number of frames and write it out:
      for (int i = 0; i < nBins_; ++i) {
        RealType Wval = MinW_ + (i + 0.5) * deltaW_;               
        osw << Wval;
        for (int l = 0; l <= lMax_; l++) {

          osw << "\t" << (RealType)W_histogram_[std::make_pair(i,l)]/(RealType)Wcount_[l]/deltaW_;
        }
        osw << "\n";
      }

      osw.close();
    } else {
      sprintf(painCave.errMsg, "BondOrderParameter: unable to open %s\n", 
              (getOutputFileName() + "w").c_str());
      painCave.isFatal = 1;
      simError();  
    }

  }
}