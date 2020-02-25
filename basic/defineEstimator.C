/**
 * 
 * This macro defines the multiplicity estimator according to the code given to it.
 * The codes are the following:
 * 
 * 
 * 0: nch in full rapidity
 * 1: nch in |eta| < 1
 * 2: nch in V0A
 * 3: nch in V0C
 * 4: nch in V0M = V0A+V0C
 * 5 : number of MPI
 * 
 * 10: nch in full rapidity in towards region
 * 11: nch in |eta| < 1 in towards region
 * 12: nch in V0A in towards region
 * 13: nch in V0C in towards region
 * 14: nch in V0M in towards region
 * 
 * 20: nch in full rapidity in transverse region
 * 21: nch in |eta| < 1 in transverse region
 * 22: nch in V0A in transverse region
 * 23: nch in V0C in transverse region
 * 24: nch in V0M in transverse region
 * 
 * 30: nch in full rapidity in away region
 * 31: nch in |eta| < 1 in away region
 * 32: nch in V0A in away region
 * 33: nch in V0C in away region
 * 34: nch in V0M in away region
 * 
 * 
 * 
 * The macro returns 3 strings:
 *  - estimatorString : to be given to the tree for drawing
 *  - stringForAxisTitle : used for the x axis title
 *  - regionString : also used in the x axis title, describes the phi region
 * 
 * In addition it returns a factor by which to scale the x axis (because multiplicity in V0M is higher than in |eta| < 1 and so on)
 * 
 * 
 * 
 * 
 */

void defineEstimator( int estimator, TString &estimatorString, TString &stringForAxisTitle, TString &regionString, double &factor){

  factor = 1.;
  switch(estimator){
    case 5:
      stringForAxisTitle = "#it{N}_{MPI}";
      estimatorString = "nMPI";
      return;
      break;
    default:
      stringForAxisTitle = "#it{N}_{ch}";
      estimatorString = "mult";
      break;
  }

  
  
  int last_digit = estimator %10;
  
  switch( last_digit ){
    case 0:
      regionString = "full rapidity";
      factor *= 2.;
      break;
    case 1:
      regionString = "|#it{#eta}|<1";
      estimatorString += "Eta1";
      break;
    case 2:
      regionString = "V0A";
      estimatorString += "V0A";
      break;
    case 3:
      regionString = "V0C";
      estimatorString += "V0C";
      break;
    case 4:
      regionString = "V0M";
      estimatorString += "V0M";
      factor *= 1.5;
      break;
  }
  
  
  int second_digit = (estimator %100 - estimator%10) / 10;
  
  switch(second_digit){
    case 1:
      regionString += ", toward region";
      estimatorString += "Region1";
      factor /= 2.;
      break;
    case 2:
      regionString += ", transverse region";
      estimatorString += "Region2";
      factor /= 2.;
      break;
    case 3:
      regionString += ", away region";
      estimatorString += "Region3";
      factor /= 2.;
      break;
  }
  
  
  
}
