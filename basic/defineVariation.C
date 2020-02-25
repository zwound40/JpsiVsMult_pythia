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
/**
 * 
 * This macro contains the information what exactly the plotRelOrigin macr should plot.
 * 
 * It gets as an input an integer that identifies what to plot and returns the following:
 *  - commonName : a string that is used for the output file name, e.g. inclusive for inclusive J/psi
 *  - variationsName : the same, e.g. pT The output name will be something like ...inclusive_pT.pdf, for inclusive J/psi in pT bins
 *  - commonCut : a cut common for all the variations you want to draw together
 *  - header : String that will be printed as header of the TLegend
 *  - inclusiveLabel: String that is used in the TLegend as label for the inclusive entry
 *  - variations : a vector of different variations what to draw, e.g. pT bins, or prompt and non-prompt
 *    each entry in the vector is a "Variation", that has a 
 *    - name : this is the label in the TLegend
 *    - cut : the string to be given as cut to the TTree
 *    - color : which color to use in the plotting
 *    - style : markerStyle to use in the plotting
 */

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
      TString *commonName_ptr = 0x0,
      TString *variationsName_ptr = 0x0,
      TString *commonCut_ptr = 0x0,
      TString *header_ptr = 0x0,
      TString *inclusiveLabel_ptr = 0x0,
      vector<Variation>*variations_ptr = 0x0){

  
  
    TString commonCut, commonName, variationsName, header, inclusiveLabel;
    inclusiveLabel = "inclusive";
    vector<Variation> variations;
    
    
    switch( plotWhat ){
      
  // plot prompt/non-pompt J/psi
      case 1 : {
        commonCut  = "jpsi";
        header = "J/#it{#psi}";
        inclusiveLabel = "Inclusive J/#psi";
        commonName =  "inclusive";
        variationsName  = "prompt_nonprompt";
        
        variations.clear();
        variations.push_back( Variation( "Prompt J/#psi",          "!nonprompt",    kRed,     21) );
        variations.push_back( Variation( "Non-prompt J/#psi",       "nonprompt", kGreen+2,   32) );
      break;   }
      
  
  // plot pT bins
      case 2:{
     commonCut  = "jpsi";
      header = "J/#it{#psi}";
    inclusiveLabel = "#it{p}_{T} integrated";
     commonName =  "inclusive";
     variationsName  = "pT";
     
    variations.clear();
   variations.push_back( Variation( "8 GeV/#it{c} < #it{p}_{T}", "onium.pt>8", kRed+2,    22) );
   variations.push_back( Variation( "4 GeV/#it{c} < #it{p}_{T} < 8 GeV/#it{c}", "onium.pt>4 && onium.pt<8", kRed,    21) );
   variations.push_back( Variation( "#it{p}_{T} < 4 GeV/#it{c}",   "onium.pt<4",    kOrange-3,    23) );
   
   
        break;}

  // plot different pT, non-prompt J/psi only
      case 3:{
     commonCut  = Form( "jpsi && nonprompt" );
      header = "Non-prompt J/#it{#psi}";
     commonName =  "nonprompt";
     variationsName  = "pT";
    inclusiveLabel = "#it{p}_{T} integrated";
    variations.clear();
   variations.push_back( Variation( "8 GeV/#it{c} < #it{p}_{T}", "onium.pt>8", kRed+2,    22) );
   variations.push_back( Variation( "4 GeV/#it{c} < #it{p}_{T} < 8 GeV/#it{c}", "onium.pt>4 && onium.pt<8", kRed,    21) );
   variations.push_back( Variation( "#it{p}_{T} < 4 GeV/#it{c}",   "onium.pt<4",    kOrange-3,    23) );
   
   
break;}

  // plot different decay channels
      case 4:{
        commonCut  = "jpsi";
        commonName =  "inclusive";
        header = "inclusive J/#it{#psi}";
        inclusiveLabel = "all channels";
        variationsName  = "decay_channels";

        variations.clear();
        variations.push_back( Variation( "dilepton decay channel",  "dilepton ",    kRed,     21) );
         variations.push_back( Variation( "other decay channels",  "!dilepton",    kBlue,     24) );
      break; }





  // plot different charmonium states
      case 5:{
        commonCut  = "charmonia && !nonprompt";
        header = "Prompt charmonium";
        inclusiveLabel = "Any charmonium";
        commonName =  "charmonium";
        variationsName  = "states";
        
        
        variations.clear();
        variations.push_back( Variation( "J/#it{#psi}",  "jpsi&&direct",    kRed,     21) );
        variations.push_back( Variation( "#psi(2s)",  "psiprime",    kGreen,     23) );
      break;  }
      
      default: cout << "UNDEFINED VARIATION!!!"<< endl;
    }
    
    if(commonCut_ptr) *commonCut_ptr = commonCut;
    if(commonName_ptr) *commonName_ptr = commonName;
    if(variationsName_ptr) *variationsName_ptr = variationsName;
    if(header_ptr) *header_ptr = header;
    if(inclusiveLabel_ptr) *inclusiveLabel_ptr = inclusiveLabel;
    if(variations_ptr) *variations_ptr = variations;

}
