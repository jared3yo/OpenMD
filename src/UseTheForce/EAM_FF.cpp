#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <iostream>
using namespace std;

#ifdef IS_MPI
#include <mpi.h>
#endif //is_mpi

#include "UseTheForce/ForceFields.hpp"
#include "primitives/SRI.hpp"
#include "utils/simError.h"
#include "types/AtomType.hpp"
#include "UseTheForce/DarkSide/eam_interface.h"

//#include "UseTheForce/fortranWrappers.hpp"

#ifdef IS_MPI
#include "UseTheForce/mpiForceField.h"
#endif // is_mpi



namespace EAM_NS{

  // Declare the structures that will be passed by the parser and  MPI
  
  typedef struct{
    char name[15];
    double mass;
    double lattice_constant;
    double eam_drho;  // The distance between each of the points indexed by rho.
    double eam_rcut;  // The cutoff radius for eam.
    double eam_dr;    // The distance between each of the rho points.    
    int eam_nrho;  // Number of points indexed by rho
    int eam_nr;    // The number of points based on r (Both Phi(r) and Rho(r)).
    int eam_ident; // Atomic number
    int ident;
    int last;      //  0  -> default
                   //  1  -> in MPI: tells nodes to stop listening
  } atomStruct;

  int parseAtom( char *lineBuffer, int lineNum, atomStruct &info, char *eamPotFile );
  int parseEAM( atomStruct &info, char *eamPotFile, double **eam_rvals,
		double **eam_rhovals, double **eam_Frhovals);
#ifdef IS_MPI
  
  MPI_Datatype mpiAtomStructType;
  
#endif

  class LinkedAtomType {
  public:
    LinkedAtomType(){ 
      next = NULL;
      name[0] = '\0';
      eam_rvals    = NULL;
      eam_rhovals  = NULL;
      eam_Frhovals = NULL;
    }

    ~LinkedAtomType(){ 
      if( next != NULL ) delete next;
      if( eam_rvals != NULL ) delete[] eam_rvals;
      if( eam_rhovals != NULL ) delete[] eam_rhovals;
      if( eam_Frhovals != NULL ) delete[] eam_Frhovals;	
    }

    LinkedAtomType* find(char* key){ 
      if( !strcmp(name, key) ) return this;
      if( next != NULL ) return next->find(key);
      return NULL;
    }
    

    void add( atomStruct &info, double *the_eam_rvals,
	      double *the_eam_rhovals,double *the_eam_Frhovals ){

      // check for duplicates
      
      if( !strcmp( info.name, name ) ){
	sprintf( painCave.errMsg,
		 "Duplicate EAM atom type \"%s\" found in "
		 "the EAM_FF param file./n",
		 name );
	painCave.isFatal = 1;
	simError();
      }
      
      if( next != NULL ) next->add(info, the_eam_rvals, the_eam_rhovals, the_eam_Frhovals);
      else{
	next = new LinkedAtomType();
	strcpy(next->name, info.name);
	next->mass             = info.mass;
	next->lattice_constant = info.lattice_constant;
	next->eam_nrho         = info.eam_nrho;
	next->eam_drho         = info.eam_drho;
	next->eam_nr           = info.eam_nr;
	next->eam_dr           = info.eam_dr;
	next->eam_rcut         = info.eam_rcut;
	next->eam_ident        = info.eam_ident;
	next->ident            = info.ident;

	next->eam_rvals    = the_eam_rvals;
	next->eam_rhovals  = the_eam_rhovals;
	next->eam_Frhovals = the_eam_Frhovals;
      }
    }
    

#ifdef IS_MPI
    
    void duplicate( atomStruct &info ){
      strcpy(info.name, name);
      info.mass             = mass;
      info.lattice_constant = lattice_constant;
      info.eam_nrho         = eam_nrho;
      info.eam_drho         = eam_drho;
      info.eam_nr           = eam_nr;
      info.eam_dr           = eam_dr;
      info.eam_rcut         = eam_rcut;
      info.eam_ident        = eam_ident;
      info.ident            = ident;
      info.last             = 0;
    }


#endif

    char name[15];
    double mass;
    double lattice_constant;
    int eam_nrho; // Number of points indexed by rho
    double eam_drho; // The distance between each of the points indexed by rho.
    int eam_nr;   // The number of points based on r (Both Phi(r) and Rho(r)).
    double eam_dr;   // The distance between each of the rho points.
    double eam_rcut; // The cutoff radius for eam.

    double *eam_rvals;    // Z of r values
    double *eam_rhovals;  // rho of r values
    double *eam_Frhovals; // F of rho values
    int eam_ident;        // eam identity (atomic number)
    int ident;
    LinkedAtomType* next;
  };

  LinkedAtomType* headAtomType; 
  LinkedAtomType* currentAtomType;

}

using namespace EAM_NS;

//****************************************************************
// begins the actual forcefield stuff.	
//****************************************************************

EAM_FF::EAM_FF() {
  EAM_FF("");
}

EAM_FF::EAM_FF(char* the_variant){

  char fileName[200];
  char* ffPath_env = "FORCE_PARAM_PATH";
  char* ffPath;
  char temp[200];

  headAtomType = NULL;
  currentAtomType = NULL;

  // Set eamRcut to 0.0
  eamRcut = 0.0;

#ifdef IS_MPI
  int i;
  
   // **********************************************************************
  // Init the atomStruct mpi type

  atomStruct atomProto; // mpiPrototype
  int atomBC[3] = {15,5,5};  // block counts
  MPI_Aint atomDspls[3];           // displacements
  MPI_Datatype atomMbrTypes[3];    // member mpi types

  MPI_Address(&atomProto.name, &atomDspls[0]);
  MPI_Address(&atomProto.mass, &atomDspls[1]);
  MPI_Address(&atomProto.eam_nrho, &atomDspls[2]);
  
  atomMbrTypes[0] = MPI_CHAR;
  atomMbrTypes[1] = MPI_DOUBLE;
  atomMbrTypes[2] = MPI_INT;
  
  for (i=2; i >= 0; i--) atomDspls[i] -= atomDspls[0];
  
  MPI_Type_struct(3, atomBC, atomDspls, atomMbrTypes, &mpiAtomStructType);
  MPI_Type_commit(&mpiAtomStructType);

  // ***********************************************************************
  
  if( worldRank == 0 ){
#endif
    
    // generate the force file name   

    strcpy( fileName, "EAM" );

    if (strlen(the_variant) > 0) {
      has_variant = 1;
      strcpy( variant, the_variant);
      strcat( fileName, ".");
      strcat( fileName, variant );

      sprintf( painCave.errMsg,
               "Using %s variant of EAM force field.\n",
               variant );
      painCave.severity = OOPSE_INFO;
      painCave.isFatal = 0;
      simError();
    }
    strcat( fileName, ".frc");

    //fprintf( stderr,"Trying to open %s\n", fileName );
    
    // attempt to open the file in the current directory first.
    
    frcFile = fopen( fileName, "r" );
    
    if( frcFile == NULL ){
      
      // next see if the force path enviorment variable is set
      
      ffPath = getenv( ffPath_env );
      if( ffPath == NULL ) {
	STR_DEFINE(ffPath, FRC_PATH );
      }
      
      
      strcpy( temp, ffPath );
      strcat( temp, "/" );
      strcat( temp, fileName );
      strcpy( fileName, temp );
      
      frcFile = fopen( fileName, "r" );
      
      if( frcFile == NULL ){
        
	sprintf( painCave.errMsg,
		 "Error opening the force field parameter file:\n"
                 "\t%s\n"
		 "\tHave you tried setting the FORCE_PARAM_PATH environment "
		 "variable?\n",
		 fileName );
        painCave.severity = OOPSE_ERROR;
	painCave.isFatal = 1;
	simError();
      }
    }

    
#ifdef IS_MPI
  }
  
  sprintf( checkPointMsg, "EAM_FF file opened sucessfully." );
  MPIcheckPoint();
  
#endif // is_mpi
}


EAM_FF::~EAM_FF(){

  if( headAtomType != NULL ) delete headAtomType;

#ifdef IS_MPI
  if( worldRank == 0 ){
#endif // is_mpi
    
    fclose( frcFile );
    
#ifdef IS_MPI
  }
#endif // is_mpi
}


void EAM_FF::calcRcut( void ){
  
#ifdef IS_MPI
  double tempEamRcut = eamRcut;
  MPI_Allreduce( &tempEamRcut, &eamRcut, 1, MPI_DOUBLE, MPI_MAX,
		 MPI_COMM_WORLD);
#endif  //is_mpi
  entry_plug->setDefaultRcut(eamRcut);
}


void EAM_FF::initForceField( ){
  initFortran(0);
}

void EAM_FF::cleanMe( void ){

#ifdef IS_MPI
  
  // keep the linked list in the mpi version

#else // is_mpi

  // delete the linked list in the single processor version

  if( headAtomType != NULL ) delete headAtomType;

#endif // is_mpi
}


void EAM_FF::readParams( void ){

  atomStruct info;
  info.last = 1; // initialize last to have the last set. 
                 // if things go well, last will be set to 0

  int identNum;
  double *eam_rvals;    // Z of r values
  double *eam_rhovals;  // rho of r values
  double *eam_Frhovals; // F of rho values
  char eamPotFile[1000];

  

  bigSigma = 0.0;
#ifdef IS_MPI
  if( worldRank == 0 ){
#endif
    
    // read in the atom types.

    headAtomType = new LinkedAtomType;

    fastForward( "AtomTypes", "eam atom readParams" );

    // we are now at the AtomTypes section.
    
    eof_test = fgets( readLine, sizeof(readLine), frcFile );
    lineNum++;
    
    
    // read a line, and start parseing out the atom types 

    if( eof_test == NULL ){
      sprintf( painCave.errMsg, 
	       "Error in reading Atoms from force file at line %d.\n",
	       lineNum );
      painCave.isFatal = 1;
      simError();
    }
    
    identNum = 1;
    // stop reading at end of file, or at next section

    while( readLine[0] != '#' && eof_test != NULL ){

      // toss comment lines
      if( readLine[0] != '!' ){
	
	// the parser returns 0 if the line was blank
	if( parseAtom( readLine, lineNum, info, eamPotFile ) ){
	  parseEAM(info,eamPotFile, &eam_rvals,
		   &eam_rhovals, &eam_Frhovals);
	  info.ident = identNum;
	  headAtomType->add( info, eam_rvals,
			     eam_rhovals,eam_Frhovals );
	  identNum++;
	}
      }
      eof_test = fgets( readLine, sizeof(readLine), frcFile );
      lineNum++;
    }
    
    

#ifdef IS_MPI
  
    
    // send out the linked list to all the other processes

    sprintf( checkPointMsg,
	     "EAM_FF atom structures read successfully." );
    MPIcheckPoint();

    currentAtomType = headAtomType->next; //skip the first element who is a place holder.
    while( currentAtomType != NULL ){
      currentAtomType->duplicate( info );

 

      sendFrcStruct( &info, mpiAtomStructType );

      // We have to now broadcast the Arrays
      MPI_Bcast(currentAtomType->eam_rvals,
		currentAtomType->eam_nr,
		MPI_DOUBLE,0,MPI_COMM_WORLD);
      MPI_Bcast(currentAtomType->eam_rhovals,
		currentAtomType->eam_nr,
		MPI_DOUBLE,0,MPI_COMM_WORLD);
      MPI_Bcast(currentAtomType->eam_Frhovals,
		currentAtomType->eam_nrho,
		MPI_DOUBLE,0,MPI_COMM_WORLD);

      sprintf( checkPointMsg,
	       "successfully sent EAM force type: \"%s\"\n",
	       info.name );
      MPIcheckPoint();

      currentAtomType = currentAtomType->next;
    }
    info.last = 1;
    sendFrcStruct( &info, mpiAtomStructType );
    
  }

  else{
    
    // listen for node 0 to send out the force params
    
    MPIcheckPoint();

    headAtomType = new LinkedAtomType;
    receiveFrcStruct( &info, mpiAtomStructType );

    while( !info.last ){
      
      // allocate the arrays

      eam_rvals    = new double[info.eam_nr];
      eam_rhovals  = new double[info.eam_nr];
      eam_Frhovals = new double[info.eam_nrho];

      // We have to now broadcast the Arrays
      MPI_Bcast(eam_rvals,
		info.eam_nr,
		MPI_DOUBLE,0,MPI_COMM_WORLD);
      MPI_Bcast(eam_rhovals,
		info.eam_nr,
		MPI_DOUBLE,0,MPI_COMM_WORLD);
      MPI_Bcast(eam_Frhovals,
		info.eam_nrho,
		MPI_DOUBLE,0,MPI_COMM_WORLD);
      

      headAtomType->add( info, eam_rvals, eam_rhovals, eam_Frhovals );
      
      MPIcheckPoint();

      receiveFrcStruct( &info, mpiAtomStructType );


    }
  }
#endif // is_mpi

  // call new A_types in fortran
  
  int isError;

  // dummy variables
  
  currentAtomType = headAtomType->next;
  while( currentAtomType != NULL ){
    
    if( currentAtomType->name[0] != '\0' ){

      AtomType* at = new AtomType();
      at->setIdent(currentAtomType->ident);
      at->setName(currentAtomType->name);
      at->setEAM();
      at->complete();

    }    
    
    currentAtomType = currentAtomType->next;
  }
      
  entry_plug->useLennardJones = 0;
  entry_plug->useEAM = 1;
  // Walk down again and send out EAM type
  currentAtomType = headAtomType->next;
  while( currentAtomType != NULL ){
    
    if( currentAtomType->name[0] != '\0' ){
      isError = 0;

      newEAMtype( &(currentAtomType->lattice_constant),
		  &(currentAtomType->eam_nrho),
		  &(currentAtomType->eam_drho),
		  &(currentAtomType->eam_nr),
		  &(currentAtomType->eam_dr),
		  &(currentAtomType->eam_rcut),
		  currentAtomType->eam_rvals,
		  currentAtomType->eam_rhovals,
		  currentAtomType->eam_Frhovals,
		  &(currentAtomType->eam_ident),
		  &isError);

      if( isError ){
	sprintf( painCave.errMsg,
		 "Error initializing the \"%s\" atom type in fortran EAM\n",
		 currentAtomType->name );
	painCave.isFatal = 1;
	simError();
      }
    }
    currentAtomType = currentAtomType->next;
  }



#ifdef IS_MPI
  sprintf( checkPointMsg,
	   "EAM_FF atom structures successfully sent to fortran\n" );
  MPIcheckPoint();
#endif // is_mpi



}


void EAM_FF::initializeAtoms( int nAtoms, Atom** the_atoms ){
  
  int i;
  
  // initialize the atoms
  
  for( i=0; i<nAtoms; i++ ){
    
    currentAtomType = headAtomType->find( the_atoms[i]->getType() );
    if( currentAtomType == NULL ){
      sprintf( painCave.errMsg, 
	       "AtomType error, %s not found in force file.\n",
	       the_atoms[i]->getType() );
      painCave.isFatal = 1;
      simError();
    }
    
    the_atoms[i]->setMass( currentAtomType->mass );
    the_atoms[i]->setIdent( currentAtomType->ident );

    if (eamRcut < currentAtomType->eam_rcut) eamRcut = currentAtomType->eam_rcut;
    
  }
}

void EAM_FF::initializeBonds( int nBonds, Bond** BondArray,
			     bond_pair* the_bonds ){
  
    if( nBonds ){
      sprintf( painCave.errMsg,
	       "LJ_FF does not support bonds.\n" );
      painCave.isFatal = 1;
      simError();
    }
}

void EAM_FF::initializeBends( int nBends, Bend** bendArray,
			     bend_set* the_bends ){

    if( nBends ){
      sprintf( painCave.errMsg,
	       "LJ_FF does not support bends.\n" );
      painCave.isFatal = 1;
      simError();
    }
}

void EAM_FF::initializeTorsions( int nTorsions, Torsion** torsionArray,
				torsion_set* the_torsions ){

    if( nTorsions ){
      sprintf( painCave.errMsg,
	       "LJ_FF does not support torsions.\n" );
      painCave.isFatal = 1;
      simError();
    }
}

void EAM_FF::fastForward( char* stopText, char* searchOwner ){

  int foundText = 0;
  char* the_token;

  rewind( frcFile );
  lineNum = 0;

  eof_test = fgets( readLine, sizeof(readLine), frcFile );
  lineNum++;
  if( eof_test == NULL ){
    sprintf( painCave.errMsg, "Error fast forwarding force file for %s: "
	     " file is empty.\n",
	     searchOwner );
    painCave.isFatal = 1;
    simError();
  }
  
  
  while( !foundText ){
    while( eof_test != NULL && readLine[0] != '#' ){
      eof_test = fgets( readLine, sizeof(readLine), frcFile );
      lineNum++;
    }
    if( eof_test == NULL ){
      sprintf( painCave.errMsg,
	       "Error fast forwarding force file for %s at "
	       "line %d: file ended unexpectedly.\n",
	       searchOwner,
	       lineNum );
      painCave.isFatal = 1;
      simError();
    }
    
    the_token = strtok( readLine, " ,;\t#\n" );
    foundText = !strcmp( stopText, the_token );
    
    if( !foundText ){
      eof_test = fgets( readLine, sizeof(readLine), frcFile );
      lineNum++;
      
      if( eof_test == NULL ){
	sprintf( painCave.errMsg, 
		 "Error fast forwarding force file for %s at "
		 "line %d: file ended unexpectedly.\n",
		 searchOwner,
		 lineNum );
	painCave.isFatal = 1;
	simError();
      } 
    }
  }  
}



int EAM_NS::parseAtom( char *lineBuffer, int lineNum,   atomStruct &info, char *eamPotFile ){

  char* the_token;
  
  the_token = strtok( lineBuffer, " \n\t,;" );
  if( the_token != NULL ){
    
    strcpy( info.name, the_token );
	  
    if( ( the_token = strtok( NULL, " \n\t,;" ) ) == NULL ){
      sprintf( painCave.errMsg, 
	       "Error parseing AtomTypes: line %d\n", lineNum );
      painCave.isFatal = 1;
      simError();
    }
    
    info.mass = atof( the_token );
    
    if( ( the_token = strtok( NULL, " \n\t,;" ) ) == NULL ){
      sprintf( painCave.errMsg, 
	       "Error parseing AtomTypes: line %d\n", lineNum );
      painCave.isFatal = 1;
      simError();
    }
	
    strcpy( eamPotFile, the_token );
    return 1;
  }
  else return 0;
}

int EAM_NS::parseEAM(atomStruct &info, char *eamPotFile, 
		     double **eam_rvals, 
		     double **eam_rhovals,
		     double **eam_Frhovals){
  double* myEam_rvals;
  double* myEam_rhovals;
  double* myEam_Frhovals;

  char* ffPath_env = "FORCE_PARAM_PATH";
  char* ffPath;
  char* the_token;
  char* eam_eof_test;
  FILE *eamFile;
  const int BUFFERSIZE = 3000;

  char temp[200];
  int linenumber;
  int nReadLines;
  char eam_read_buffer[BUFFERSIZE];


  int i,j;

  linenumber = 0;

  // Open eam file
  eamFile = fopen( eamPotFile, "r" );
  
  
  if( eamFile == NULL ){
    
      // next see if the force path enviorment variable is set
    
    ffPath = getenv( ffPath_env );
    if( ffPath == NULL ) {
      STR_DEFINE(ffPath, FRC_PATH );
    }
    
    
    strcpy( temp, ffPath );
    strcat( temp, "/" );
    strcat( temp, eamPotFile );
    strcpy( eamPotFile, temp );
    
    eamFile = fopen( eamPotFile, "r" );

    
    
    if( eamFile == NULL ){
      
      sprintf( painCave.errMsg,
	       "Error opening the EAM force parameter file: %s\n"
	       "Have you tried setting the FORCE_PARAM_PATH environment "
	       "variable?\n",
	       eamPotFile );
      painCave.isFatal = 1;
      simError();
    }
  }

  // First line is a comment line, read and toss it....
  eam_eof_test = fgets(eam_read_buffer, sizeof(eam_read_buffer),eamFile);
  linenumber++;
  if(eam_eof_test == NULL){
    sprintf( painCave.errMsg,
	     "error in reading commment in %s\n", eamPotFile);
    painCave.isFatal = 1;
    simError();
  }



  // The Second line contains atomic number, atomic mass and a lattice constant
  eam_eof_test = fgets(eam_read_buffer, sizeof(eam_read_buffer),eamFile);
  linenumber++;
  if(eam_eof_test == NULL){
    sprintf( painCave.errMsg,
	     "error in reading Identifier line in %s\n", eamPotFile);
    painCave.isFatal = 1;
    simError();
  }



    
  if ( (the_token = strtok( eam_read_buffer, " \n\t,;")) == NULL){
    sprintf( painCave.errMsg, 
	     "Error parsing EAM ident  line in %s\n", eamPotFile );
    painCave.isFatal = 1;
    simError();
  }
  
  info.eam_ident = atoi( the_token );
 
  if( ( the_token = strtok( NULL, " \n\t,;" ) ) == NULL ){
    sprintf( painCave.errMsg, 
	     "Error parsing EAM mass in %s\n", eamPotFile );
    painCave.isFatal = 1;
    simError();
  }
  info.mass = atof( the_token);
 
  if( ( the_token = strtok( NULL, " \n\t,;" ) ) == NULL ){
    sprintf( painCave.errMsg, 
	     "Error parsing EAM Lattice Constant %s\n", eamPotFile );
    painCave.isFatal = 1;
    simError();
  }
  info.lattice_constant = atof( the_token);
 
  // Next line is nrho, drho, nr, dr and rcut
  eam_eof_test = fgets(eam_read_buffer, sizeof(eam_read_buffer),eamFile);
  if(eam_eof_test == NULL){
    sprintf( painCave.errMsg,
	     "error in reading number of points line in %s\n", eamPotFile);
    painCave.isFatal = 1;
    simError();
  }

  if ( (the_token = strtok( eam_read_buffer, " \n\t,;")) == NULL){
    sprintf( painCave.errMsg, 
	     "Error parseing EAM nrho: line in %s\n", eamPotFile );
    painCave.isFatal = 1;
    simError();
  }
  
  info.eam_nrho = atoi( the_token );
  
  if( ( the_token = strtok( NULL, " \n\t,;" ) ) == NULL ){
    sprintf( painCave.errMsg, 
	     "Error parsing EAM drho in %s\n", eamPotFile );
    painCave.isFatal = 1;
    simError();
  }
  info.eam_drho = atof( the_token);

  if( ( the_token = strtok( NULL, " \n\t,;" ) ) == NULL ){
    sprintf( painCave.errMsg, 
	     "Error parsing EAM # r in %s\n", eamPotFile );
    painCave.isFatal = 1;
    simError();
  }
  info.eam_nr = atoi( the_token);
  
  if( ( the_token = strtok( NULL, " \n\t,;" ) ) == NULL ){
    sprintf( painCave.errMsg, 
	     "Error parsing EAM dr in %s\n", eamPotFile );
    painCave.isFatal = 1;
    simError();
  }
  info.eam_dr = atof( the_token);

  if( ( the_token = strtok( NULL, " \n\t,;" ) ) == NULL ){
    sprintf( painCave.errMsg, 
	     "Error parsing EAM rcut in %s\n", eamPotFile );
    painCave.isFatal = 1;
    simError();
  }
  info.eam_rcut = atof( the_token);





  // Ok now we have to allocate point arrays and read in number of points
  // Index the arrays for fortran, starting at 1
  myEam_Frhovals = new double[info.eam_nrho];
  myEam_rvals    = new double[info.eam_nr];
  myEam_rhovals  = new double[info.eam_nr];

  // Parse F of rho vals.

  // Assume for now that we have a complete number of lines
  nReadLines = int(info.eam_nrho/5);
  


  for (i=0;i<nReadLines;i++){
    j = i*5;

    // Read next line
    eam_eof_test = fgets(eam_read_buffer, sizeof(eam_read_buffer),eamFile);
    linenumber++;
    if(eam_eof_test == NULL){
      sprintf( painCave.errMsg,
	       "error in reading EAM file %s at line %d\n",
	       eamPotFile,linenumber);
      painCave.isFatal = 1;
      simError();
    }
    
    // Parse 5 values on each line into array
    // Value 1
    if ( (the_token = strtok( eam_read_buffer, " \n\t,;")) == NULL){
      sprintf( painCave.errMsg, 
	       "Error parsing EAM nrho: line in %s\n", eamPotFile );
      painCave.isFatal = 1;
      simError();
    }

    myEam_Frhovals[j+0] = atof( the_token );
    
    // Value 2
    if ( (the_token = strtok( NULL, " \n\t,;")) == NULL){
      sprintf( painCave.errMsg, 
	       "Error parsing EAM nrho: line in %s\n", eamPotFile );
      painCave.isFatal = 1;
      simError();
    }

    myEam_Frhovals[j+1] = atof( the_token );
    
    // Value 3
    if ( (the_token = strtok( NULL, " \n\t,;")) == NULL){
      sprintf( painCave.errMsg, 
	       "Error parsing EAM nrho: line in %s\n", eamPotFile );
      painCave.isFatal = 1;
      simError();
    }

    myEam_Frhovals[j+2] = atof( the_token );
    
    // Value 4
    if ( (the_token = strtok( NULL, " \n\t,;")) == NULL){
      sprintf( painCave.errMsg, 
	       "Error parsing EAM nrho: line in %s\n", eamPotFile );
      painCave.isFatal = 1;
      simError();
    }

    myEam_Frhovals[j+3] = atof( the_token );

    // Value 5
    if ( (the_token = strtok( NULL, " \n\t,;")) == NULL){
      sprintf( painCave.errMsg, 
	       "Error parsing EAM nrho: line in %s\n", eamPotFile );
      painCave.isFatal = 1;
      simError();
    }

    myEam_Frhovals[j+4] = atof( the_token );
    
  }
  // Parse Z of r vals
  
  // Assume for now that we have a complete number of lines
  nReadLines = int(info.eam_nr/5);

  for (i=0;i<nReadLines;i++){
    j = i*5;

    // Read next line
    eam_eof_test = fgets(eam_read_buffer, sizeof(eam_read_buffer),eamFile);
    linenumber++;
    if(eam_eof_test == NULL){
      sprintf( painCave.errMsg,
	       "error in reading EAM file %s at line %d\n",
	       eamPotFile,linenumber);
      painCave.isFatal = 1;
      simError();
    }
    
    // Parse 5 values on each line into array
    // Value 1
    if ( (the_token = strtok( eam_read_buffer, " \n\t,;")) == NULL){
      sprintf( painCave.errMsg, 
	       "Error parsing EAM nrho: line in %s\n", eamPotFile );
      painCave.isFatal = 1;
      simError();
    }
    
    myEam_rvals[j+0] = atof( the_token );

    // Value 2
    if ( (the_token = strtok( NULL, " \n\t,;")) == NULL){
      sprintf( painCave.errMsg, 
	       "Error parsing EAM nrho: line in %s\n", eamPotFile );
      painCave.isFatal = 1;
      simError();
    }
  
    myEam_rvals[j+1] = atof( the_token );

    // Value 3
    if ( (the_token = strtok( NULL, " \n\t,;")) == NULL){
      sprintf( painCave.errMsg, 
	       "Error parsing EAM nrho: line in %s\n", eamPotFile );
      painCave.isFatal = 1;
      simError();
    }
  
    myEam_rvals[j+2] = atof( the_token );

    // Value 4
    if ( (the_token = strtok( NULL, " \n\t,;")) == NULL){
      sprintf( painCave.errMsg, 
	       "Error parsing EAM nrho: line in %s\n", eamPotFile );
      painCave.isFatal = 1;
      simError();
    }
  
    myEam_rvals[j+3] = atof( the_token );

    // Value 5
    if ( (the_token = strtok( NULL, " \n\t,;")) == NULL){
      sprintf( painCave.errMsg, 
	       "Error parsing EAM nrho: line in %s\n", eamPotFile );
      painCave.isFatal = 1;
      simError();
    }
  
    myEam_rvals[j+4] = atof( the_token );

  }
  // Parse rho of r vals

  // Assume for now that we have a complete number of lines

  for (i=0;i<nReadLines;i++){
    j = i*5;

    // Read next line
    eam_eof_test = fgets(eam_read_buffer, sizeof(eam_read_buffer),eamFile);
    linenumber++;
    if(eam_eof_test == NULL){
      sprintf( painCave.errMsg,
	       "error in reading EAM file %s at line %d\n",
	       eamPotFile,linenumber);
      painCave.isFatal = 1;
      simError();
    }
  
    // Parse 5 values on each line into array
    // Value 1
    if ( (the_token = strtok( eam_read_buffer, " \n\t,;")) == NULL){
      sprintf( painCave.errMsg, 
	       "Error parsing EAM nrho: line in %s\n", eamPotFile );
      painCave.isFatal = 1;
      simError();
    }
  
    myEam_rhovals[j+0] = atof( the_token );

    // Value 2
    if ( (the_token = strtok( NULL, " \n\t,;")) == NULL){
      sprintf( painCave.errMsg, 
	       "Error parsing EAM nrho: line in %s\n", eamPotFile );
      painCave.isFatal = 1;
      simError();
    }
  
    myEam_rhovals[j+1] = atof( the_token );

    // Value 3
    if ( (the_token = strtok( NULL, " \n\t,;")) == NULL){
      sprintf( painCave.errMsg, 
	       "Error parsing EAM nrho: line in %s\n", eamPotFile );
      painCave.isFatal = 1;
      simError();
    }
  
    myEam_rhovals[j+2] = atof( the_token );

    // Value 4
    if ( (the_token = strtok( NULL, " \n\t,;")) == NULL){
      sprintf( painCave.errMsg, 
	       "Error parsing EAM nrho: line in %s\n", eamPotFile );
      painCave.isFatal = 1;
      simError();
    }
  
    myEam_rhovals[j+3] = atof( the_token );

    // Value 5
    if ( (the_token = strtok( NULL, " \n\t,;")) == NULL){
      sprintf( painCave.errMsg, 
	       "Error parsing EAM nrho: line in %s\n", eamPotFile );
      painCave.isFatal = 1;
      simError();
    }
  
    myEam_rhovals[j+4] = atof( the_token );
 
  }
  *eam_rvals = myEam_rvals;
  *eam_rhovals = myEam_rhovals;
  *eam_Frhovals = myEam_Frhovals;

  fclose(eamFile);
  return 0;
}

double EAM_FF::getAtomTypeMass (char* atomType) {

  currentAtomType = headAtomType->find( atomType );
  if( currentAtomType == NULL ){
    sprintf( painCave.errMsg,
            "AtomType error, %s not found in force file.\n",
             atomType );
    painCave.isFatal = 1;
    simError();
  }

  return currentAtomType->mass;
}

