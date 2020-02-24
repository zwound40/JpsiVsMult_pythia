void addSmearedMultiplicity( int rap = 1 ){
  
  
  TString fn = rap ? "detector_response_fwd_pythia.root" : "detector_response_mid_pythia.root";
  TFile f2( fn.Data() );
  
  TString hn = rap ? "VZERO_NchV0" : "SPDntrackletsCorrMax_Nch10";
  
  TH2F* matrix = (TH2F*)  f2.Get( hn.Data()  );
  if(!matrix){
   cout << hn.Data() << " not found!" << endl; 
   return;
  }
  
  matrix->SetDirectory(0);
  f2.Close();
  
  
  TString branchname = rap ? "multV0M" : "multEta1";
  
  TFile *f =  TFile::Open( Form("onium_full.root" ) , "update" );
  TTree * eventTree = (TTree*) f->Get("eventTree");
  TTree * oniumTree = (TTree*) f->Get("oniumTree");
  

  
  
  

  
  
  
  
  
  const UShort_t maxMult[2] = {119,199};
  TH1D* response[ maxMult[rap]   ];
  for(int i=0; i<= maxMult[rap]; i++){
    response[i] = matrix->ProjectionY( Form("reponse_%d",i), i+1, i+1   );
  }
  
  if( eventTree->GetBranch( Form("%s_smeared", branchname.Data())  ) && oniumTree->GetBranch(  Form("%s_smeared", branchname.Data())   ) ){
    
    cout << Form("%s_smeared", branchname.Data()) << " already in tree!" << endl;
    
    return;
  }


  UShort_t mult_event, multSmeared_event, mult_onium, multSmeared_onium;
  
  
  
  auto newBranch_eventTree = eventTree->Branch( Form("%s_smeared", branchname.Data())  , &multSmeared_event, Form("%s_smeared/s", branchname.Data() ) );
  auto newBranch_oniumTree = oniumTree->Branch( Form("%s_smeared", branchname.Data())  , &multSmeared_onium, Form("%s_smeared/s", branchname.Data() ) );

  
  
  eventTree->SetBranchAddress(   branchname.Data(), &mult_event);
  

  for(int iEvent=0; iEvent< eventTree->GetEntries(); ++iEvent){
    eventTree->GetEntry( iEvent );
    UShort_t mult_tmp = TMath::Min( mult_event, maxMult[rap] );
    multSmeared_event = TMath::Nint( response[ mult_tmp ]->GetRandom()  );
    newBranch_eventTree->Fill();
  }


  oniumTree->SetBranchAddress(  branchname.Data(), &mult_onium);
  for(int iOnium =0; iOnium< oniumTree->GetEntries(); ++ iOnium){
    oniumTree->GetEntry( iOnium );
    UShort_t mult_tmp = TMath::Min( mult_onium, maxMult[rap] ); 
    multSmeared_onium = TMath::Nint( response[ mult_tmp]->GetRandom()  );
    newBranch_oniumTree->Fill();
  }

    eventTree->Write("", TObject::kOverwrite); // save only the new version of the tree
    oniumTree->Write("", TObject::kOverwrite); // save only the new version of the tree

}
