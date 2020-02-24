void defineAliases(TTree *eventTree, TTree *oniumTree){
    
  
  

  
    
    
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
    
    
    
    
    
//     eventTree->SetAlias("multV0M","(multV0A+multV0C)");
//     oniumTree->SetAlias("multV0M","(multV0A+multV0C)");
//     
//     eventTree->SetAlias("multV0MRegionRnd","(multV0ARegionRnd+multV0CRegionRnd)");
//     oniumTree->SetAlias("multV0MRegionRnd","(multV0ARegionRnd+multV0CRegionRnd)");
//     
//     eventTree->SetAlias("multV0MRegion1","(multV0ARegion1+multV0CRegion1)");
//     oniumTree->SetAlias("multV0MRegion1","(multV0ARegion1+multV0CRegion1)");
//     
//     eventTree->SetAlias("multV0MRegion2","(multV0ARegion2+multV0CRegion2)");
//     oniumTree->SetAlias("multV0MRegion2","(multV0ARegion2+multV0CRegion2)");
//     
//     eventTree->SetAlias("multV0MRegion3","(multV0ARegion3+multV0CRegion3)");
//     oniumTree->SetAlias("multV0MRegion3","(multV0ARegion3+multV0CRegion3)");
    

    
    
  
  
    oniumTree->SetAlias("p1", "onium.mother1process");
    oniumTree->SetAlias("p2", "onium.mother2process");
    
    
    oniumTree->SetAlias( "pdg" , "(onium.initialPdg<0?-1*onium.initialPdg:onium.initialPdg )");
    oniumTree->SetAlias( "PDG" , "(onium.pdg<0?-1*onium.pdg:onium.pdg )");

    
    eventTree->SetAlias("inel0",   "(multEta1>0)");
    oniumTree->SetAlias("inel0",   "(multEta1>0)");
    eventTree->SetAlias("diffractive", "(type!=101)");
    oniumTree->SetAlias("diffractive", "(type!=101)");

     oniumTree->SetAlias("midRapidity", "TMath::Abs(onium.y) < .9");
     oniumTree->SetAlias("muonRapidity", "TMath::Abs(onium.y) >2.5 && TMath::Abs(onium.y) < 4.");
     
     
     oniumTree->SetAlias("muonRapidityPos", "onium.y>2.5 && onium.y<4.");
     oniumTree->SetAlias("muonRapidityNeg", "onium.y<-2.5 && onium.y>-4.");
     
     
     
     
    oniumTree->SetAlias("jpsi", "PDG==443");
    oniumTree->SetAlias("ypsilon1s", "PDG==553");
    oniumTree->SetAlias("ypsilon2s", "PDG==100553");
    oniumTree->SetAlias("ypsilon3s", "PDG==200553");
    oniumTree->SetAlias("initialypsilon1s", "pdg==553");
    oniumTree->SetAlias("initialypsilon2s", "pdg==100553");
    oniumTree->SetAlias("initialypsilon3s", "pdg==200553");
    
    oniumTree->SetAlias("ypsilonns", "(ypsilon1s || ypsilon2s || ypsilon3s)" );
    oniumTree->SetAlias("psiprime", "PDG==100443");
    oniumTree->SetAlias("chic1", "PDG==20443");
    oniumTree->SetAlias("chic2", "PDG==445");
    
    oniumTree->SetAlias("initialpsiprime", "pdg==100443");
    oniumTree->SetAlias("initialchic1", "pdg==20443");
    oniumTree->SetAlias("initialchic2", "pdg==445");
    
    oniumTree->SetAlias("Bmeson", "((PDG%1000 - PDG%100)/100 == 5 && (PDG%100 - PDG%10)/10 != 5)");
    oniumTree->SetAlias("Dmeson", "((PDG%1000 - PDG%100)/100 == 4 && (PDG%100 - PDG%10)/10 != 4)");
    oniumTree->SetAlias("Dplus", "PDG==411");
    oniumTree->SetAlias("Bplus", "PDG==521");

    
    oniumTree->SetAlias( "direct", "(pdg==443 || pdg == 9940003 || pdg == 9941003 || pdg == 9942003)" );
    oniumTree->SetAlias( "nonprompt", "(pdg < 9940000 ) && ( (pdg%10000 - pdg%1000)/1000 == 5  ||  (pdg%1000 - pdg%100)/100 == 5 )" );
    
    
    
    oniumTree->SetAlias("bmeson", "( nonprompt && (pdg%1000 - pdg%100)/100 == 5 && (pdg%100 - pdg%10)/10 != 5 )");
    oniumTree->SetAlias("bs", "( nonprompt && pdg>530 && pdg<540)");
    oniumTree->SetAlias("bc", "( nonprompt && pdg>540 && pdg<550)");
    oniumTree->SetAlias("blight", "( bmeson && !bc && !bs)");
    oniumTree->SetAlias("bbaryon","( nonprompt && (pdg%10000 - pdg%1000)/1000 == 5 )");
    
    
    oniumTree->SetAlias( "cfeeddown", "(!direct && !nonprompt )" );
    oniumTree->SetAlias( "octet", "(pdg > 9900000)" );
    
    oniumTree->SetAlias( "dilepton", "abs(onium.decayChannel)==1");
    
    oniumTree->SetAlias( "gg2QQ", "onium.process==1" );
    oniumTree->SetAlias( "qq2QQ", "onium.process==2" );
    oniumTree->SetAlias( "gq2QQq", "onium.process==3" );
    oniumTree->SetAlias( "hardProcess", "onium.process<4" );
    oniumTree->SetAlias( "fragmentation", "onium.process==4" );
    oniumTree->SetAlias( "coalescence", "fragmentation&&!nonprompt" );
    oniumTree->SetAlias( "coalescencey", "fragmentation" );
    
    
    
//     oniumTree->SetAlias( "initial1", "(!onium.mother1mpi && onium.mother1process!=14)");
//     oniumTree->SetAlias( "initial2", "(!onium.mother2mpi && onium.mother2process!=14)");
    
    oniumTree->SetAlias( "initial1", "(!onium.mother1mpi && onium.mother1process!=44)");
    oniumTree->SetAlias( "initial2", "(!onium.mother2mpi && onium.mother2process!=44)");
    
    
    eventTree->SetAlias( "mpi", "(nMPI-1)");
    oniumTree->SetAlias( "mpi", "(nMPI-1)");
    
    
    
    oniumTree->SetAlias( "mpi1", "onium.mother1mpi");
    oniumTree->SetAlias( "mpi2", "onium.mother2mpi");
    
    
    oniumTree->SetAlias( "bothInitial", "( initial1 && initial2 )");
    oniumTree->SetAlias( "bothMPI", "( onium.mother1mpi && onium.mother2mpi )");
    oniumTree->SetAlias( "InitialPlusMPI", "((initial1&&mpi2) || (mpi1&&initial2))" );

}
