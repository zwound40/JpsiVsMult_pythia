/*
 * This macro defines the aliases in the event and quarkonium trees that are used
 * in the plotRelOrigin macro.
 * 
 */


void defineAliases(TTree *eventTree, TTree *oniumTree){
    
    
    // V0M = V0A + V0C. 
    
    eventTree->SetAlias("multV0M","(multV0A+multV0C)");
    oniumTree->SetAlias("multV0M","(multV0A+multV0C)");
    
    eventTree->SetAlias("multV0MRegion1","(multV0ARegion1+multV0CRegion1)");
    oniumTree->SetAlias("multV0MRegion1","(multV0ARegion1+multV0CRegion1)");
    
    eventTree->SetAlias("multV0MRegion2","(multV0ARegion2+multV0CRegion2)");
    oniumTree->SetAlias("multV0MRegion2","(multV0ARegion2+multV0CRegion2)");
    
    eventTree->SetAlias("multV0MRegion3","(multV0ARegion3+multV0CRegion3)");
    oniumTree->SetAlias("multV0MRegion3","(multV0ARegion3+multV0CRegion3)");
    
    
    // for the event tree, the 3 regions are chosen randomly. 
    // in the trees only one value of the multiplicity in a randomy chosen region is stored
    // and used for all 3 regions
    
    eventTree->SetAlias("multRegion1","multRegionRnd");
    eventTree->SetAlias("multRegion2","multRegionRnd");
    eventTree->SetAlias("multRegion3","multRegionRnd");
   
    eventTree->SetAlias("multEta1Region1", "multEta1RegionRnd");
    eventTree->SetAlias("multEta1Region2", "multEta1RegionRnd");
    eventTree->SetAlias("multEta1Region3", "multEta1RegionRnd");
    
    eventTree->SetAlias("multV0ARegion1", "multV0ARegionRnd");
    eventTree->SetAlias("multV0ARegion2", "multV0ARegionRnd");
    eventTree->SetAlias("multV0ARegion3", "multV0ARegionRnd");
    
    eventTree->SetAlias("multV0CRegion1", "multV0CRegionRnd");
    eventTree->SetAlias("multV0CRegion2", "multV0CRegionRnd");
    eventTree->SetAlias("multV0CRegion3", "multV0CRegionRnd");
    
    // INEL>0 cut
    eventTree->SetAlias("inel0",   "(multEta1>0)");
    oniumTree->SetAlias("inel0",   "(multEta1>0)");
    
    // diffractive events
    eventTree->SetAlias("diffractive", "(type!=101)");
    oniumTree->SetAlias("diffractive", "(type!=101)");
    
    
    // number of MPI (nMPI is one for events with one hard scattering, 2 for events with 1 additional scattering)
    // so subtract 1 to get number of MPI
    eventTree->SetAlias( "mpi", "(nMPI-1)");
    oniumTree->SetAlias( "mpi", "(nMPI-1)");
    

    
    
     oniumTree->SetAlias("midRapidity", "TMath::Abs(onium.y) < .9");
     oniumTree->SetAlias("muonRapidity", "TMath::Abs(onium.y) >2.5 && TMath::Abs(onium.y) < 4.");
     
     
     oniumTree->SetAlias("muonRapidityPos", "onium.y>2.5 && onium.y<4.");
     oniumTree->SetAlias("muonRapidityNeg", "onium.y<-2.5 && onium.y>-4.");
    
    
    oniumTree->SetAlias( "dilepton", "abs(onium.decayChannel)==1");
    
    
    // absolute value of the PDG code of the initially produced particle
    oniumTree->SetAlias( "pdg" , "(onium.initialPdg<0?-1*onium.initialPdg:onium.initialPdg )");
    
    // absolute value of the PDG code of the final particle
    oniumTree->SetAlias( "PDG" , "(onium.pdg<0?-1*onium.pdg:onium.pdg )");

     
    oniumTree->SetAlias("jpsi", "PDG==443");
    
    oniumTree->SetAlias("charmonia", "((PDG%1000 - PDG%100)/100 == 4 && (PDG%100 - PDG%10)/10 == 4   &&  (PDG%10000 - PDG%1000)/1000 == 0 )");
    
    oniumTree->SetAlias("bottomonia", "((PDG%1000 - PDG%100)/100 == 5 && (PDG%100 - PDG%10)/10 == 5   &&  (PDG%10000 - PDG%1000)/1000 == 0 )");
    

    // directly produced J/psi, i.e. no non-prompt and no feeddown of higher mass charmonia
    // but J/psi can be initially produced in a color octet state, these are the codes starting with 99...
    // see http://home.thep.lu.se/~torbjorn/pythia82html/OniaProcesses.html
    oniumTree->SetAlias( "direct", "(pdg==443 || pdg == 9940003 || pdg == 9941003 || pdg == 9942003)" );
    oniumTree->SetAlias( "nonprompt", "(pdg < 9940000 ) && ( (pdg%10000 - pdg%1000)/1000 == 5  ||  (pdg%1000 - pdg%100)/100 == 5 )" );
    oniumTree->SetAlias( "cfeeddown", "(!direct && !nonprompt )" );
    oniumTree->SetAlias( "octet", "(pdg > 9900000)" );

}
