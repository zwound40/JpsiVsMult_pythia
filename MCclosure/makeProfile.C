void makeProfile( int rap = 1, int gen = 1, bool corr= true){

  
    TProfile::Approximate();
    
  TString genString = gen ? "epos" : "pythia";
  TString rapString = rap ? "fwd" : "mid";
  if(corr) genString += "Corr" ;
  
  
  TString hn = rap ? "VZERO_NchV0" : "SPDntrackletsCorrMax_Nch10";  
  if (corr) hn = "corrected";
  
  
  int fitFrom = rap ? 500 : 90; 
  int fitUntil = rap ? 1100 : 190;  
  
  
  TFile f2(  Form("detector_response_%s_%s.root", rapString.Data(), genString.Data()  ) );
  
  TH2F* matrix = (TH2F*)  f2.Get( hn.Data() );
  matrix->SetDirectory(0);
  
  f2.Close();

//   return;
  

  TProfile* pfy = corr ? matrix->ProfileX() :  matrix->ProfileY();
  
  TF1* pol2 = new TF1("pol2","pol2");
  pfy->Fit( pol2,"","",fitFrom, fitUntil );
  Int_t nBins = pfy->GetXaxis()->GetNbins();
  
  for(int iBin = pfy->GetXaxis()->FindBin( fitFrom ) ;  iBin <=  pfy->GetXaxis()->FindBin( fitUntil )  ; ++iBin){
    pfy->SetBinEntries( iBin, 10000000 );
    pfy->SetBinContent( iBin, 10000000 * pol2->Eval( pfy->GetBinCenter(iBin)   )    );
  }

pfy->Draw();

  pfy->SaveAs( Form( "profile_%s_%s.root", rapString.Data(), genString.Data() ) );
  





}
