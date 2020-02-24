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
#include "TMath.h"
#include "TPaveText.h"
#include "defineEstimator.C"
#include "defineVariation.C"
#include "defineAliases.C"


void plotRelOrigin(
  int setTrigger = 0,
  int setProbe = 0,
  int plotWhat = 4444,
  int selector = 4,
  int estimator = 4,
  TString setSettings = "full",
  int setDiffractive = 0,
  bool setInel0 = 1,
  int signalRapidity = 1,
  int decayChannel = 0,
  int normalization = 2,
  double maxY = 22,
  bool drawQuad = 0,
  TString folder="/home/steffen/ALICE/pythia/jpsiVsMult"
);



using namespace std;

struct Variation;



TCanvas * makeCanvas( TString name, bool logx= false, bool logy= false);
TLegend * makeLegend( TString header, int xPos = 0, int yPos = 0 );
void save( TCanvas * c, TString folder, TString filename );
void stylePlot(TGraphErrors* h, Color_t color, int style, int line, TString  title, TString  xtitle, TString ytitle, double xMin=0, double xMax=0, double yMin=0, double yMax=0 );
void styleFunction(TF1* h, Color_t color, int line, TString  title, TString  xtitle, TString ytitle, double xMin=0, double xMax=0, double yMin=0, double yMax=0 );
void plot( TString commonCut, TString commonName, TString header, TString panel, TString inclusiveName, TString variationsName, vector<Variation> variations, bool plotMult, int normalization, bool plotPt, bool plotInclusive, bool plotY,  int* legendsX, int* legendsY, double maxY, double maxX, bool drawQuad, bool drawSqrt );
TString normalized(TString str);
TString subtracted(TString str);
void crash (TString message);
void drawInCanvas( TCanvas* c, TObject* o, TString option="");


TTree *oniumTree;

double nEventsMB;
double meanMultMB;
const int maxPt = 16;
const int nPtBins = 32;

const int maxRapidity = 8;
const int nRapidityBins = 80;


TString nJpsi;
TString nCh;
TString nTrk;
double rapInterval;
double rapIntervalSignal;


TString settings;
TString multiplicityEstimator="";
TString multiplicitySelector="";
int diffractive; 
bool inel0;
bool noMPI;
bool noCR;
double sqrts;
int probe;
int trigger;


// const unsigned int nMultiplicityBins = 27;
// double multiplicityBins[nMultiplicityBins] = {
//         0.,  4.,  8.,  12., 16.
//         ,20.,  24., 28., 32., 36.
//         ,40., 44., 48., 52., 56.
//         ,60., 64., 68., 72., 76.
//          ,84., 92., 100., 108.
//          ,116.,124. , 132.//, 140.
//       };
      

const unsigned int nMultiplicityBins = 24;


double multiplicityBins[nMultiplicityBins] = {
        0.,  4.,  8.,  12., 16.
        ,20.,  24., 28., 32., 36.
        ,40., 44., 48., 52., 56.
        ,60., 64., 68., 72., 76.
         ,84., 92., 100. ,116.//, 140.
      };
      
      
      
      // for lf
      /*
const unsigned int nMultiplicityBins = 18;
double multiplicityBins[nMultiplicityBins] = {
        0.,  4.,  8.,  12., 16.
        ,20.,  24., 28., 32., 36.
        ,40., 44., 48., 52., 56.
        ,60., 64., 68. //, 72., 76.
      //   ,84., 92., 108. ,132.//, 140.
      };*/
//       
      
      
      
      
      

double meanMultMBPerBin[nMultiplicityBins];
double nEventsPerBin[nMultiplicityBins];
double errPerBin[nMultiplicityBins];


void plotRelOrigin( int setTrigger, int setProbe, int plotWhat, int selector, int estimator, TString setSettings, int setDiffractive , bool setInel0,  int signalRapidity, int decayChannel,   int normalization, double maxY, bool drawQuad, TString folder){

  
  gEnv->SetValue("Hist.Precision.1D", "double");
  settings = setSettings;
  
  diffractive = setDiffractive;
  inel0 = setInel0;
  probe = setProbe;
  trigger = setTrigger;
  
  
  gStyle->SetOptStat(0);
  gStyle->SetTitleW(.8);
  gStyle->SetTitleX(.58);
  
  
  TString fn;
  if( probe == 1) fn=  "D";
  else if(probe==-1) fn = "lf";
  else fn = "onium";
  
  TFile *f =  TFile::Open( Form("%s/%s_%s.root",  folder.Data(), fn.Data(),settings.Data() )  );
  
  cout << "opening " << Form("%s/%s_%s.root",  folder.Data(), fn.Data(),settings.Data() )<<endl;;

  noMPI = strstr( settings.Data(), "noMPI"   );
  noCR = strstr( settings.Data(), "noCR" );
  sqrts = strstr( settings.Data(), "5tev" ) ? 5. : 13.;


  if( probe == 1) nJpsi = "#it{N}_{D}";
  else if( probe == -1) nJpsi = "#it{N}_{ch}";
  else  nJpsi = "#it{N}_{J/#it{#psi}}";

  if( false && normalization < 1  )nJpsi = Form( "%s/1k events", nJpsi.Data()  );
  else if (probe==-1) nJpsi = Form( "d%s/d#it{#eta}", nJpsi.Data()  );
  else nJpsi = Form( "d%s/d#it{y}", nJpsi.Data()  );
  if( true || normalization==2  ) nJpsi = normalized(nJpsi); 
  
  TString region_est;
  TString region_sel;
  Double_t factor;
  bool particles;
  
  defineEstimator( estimator,  noCR, noMPI, multiplicityEstimator, nCh, region_est,  rapInterval, factor, particles );
  defineEstimator( selector,  noCR, noMPI, multiplicitySelector, nTrk, region_sel,  rapInterval, factor, particles );

  
  cout << "estimator: " << multiplicityEstimator << endl;
  cout << "selector: " << multiplicitySelector << endl;
  
  
  if (normalization >0 && particles ) nCh = Form( "d%s/d#it{#eta}", nCh.Data()  );
  if( normalization==2 ) nCh = normalized(nCh); 
  else if( normalization == -1) nCh = subtracted(nCh); 
  if(particles) nCh = Form( "%s(%s, selection: %s)", nCh.Data(), region_est.Data(), region_sel.Data()  );
  
  for(unsigned int i=0; i< nMultiplicityBins; ++i) {
    
    cout << multiplicityBins[i] << " " ;
    multiplicityBins[i] *= factor;
    cout << multiplicityBins[i] << " " ;
    multiplicityBins[i] -= 0.5;
    
    cout << multiplicityBins[i] << endl ;
    
    
  }

  
  TTree * eventTree = (TTree*) f->Get("eventTree");
  if(probe == 1 ) oniumTree = (TTree*) f->Get("openTree");
  else if(probe == -1 ) oniumTree = (TTree*) f->Get("hardTree");
  else oniumTree = (TTree*) f->Get("oniumTree");

  
  // set aliases
   defineAliases(eventTree, oniumTree);
           
    
    eventTree->SetAlias("mul", multiplicityEstimator.Data());
    
    
    eventTree->SetAlias("sel", multiplicitySelector.Data());
    oniumTree->SetAlias("sel", multiplicitySelector.Data());
           
  
  
TString  eventcut;
  switch(diffractive){
    case -1 :
      eventcut = inel0 ? "inel0 && !diffractive" : "!diffractive" ;
      break;
    case 1:
      eventcut = inel0 ? "inel0 &&  diffractive" : "diffractive" ;
      break;
   default:
      eventcut = inel0 ? "inel0" : "1/1"  ;
      break;
  }
  
  
  eventTree->SetAlias( "eventcut",eventcut.Data() );
  oniumTree->SetAlias( "eventcut",eventcut.Data() );
  
  
  TString scaling = "1";
  
  if(probe == 0){
    
    int threshold2 = 350;
    if(noMPI) threshold2 = 120;
    else if(noCR) threshold2 = 550;
    scaling = Form( "  (1+9*(mult<100)) * (1+99*(mult<%d))   ", threshold2 )  ;
    
    
    
  }
  
  eventTree->SetAlias( "scaling",scaling.Data() );
  
  
  
  TH2D* hMult = new TH2D("hMult", "hMult", 1000, -.5, 999.5, 1000, -.5, 999.5);
   hMult->Sumw2();
  hMult->SetDirectory(gDirectory);
  eventTree->Draw( "mul:sel>>hMult", "(eventcut)*(scaling)", "goff" );
  hMult->GetXaxis()->SetRange( 0, hMult->GetNbinsX() );

  meanMultMB = hMult->GetMean(2); 
  nEventsMB  = hMult->Integral();
  
  if(trigger){
    TString triggerString = trigger == 1 ? "335-334/(1+exp((multV0M_smeared-472)/14))" : "335-335/(1+exp((multV0M_smeared-472)/14))"; 
//     TString triggerString = "1+334000*(multV0M>150)"; 
    eventTree->SetAlias( "trigger",triggerString.Data() );
    oniumTree->SetAlias( "trigger",triggerString.Data() );
    eventTree->Draw( "mul:sel>>hMult", "(eventcut)*(scaling)*(trigger)", "goff" );
  }
  
  
//   hMult->SaveAs("hm.root");
  
  cout << "nEventsMB " << nEventsMB <<endl << "mean mult: " << meanMultMB<<endl;
  
  
  
  if(!nEventsMB)  crash( "nEventsMB == 0!!!" );
  if(!meanMultMB) crash( "meanMultMB == 0!!!" );
  for(unsigned int i=0; i< nMultiplicityBins-1; ++i) {

      hMult->GetXaxis()->SetRangeUser( multiplicityBins[i], multiplicityBins[i+1] );
      meanMultMBPerBin[i] = hMult->GetMean(2);
      int binMin = hMult->GetXaxis()->FindBin( multiplicityBins[i]  );
      int binMax = hMult->GetXaxis()->FindBin( multiplicityBins[i+1] ) -1;
      nEventsPerBin[i] =  hMult->IntegralAndError( binMin, binMax, 0, 1000, errPerBin[i]   );
  }


    TString commonCut, commonName, variationsName, header, panel, inclusiveName;

    int legendsX[6]  = {0,0,1,1,0,0};
    int legendsY[6]  = {0,0,0,0,0,0};
    bool plotMult = true;
    bool plotPt = true;
    bool plotInclusive = false;
    bool plotY = false;
    bool drawSqrt = false;
    vector<Variation> variations;
    
    defineVariation(
      plotWhat,  &commonName, &variationsName, &commonCut, &header, &panel, &inclusiveName, legendsX,legendsY,
      &plotMult, &plotPt,&plotInclusive, &plotY, &drawSqrt, &variations
    );
    
    double maxX = normalization < 1 ? 30 :  7;
    
    // lf
//     double maxX = normalization < 1 ? 30 :  10;
    
    if( plotWhat == 0 && noMPI ) legendsY[1] = 1;
    
    
    if(signalRapidity==1){
      commonCut += "&&midRapidity";
      header += probe==-1 ? " (|#it{#eta}|<0.9)": " (|#it{y}|<0.9)";
      commonName += "Mid";
      rapIntervalSignal = 1.8;
    }
    if(signalRapidity==2){
      commonCut += "&&muonRapidity";
      header += " (2.5<|#it{y}|<4.0)";
      commonName += "Muons";
      rapIntervalSignal = 3.;
    }
    if(signalRapidity==3){
      commonCut += "&&muonRapidityPos";
      header += " (2.5<#it{y}<4.0)";
      commonName += "MuonsFwd";
      rapIntervalSignal = 3.;
    }
    if(signalRapidity==4){
      commonCut += "&&muonRapidityNeg";
      header += " (-2.5>#it{y}>-4.0)";
      commonName += "MuonsBkwd";
      rapIntervalSignal = 3.;
    }
    else{
      rapIntervalSignal = 19.;
    }
    if(decayChannel==1){
      commonCut += "&&dilepton";
      header += "#rightarrow l^{+}l^{-} ";
      commonName += "Dilepton";
    }
 if (probe==1) commonName += "_Dmesons_";   
 else if (probe==-1) commonName += "_LF_";   
    
    
  plot( commonCut, commonName, header, panel, inclusiveName, variationsName, variations, plotMult, normalization, plotPt, plotInclusive, plotY, legendsX,legendsY, maxY , maxX, drawQuad, drawSqrt );

}

void plot ( TString commonCut, TString commonName, TString header, TString panel, TString inclusiveName, TString variationsName, vector<Variation> variations, bool plotMult, int normalization,  bool plotPt,  bool plotInclusive, bool plotY, int* legendsX, int* legendsY, double maxY, double maxX, bool drawQuad, bool drawSqrt){

  cout << commonName << " vs. " << variationsName << endl;
  
  TString title;
  
  TString diffractiveString = "";
  if     (diffractive ==  1){
    diffractiveString = "diffractive";
    title +="diffractive events";
    
  }
  else if(diffractive == -1){
    diffractiveString = "nondiffractive";
    title += "nondiffractive events";
  }
  else{
    title += "inelastic events";
  }
  
  
  TString inelString;
  if(inel0){
    inelString += "INEL0";
    title += ", INEL>0"; 
  }
  
  if( noCR ){
    title += ", CR off";
  }
  
  if( noMPI ){
    title += ", MPI off"; 
  }
  
  
  TString folder1 = Form("%s%s%s",settings.Data(), diffractiveString.Data(), inelString.Data() );
  TString identifier = Form("%s_%s_%s",folder1.Data(), multiplicitySelector.Data(), multiplicityEstimator.Data() );
  TString probeString = "";
  if(probe == 1) probeString = "D";
  else if(probe == -1) probeString ="LF";
  
  
  
  gSystem->mkdir(  Form("output%s/%s", probeString.Data(), folder1.Data() )  );
  gSystem->mkdir(  Form("output%s/%s/%s_%s", probeString.Data(),folder1.Data(),multiplicitySelector.Data(),  multiplicityEstimator.Data() )  );
  TString subfolder = Form( "%sVS%s", commonName.Data(), variationsName.Data()  );
  
  if(trigger==1) subfolder +="_both";
  else if(trigger==2) subfolder +="_HM";
  else if(trigger==0) subfolder +="_MB";
  
  TString folder = Form("output%s/%s/%s_%s/%s", probeString.Data(),folder1.Data(),  multiplicitySelector.Data(),  multiplicityEstimator.Data(), subfolder.Data() );
  gSystem->mkdir(  folder.Data() ); 
  
  
  TString fn_add = Form("%s_%s", identifier.Data(),  subfolder.Data() );
  
  
  TCanvas *cYieldVsMult    = plotMult ? makeCanvas ( "yieldVsMult" ) : 0x0;
  TCanvas *cRatioToLin     = plotMult && normalization==2 ? makeCanvas ( "ratioToLin" ) : 0x0;
//   TCanvas *cRatioToQuad    = drawQuad ? makeCanvas ( "ratioToQuad" ) : 0x0;
  
  TCanvas *cYieldVsPt      = plotPt ? makeCanvas ( "yieldVsPt" , false, true ) : 0x0;
  TCanvas *cYieldVsY       = plotY ? makeCanvas ( "yieldVsY" ) : 0x0;
  
  TCanvas *cFractionVsMult = plotMult ? makeCanvas ( "fractionVsMult" ) : 0x0;
  TCanvas *cFractionVsPt   = plotPt ? makeCanvas ( "fractionVsPt" ) : 0x0;
  TCanvas *cFractionVsY    = plotY ? makeCanvas ( "fractionVsY" ) : 0x0;
  
  TLegend *lmy = makeLegend( header, legendsX[0], legendsY[0] );
  TLegend *lmf = makeLegend( header, legendsX[1], legendsY[1] );
  TLegend *lpy = makeLegend( header, legendsX[2], legendsY[2] );
  TLegend *lpf = makeLegend( header, legendsX[3], legendsY[3] );
  TLegend *lyy = makeLegend( header, legendsX[4], legendsY[4] );
  TLegend *lyf = makeLegend( header, legendsX[5], legendsY[5] );
  
  
  TH1D *hJpsiVsMultTotal = 0x0;
  TH1D *hJpsiVsRawMultTotal = 0x0;
  TH1D *hJpsiVsPtTotal   = 0x0;
  TH1D *hJpsiVsYTotal   = 0x0;
  
  TString linearString;
  if(normalization == 2) linearString ="x";
  else  linearString =  Form("x/%f", meanMultMB  );
  
  
  
  TString quadraticString;
  if(normalization == 2) quadraticString =".5*x*x";
  else quadraticString =  Form(".5*x*x/%f/%f", meanMultMB, meanMultMB  );
  
  TString sqrtString;
  if(normalization == 2) sqrtString ="-1.3+2.5*sqrt(x)";
  else sqrtString =  Form("-1.3+2.5*sqrt(x/%f)", meanMultMB  );
  
  
  
    TF1* linear = new TF1("linear",  linearString.Data() , 0., maxX);
    TF1* quadratic = new TF1("quadratic", quadraticString.Data(), 0., maxX);
    TF1* sqrt = new TF1("sqrt", sqrtString.Data(), 0., maxX);
  
  if(plotMult){
   
  
    styleFunction( linear, kBlack, 2, title, nCh, nJpsi, 0., maxX, 0., maxY );
    drawInCanvas( cYieldVsMult, linear, "L");
    
    if(drawQuad){
        styleFunction( quadratic, kBlack, 5, title, nCh , nJpsi,0., maxX,0., maxY );
        drawInCanvas( cYieldVsMult, quadratic, "same");
    }
    
    if(drawSqrt){
        styleFunction( sqrt, kBlack, 7, title, nCh , nJpsi,0., maxX,0., maxY );
        drawInCanvas( cYieldVsMult, sqrt, "same");
    }

    
    if( normalization == 2){
      TF1* flat = new TF1("flat", "1", 0., maxX);
      styleFunction( flat,   kBlack, 2, title, nCh , "ratio to lin",0., maxX,0., 2. );
      drawInCanvas( cRatioToLin, flat, "L" );

    }
    
    TF1* nul = new TF1("nul", "0", 0., maxX);
    styleFunction( nul,   kWhite, 2, title, nCh , "Fraction of total yield",0., maxX,0., 1. );
    drawInCanvas( cFractionVsMult, nul, "L" );
          
  }
  
    TF1* onePt = new TF1("onePt", "0", 0., maxPt);
    TF1* nullPt = new TF1("nullPt", "0", 0., maxPt);
  if(plotPt){
    styleFunction( nullPt,   kWhite, 2, title,"#it{p}_{T} (GeV/#it{c})", "Fraction of total yield", 0., maxPt,0., 1. );
    drawInCanvas( cFractionVsPt, nullPt, "L" );
    TF1* one = new TF1("one", "1", 0., maxPt);
    styleFunction( onePt,   kWhite, 2, title,"#it{p}_{T} (GeV/#it{c})", "yield( a.u.)", 0., maxPt, 0.01, 2. );
    drawInCanvas( cYieldVsPt, onePt, "L" );
  }
  
    TF1* nullY = new TF1("nullY", "0", -1.*maxRapidity, maxRapidity);
    TF1* oneY = new TF1("oneY", "1", -1.*maxRapidity, maxRapidity);
  if(plotY){
    styleFunction( nullY,   kWhite, 2, title,"y", "Fraction of total yield", -1.*maxRapidity, maxRapidity,0., 1.3 );
    drawInCanvas( cFractionVsY, nullY, "L" );
    styleFunction( oneY,   kWhite, 2, title,"y", "yield (a.u.)", -1.*maxRapidity, maxRapidity, 0., 1.1 );
    drawInCanvas( cYieldVsY, oneY, "L" );
  }
  
  
  
    TString normString;
    
    if(normalization == 0 ) normString = "Raw";
    else if(normalization == 1 ) normString = "dy";
    else if(normalization == -1 ) normString = "diff";
  
  
    TFile out( Form( "%s/output%s.root", folder.Data(), normString.Data() ), "RECREATE");
  
  
  
  double normalizationMult = 1;
  if (normalization ==1 ) normalizationMult = rapInterval;
  else if(normalization == 2 || normalization == -1 ) normalizationMult = meanMultMB;
  
  for( int iCut = -1; iCut < (int) variations.size(); ++iCut ){
    
    
    
    bool cutBin   = iCut > -1;
    
    TString cut      = cutBin ? variations[iCut].cut   : "1" ;
    TString name     = cutBin ? variations[iCut].name  : inclusiveName;
    Color_t color   = cutBin ? variations[iCut].color : 1;
    int style       = cutBin ? variations[iCut].style : 20;
    int lineStyle    = 1;
    TString  optionY = "PLsame";
    TString  optionF = "PLsame";
    
    
    cout << "  " << name << endl;

    bool noname =  (name == "" ); 
    

    TGraphErrors* gFractionVsMult = new TGraphErrors();
    TGraphErrors* gFractionVsPt   = new TGraphErrors();
    TGraphErrors* gFractionVsY   = new TGraphErrors();
    
    TGraphErrors* gYieldVsMult    = new TGraphErrors();
    TGraphErrors* gRatioToLin     = new TGraphErrors();
    TGraphErrors* gRatioToQuad    = new TGraphErrors();
    TGraphErrors* gYieldVsPt      = new TGraphErrors();
    TGraphErrors* gYieldVsY       = new TGraphErrors();
    
    gDirectory->Delete("hJpsi");
    TH3D* hJpsi = new TH3D("hJpsi", "hJpsi",
                           nRapidityBins, -1. * maxRapidity, maxRapidity,
                           200,   0., 100.,
                           1000, -.5, 999.5
    );
    hJpsi->SetDirectory(gDirectory);
    oniumTree->Draw( "sel:onium.pt:onium.y>>hJpsi", Form("eventcut && (%s) && (%s)", commonCut.Data(), cut.Data()) , "goff" );
    
    if(plotMult){
      TH1D *hJpsiVsMult = hJpsi->ProjectionZ();
      double meanYield = hJpsiVsMult->Integral() / nEventsMB;
      
      
      if(trigger){
        oniumTree->Draw( "sel:onium.pt:onium.y>>hJpsi", Form("trigger * (eventcut && (%s) && (%s))", commonCut.Data(), cut.Data()) , "goff" );
        hJpsiVsMult = hJpsi->ProjectionZ();
      }
      
      
      
      hJpsiVsMult = (TH1D*) hJpsiVsMult->Rebin( nMultiplicityBins-1, "jpsiNewBins", multiplicityBins );
      
      if( !cutBin ) {
        hJpsiVsMultTotal = (TH1D*) hJpsiVsMult->Clone();
        hJpsiVsMultTotal->SetDirectory(0);
      }
      
//       cout << "nJpsi " <<  hJpsiVsMult->Integral() << "  nEvts  " << nEventsMB <<  "   mean yield " <<  meanYield <<  endl;
      if ( !meanYield ) {
        cout <<  "meanYield == 0 !!!" << endl;
        continue;
      }
      
      
      double normalizationSignal = 1e-3 ;
      if (normalization ==1 ) normalizationSignal = rapIntervalSignal;
      else if(true || normalization == 2) normalizationSignal = meanYield;
      
//       normalizationSignal = meanYield;
      
      for( unsigned int iMultBin=0;  iMultBin < nMultiplicityBins-1; ++iMultBin )  {
        
//         if ( !nEventsPerBin[iMultBin] ) crash( Form("nEventsPerBin[%d] == 0!!! ", iMultBin) );
        if ( !nEventsPerBin[iMultBin] ) {
          cout << ( Form("nEventsPerBin[%d] == 0!!! ", iMultBin) ) << endl;
          nEventsPerBin[iMultBin] = 1;
        }

        double mult =  normalization != -1
         ?  meanMultMBPerBin[iMultBin] / normalizationMult
         : ( meanMultMBPerBin[iMultBin] - normalizationMult) * rapInterval;
        
        // error calculation.
        // assumption: mean yield has no statistical uncertainty
        
        double nJpsi  = hJpsiVsMult->GetBinContent(iMultBin+1);
        double eJpsi = hJpsiVsMult->GetBinError(iMultBin+1);
        double nEv  = nEventsPerBin[iMultBin];
        double eEv = errPerBin[iMultBin];
        
        double yield      = nJpsi / nEv ;
        double yieldError = TMath::Sqrt(
          TMath::Power( eJpsi / nEv, 2 ) + 
          TMath::Power( eEv * nJpsi / nEv / nEv, 2 ) );
        
        
        
        
        gYieldVsMult->SetPoint(      iMultBin, mult , yield / normalizationSignal    );
        gYieldVsMult->SetPointError( iMultBin, 0.  , yieldError / normalizationSignal );
        
        
//         cout << "mult, yield: "<< iMultBin << "  "<< meanMultMBPerBin[iMultBin] << "  " << yield << endl;
        
        
        
//         cout << "setting point: " << mult << "  " << (yield / normalizationSignal)<< endl;
        
        if( normalization == 2 &&  mult){
          gRatioToLin->SetPoint(      iMultBin, mult , yield / normalizationSignal /mult      );
          gRatioToLin->SetPointError( iMultBin, 0.  , yieldError / normalizationSignal /mult );
          
          gRatioToQuad->SetPoint(      iMultBin, mult , yield/ normalizationSignal / mult  /mult    );
          gRatioToQuad->SetPointError( iMultBin, 0.  , yieldError / normalizationSignal /mult /mult );
        }
        
          
        if(cutBin){
          double total = hJpsiVsMultTotal->GetBinContent(iMultBin+1);
          double fraction      = total ? (hJpsiVsMult->GetBinContent(iMultBin+1) / total) : 0;
          double fractionError = total ? (hJpsiVsMult->GetBinError(iMultBin+1)   / total) : 1;
          
          
//           1. - 2.* b1 / b2) * e1sq  + b1sq * e2sq / b2sq ) / b2sq 
          
          
          
          gFractionVsMult->SetPoint( iMultBin,mult ,fraction  );
          gFractionVsMult->SetPointError( iMultBin, 0. ,  fractionError   );
          
        }
      }
      
      
      
      
      if(cutBin){
        
        stylePlot( gFractionVsMult, color, style,lineStyle, title, nCh, "Fraction of total yield", 0., maxX, 0., 1.);
        drawInCanvas( cFractionVsMult, gFractionVsMult, optionF);
        if(!noname) lmf->AddEntry( gFractionVsMult, name.Data(), "LP" );
      }
        
       if( cutBin || plotInclusive){
        stylePlot( gYieldVsMult, color, style,lineStyle, title,   nCh ,               nJpsi,0., maxX,0., maxY );
        drawInCanvas( cYieldVsMult, gYieldVsMult, optionY);
        
        if(normalization ==2){
          stylePlot( gRatioToLin, color, style,lineStyle, title,   nCh , "ratio to lin",0., maxX,0., 2. );
          drawInCanvas( cRatioToLin, gRatioToLin, optionY);
        }
//         if(drawQuad){
//           stylePlot( gRatioToQuad, color, style,lineStyle, title,   nCh , "ratio to quad", 0., maxX,0., 5. );
//           drawInCanvas( cRatioToQuad, gRatioToQuad, optionY);
//         }
        
        if(!noname) lmy->AddEntry( gYieldVsMult, name.Data() , "LP" );
      }
      else{
        stylePlot( gYieldVsMult, kWhite, style,lineStyle, title,   nCh ,  nJpsi,0., maxX,0., maxY );
      }

    }
    
    
    
    if(plotPt){

      TH1D *hJpsiVsPt = hJpsi->ProjectionY();
      
      if( !cutBin ) {
        hJpsiVsPtTotal   = (TH1D*) hJpsiVsPt->Clone();
        hJpsiVsPtTotal->SetDirectory(0);
      }
      int iPoint = 0;
      for( unsigned int iBin=0; iBin < nPtBins; ++iBin){
        double maxYieldVsPt = hJpsiVsPt->GetMaximum();
        double yield      = maxYieldVsPt ? hJpsiVsPt->GetBinContent(iBin+1) / maxYieldVsPt : 0.;
        double yieldError = maxYieldVsPt ? hJpsiVsPt->GetBinError(iBin+1)   / maxYieldVsPt : 0.;
        double pt = hJpsiVsPt->GetBinCenter(iBin+1) ;
        
        if(yield){ 
          gYieldVsPt->SetPoint( iPoint, pt  ,yield  );
          gYieldVsPt->SetPointError( iPoint, 0. ,  yieldError   );
        
          if(cutBin){
            double total =  hJpsiVsPtTotal->GetBinContent(iBin+1);
            double fraction      = total ? hJpsiVsPt->GetBinContent(iBin+1) / total : 0 ;
            double fractionError = total ? hJpsiVsPt->GetBinError(iBin+1)   / total  : 1;
          
            gFractionVsPt->SetPoint( iPoint, pt ,fraction  );
            gFractionVsPt->SetPointError( iPoint, 0. ,  fractionError   );
          }
          ++iPoint;
        }
      }
      
      stylePlot( gYieldVsPt,   color, style,lineStyle, title, "#it{p}_{T} (GeV/c)", "yield (a.u.)", 0., maxPt);
      drawInCanvas( cYieldVsPt, gYieldVsPt, optionY);
      if(!noname) lpy->AddEntry( gYieldVsPt,   name.Data() , "LP" );
      
      if(cutBin){
        stylePlot( gFractionVsPt,   color, style,lineStyle, title, "#it{p}_{T} (GeV/c)", "Fraction of total yield", 0., maxPt,0., 1.);
        drawInCanvas( cFractionVsPt, gFractionVsPt, optionF);
        if(!noname) lpf->AddEntry( gFractionVsPt,   name.Data(), "LP" );
      }
      
    }


    if(plotY){

      TH1D *hJpsiVsY = hJpsi->ProjectionX();
      
      if( !cutBin ) hJpsiVsYTotal   = (TH1D*) hJpsiVsY->Clone();
      
      int iPoint = 0;
      for( unsigned int iBin=0; iBin < nRapidityBins; ++iBin){
        double maxYieldVsY = hJpsiVsY->GetMaximum();
        double yield      = maxYieldVsY ? hJpsiVsY->GetBinContent(iBin+1) /maxYieldVsY : 0.;
        double yieldError = maxYieldVsY ? hJpsiVsY->GetBinError(iBin+1) /maxYieldVsY : 0.;
        double y = hJpsiVsY->GetBinCenter(iBin+1) ;
        
        if(yield){ 
          gYieldVsY->SetPoint( iPoint, y  ,yield  );
          gYieldVsY->SetPointError( iPoint, 0. ,  yieldError   );
          
          if(cutBin){
            double total =  hJpsiVsYTotal->GetBinContent(iBin+1);
            double fraction      = total ? hJpsiVsY->GetBinContent(iBin+1) / total : 0 ;
            double fractionError = total ? hJpsiVsY->GetBinError(iBin+1)   / total  : 1;
          
            gFractionVsY->SetPoint( iPoint, y ,fraction  );
            gFractionVsY->SetPointError( iPoint, 0. ,  fractionError   );
          }
          ++iPoint;
        }
      }
      
      stylePlot( gYieldVsY,   color, style,lineStyle, title, "y", "yield (a.u.)", -6., 6.);
      drawInCanvas( cYieldVsY, gYieldVsY, optionY);
      if(!noname) lyy->AddEntry( gYieldVsY,   name.Data() , "LP" );
      
      if(cutBin){
        stylePlot( gFractionVsY,   color, style,lineStyle, title, "y", "Fraction of total yield", -6., 6., 0., 1.);
        drawInCanvas( cFractionVsY, gFractionVsY, optionF);
        if(!noname) lyf->AddEntry( gFractionVsY,   name.Data(), "LP" );
      }
      
    }

    
    
    
    
    
      gFractionVsMult->Write( Form("fractonVsMult_%d", iCut) );
      gFractionVsPt  ->Write( Form("fractionVsPt_%d", iCut) );
      gFractionVsY   ->Write( Form("fractionVsY_%d", iCut) );
      
      gYieldVsMult   ->Write( Form("yieldVsMult_%d", iCut) );
      gRatioToLin    ->Write( Form("ratioToLin_%d", iCut) );
//       if(drawQuad)  gRatioToQuad   ->Write( Form("ratioToQuad_%d", iCut) );
      gYieldVsPt     ->Write( Form("yieldVsPt_%d", iCut) );
      gYieldVsY      ->Write( Form("yieldVsY_%d", iCut) );
     
    
    
    
    
  }
  
  
  
  if(true || normalization )  lmy->AddEntry( linear, "linear" , "L" ) ;
  if(drawQuad)  {
    lmy->AddEntry( quadratic, "quadratic" , "L" ) ;
    quadratic->Write("quadratic");
  }
  if(drawSqrt)  {
    lmy->AddEntry( sqrt, "sqrt" , "L" ) ;
    sqrt->Write("sqrt");
  }
  linear->Write("linear");
  if(plotPt){
    nullPt->Write("nullPt");
    onePt->Write("onePt");
  }
  if(plotY){
    nullY->Write("nullY");
    oneY->Write("oneY");
  }
  if(panel != ""){
    lmy->AddEntry("aaa",panel.Data(),"");
    
  }
  
  
  lmy->Write("legend");
  lpy->Write("legend2");
  out.Close();
  
  
  if(plotMult){
    drawInCanvas( cYieldVsMult, lmy);
    if(normalization==2) drawInCanvas( cRatioToLin, lmy); 
//     if(drawQuad)drawInCanvas( cRatioToQuad, lmy);
    drawInCanvas( cFractionVsMult, lmf);
    
    save( cYieldVsMult, folder,   Form("yieldVsMult%s_%s",  normString.Data(), fn_add.Data() ) );
    save( cFractionVsMult, folder,   Form("fractionVsMult%s_%s",  normString.Data(), fn_add.Data()   ) );
    if(normalization==2) save( cRatioToLin, folder,   Form("ratioToLin%s_%s",  normString.Data(), fn_add.Data()  ) );
//    if(drawQuad)save( cRatioToQuad,   folder,   Form("ratioToQuad%s_%s",  normString.Data(), fn_add.Data()  ) );
  }
  if(plotPt){
    drawInCanvas( cYieldVsPt, lpy);
    drawInCanvas( cFractionVsPt, lpf);
    save( cYieldVsPt, folder,  Form("yieldVsPt_%s", fn_add.Data() ) );
    save( cFractionVsPt, folder, Form("fractionVsPt_%s", fn_add.Data() ) );
  }
  if(plotY){
    drawInCanvas( cYieldVsY, lyy);
    drawInCanvas( cFractionVsY, lyf);
    save( cYieldVsY, folder , Form("yieldVsY_%s", fn_add.Data() ) );
    save( cFractionVsY, folder, Form("fractionVsY_%s", fn_add.Data() ) );
  }
}


void save( TCanvas * c, TString folder, TString filename ){
  c->SaveAs( Form(  "%s/%s.pdf" , folder.Data(), filename.Data() ) );
  c->SaveAs( Form(  "%s/%s.png" , folder.Data(), filename.Data() ) );
//   c->SaveAs( Form(  "%s/%s.root" , folder.Data(), filename.Data() ) );
}



void stylePlot( TGraphErrors * g, Color_t color, int style, int lineStyle, TString title, TString xtitle, TString ytitle, double xMin, double xMax, double yMin, double yMax ){
 
  g->SetLineColor( color  );
  g->SetMarkerColor( color );
  g->SetMarkerStyle( style );
  g->SetMarkerSize( 1.4 );
//   g->SetMarkerSize( 2 );
  g->SetLineStyle( lineStyle );
  g->SetLineWidth( 2  );
  
//   g->SetTitle(title.Data());
  
  
  g->SetTitle( Form("PYTHIA 8.230, pp #sqrt{#it{s}} = %1.0f TeV, %s", sqrts, title.Data() ) );
  g->GetXaxis()->SetTitle(xtitle.Data());
  g->GetYaxis()->SetTitle(ytitle.Data());
  g->GetXaxis()->SetTitleOffset(1.5);
  g->GetYaxis()->SetTitleOffset(1.2);
  g->GetXaxis()->SetTitleSize(0.055);
  g->GetYaxis()->SetTitleSize(0.055);
  g->GetXaxis()->SetLabelSize(0.05);
  g->GetYaxis()->SetLabelSize(0.05);
  
  if(xMax) g->GetXaxis()->SetRangeUser(xMin, xMax);
  if(yMax) g->GetYaxis()->SetRangeUser(yMin, yMax);
}



void styleFunction( TF1 * g, Color_t color, int lineStyle, TString title, TString xtitle, TString ytitle, double xMin, double xMax, double yMin, double yMax ){
 
  g->SetLineColor( color  );
  g->SetLineStyle( lineStyle );
  g->SetLineWidth( 2  );
  
  g->SetTitle( Form("PYTHIA 8.230, pp #sqrt{#it{s}} = %1.0f TeV, %s", sqrts, title.Data() ) );
  g->GetXaxis()->SetTitle(xtitle.Data());
  g->GetYaxis()->SetTitle(ytitle.Data());
  g->GetXaxis()->SetTitleOffset(1.5);
  g->GetYaxis()->SetTitleOffset(1.2);
  g->GetXaxis()->SetTitleSize(0.055);
  g->GetYaxis()->SetTitleSize(0.055);
  g->GetXaxis()->SetLabelSize(0.05);
  g->GetYaxis()->SetLabelSize(0.05);
  if(xMax) g->GetXaxis()->SetRangeUser(xMin, xMax);
  if(yMax) g->GetYaxis()->SetRangeUser(yMin, yMax);
}

TCanvas * makeCanvas( TString name , bool logx , bool logy ){
 
  TCanvas * c = new TCanvas( name.Data(), name.Data(), 800, 600   );
  
  c->SetTickx();
  c->SetTicky();
  
  c->SetTopMargin(0.08);
  c->SetRightMargin(0.02);
  
  c->SetLeftMargin(0.18);
  c->SetBottomMargin(0.2);
  if(logx) c->SetLogx();
  if(logy) c->SetLogy();
  return c;
  
}
TLegend * makeLegend( TString header, int xPos, int yPos){
  double x1, x2, y1, y2;
  
  switch (xPos) {
    case  1: x1 = .55; x2 = .8; break;
    case  2: x1 = .40; x2 = .80; break;
    case  3: x1 = .5; x2 = .9; break;
    default: x1 = .2; x2 = .65; break;
  }
  switch (yPos) {
    case  1: y1 = .45; y2 = .79; break;
    case  2: y1 = .21; y2 = .55; break;
    case  3: y1 = .45; y2 = .89; break;
    case  4: y1 = .65; y2 = .89; break;
    case  5: y1 = .35; y2 = .65; break;
    default: y1 = .55; y2 = .89; break;
  }
  TLegend * l = new TLegend( x1, y1, x2, y2);
  l->SetHeader(header.Data());
  l->SetTextSize(.05);
  l->SetBorderSize(0);
  l->SetFillColor(0); 
  l->SetFillStyle(0);
  return l;
}

void crash(TString message){
  cout << message << endl;
  exit(1);
}

void drawInCanvas( TCanvas* c, TObject* o, TString option){
  c->cd();
  o->Draw( option );
}






TString normalized(TString str){
   return "#frac{"  + str + "}{#LT" + str + "#GT}";
}

TString subtracted(TString str){
   return str + " - #LT" + str + "#GT";
}
