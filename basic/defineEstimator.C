void defineEstimator( int estimator, bool noCR, bool noMPI, TString &multiplicityEstimator, TString &nCh, TString &region,  double &rapInterval, double &factor, bool &particles )   {

  particles = estimator != 5 && estimator !=6;
  factor = 1.;
  switch(estimator){
    case 5:
      nCh = "#it{N}_{MPI}";
      multiplicityEstimator = "nMPI";
      break;
    case 6:
      nCh = "#hat{#it{p}_{T}}";
      multiplicityEstimator = "ptHat";
      break;
    default:
      nCh = "#it{N}_{ch}";
      multiplicityEstimator = "mult";
      break;
  }

  
  
  int last_digit = estimator %10;
  
  switch( last_digit ){
    case 0:
      rapInterval = 19;
      region = "full rapidity";
      factor *= 2.;
      break;
    case 1:
      rapInterval = 2;
      region = "|#it{#eta}|<1";
      multiplicityEstimator += "Eta1";
      break;
    case 2:
      rapInterval = 2.3;
      region = "V0A";
      multiplicityEstimator += "V0A";
      break;
    case 3:
      rapInterval = 2.0;
      region = "V0C";
      multiplicityEstimator += "V0C";
      break;
    case 4:
      rapInterval = 4.3;
      region = "V0M";
      multiplicityEstimator += "V0M";
      factor *= 1.5;
      break;
  }
  
  
  int second_digit = (estimator %100 - estimator%10) / 10;
  
  switch(second_digit){
    case 1:
      region += ", toward region";
      multiplicityEstimator += "Region1";
      factor /= 2.;
      break;
    case 2:
      region += ", transverse region";
      multiplicityEstimator += "Region2";
      factor /= 2.;
      break;
    case 3:
      region += ", away region";
      multiplicityEstimator += "Region3";
      factor /= 2.;
      break;
    case 4:
      region += ", random region";
      multiplicityEstimator += "RegionRnd";
      factor /= 2.;
      break;
  }
  
  int third_digit = (estimator %1000  - estimator %100) / 100;
  switch(third_digit){
    case 1:
      region += ", (detector smearing)";
      multiplicityEstimator += "_smeared";
      break;
    case 2:
      region += ", (alpha corrected, PYTHIA N_{ch} distr.)";
      multiplicityEstimator += "_alpha_pythia";
      break;
    case 3:
      region += ", (alpha corr., EPOS N_{ch} distr.)";
      multiplicityEstimator += "_alpha_epos";
      break;
    case 4:
      region += ", (alpha corr., EPOS N_{ch} distr., unfolded)";
      multiplicityEstimator += "_alpha_eposCorr";
      break;

  }
    
  if(estimator == 104) factor *= 4.;
  if(estimator == 204) factor *= 5./6.;
  
  
  
  if(noCR) factor *= 3.;
  if(noMPI) factor /= 2.;
  
}
