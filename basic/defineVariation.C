#include <iostream>
#include "TTree.h"
#include "TLegend.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TStyle.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TF1.h"
#include "TSystem.h"
#include "TEnv.h"
#include "TROOT.h"


struct Variation{
  TString name;
  TString cut;
  Color_t color;
  int style;
  Variation( TString name, TString cut, Color_t color, int style   ):
  name(name),
  cut(cut),
  color(color),
  style(style){}
};

void defineVariation(
      int plotWhat=0,
      TString  *commonName_ptr = 0x0,
      TString *variationsName_ptr = 0x0,
      TString *commonCut_ptr = 0x0,
      TString *header_ptr = 0x0,
      TString *panel_ptr = 0x0,
      TString *inclusive_ptr = 0x0,
      int*legendsX_ptr = 0x0,
      int*legendsY_ptr = 0x0,
      bool*plotMult_ptr = 0x0,
      bool*plotPt_ptr = 0x0,
      bool*plotInclusive_ptr = 0x0,
      bool*plotY_ptr = 0x0,
      bool*drawSqrt_ptr = 0x0,
      vector<Variation>*variations_ptr = 0x0){

  
  
    TString commonCut, commonName, variationsName, header, panel, inclusive;
    inclusive = "inclusive";
    int legendsX[6] = {0,0,1,1,2,2};
    int legendsY[6] = {0,0,0,0,2,0};
    bool plotMult = true;
    bool plotPt = true;
    bool plotInclusive = true;
    bool plotY = true;
    bool drawSqrt = false;
    vector<Variation> variations;
    
    
    switch( plotWhat ){
      
  // plot LF
      case -10 : {
//         commonCut  = "onium.pt>2";
        commonCut  = "onium.pt>2 && onium.pt<10";
        commonName =  "LF";
        header = "Charged hadrons";
        variationsName  = "pt";
        plotMult = true;
        plotPt = true;
        plotInclusive = false;
        legendsX[1] =1;
        legendsY[3] =4;
        variations.clear();
//    variations.push_back( Variation( "4 < #it{p}_{T} < 10 GeV/#it{c}", "onium.pt>4", kOrange,    22) );
   variations.push_back( Variation( "6 < #it{p}_{T} < 10 GeV/#it{c}", "onium.pt>6 && onium.pt<10", kRed+2,    22) );
   variations.push_back( Variation( "4 < #it{p}_{T} < 6 GeV/#it{c}", "onium.pt>4 && onium.pt<6", kRed,    21) );
   variations.push_back( Variation( "2 < #it{p}_{T} < 4 GeV/#it{c}", "onium.pt>2 && onium.pt<4", kOrange-3,    23) );
      break; }
      
      

      
  
  // plot inclusive J/psi
      case 0 : {
        commonCut  = "jpsi";
        commonName =  "inclusive";
        header = "J/#it{#psi}";
        variationsName  = "Sources_coalescence_hard";
        plotMult = true;
        plotPt = true;
        plotInclusive = true;
        legendsX[1] =1;
        legendsY[3] =4;
        variations.clear();
//         variations.push_back( Variation( "NRQCD #rightarrow J/#it{#psi}",  "direct && hardProcess",    kRed,     21) );
//         variations.push_back( Variation( "NRQCD #psi^{'}, #chi_{c}", "cfeeddown", kBlue,    23) );
        variations.push_back( Variation( "NRQCD",  "hardProcess",    kRed,     21) );
        variations.push_back( Variation( "cluster collapse",   "direct && coalescence", kBlue,    25) );
        variations.push_back( Variation( "non-prompt",       "nonprompt", kGreen+2,   32) );
      break; }
        // plot inclusive J/psi, wo cluster collaps
      case 10000 : {
        commonCut  = "jpsi";
        commonName =  "inclusive";
        header = "J/#it{#psi}";
        variationsName  = "Sources_nonprompt_nrqcd";
        plotMult = true;
        plotPt = true;
        plotInclusive = true;
        legendsX[1] =1;
        variations.clear();
//         variations.push_back( Variation( "NRQCD #rightarrow J/#it{#psi}",  "direct && hardProcess",    kRed,     21) );
//         variations.push_back( Variation( "NRQCD #psi^{'}, #chi_{c}", "cfeeddown", kBlue,    23) );
        variations.push_back( Variation( "NRQCD",  "hardProcess",    kRed,     21) );
//         variations.push_back( Variation( "cluster collapse",   "direct && coalescence", kBlue,    25) );
        variations.push_back( Variation( "non-prompt",       "nonprompt", kGreen+2,   32) );
      break; }
      
      
      
  // plot different sources
      case 1 : {
        commonCut  = "jpsi";
        header = "J/#it{#psi}";
        commonName =  "inclusive";
        variationsName  = "prompt_nonprompt";
        plotMult = true;
        plotPt = true;
        plotInclusive =true;
        
        legendsX[1] =1;
        variations.clear();
//         variations.push_back( Variation( "direct J/#it{#psi}",          "direct",    kRed,     21) );
//         variations.push_back( Variation( "(c#bar{c}) feed-down", "cfeeddown", kBlue,    23) );
//         variations.push_back( Variation( "b feed-down",       "nonprompt", kGreen+2,   32) );
        
        
        variations.push_back( Variation( "prompt",          "!nonprompt",    kRed,     21) );
        variations.push_back( Variation( "nonprompt",       "nonprompt", kGreen+2,   32) );
      break;   }
  // plot sources for nonprompt
      case 2:{
        commonCut  = "jpsi&&nonprompt";
        header = "Non-prompt J/#it{#psi}";
        commonName =  "nonprompt";
        variationsName  = "sources";
        plotMult = true;
        plotPt = true;
        legendsY[3] = 1;
        
//         legendsX[1] =1;
        variations.clear();
        variations.push_back( Variation( "J/#it{#psi} #leftarrow B meson",  "bmeson",    kRed,     21) );
//         variations.push_back( Variation( "#Upsilon", "ypsilon", kBlue,    22) );
        variations.push_back( Variation( "J/#it{#psi} #leftarrow b baryon", "bbaryon", kGreen+2,   23) );
      break;}
  // plot different sources for prompt , split direct in hard, coalescence
      case 3:{
     commonCut  = "jpsi && !nonprompt";
     header = "Prompt J/#it{#psi}";
     commonName =  "prompt";
     variationsName  = "sources_hard_fragment";
     plotMult = true;
     plotPt = true;
     
     variations.clear();
     variations.push_back( Variation( "direct J/#it{#psi} (hard process)",          "direct&&hardProcess",    kRed,     21) );
     variations.push_back( Variation( "(c#bar{c}) feed-down (hard process)", "cfeeddown", kBlue,    23) );
     variations.push_back( Variation( "direct J/#it{#psi} (coalescence)",          "direct&&coalescence",    kRed,  25) );
//      variations.push_back( Variation( "beauty feed-down",       "nonprompt", kGreen+2,   32) );
   break;}
  // plot different pT
      case 4:{
     commonCut  = Form( "jpsi" );
      header = "J/#it{#psi}";
     commonName =  "inclusive";
     variationsName  = "pT";
     plotMult = true;
     plotPt = false;
    plotInclusive = true;
    inclusive = "#it{p}_{T} integrated";
    variations.clear();
   variations.push_back( Variation( "8 GeV/#it{c} < #it{p}_{T}", "onium.pt>8", kRed+2,    22) );
   variations.push_back( Variation( "4 GeV/#it{c} < #it{p}_{T} < 8 GeV/#it{c}", "onium.pt>4 && onium.pt<8", kRed,    21) );
   variations.push_back( Variation( "#it{p}_{T} < 4 GeV/#it{c}",   "onium.pt<4",    kOrange-3,    23) );
   
   
break;}

  // plot different pT, bins as in data
      case 4444:{
     commonCut  = Form( "jpsi" );
      header = "J/#it{#psi}";
     commonName =  "inclusive";
     variationsName  = "pTdata";
    inclusive = "#it{p}_{T} integrated";
     plotMult = true;
     plotPt = false;
    plotInclusive = true;
    variations.clear();
   variations.push_back( Variation( "15 < #it{p}_{T} < 40 GeV/#it{c}", "onium.pt>15 && onium.pt<40", kViolet,    26) );
   variations.push_back( Variation( "8 < #it{p}_{T} < 15 GeV/#it{c}", "onium.pt>8 && onium.pt<15", kGreen+2,    32) );
   variations.push_back( Variation( "4 < #it{p}_{T} < 8 GeV/#it{c}", "onium.pt>4 && onium.pt<8", kBlue,    22) );
   variations.push_back( Variation( "#it{p}_{T} < 4 GeV/#it{c}",   "onium.pt<4",    kRed,    23) );
   
   
break;}


  // plot different pT, bins as in data
      case 44444444:{
     commonCut  = Form( "jpsi" );
      header = "J/#it{#psi}";
     commonName =  "inclusive";
     variationsName  = "incl_highPt";
    inclusive = "#it{p}_{T} > 0";
     plotMult = true;
     plotPt = false;
    plotInclusive = true;
    variations.clear();
   variations.push_back( Variation( "#it{p}_{T} > 8 GeV/#it{c}",   "onium.pt>8",    kRed,    23) );
   
   
break;}





  // plot different pT, prompt
      case 44:{
     commonCut  = Form( "jpsi && !nonprompt" );
      header = "Prompt J/#it{#psi}";
     commonName =  "prompt";
     variationsName  = "pT";
    inclusive = "#it{p}_{T} integrated";
     plotMult = true;
     plotPt = false;
    plotInclusive = true;
    variations.clear();
    
   variations.push_back( Variation( "12 GeV/#it{c} < #it{p}_{T}", "onium.pt>12", kRed+2,    22) );
   variations.push_back( Variation( "6 GeV/#it{c} < #it{p}_{T} < 12 GeV/#it{c}", "onium.pt>6 && onium.pt<12", kRed,    21) );
   variations.push_back( Variation( "#it{p}_{T} < 6 GeV/#it{c}",   "onium.pt<6",    kOrange-3,    23) );
   
   
//    variations.push_back( Variation( "8 GeV/#it{c} < #it{p}_{T}", "onium.pt>8", kRed+2,    22) );
//    variations.push_back( Variation( "4 GeV/#it{c} < #it{p}_{T} < 8 GeV/#it{c}", "onium.pt>4 && onium.pt<8", kRed,    21) );
//    variations.push_back( Variation( "#it{p}_{T} < 4 GeV/#it{c}",   "onium.pt<4",    kOrange-3,    23) );
   
   
break;}
  // plot different pT, non-prompt
      case 444:{
     commonCut  = Form( "jpsi && nonprompt" );
      header = "Non-prompt J/#it{#psi}";
     commonName =  "nonprompt";
     variationsName  = "pT";
    inclusive = "#it{p}_{T} integrated";
     plotMult = true;
     plotPt = false;
    plotInclusive = true;
    variations.clear();
   variations.push_back( Variation( "8 GeV/#it{c} < #it{p}_{T}", "onium.pt>8", kRed+2,    22) );
   variations.push_back( Variation( "4 GeV/#it{c} < #it{p}_{T} < 8 GeV/#it{c}", "onium.pt>4 && onium.pt<8", kRed,    21) );
   variations.push_back( Variation( "#it{p}_{T} < 4 GeV/#it{c}",   "onium.pt<4",    kOrange-3,    23) );
   
   
break;}
  // plot different pT, hard process
      case 5:{
     commonCut  = Form( "(jpsi && hardProcess)" );
        header = "J/#it{#psi} from NRQCD";
     commonName =  "hardProcess";
     variationsName  = "pT";
     plotMult = true;
     plotPt = false;
    variations.clear();
   variations.push_back( Variation( "8 GeV/#it{c} < #it{p}_{T}", "onium.pt>8", kRed+2,    22) );
   variations.push_back( Variation( "4 GeV/#it{c} < #it{p}_{T} < 8 GeV/#it{c}", "onium.pt>4 && onium.pt<8", kRed,    21) );
   variations.push_back( Variation( "#it{p}_{T} < 4 GeV/#it{c}",   "onium.pt<4",    kOrange-3,    23) );
     
    
break;}  
// plot different pT, hard process, high pT
      case 55:{
     commonCut  = Form( "(jpsi && hardProcess)" );
        header = "J/#it{#psi} from NRQCD";
     commonName =  "hardProcesshighpT";
     variationsName  = "pT";
     plotMult = true;
     plotPt = false;
     
    variations.clear();
   variations.push_back( Variation( "12 GeV/#it{c} < #it{p}_{T}", "onium.pt>12", kRed+2,    22) );
   variations.push_back( Variation( "6 GeV/#it{c} < #it{p}_{T} < 12 GeV/#it{c}", "onium.pt>6 && onium.pt<12", kRed,    21) );
   variations.push_back( Variation( "#it{p}_{T} < 6 GeV/#it{c}",   "onium.pt<6",    kOrange-3,    23) );
   
   
   
//    variations.push_back( Variation( "12 GeV/#it{c} < #it{p}_{T}", "onium.pt>12", kRed+2,    22) );
//    variations.push_back( Variation( "8 GeV/#it{c} < #it{p}_{T} < 12 GeV/#it{c}", "onium.pt>8 && onium.pt<12", kRed,    21) );
//    variations.push_back( Variation( "#it{p}_{T} < 8 GeV/#it{c}",   "onium.pt<8",    kOrange-3,    23) );
    
    
break;}

  // plot different pT, bmeson
      case 6:{
     commonCut  = "jpsi&&bmeson" ;
        header = "J/#it{#psi} #leftarrow B";
     commonName =  "bmesons";
     variationsName  = "pT";
     plotMult = true;
     plotPt = false;
     plotInclusive = true;
     
    variations.clear();
//    variations.push_back( Variation( "#it{p}_{T} < 2 GeV/#it{c}",   "onium.pt<2",    kOrange-3,    20) );
//    variations.push_back( Variation( "2 GeV/#it{c} < #it{p}_{T} < 4 GeV/#it{c}", "onium.pt>2 && onium.pt<4", kRed,    21) );
//    variations.push_back( Variation( "4 GeV/#it{c} < #it{p}_{T} < 6 GeV/#it{c}", "onium.pt>4 && onium.pt<6", kRed+2,    22) );
//    variations.push_back( Variation( "6 GeV/#it{c} < #it{p}_{T} < 8 GeV/#it{c}", "onium.pt>6 && onium.pt<8", kBlack,    23) );
//    variations.push_back( Variation( "8 GeV/#it{c} < #it{p}_{T}", "onium.pt>8", kBlue,    24) );
   
   
   
   variations.push_back( Variation( "8 GeV/#it{c} < #it{p}_{T}", "onium.pt>8", kRed+2,    22) );
   variations.push_back( Variation( "4 GeV/#it{c} < #it{p}_{T} < 8 GeV/#it{c}", "onium.pt>4 && onium.pt<8", kRed,    21) );
   variations.push_back( Variation( "#it{p}_{T} < 4 GeV/#it{c}",   "onium.pt<4",    kOrange-3,    23) );
break;}



  // plot different pT, nonprompt, hard process only
      case 66:{
     commonCut  = "jpsi && nonprompt && onium.mother1process<4" ;
        header = "Non-prompt J/#it{#psi} (b from pQCD)";
     commonName =  "nonpromptHard";
     variationsName  = "pT";
     plotMult = true;
     plotPt = false;
     plotInclusive = true;
     
    variations.clear();
    
   variations.push_back( Variation( "#it{p}_{T}>8 GeV/#it{c}", "onium.pt>8", kRed,    22) );
    
    
    
//    variations.push_back( Variation( "#it{p}_{T} < 2 GeV/#it{c}",   "onium.pt<2",    kOrange-3,    20) );
//    variations.push_back( Variation( "2 GeV/#it{c} < #it{p}_{T} < 4 GeV/#it{c}", "onium.pt>2 && onium.pt<4", kRed,    21) );
//    variations.push_back( Variation( "4 GeV/#it{c} < #it{p}_{T} < 6 GeV/#it{c}", "onium.pt>4 && onium.pt<6", kRed+2,    22) );
//    variations.push_back( Variation( "6 GeV/#it{c} < #it{p}_{T} < 8 GeV/#it{c}", "onium.pt>6 && onium.pt<8", kBlack,    23) );
//    variations.push_back( Variation( "8 GeV/#it{c} < #it{p}_{T}", "onium.pt>8", kBlue,    24) );
   
   
   
 //  variations.push_back( Variation( "16 GeV/#it{c} < #it{p}_{T}", "onium.pt>16", kBlue,    24) );
 //  variations.push_back( Variation( "8 GeV/#it{c} < #it{p}_{T} < 16 GeV/#it{c}", "onium.pt>8 && onium.pt<16", kRed+2,    22) );
//    variations.push_back( Variation( "8 GeV/#it{c} < #it{p}_{T}", "onium.pt>8", kRed+2,    22) );
//    variations.push_back( Variation( "4 GeV/#it{c} < #it{p}_{T} < 8 GeV/#it{c}", "onium.pt>4 && onium.pt<8", kRed,    21) );
//    variations.push_back( Variation( "#it{p}_{T} < 4 GeV/#it{c}",   "onium.pt<4",    kOrange-3,    23) );
break;}



  // plot different pT, nonprompt, ISR,FSR
      case 666:{
     commonCut  = "nonprompt && onium.mother1process>5&& onium.mother1process<11" ;
        header = "Non-prompt J/#it{#psi} from ISR, FSR";
     commonName =  "nonpromptISRFSR";
     variationsName  = "pT";
     plotMult = true;
     plotPt = false;
     plotInclusive = true;
     
    variations.clear();
//    variations.push_back( Variation( "#it{p}_{T} < 2 GeV/#it{c}",   "onium.pt<2",    kOrange-3,    20) );
//    variations.push_back( Variation( "2 GeV/#it{c} < #it{p}_{T} < 4 GeV/#it{c}", "onium.pt>2 && onium.pt<4", kRed,    21) );
//    variations.push_back( Variation( "4 GeV/#it{c} < #it{p}_{T} < 6 GeV/#it{c}", "onium.pt>4 && onium.pt<6", kRed+2,    22) );
//    variations.push_back( Variation( "6 GeV/#it{c} < #it{p}_{T} < 8 GeV/#it{c}", "onium.pt>6 && onium.pt<8", kBlack,    23) );
//    variations.push_back( Variation( "8 GeV/#it{c} < #it{p}_{T}", "onium.pt>8", kBlue,    24) );
   
   
   
   variations.push_back( Variation( "16 GeV/#it{c} < #it{p}_{T}", "onium.pt>16", kBlue,    24) );
   variations.push_back( Variation( "8 GeV/#it{c} < #it{p}_{T} < 16 GeV/#it{c}", "onium.pt>8 && onium.pt<16", kRed+2,    22) );
   variations.push_back( Variation( "4 GeV/#it{c} < #it{p}_{T} < 8 GeV/#it{c}", "onium.pt>4 && onium.pt<8", kRed,    21) );
   variations.push_back( Variation( "#it{p}_{T} < 4 GeV/#it{c}",   "onium.pt<4",    kOrange-3,    23) );
break;}



  // plot different pT, nonprompt, beam remnant
      case 6666:{
     commonCut  = "nonprompt && onium.mother1process==14" ;
        header = "Non-prompt J/#it{#psi} from beam remnant";
     commonName =  "beamRemnant";
     variationsName  = "pT";
     plotMult = true;
     plotPt = false;
     plotInclusive = true;
     
    variations.clear();
//    variations.push_back( Variation( "#it{p}_{T} < 2 GeV/#it{c}",   "onium.pt<2",    kOrange-3,    20) );
//    variations.push_back( Variation( "2 GeV/#it{c} < #it{p}_{T} < 4 GeV/#it{c}", "onium.pt>2 && onium.pt<4", kRed,    21) );
//    variations.push_back( Variation( "4 GeV/#it{c} < #it{p}_{T} < 6 GeV/#it{c}", "onium.pt>4 && onium.pt<6", kRed+2,    22) );
//    variations.push_back( Variation( "6 GeV/#it{c} < #it{p}_{T} < 8 GeV/#it{c}", "onium.pt>6 && onium.pt<8", kBlack,    23) );
//    variations.push_back( Variation( "8 GeV/#it{c} < #it{p}_{T}", "onium.pt>8", kBlue,    24) );
   
   
   
//    variations.push_back( Variation( "16 GeV/#it{c} < #it{p}_{T}", "onium.pt>16", kBlue,    24) );
    variations.push_back( Variation( "8 GeV/#it{c} < #it{p}_{T} ", "onium.pt>8", kRed+2,    22) );
   variations.push_back( Variation( "4 GeV/#it{c} < #it{p}_{T}", "onium.pt>4 ", kRed,    21) );
   variations.push_back( Variation( "#it{p}_{T} < 4 GeV/#it{c}",   "onium.pt<4",    kOrange-3,    23) );
break;}



  // plot different pT, coalescence
      case 7:{
     commonCut  = "jpsi && coalescence";
        header = "J/#it{#psi} from cluster collapse";
     commonName =  "coalescence";
     variationsName  = "pT";
     plotMult = true;
     plotPt = false;
//      plotY = false;
     
    variations.clear();
   variations.push_back( Variation( "8 GeV/#it{c} < #it{p}_{T}", "onium.pt>8", kRed+2,    22) );
   variations.push_back( Variation( "4 GeV/#it{c} < #it{p}_{T} < 8 GeV/#it{c}", "onium.pt>4 && onium.pt<8", kRed,    21) );
   variations.push_back( Variation( "#it{p}_{T} < 4 GeV/#it{c}",   "onium.pt<4",    kOrange-3,    23) );
break;}


  // plot number of mpi for hard processes
      case 8:{
        commonCut  = "jpsi && hardProcess";
        header = "J/#it{#psi} from NRQCD";
        commonName =  "hardProcess";
        variationsName  = "iMPI";
        plotMult = true;
        plotPt = true;
        
        legendsX[1] = 1;
        variations.clear();
        variations.push_back( Variation( "1^{st} collision",      "onium.iMPI==0",    kBlue,   21  ) );
        variations.push_back( Variation( "3^{rd} collision",       " onium.iMPI ==2", kMagenta+3,   24) );
        variations.push_back( Variation( "5^{th} collision",       " onium.iMPI ==4", kMagenta+2,   24) );
        variations.push_back( Variation( "7^{th} collision",       " onium.iMPI ==6", kMagenta+1,   24) );
        variations.push_back( Variation( "9^{th} collision",       " onium.iMPI ==8", kMagenta+0,   24) );
        variations.push_back( Variation( "11^{th} collision",       " onium.iMPI ==10", kRed,   24) );
//         variations.push_back( Variation( "i^{th} collision",       " 1", kRed,   20) );
//         variations.push_back( Variation( "iMPI=10",     " onium.iMPI ==10", kRed,   24) );
//         variations.push_back( Variation( "iMPI=11-12",   "onium.iMPI ==11||onium.iMPI ==12", kRed+1,   24) );
//         variations.push_back( Variation( "iMPI=13-14",   "onium.iMPI ==13||onium.iMPI ==14", kRed+2,   24) );
      break;}
      
        // for prompt hard process, plot color octet vs. color singlet
      case 88:{
        commonCut  = "jpsi && hardProcess";
        header = "Prompt J/#it{#psi} from NRQCD";
        commonName =  "hardProcess";
        variationsName  = "colorSingletOctet";
        plotMult = true;
        plotPt = true;
        
        legendsX[1] = 1;
        variations.clear();
        variations.push_back( Variation( "color singlet",      "!octet",    kBlue,   22  ) );
        variations.push_back( Variation( "color octet",       "octet", kRed,   25) );
//         variations.push_back( Variation( "i^{th} collision",       " 1", kRed,   20) );
//         variations.push_back( Variation( "iMPI=10",     " onium.iMPI ==10", kRed,   24) );
//         variations.push_back( Variation( "iMPI=11-12",   "onium.iMPI ==11||onium.iMPI ==12", kRed+1,   24) );
//         variations.push_back( Variation( "iMPI=13-14",   "onium.iMPI ==13||onium.iMPI ==14", kRed+2,   24) );
      break;} 
      
// for NRQCD, plot color octet vs. color singlet, high vs low pT
      case 999:{
        commonCut  = "jpsi && hardProcess";
        header = "Prompt J/#it{#psi} from NRQCD";
        commonName =  "hardProcess";
        variationsName  = "colorSingletOctetpT";
        plotMult = true;
        plotPt = true;
        
        legendsX[1] = 1;
        variations.clear();
        variations.push_back( Variation( "color singlet",  "!octet",    kBlue,   21  ) );
        variations.push_back( Variation( "color singlet, #it{p}_{T} > 12 GeV/#it{c}", "!octet&& onium.pt>12",    kBlue+2, 25 ) );
        variations.push_back( Variation( "color octet",   "octet", kRed,   22) );
        variations.push_back( Variation( "color octet, #it{p}_{T} > 12 GeV/#it{c}",  "octet && onium.pt>12", kRed+2,   26) );
//         variations.push_back( Variation( "i^{th} collision",       " 1", kRed,   20) );
//         variations.push_back( Variation( "iMPI=10",     " onium.iMPI ==10", kRed,   24) );
//         variations.push_back( Variation( "iMPI=11-12",   "onium.iMPI ==11||onium.iMPI ==12", kRed+1,   24) );
//         variations.push_back( Variation( "iMPI=13-14",   "onium.iMPI ==13||onium.iMPI ==14", kRed+2,   24) );
      break;} 
      
      // for prompt hard process, plot processes
      case 8888:{
        commonCut  = "jpsi && hardProcess";
        header = "J/#it{#psi} from hard process";
        commonName =  "hardProcess";
        variationsName  = "processes";
        plotMult = true;
        plotPt = true;
        
        legendsX[1] = 1;
        variations.clear();
        variations.push_back( Variation( "g g #rightarrow Q#bar{Q} g",      "onium.process ==1",    kBlue,   22  ) );
        variations.push_back( Variation( "g q #rightarrow Q#bar{Q} q",      "onium.process ==3",    kGreen,   23  ) );
        variations.push_back( Variation( "q #bar{q} #rightarrow Q#bar{Q} g",      "onium.process ==2",    kRed,   24 ) );
//         variations.push_back( Variation( "i^{th} collision",       " 1", kRed,   20) );
//         variations.push_back( Variation( "iMPI=10",     " onium.iMPI ==10", kRed,   24) );
//         variations.push_back( Variation( "iMPI=11-12",   "onium.iMPI ==11||onium.iMPI ==12", kRed+1,   24) );
//         variations.push_back( Variation( "iMPI=13-14",   "onium.iMPI ==13||onium.iMPI ==14", kRed+2,   24) );
      break;}  
      // for prompt hard process, plot processes, octet vs. singlet
      case 88888:{
        commonCut  = "jpsi && hardProcess";
        header = "J/#it{#psi} from hard process";
        commonName =  "hardProcess";
        variationsName  = "processesOctet";
        plotMult = true;
        plotPt = true;
        
        legendsX[1] = 1;
        variations.clear();
        variations.push_back( Variation( "g g #rightarrow Q#bar{Q} g (singlet)",      "onium.process ==1 && !octet",    kBlue,   21  ) );
        variations.push_back( Variation( "g g #rightarrow Q#bar{Q} g (octet)",      "onium.process ==1 && octet",    kBlue+2,   25  ) );
        variations.push_back( Variation( "g q #rightarrow Q#bar{Q} q (singlet)",      "onium.process ==3 && !octet",    kGreen,   22  ) );
        variations.push_back( Variation( "g q #rightarrow Q#bar{Q} q (octet)",      "onium.process ==3&& octet",    kGreen+2,   26  ) );
        variations.push_back( Variation( "q #bar{q} #rightarrow Q#bar{Q} g (singlet)",      "onium.process ==2 && !octet",    kRed,   23 ) );
        variations.push_back( Variation( "q #bar{q} #rightarrow Q#bar{Q} g (octet)",      "onium.process ==2&& octet",    kRed+2,   32 ) );
      break;}
      
  // plot production processes for Bmeson
      case 9:{
        commonCut  = "jpsi && bmeson";
        header = "J/#it{#psi} #leftarrow B";
        commonName =  "bmeson";
        variationsName  = "processes";
        plotMult = true;
        plotPt = true;
        
        variations.clear();
        const Int_t nVar = 10;
        TString names[nVar] = {
          "gluon fusion", "q#bar{q} annihilation", "flavor excitation",
         "gluon from hard process", "gluon off heavy quark", 
          "gluon from ISR", "gluon from FSR", "gluon from ISR+FSR",
          "beam remnant", "beam remnant to hard process"
        };
        
        Int_t colors[nVar] = {
          kBlue, kBlue + 1 , kBlue+2,
          kGreen, kGreen+1, 
          kRed, kRed+1, kRed+2, 
          kGray, kBlack
        };
        Int_t processes[nVar] = {
       1, 2, 3,
        4, 6,
        8, 9, 10,
        14, 15};
        
        Int_t styles[nVar] = {
          20, 21, 22,
        33, 34,
        24,25,26,
        30, 29};
        
        
        for( int i = 0 ; i < nVar; ++ i ){
          variations.push_back( Variation(names[i].Data(), Form("onium.mother1process==%d", processes[i] ),  colors[i],  styles[i] ) );
//           variations.push_back( Variation( Form("%d (initial)",i), Form("!onium.mother1mpi && onium.mother1process==%d",i),  kRed,     20+i) );
//           variations.push_back( Variation( Form("%d (mpi)",i), Form("onium.mother1mpi && onium.mother1process==%d",i),  kBlue,     20+i) );
        }
      break;}
      
  // plot production processes for Bmeson, hard processes only
      case 99:{
        commonCut  = "jpsi && bmeson";
        header = "J/#it{#psi} #leftarrow B (hard process)";
        commonName =  "bmeson";
        variationsName  = "processesHard";
        plotMult = true;
        plotPt = true;
        
        variations.clear();
        const Int_t nVar = 10;
        TString names[nVar] = {
          "gluon fusion", "q#bar{q} annihilation", "flavor excitation",
         "gluon from hard process", "gluon off heavy quark", 
          "gluon from ISR", "gluon from FSR", "gluon from ISR+FSR",
          "beam remnant", "beam remnant to hard process"
        };
        
        Int_t colors[nVar] = {
          kBlue, kBlue + 1 , kBlue+2,
          kGreen, kGreen+1, 
          kRed, kRed+1, kRed+2, 
          kGray, kBlack
        };
        Int_t processes[nVar] = {
       1, 2, 3,
        4, 6,
        8, 9, 10,
        14, 15};
        
        Int_t styles[nVar] = {
          20, 21, 22,
        33, 34,
        24,25,26,
        30, 29};
        
        
        for( int i = 0 ; i < 3; ++ i ){
          variations.push_back( Variation(names[i].Data(), Form("onium.mother1process==%d", processes[i] ),  colors[i],  styles[i] ) );
//           variations.push_back( Variation( Form("%d (initial)",i), Form("!onium.mother1mpi && onium.mother1process==%d",i),  kRed,     20+i) );
//           variations.push_back( Variation( Form("%d (mpi)",i), Form("onium.mother1mpi && onium.mother1process==%d",i),  kBlue,     20+i) );
        }
      break;}
      
      
      // mpi for coalescence
      case 10:{
        commonCut  = "jpsi && coalescence ";
        header = "J/#it{#psi} from coalescence";
        
        commonName =  "coalescence";
        variationsName  = "mpi";
        plotMult = true;
        
        drawSqrt = true;
        
        variations.clear();
        variations.push_back( Variation( "both initial",      "initial1 && initial2",    kBlue,   21  ) );
        variations.push_back( Variation( "one initial",      "initial1 || initial2",    kViolet,   22  ) );
//         variations.push_back( Variation( "both from MPI",      "bothMPI",    kRed,  24 ) );

      break;}         
      
      // mpi for coalescence, ancestor, only gluon fusion 
      case 100:{
//         commonCut  = "(jpsi && coalescence  && onium.mother1process == 1 && onium.mother2process == 1)";
//         commonCut  = "(jpsi && coalescence  && onium.mother1process <2 && onium.mother2process <2)";
//         commonCut  = "(jpsi && bmeson )";
        commonCut  = "(jpsi && bmeson && onium.mother1process ==1 && onium.mother2process ==1 )";
        header = "J/#it{#psi} from B";
        
        commonName =  "bmesonSame";
        variationsName  = "mpi";
        plotMult = true;
        
        variations.clear();
        variations.push_back( Variation( "both initial, diff",      "bothInitial&& !onium.sameAncestor",    kBlue,   21  ) );
        variations.push_back( Variation( "one from MPI, diff",      "InitialPlusMPI&& !onium.sameAncestor",    kViolet,   21  ) );
        variations.push_back( Variation( "both from MPI, diff",      "bothMPI&& !onium.sameAncestor",    kRed,  21 ) );
        
        
        
        variations.push_back( Variation( "both initial, same",      "bothInitial && onium.sameAncestor",    kBlue,   24  ) );
        variations.push_back( Variation( "one from MPI, same",      "InitialPlusMPI&& onium.sameAncestor",    kViolet,   24  ) );
        variations.push_back( Variation( "both from MPI, same",      "bothMPI&& onium.sameAncestor",    kRed,  24 ) );

      break;}    
      
      
            // ancestor for coalescence 
      case 1000:{
        commonCut  = "(jpsi && coalescence )";
        header = "J/#it{#psi} from coalescence";
        
        commonName =  "coalescence";
        variationsName  = "ancestor";
        plotMult = true;
        
        variations.clear();
        variations.push_back( Variation( "same",      "onium.sameAncestor",    kBlue,   21  ) );
        variations.push_back( Variation( "diff",      "!onium.sameAncestor",    kRed,   24  ) );

      break;} 
      

      // mpi for bmeson, light vs heavy quark
      case 12:{
        commonCut  = "jpsi && bmeson  ";
        header = "J/#it{#psi} #leftarrow B";
        
        commonName =  "bmeson";
        variationsName  = "mpi_lightVsHeavy";
        plotMult = true;
        plotPt = true;
        
        variations.clear();
        variations.push_back( Variation( "both initial",      " initial1 && initial2",    kBlue,   21  ) );
        variations.push_back( Variation( "light quark initial",    " initial2 ",    kViolet+2,   22  ) );
        variations.push_back( Variation( "heavy quark initial",    "  initial1",    kViolet,   23  ) );
//         variations.push_back( Variation( "both from MPI",      "bothMPI",    kRed,   24  ) );

      break;}
      
      
      // mpi for B_c, light vs heavy quark
      case 122:{
        commonCut  = "jpsi && bc  ";
        header = "J/#it{#psi} #leftarrow B_{c}";
        
        commonName =  "bc";
        variationsName  = "mpi_lightVsHeavy";
        plotMult = true;
        plotPt = true;
        
        variations.clear();
        variations.push_back( Variation( "both initial",      " bothInitial",    kBlue,   21  ) );
        variations.push_back( Variation( "light quark from MPI",    " initial1 && mpi2 ",    kViolet+2,   22  ) );
        variations.push_back( Variation( "heavy quark from MPI",    " mpi1 && initial2",    kViolet,   23  ) );
        variations.push_back( Variation( "both from MPI",      "bothMPI",    kRed,   24  ) );

      break;}      
      // mpi for B_s, light vs heavy quark
      case 1222:{
        commonCut  = "jpsi && bs  ";
        header = "J/#it{#psi} #leftarrow B_{s}";
        
        commonName =  "bs";
        variationsName  = "mpi_lightVsHeavy";
        plotMult = true;
        plotPt = true;
        
        variations.clear();
        variations.push_back( Variation( "both initial",      " bothInitial",    kBlue,   21  ) );
        variations.push_back( Variation( "light quark from MPI",    " initial1 && mpi2 ",    kViolet+2,   22  ) );
        variations.push_back( Variation( "heavy quark from MPI",    " mpi1 && initial2",    kViolet,   23  ) );
        variations.push_back( Variation( "both from MPI",      "bothMPI",    kRed,   24  ) );

      break;}
      
      
            // B, Bs, Bc
      case 12222:{
//         commonCut  = "onium.process==4";
        commonCut  = "1";
        header = "";
//         header = "J/#it{#psi} from B or coalescence";
        
        commonName =  "fragmentation";
        variationsName  = "y_bc_coalescence";
        plotMult = true;
        plotPt = true;
        
        plotInclusive = false;
        
        variations.clear();
        variations.push_back( Variation( "J/#it{#psi} #leftarrow B_{ud}",      " jpsi&&blight",    kGreen,   21  ) );
        variations.push_back( Variation( "J/#it{#psi} #leftarrow B_{s}",      " jpsi&&bs",    kBlue,   23  ) );
        variations.push_back( Variation( "J/#it{#psi} #leftarrow B_{c}",   " jpsi&&bc",    kViolet+2,   22  ) );
        variations.push_back( Variation( "J/#it{#psi} from cluster collapse",   "jpsi&& coalescence", kRed, 25 ) );
//          variations.push_back( Variation( "#Upsilon from cluster collapse",   " ypsilon1s&& coalescencey", kBlack, 26 ) );       
//         variations.push_back( Variation( "#Upsilon",   " ypsilon1s", kOrange, 26 ) );

      break;}

  // plot production processes for Bmesons (simplified)
       case 13:{
        commonCut  = "jpsi && bmeson";
        header = "J/#it{#psi} #leftarrow B";
        
        commonName =  "bmesons";
        variationsName  = "processes_simplified";
        plotMult = true;
        plotPt = true;
        
        legendsY[3] = 5;
        
        variations.clear();
        const Int_t nVar = 4;
        TString names[nVar] = {
          "hard process",
         "hard gluon",
          "ISR, FSR",
          "beam remnant"
        };
        
        Int_t colors[nVar] = {
          kBlue,
          kGreen, 
          kRed,
          kGray+2
        };
        Int_t processes_min [nVar] = { 1, 4, 6,  14 };
        Int_t processes_max [nVar] = { 3, 4, 10, 15 };
        
        Int_t styles[nVar] = {
        21,
        25, 
        32,
        24};
        
        
        for( int i = 0 ; i < 4; ++ i ){
          variations.push_back( Variation(names[i].Data(), Form("onium.mother1process>=%d&&onium.mother1process<=%d", processes_min[i] , processes_max[i] ),  colors[i],  styles[i] ) );
//           variations.push_back( Variation( Form("%d (initial)",i), Form("!onium.mother1mpi && onium.mother1process==%d",i),  kRed,     20+i) );
//           variations.push_back( Variation( Form("%d (mpi)",i), Form("onium.mother1mpi && onium.mother1process==%d",i),  kBlue,     20+i) );
        }
      break;}

  // plot production processes for b baryon (simplified)
      case 133:{
        commonCut  = "jpsi && nonprompt && !bmeson";
        header = "J/#it{#psi} #leftarrow b baryon";
        
        commonName =  "bbaryon";
        variationsName  = "processes_simplified";
        plotMult = true;
        plotPt = true;
        
        variations.clear();
        const Int_t nVar = 4;
        TString names[nVar] = {
          "hard process",
         "hard gluon",
          "ISR, FSR",
          "beam remnant"
        };
        
        Int_t colors[nVar] = {
          kBlue,
          kGreen, 
          kRed,
          kGray+2
        };
        Int_t processes_min [nVar] = { 1, 4, 6,  14 };
        Int_t processes_max [nVar] = { 3, 4, 10, 15 };
        
        Int_t styles[nVar] = {
        21,
        25, 
        32,
        24};
        
        
        for( int i = 0 ; i < nVar; ++ i ){
          variations.push_back( Variation(names[i].Data(), Form("onium.mother1process>=%d&&onium.mother1process<=%d", processes_min[i] , processes_max[i] ),  colors[i],  styles[i] ) );
//           variations.push_back( Variation( Form("%d (initial)",i), Form("!onium.mother1mpi && onium.mother1process==%d",i),  kRed,     20+i) );
//           variations.push_back( Variation( Form("%d (mpi)",i), Form("onium.mother1mpi && onium.mother1process==%d",i),  kBlue,     20+i) );
        }
      break;}

  // plot production processes for nonprompt (simplified)
      case 1333:{
        commonCut  = "jpsi && nonprompt ";
        header = "Non-prompt J/#it{#psi}";
        
        commonName =  "nonprompt";
        variationsName  = "processes_simplified";
        plotMult = true;
        plotPt = true;
        plotInclusive = false;
        
        legendsY[3] = 5;
        variations.clear();
        const Int_t nVar = 4;
        TString names[nVar] = {
          "hard process",
         "hard gluon",
          "ISR, FSR",
          "beam remnant"
        };
        
        Int_t colors[nVar] = {
          kBlue,
          kGreen, 
          kRed,
          kGray+2
        };
        Int_t processes_min [nVar] = {  1, 4, 6,  14 };
        Int_t processes_max [nVar] = {  3, 4, 10, 15 };
        
        Int_t styles[nVar] = {
        21,
        25, 
        32,
        24};
        
        
        for( int i = 0 ; i < nVar; ++ i ){
          variations.push_back( Variation(names[i].Data(), Form("onium.mother1process>=%d&&onium.mother1process<=%d", processes_min[i] , processes_max[i] ),  colors[i],  styles[i] ) );
//           variations.push_back( Variation( Form("%d (initial)",i), Form("!onium.mother1mpi && onium.mother1process==%d",i),  kRed,     20+i) );
//           variations.push_back( Variation( Form("%d (mpi)",i), Form("onium.mother1mpi && onium.mother1process==%d",i),  kBlue,     20+i) );
        }
      break;}
      
      
        // plot production processes for Bmesons (more simplified)
       case 13333:{
        commonCut  = "jpsi && bmeson";
        header = "J/#it{#psi} #leftarrow B";
        
        commonName =  "bmesons";
        variationsName  = "processes_oversimplified";
        plotMult = true;
        plotPt = true;
        
        variations.clear();
        const Int_t nVar = 2;
        TString names[nVar] = {
          "hard process",
         "gluon splitting"
        };
        
        Int_t colors[nVar] = {
          kBlue,
          kRed
        };
        Int_t processes_min [nVar] = { 1, 4  };
        Int_t processes_max [nVar] = { 3, 10  };
        
        Int_t styles[nVar] = {
        21,
        25};
        
        
        for( int i = 0 ; i < 2; ++ i ){
          variations.push_back( Variation(names[i].Data(), Form("onium.mother1process>=%d&&onium.mother1process<=%d", processes_min[i] , processes_max[i] ),  colors[i],  styles[i] ) );
//           variations.push_back( Variation( Form("%d (initial)",i), Form("!onium.mother1mpi && onium.mother1process==%d",i),  kRed,     20+i) );
//           variations.push_back( Variation( Form("%d (mpi)",i), Form("onium.mother1mpi && onium.mother1process==%d",i),  kBlue,     20+i) );
        }
      break;}

      
      
  // plot production processes for Bmesons for the light quark
      case 43:{
        commonCut  = "jpsi && bmeson";
        header = "J/#it{#psi} #leftarrow B";
        
        commonName =  "bmesons";
        variationsName  = "processes_simplified";
        plotMult = true;
        plotPt = true;
        
        variations.clear();
        const Int_t nVar = 4;
        TString names[nVar] = {"hard process",
         "gluon from hard process",  
          "gluon from ISR, FSR", 
          "beam remnant"
        };
        
        Int_t colors[nVar] = {
          kBlue, 
          kGreen,
          kRed, 
          kBlack
        };
        Int_t processes_min [nVar] = { 1, 4, 8,  14 };
        Int_t processes_max [nVar] = { 3, 6, 10, 15 };
        
        Int_t styles[nVar] = {21,
        20, 
        24,
        34};
        
        
        for( int i = 0 ; i < 3; ++ i ){
          variations.push_back( Variation(names[i].Data(), Form("onium.mother2process>=%d&&onium.mother2process<=%d", processes_min[i] , processes_max[i] ),  colors[i],  styles[i] ) );
        }
      break;}

  // plot production processes for coalescence J/psi (simplified)
      case 53:{
        commonCut  = "jpsi && !nonprompt && onium.process==4";
        header = "J/#it{#psi} from coalescence";
        
        commonName =  "coalescence";
        variationsName  = "processes_simplified";
        plotMult = true;
        plotPt = true;
        plotInclusive = false;
        
        variations.clear();
        const Int_t nVar = 3;
        TString names[nVar] = {
          "hard",
         //"gluon from hard process",  
          "ISR, FSR", 
          "beam remnant"
        };
        
        Int_t colors[nVar] = {
          kBlue, 
//           kGreen,
          kRed, 
          kGray
        };
        Int_t processes_min [nVar] = { 1, 8,  14 };
        Int_t processes_max [nVar] = { 6, 10, 15 };
        
        Int_t styles[nVar] = {21,
//         20, 
        24,
        34};
        
        
//       for( int i = 0 ; i < 2; ++ i ){
        for( int i = 0 ; i < 3; ++ i ){
          variations.push_back( Variation(
            names[i].Data() ,
                Form("(p1>=%d&&p1<=%d)||(p2>=%d&&p2<=%d)", processes_min[i] , processes_max[i] ,processes_min[i] , processes_max[i] ),  colors[i],  styles[i] ) );
        }
//       }
      break;}

  // plot production processes for coalescence J/psi (2D)
      case 63:{
        commonCut  = "jpsi && !nonprompt && onium.process==4";
        header = "J/#it{#psi} from coalescence";
        
        commonName =  "coalescence";
        variationsName  = "processes_2D";
        plotMult = true;
        plotPt = true;
//         plotInclusive = false;
        
        variations.clear();
        const Int_t nVar = 3;
        TString names[nVar] = {
          "hard",
         //"gluon from hard process",  
          "ISR, FSR", 
          "beam remnant"
        };
        
        Int_t processes_min [nVar] = { 1, 8,  14 };
        Int_t processes_max [nVar] = { 6, 10, 15 };
        
        Int_t styles[4] = {
          21,
          25,
        24,
        34};
        
        Int_t colors[4] = {
          kBlue, 
          kViolet,
          kRed, 
          kGray
        };
        int k =0;
       for( int j = 0 ; j < 2; ++ j ){
        for( int i = j ; i < 2; ++ i ){
        
          variations.push_back( Variation(
            Form( "%s + %s", names[i].Data(),names[j].Data() ),
                Form("(p1>=%d&&p1<=%d&&p2>=%d&&p2<=%d)||(p2>=%d&&p2<=%d&&p1>=%d&&p1<=%d)",
                     processes_min[i] , processes_max[i] ,
                     processes_min[j] , processes_max[j],
                     processes_min[i] , processes_max[i] ,
                     processes_min[j] , processes_max[j] ),  colors[k],  styles[k] ) );
          k++;
        }
       }       
       
          variations.push_back( Variation(
            Form( "%s + anything", names[2].Data() ),
                Form("(p1>=%d&&p1<=%d)||(p2>=%d&&p2<=%d)",
                     processes_min[2] , processes_max[2],
                     processes_min[2] , processes_max[2] ),  colors[k],  styles[k] ) );

      break;}
      
  // plot processes, initial, mpi for Bmesons
     
        // plot production processes for Bmesons (light and heavy quark)
      case 33:{
        commonCut  = "jpsi && bmeson";
        header = "J/#it{#psi} #leftarrow B";
        
        commonName =  "bmesons";
        variationsName  = "processes_simplified_light_heavy";
        plotMult = true;
        plotPt = true;
        
        variations.clear();
        const Int_t nVar = 4;
        TString names[nVar] = {"hard process",
         "gluon from hard process",  
          "gluon from ISR, FSR", 
          "beam remnant"
        };
        
        Int_t colors[nVar] = {
          kBlue, 
          kGreen,
          kRed, 
          kBlack
        };
        Int_t processes_min [nVar] = { 1, 4, 8,  14 };
        Int_t processes_max [nVar] = { 3, 6, 10, 15 };
        
        Int_t styles[nVar] = {21,
        20, 
        24,
        34};
        
        
        for( int i = 0 ; i < 3; ++i ){
          for( int j = 0 ; j < 3; ++j ){
            variations.push_back( Variation(  Form(  "%s %s", names[i].Data() , names[j].Data()  )  , Form("onium.mother1process>=%d&&onium.mother1process<=%d&&onium.mother2process>=%d&&onium.mother2process<=%d", processes_min[i] , processes_max[i],processes_min[j] , processes_max[j] ),  colors[i],  styles[j] ) );
          }
        }
      break;}
      
      // plot production processes for Bmesons (mpi)
      case 14:{
        commonCut  = "jpsi && bmeson";
        header = "J/#it{#psi} #leftarrow B";
        
        commonName =  "bmeson";
        variationsName  = "processes_mpi";
        plotMult = true;
        plotPt = true;
        
//         
        
        
        variations.clear();
        const Int_t nVar = 4;
        TString names[nVar] = {"hard process",
         "gluon from hard process",  
          "gluon from ISR, FSR",
          "beam remnant"
        };
        
        Int_t colors[nVar] = {
          kBlue, 
          kGreen,
          kRed,
          kBlack
        };
        Int_t processes_min [nVar] = { 1, 4, 8,  14 };
        Int_t processes_max [nVar] = { 3, 6, 10, 15 };
        
        Int_t styles[nVar] = {21,
        20, 
        24,
        34
        };
        
        
        
        for( int i = 0 ; i < 3; ++ i ){
          variations.push_back( Variation( Form( "%s (initial)", names[i].Data() ), Form("!onium.mother1mpi&&onium.mother1process>=%d&&onium.mother1process<=%d", processes_min[i] , processes_max[i] ),  kBlue,  styles[i] ) );
//           variations.push_back( Variation( Form( "%s (mpi)", names[i].Data() ), Form("onium.mother1mpi&&onium.mother1process>=%d&&onium.mother1process<=%d", processes_min[i] , processes_max[i] ),  kRed,  styles[i] ) );
        }
        
        for( int i = 0 ; i < 3; ++ i ){
//           variations.push_back( Variation( Form( "%s (initial)", names[i].Data() ), Form("!onium.mother1mpi&&onium.mother1process>=%d&&onium.mother1process<=%d", processes_min[i] , processes_max[i] ),  kBlue,  styles[i] ) );
          variations.push_back( Variation( Form( "%s (mpi)", names[i].Data() ), Form("onium.mother1mpi&&onium.mother1process>=%d&&onium.mother1process<=%d", processes_min[i] , processes_max[i] ),  kRed,  styles[i] ) );
        }
      break;}
  // plot decay channel differential
      case 15:{
        commonCut  = "jpsi";
        commonName =  "inclusive";
        header = "inclusive J/#it{#psi}";
        
        variationsName  = "decay_channels";
        plotMult = true;
        plotPt = true;
        
        legendsX[1]=1;
        variations.clear();
        variations.push_back( Variation( "dilepton decay channel",  "dilepton ",    kRed,     21) );
//         variations.push_back( Variation( "dimuon",  "onium.decayChannel==-1",    kGreen,     21) );
         variations.push_back( Variation( "other decay channels",  "!dilepton",    kBlue,     24) );
      break; }
  // plot different cc states
      case 16:{
        commonCut  = "!nonprompt";
        commonName =  "charmonium";
        header = "Prompt charmonium";
        variationsName  = "states";
        plotMult = true;
        plotPt = true;
        
        
        legendsX[1]=1;
        variations.clear();
        variations.push_back( Variation( "J/#it{#psi}",  "jpsi&&direct",    kRed,     21) );
        variations.push_back( Variation( "#psi(2s)",  "psiprime",    kGreen,     23) );
      break;  }
  // plot different cc states, hard processes only
      case 26:{
        commonCut  = "!nonprompt&&hardProcess";
        commonName =  "hardCharmonium";
        header = "Charmonium from hard process";
        
        variationsName  = "states";
        plotMult = true;
        plotPt = true;
        
        legendsX[1]=1;
        variations.clear();
        variations.push_back( Variation( "J/#it{#psi}",  "jpsi&&direct",    kRed,     21) );
        variations.push_back( Variation( "#psi(2s)",  "psiprime",    kGreen,     23) );
//         variations.push_back( Variation( "J/#it{#psi}",  "jpsi&&direct",    kRed,     21) );
//         variations.push_back( Variation( "#psi(2s)",  "psiprime&&initialpsiprime",    kGreen,     23) );
//         variations.push_back( Variation( "#chi_{c1}",  "chic1&&initialchic1",    kBlue,     5) );
//         variations.push_back( Variation( "#chi_{c2}",  "chic2&&initialchic2",    kAzure,     2) );
      break;  }

        // plot decay channel differential for non-prompt
      case 19:{
        commonCut  = "jpsi && nonprompt";
        commonName =  "nonprompt";
        header = "Non-promp J/#it{#psi}";
        
        variationsName  = "decay_channels";
        plotMult = true;
        plotPt = true;
        
        legendsX[1]=1;
        variations.clear();
        variations.push_back( Variation( "dilepton",  "abs(onium.decayChannel==1)",    kRed,     21) );
//         variations.push_back( Variation( "dimuon",  "onium.decayChannel==-1",    kGreen,     21) );
         variations.push_back( Variation( "other",  "onium.decayChannel==0",    kBlue,     24) );
      break; }
        // plot decay channel differential for prompt
      case 20:{
        commonCut  = "jpsi && !nonprompt";
        commonName =  "prompt";
        header = "Prompt J/#it{#psi}";
        
        variationsName  = "decay_channels";
        plotMult = true;
        plotPt = true;

        
        legendsX[1]=1;
        variations.clear();
        variations.push_back( Variation( "dilepton",  "abs(onium.decayChannel==1)",    kRed,     21) );
//         variations.push_back( Variation( "dimuon",  "onium.decayChannel==-1",    kGreen,     21) );
         variations.push_back( Variation( "other",  "onium.decayChannel==0",    kBlue,     24) );
      break; }
        // plot decay channel differential for coalescence
      case 200:{
        commonCut  = "jpsi && coalescence";
        commonName =  "coalescence";
        header = "J/#it{#psi} (coalescence)";
        
        variationsName  = "decay_channels";
        plotMult = true;
        plotPt = true;

        
        legendsX[1]=1;
        variations.clear();
        variations.push_back( Variation( "dilepton",  "abs(onium.decayChannel==1)",    kRed,     21) );
//         variations.push_back( Variation( "dimuon",  "onium.decayChannel==-1",    kGreen,     21) );
         variations.push_back( Variation( "other",  "onium.decayChannel==0",    kBlue,     24) );
      break; }
        // plot decay channel differential for hard process
      case 2000:{
        commonCut  = "jpsi && hardProcess";
        commonName =  "hard";
        header = "J/#it{#psi} (hard process)";
        
        variationsName  = "decay_channels";
        plotMult = true;
        plotPt = true;

        
        legendsX[1]=1;
        variations.clear();
        variations.push_back( Variation( "dilepton",  "abs(onium.decayChannel==1)",    kRed,     21) );
//         variations.push_back( Variation( "dimuon",  "onium.decayChannel==-1",    kGreen,     21) );
         variations.push_back( Variation( "other",  "onium.decayChannel==0",    kBlue,     24) );
      break; }

  // plot prompt, nonprompt
      case 22 : {
        commonCut  = "jpsi";
        header = "J/#it{#psi}";
        commonName =  "inclusive";
        variationsName  = "Prompt_Nonpompt";
        plotMult = true;
        plotPt = true;
        plotInclusive =true;
        
        legendsX[1] =1;
        variations.clear();
        variations.push_back( Variation( "prompt",          "!nonprompt",    kViolet,     21) );
        variations.push_back( Variation( "non-prompt",       "nonprompt", kGreen+2,   32) );
      break;   }
      
  // plot prompt -- nonprompt, inclusive -- high pT
      case 222 : {
        commonCut  = "jpsi";
        header = "J/#it{#psi}";
        commonName =  "inclusive";
        variationsName  = "Prompt_Nonpompt_pT";
        plotMult = true;
        plotPt = true;
        plotInclusive =false;
        
        panel = "open red: #it{p}_{T} > 8 GeV/#it{c}";
        
        legendsX[1] =1;
//         legendsY[0] =3;
        
        variations.clear();
        variations.push_back( Variation( "Prompt ",     "!nonprompt", kBlack,  21) );
        variations.push_back( Variation( "", "!nonprompt&& onium.pt>4", kRed, 25) );
        variations.push_back( Variation( "Non-prompt", "nonprompt", kBlack,   23) );
        variations.push_back( Variation( "", "nonprompt&& onium.pt>4", kRed,  32) );
        variations.push_back( Variation( "Inclusive",  "1",    kBlack,   20) );
        variations.push_back( Variation( "",  "onium.pt>4",    kRed,   24) );
      break;   }
      
  // plot prompt -- nonprompt, inclusive -- high pT (no coalescence)
      case 2222 : {
        commonCut  = "jpsi && !(!nonprompt && onium.process==4)";
        header = "Inclusive J/#it{#psi}, excluding coalescence";
        commonName =  "inclusive";
        variationsName  = "PromptHard_Nonpompt_pT";
        plotMult = true;
        plotPt = true;
        plotInclusive =false;
        
        legendsX[1] =1;
        legendsY[0] =3;
        variations.clear();
        variations.push_back( Variation( "Prompt J/#it{#psi}",     "!nonprompt", kBlack,  21) );
        variations.push_back( Variation( "Prompt J/#it{#psi} #it{p}_{T} > 8 GeV/#it{c}",     "!nonprompt&& onium.pt>8", kRed, 25) );
        variations.push_back( Variation( "Non-prompt J/#it{#psi}", "nonprompt",kBlack,   23) );
        variations.push_back( Variation( "Non-prompt J/#it{#psi} #it{p}_{T} > 8 GeV/#it{c}", "nonprompt&& onium.pt>8", kRed,  32) );
        variations.push_back( Variation( "J/#it{#psi}",  "1",    kBlack,   20) );
        variations.push_back( Variation( "Inclusive J/#it{#psi} #it{p}_{T} > 8 GeV/#it{c}",  "onium.pt>8",    kRed,   24) );
      break;   }      
  // plot prompt -- nonprompt, inclusive -- high pT (only coalescence)
      case 22222 : {
        commonCut  = "jpsi && onium.process==4";
        header = "Inclusive J/#it{#psi} (excluding NRQCD)";
        commonName =  "fragmentation";
        variationsName  = "Prompt_Nonpompt_pT";
        plotMult = true;
        plotPt = true;
        plotInclusive =false;
        
        panel = "open red: #it{p}_{T} > 8 GeV/#it{c}";
        
        legendsX[1] =1;
//         legendsY[0] =3;
        
        variations.clear();
        variations.push_back( Variation( "Prompt ",     "!nonprompt", kBlack,  21) );
        variations.push_back( Variation( "", "!nonprompt&& onium.pt>8", kRed, 25) );
        variations.push_back( Variation( "Non-prompt", "nonprompt", kBlack,   23) );
        variations.push_back( Variation( "", "nonprompt&& onium.pt>8", kRed,  32) );
        variations.push_back( Variation( "Inclusive",  "1",    kBlack,   20) );
        variations.push_back( Variation( "",  "onium.pt>8",    kRed,   24) ); 
        break;
      }
            
      // mpi for D, light vs heavy quark
      case -12:{
        commonCut  = "Dplus";
        header = "D^{+-}";
        
        commonName =  "D";
        variationsName  = "mpi_lightVsHeavy";
        plotMult = true;
        plotPt = true;
        
        variations.clear();
        variations.push_back( Variation( "both initial",      " bothInitial",    kBlue,   21  ) );
        variations.push_back( Variation( "light quark from MPI",    " !onium.mother1mpi &&  onium.mother2mpi",    kViolet+2,   22  ) );
        variations.push_back( Variation( "heavy quark from MPI",    " onium.mother1mpi  && !onium.mother2mpi",    kViolet,   23  ) );
        variations.push_back( Variation( "both from MPI",      "bothMPI",    kRed,   24  ) );

      break;}
      
  // plot inclusive D vs sources
      case -5 : {
        commonCut  = "Dplus";
        commonName =  "inclusiveD";
        header = "D^{+-}";
        variationsName  = "Sources";
        plotMult = true;
        plotPt = true;
        plotInclusive = true;
        legendsX[1] =1;
        variations.clear();
        
        
        const Int_t nVar = 5;
        TString names[nVar] = {
          "hard process",
         "hard gluon",
          "ISR,FSR",
          "beam remnant",
          "from charmonium decay"
        };
        
        Int_t colors[nVar] = {
          kBlue,
          kGreen, 
          kRed,
          kGray+2,
          kViolet
        };
        Int_t processes_min [nVar] = { 1, 4, 6,  14, 16 };
        Int_t processes_max [nVar] = { 3, 4, 10, 15, 16 };
        
        Int_t styles[nVar] = {
        21,
        25, 
        32,
        24, 29};
        
        
        for( int i = 0 ; i < 4; ++ i ){
          variations.push_back( Variation(names[i].Data(), Form("onium.mother1process>=%d&&onium.mother1process<=%d", processes_min[i] , processes_max[i] ),  colors[i],  styles[i] ) );
        }
      break; }
  // plot inclusive B vs sources
      case -50 : {
        commonCut  = "Bplus";
        commonName =  "inclusiveB";
        header = "B^{+-}";
        variationsName  = "Sources";
        plotMult = true;
        plotPt = true;
        plotInclusive = true;
        legendsX[1] =1;
        variations.clear();
        
        
        const Int_t nVar = 5;
        TString names[nVar] = {
          "hard process",
         "hard gluon",
          "ISR,FSR",
          "beam remnant",
          "from charmonium decay"
        };
        
        Int_t colors[nVar] = {
          kBlue,
          kGreen, 
          kRed,
          kGray+2,
          kViolet
        };
        Int_t processes_min [nVar] = { 1, 4, 6,  14, 16 };
        Int_t processes_max [nVar] = { 3, 4, 10, 15, 16 };
        
        Int_t styles[nVar] = {
        21,
        25, 
        32,
        24, 29};
        
        
        for( int i = 0 ; i < 4; ++ i ){
          variations.push_back( Variation(names[i].Data(), Form("onium.mother1process>=%d&&onium.mother1process<=%d", processes_min[i] , processes_max[i] ),  colors[i],  styles[i] ) );
        }
      break; }
      
  // plot inclusive D vs pT
      case -4 : {
        commonCut  = "Dplus";
        commonName =  "inclusiveD";
        header = "D^{+-}";
        variationsName  = "pT";
        plotMult = true;
        plotPt = false;
        plotInclusive = true;
        legendsX[1] =1;
        variations.clear();
   variations.push_back( Variation( "6 GeV/#it{c} < #it{p}_{T} ", "onium.pt>6", kBlue,    22) );
   variations.push_back( Variation( "2 < #it{p}_{T} < 6 GeV/#it{c}", "onium.pt>2 && onium.pt<6", kOrange-3,    24) );
   variations.push_back( Variation( "0 < #it{p}_{T} < 2 GeV/#it{c}", "onium.pt>0 && onium.pt<2", kRed,    23) );
      break; }   
      
      
      
      
      
      
      
  // plot production processes for D meson (simplified)
       case -13:{
        commonCut  = "Dplus";
        header = "D^{+-}";
        
        commonName =  "inclusiveD";
        variationsName  = "processes_verySimplified";
        plotMult = true;
        plotPt = true;
        
        legendsY[3] = 5;
        
        variations.clear();
        const Int_t nVar = 2;
        TString names[nVar] = {
          "hard process/gluon",
//          "hard gluon",
          "ISR, FSR"
//           "beam remnant"
        };
        
        Int_t colors[nVar] = {
          kBlue,
//           kGreen, 
          kRed
//           kGray+2
        };
        Int_t processes_min [nVar] = { 1, 6  };
        Int_t processes_max [nVar] = { 4, 10 };
        
        Int_t styles[nVar] = {
        21,
//         25, 
        32
//         24
          
        };
        
        
        for( int i = 0 ; i < 2; ++ i ){
          variations.push_back( Variation(names[i].Data(), Form("onium.mother1process>=%d&&onium.mother1process<=%d", processes_min[i] , processes_max[i] ),  colors[i],  styles[i] ) );
//           variations.push_back( Variation( Form("%d (initial)",i), Form("!onium.mother1mpi && onium.mother1process==%d",i),  kRed,     20+i) );
//           variations.push_back( Variation( Form("%d (mpi)",i), Form("onium.mother1mpi && onium.mother1process==%d",i),  kBlue,     20+i) );
        }
      break;}
      
      
      
      
      
  // plot inclusive B vs pT
      case -40 : {
        commonCut  = "Bplus";
        commonName =  "inclusiveB";
        header = "B^{+-}";
        variationsName  = "pT";
        plotMult = true;
        plotPt = false;
        plotInclusive = true;
        legendsX[1] =1;
        variations.clear();
   variations.push_back( Variation( "8 GeV/#it{c} < #it{p}_{T} < 12 GeV/#it{c}", "onium.pt>8 && onium.pt<12", kViolet,    24) );
   variations.push_back( Variation( "4 GeV/#it{c} < #it{p}_{T} < 8 GeV/#it{c}", "onium.pt>4 && onium.pt<8", kRed+2,    22) );
   variations.push_back( Variation( "2 GeV/#it{c} < #it{p}_{T} < 4 GeV/#it{c}", "onium.pt>2 && onium.pt<4", kRed,    21) );
   variations.push_back( Variation( "1 GeV/#it{c} < #it{p}_{T} < 2 GeV/#it{c}", "onium.pt>1 && onium.pt<2", kOrange-3,    23) );
      break; }
  // plot inclusive Ypsilon(3s), decay channel
      case -3 : {
        commonCut  = "ypsilon3s";
        commonName =  "inclusive";
        header = "inclusive #Upsilon";
        variationsName  = "decay_channels";
        plotMult = true;
        plotPt = true;
        plotInclusive = true;
        legendsX[1] =1;
        variations.clear();
        variations.push_back( Variation( "dilepton",  "dilepton",    kRed,     21) );
        variations.push_back( Variation( "other",   "!dilepton", kRed,    25) );
//         variations.push_back( Variation( "(c#bar{c}) feed-down", "cfeeddown", kBlue,    23) );
//         variations.push_back( Variation( "non-prompt",       "nonprompt", kGreen+2,   32) );
      break; }
  // plot inclusive Ypsilon(1s), decay channel
      case -2 : {
        commonCut  = "ypsilon1s";
        commonName =  "inclusive";
        header = "inclusive #Upsilon";
        variationsName  = "decay_channels";
        plotMult = true;
        plotPt = true;
        plotInclusive = true;
        legendsX[1] =1;
        variations.clear();
        variations.push_back( Variation( "dilepton",  "dilepton",    kRed,     21) );
        variations.push_back( Variation( "other",   "!dilepton", kRed,    25) );
//         variations.push_back( Variation( "(c#bar{c}) feed-down", "cfeeddown", kBlue,    23) );
//         variations.push_back( Variation( "non-prompt",       "nonprompt", kGreen+2,   32) );
      break; }
  // plot inclusive Ypsilon(1s), coalescence
      case -1 : {
        commonCut  = "ypsilon1s||ypsilon2s||ypsilon3s";
        commonName =  "Upsilon";
        header = "#Upsilon(nS)";
        variationsName  = "Y_1s_2s_3s";
        plotMult = true;
        plotPt = true;
        plotInclusive = true;
        legendsX[1] =1;
        variations.clear();
        variations.push_back( Variation( "#Upsilon(1S)", "ypsilon1s", kRed,   20) );
        variations.push_back( Variation( "#Upsilon(2S)", "ypsilon2s", kBlue,  21) );
        variations.push_back( Variation( "#Upsilon(3S)", "ypsilon3s", kGreen, 22) );
//         variations.push_back( Variation( "(c#bar{c}) feed-down", "cfeeddown", kBlue,    23) );
//         variations.push_back( Variation( "non-prompt",       "nonprompt", kGreen+2,   32) );
      break; }
  
  
      
      
      
      
      
      
      
      
      
      
      
      default: cout << "UNDEFINED VARIATION!!!"<< endl;
    }
      
  
    
    
    if(commonCut_ptr) *commonCut_ptr = commonCut;
    if(commonName_ptr) *commonName_ptr = commonName;
    if(variationsName_ptr) *variationsName_ptr = variationsName;
    if(header_ptr) *header_ptr = header;
    if(inclusive_ptr) *inclusive_ptr = inclusive;
    if(panel_ptr) *panel_ptr = panel;
    if(legendsX_ptr) {
      legendsX_ptr[0] = legendsX[0];
      legendsX_ptr[1] = legendsX[1];
      legendsX_ptr[2] = legendsX[2];
      legendsX_ptr[3] = legendsX[3];
      legendsX_ptr[4] = legendsX[4];
      legendsX_ptr[5] = legendsX[5];
    }
    if(legendsY_ptr) {
      legendsY_ptr[0] = legendsY[0];
      legendsY_ptr[1] = legendsY[1];
      legendsY_ptr[2] = legendsY[2];
      legendsY_ptr[3] = legendsY[3];
      legendsY_ptr[4] = legendsY[4];
      legendsY_ptr[5] = legendsY[5];
    }
    if(plotMult_ptr) *plotMult_ptr = plotMult;
    if(plotPt_ptr) *plotPt_ptr = plotPt;
    if(plotInclusive_ptr) *plotInclusive_ptr = plotInclusive;
    if(plotY_ptr) *plotY_ptr = plotY;
    if(drawSqrt_ptr) *drawSqrt_ptr = drawSqrt;
    if(variations_ptr) *variations_ptr = variations;

}
