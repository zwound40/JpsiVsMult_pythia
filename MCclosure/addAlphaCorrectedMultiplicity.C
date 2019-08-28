void addAlphaCorrectedMultiplicity(int rap =1, int gen = 1, bool corr = true){

  
  const UShort_t maxMult_smeared = rap ? 1100 : 190;
  
 TString genString = gen ? "epos" : "pythia";
  TString rapString = rap ? "fwd" : "mid";
  if(corr) genString += "Corr" ;
  
  
  
  TString hn = rap ? "VZERO_NchV0_pfy" : "SPDntrackletsCorrMax_Nch10_pfy";  
  if(corr) hn = "corrected_pfx";
  
  
  TFile f2( Form("profile_%s_%s.root", rapString.Data(), genString.Data()  ) );
  TProfile* p = (TProfile*)  f2.Get( hn.Data() )  ;
  p->SetDirectory(0);
  f2.Close();

  
  

  TFile *f =  TFile::Open( Form("onium_full.root" ) , "update" );
  TTree * eventTree = (TTree*) f->Get("eventTree");
  TTree * oniumTree = (TTree*) f->Get("oniumTree");

  
  UShort_t mult_event_smeared, multAlpha_event, mult_onium_smeared, multAlpha_onium;
  
  
  TString branchname = rap ? "multV0M" : "multEta1";
  
  
  auto newBranch_eventTree = eventTree->Branch( Form("%s_alpha_%s",branchname.Data(), genString.Data() ), &multAlpha_event, Form("%s_alpha_%s/s",branchname.Data(), genString.Data() )  );
  auto newBranch_oniumree = oniumTree->Branch( Form("%s_alpha_%s",branchname.Data(), genString.Data() ), &multAlpha_onium, Form("%s_alpha_%s/s",branchname.Data(), genString.Data() ) );  
  
  eventTree->SetBranchAddress(  Form("%s_smeared", branchname.Data()), &mult_event_smeared);
  

  for(int iEvent=0; iEvent< eventTree->GetEntries(); ++iEvent){
    eventTree->GetEntry( iEvent );
    UShort_t mult_tmp = TMath::Min(mult_event_smeared, maxMult_smeared);
    Int_t bin = p->FindBin(mult_tmp);
    multAlpha_event = p->GetBinContent( bin );
    newBranch_eventTree->Fill();
  }


  oniumTree->SetBranchAddress( Form("%s_smeared", branchname.Data()), &mult_onium_smeared);
  for(int iOnium =0; iOnium< oniumTree->GetEntries(); ++ iOnium){
    oniumTree->GetEntry(iOnium);
    UShort_t mult_tmp = TMath::Min(mult_onium_smeared, maxMult_smeared ); 
    
    Int_t bin = p->FindBin(mult_tmp);
    multAlpha_onium = p->GetBinContent( bin );
    newBranch_oniumree->Fill();
  }

  
    eventTree->Write("", TObject::kOverwrite); // save only the new version of the tree
    oniumTree->Write("", TObject::kOverwrite); // save only the new version of the tree


}
