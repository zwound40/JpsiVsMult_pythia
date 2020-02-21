/**
 * 
 * This macro generates events with PYTHIA, and stores information about
 * the events, about produced quarkonia, D-meson, and/or high pT particles
 * in TTree's that it writes into a root file.
 * 
 *
 * General structure:
 * 
 * 
 *
 *
 *
 * Details:
 * 
 * The code was written to be run on a computing farm (like GSI kronos)
 *
 *
 * 
 * 
 * 
 * 
 * It takes arguments from the command line:
 *
 * argument 1:  number of events to produce (default: 10000)
 * argument 2:  whether or not to scale down the MB events (0=no, 1=yes) (default: 0)
 * argument 3:  whether to print out more information while running (default: 0)
 * argument 4:  the settings file to use (default: "settings.cmd")
 * argument 5:  the output filename (without the final .root) (default: "out")
 * argument 6:  an initial seed for the random number generator (default:1)
 * argument 7:  whether to search for and write out quarkonia (default: 1)
 * argument 8:  whether to search for and write out D mesons (default: 0)
 * argument 9:  whether to search for and write out high pT particles (default: 0)
 * argument 10: whether to write out the multiplicity in the different regions (default:0)
 * argument 11: whether to find out the production mechanism of the quarkonia or D-meson or
 *              high-pT particle (default:0) 
 * 
 */




#include <omp.h>
#include <time.h>
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TH2.h"
#include "TRandom3.h"
#include "Pythia8/Pythia.h"

using namespace Pythia8;

enum  QuarkoniumProcesses {
  kUndefined,                 // 0
  kgg2QQ,                     // 1
  kqq2QQ,                     // 2
  kgq2QQq,                    // 3
  kStringFragmentation,       // 4
 // kNonPrompt,                 // 5
  kNQuarkoniumProcesses       // 6
};
  
std::array<string,kNQuarkoniumProcesses> charmoniumProcessNames = {
  "Undefined",
  "gg2QQ",
  "qq2QQ",
  "gq2QQq",
  "string fragmentation"
 // "nonprompt"
};




enum HeavyQuarkProcesses {
  kGluonFusion = 1,              // 1
  kQuarkAnnihilation,            // 2
  kFlavorExcitation,             // 3
  kGluonFromHardProcess,         // 4
  kPhotonFromHardProcess,        // 5
  kGluonFromHeavyQuark,          // 6
  kGluonFromDecay,               // 7
  kGluonFromISR,                 // 8
  kGluonFromFSR,                 // 9
  kGluonFromIFSR,                // 10
  kPhotonFromISR,                // 11
  kPhotonFromFSR,                // 12
  kPhotonFromIFSR,               // 13
  kBeamRemnant,                  // 14
  kBeamRemnantToHardProcess,     // 15
  kFromOnium,                    // 16
  kNHeavyQuarkProcesses          // 17
};

std::array<string,kNHeavyQuarkProcesses> heavyQuarkProcessNames = {
  "Undefined",
  "gluon fusion",
  "quark annihilation",
  "flavor excitation",
  "gluon from hard process",
  "photon from hard process",
  "gluon from heavy quark",
  "gluon from decay",
  "gluon from ISR",
  "gluon from FSR",
  "gluon from ISR + FSR",
  "photon from ISR",
  "photon from FSR",
  "photon from ISR + FSR",
  "beam remnant",
  "beam remnant to hard process",
  "from quarkonium decay"
};


struct HardProbe{
  int pdg;
  float pt,y;
  unsigned short multRegion1;
  unsigned short multRegion2;
  unsigned short multRegion3;
  unsigned short multRegionRnd;
  unsigned short multEta1Region1;
  unsigned short multEta1Region2;
  unsigned short multEta1Region3;
  unsigned short multEta1RegionRnd;
  unsigned short multV0ARegion1;
  unsigned short multV0ARegion2;
  unsigned short multV0ARegion3;
  unsigned short multV0ARegionRnd;
  unsigned short multV0CRegion1;
  unsigned short multV0CRegion2;
  unsigned short multV0CRegion3;
  unsigned short multV0CRegionRnd;
  HardProbe():
    pdg(0),
    pt(0.),
    y(0.),
    multRegion1(0),
    multRegion2(0),
    multRegion3(0),
    multRegionRnd(0),
    multEta1Region1(0),
    multEta1Region2(0),
    multEta1Region3(0),
    multEta1RegionRnd(0),
    multV0ARegion1(0),
    multV0ARegion2(0),
    multV0ARegion3(0),
    multV0ARegionRnd(0),
    multV0CRegion1(0),
    multV0CRegion2(0),
    multV0CRegion3(0),
    multV0CRegionRnd(0)
    {};
};



struct HeavyProbe: HardProbe{
  unsigned char mother1process, mother2process;
  bool mother1mpi, mother2mpi;
  int initialPdg;
  HeavyProbe():
    HardProbe(),
    mother1process( kUndefined ),
    mother2process( kUndefined ),
    mother1mpi( false ),
    mother2mpi( false ),
    initialPdg( 0 )
    {};
};



struct Quarkonium: HeavyProbe{
  int decayChannel;
  unsigned char process;
  bool sameAncestor;
  unsigned short iMPI;
  Quarkonium():
    HeavyProbe(),
    decayChannel(0),
    process( kUndefined ),
    sameAncestor( false ),
    iMPI(0)
    {};
};




struct Quark{
  unsigned char process;
  bool mpi;
  unsigned short ancestor;
  Quark():
  process( kUndefined ),
  mpi( false ),
  ancestor(0) {};
};

struct Gluon{
  unsigned char process;
  bool mpi;
  unsigned short id;
  Gluon():
  process(kUndefined),
  mpi( false ),
  id(0){};
};

void traceBackQuarkonium( Quarkonium &found, unsigned short particle , Pythia &pythia);
void traceBackOpenHeavyFlavor( HeavyProbe &found, unsigned short particle , Pythia &pythia);
Quark traceBackQuark( unsigned short particle , Pythia &pythia);
Gluon traceBackGluon( unsigned short particle , Pythia& pythia, unsigned short dExcl, bool rigorously );
Gluon traceForwardGluon( unsigned short particle , Pythia &pythia,  unsigned short dExcl, bool rigorously );
bool isOnium(int pdg);
bool isOniumOctet(int pdg);
bool isDmeson(int pdg);
bool isCbaryon(int pdg);
bool isChadron( int pdg );
bool isBmeson(int pdg);
bool isBbaryon(int pdg);
bool isBhadron( int pdg );

void printParticle( Particle * part);
void getInformation( Particle *  part,
                     Pythia &pythia,
                     unsigned int &status,
                     unsigned int &pdg,
                     unsigned int &m1,
                     unsigned int &m2,
                     unsigned int &d1,
                     unsigned int &d2,
                     unsigned int &s,
                     unsigned int &pdg_m1,
                     unsigned int &pdg_m2,
                     unsigned int &pdg_d1,
                     unsigned int &pdg_d2,
                     unsigned int &pdg_s  );

bool inV0Aacceptance( float eta );
bool inV0Cacceptance( float eta );
bool isParton( unsigned short idx, Pythia &pythia );
bool isPrimaryChargedALICE( unsigned short idx , Pythia &pythia);
bool isLongLived( unsigned int pdg);
void fillRegions( unsigned short &multRegion1, unsigned short &multEta1Region1, unsigned short &multV0ARegion1, unsigned short &multV0CRegion1,
                  unsigned short &multRegion2, unsigned short &multEta1Region2, unsigned short &multV0ARegion2, unsigned short &multV0CRegion2,
                  unsigned short &multRegion3, unsigned short &multEta1Region3, unsigned short &multV0ARegion3, unsigned short &multV0CRegion3,
                  double phi, double eta);
void fillRegion( unsigned short &multRegion, unsigned short &multEta1Region, unsigned short &multV0ARegion, unsigned short &multV0CRegion,
                  double phi, double eta);
void fillRegionsForHardProbe( HardProbe &found, Pythia& pythia, double phi, double phi_rnd);

unsigned short totalCallsPerEvent;
const unsigned short maxCalls = 10000000000;
int verbose;

int main( int argc, char** argv ){

  int nev            = argc > 1 ? std::stoi(argv[1]) : 10000;
  int scaleDown      = argc > 2 ? std::stoi(argv[2]) : 0;
  verbose            = argc > 3 ? std::stoi(argv[3]) : 0;
  string settings    = argc > 4 ? argv[4]            : "settings.cmnd";  
  string outfilename = argc > 5 ? argv[5]            : "out"; 
  int taskid         = argc > 6 ? std::stoi(argv[6]) : 1;
  int writeOnia      = argc > 7 ? std::stoi(argv[7]) : 1;
  int writeOpen      = argc > 8 ? std::stoi(argv[8]) : 0;
  int writeHard      = argc > 9 ? std::stoi(argv[9]) : 0;
  int writeRegions   = argc >10 ? std::stoi(argv[10]): 0; 
  int traceBack      = argc >11 ? std::stoi(argv[11]): 0; 
  
  if(writeOnia) cout << "writing onia tree"<<endl;
  if(writeOpen) cout << "writing open tree"<<endl;
  if(writeHard) cout << "writing hard tree"<<endl;
  if(writeRegions) cout << "writing regions"<<endl;
  if(traceBack) cout << "tracing back origin"<<endl;
  

 std::size_t found = settings.find("noMPI");
 bool noMPI =  found !=std::string::npos;
 
 std::size_t found2 = settings.find("noCR");
 bool noCR =  found2 !=std::string::npos;

  gRandom = new TRandom3();

  
   ROOT::EnableThreadSafety();
  
  
#pragma omp parallel
{

  
  
  int thread = omp_get_thread_num(); 
  
  cout << thread << endl;

  Pythia pythia;


  unsigned short multRegionRnd;
  unsigned short multEta1RegionRnd;
  unsigned short multV0ARegionRnd;
  unsigned short multV0CRegionRnd;
  
  
  
  unsigned short  mult, multEta1, multV0A, multV0C, nMPI;
  unsigned char type;
  float ptHat;
  float ptHat1;
  float b;
  
  
  string outfilename_thread = outfilename + std::to_string(thread);
  outfilename_thread += ".root";


   TFile *fout;
   TTree *eventTree;
   TTree *oniumTree, *openTree, *hardTree;
   
   
   #pragma omp critical
    { 
      fout = TFile::Open(  outfilename_thread.c_str(),"RECREATE", "", 9);
     
    }
    fout->cd();
   
      #pragma omp critical
    { 
      eventTree = new TTree( Form("eventTree", thread), "event information");
      if(writeOnia) oniumTree = new TTree( Form("oniumTree", thread), "Onia information");
      if(writeOpen) openTree  = new TTree( Form("openTree", thread), "D, B meson information");
      if(writeHard)  hardTree  = new TTree( Form("hardTree", thread), "high pT hadron information");
     }

  
  
{
  eventTree->Branch( "mult",                &mult );
  eventTree->Branch( "multEta1",            &multEta1 );
  eventTree->Branch( "multV0A",             &multV0A );
  eventTree->Branch( "multV0C",             &multV0C );
  eventTree->Branch( "type",                &type );
  eventTree->Branch( "nMPI",                &nMPI );
  eventTree->Branch( "ptHat",               &ptHat );
  eventTree->Branch( "ptHat1",               &ptHat1 );
  eventTree->Branch( "b",               &b );

  if( writeRegions ){
    eventTree->Branch( "multRegionRnd",              &multRegionRnd );
    eventTree->Branch( "multEta1RegionRnd",          &multEta1RegionRnd );
    eventTree->Branch( "multV0ARegionRnd",           &multV0ARegionRnd );
    eventTree->Branch( "multV0CRegionRnd",           &multV0CRegionRnd );
  }
}
  
  
  
  

   Quarkonium onium;
  if(writeOnia) {
    oniumTree->Branch( "mult",                  &mult );
    oniumTree->Branch( "multEta1",              &multEta1 );
    oniumTree->Branch( "multV0A",               &multV0A );
    oniumTree->Branch( "multV0C",               &multV0C );
    oniumTree->Branch( "nMPI",                  &nMPI );
    oniumTree->Branch( "ptHat",                 &ptHat );
    oniumTree->Branch( "type",                  &type );
    oniumTree->Branch( "b",                     &b );
    oniumTree->Branch( "onium.pdg",             &onium.pdg );
    oniumTree->Branch( "onium.decayChannel",    &onium.decayChannel );
    oniumTree->Branch( "onium.pt",              &onium.pt );
    oniumTree->Branch( "onium.y",               &onium.y );
    if(traceBack){
        oniumTree->Branch( "onium.initialPdg",      &onium.initialPdg );
        oniumTree->Branch( "onium.process",         &onium.process );
        oniumTree->Branch( "onium.mother1process",  &onium.mother1process );
        oniumTree->Branch( "onium.mother2process",  &onium.mother2process );
        oniumTree->Branch( "onium.mother1mpi",      &onium.mother1mpi );
        oniumTree->Branch( "onium.mother2mpi",      &onium.mother2mpi );
        oniumTree->Branch( "onium.sameAncestor",    &onium.sameAncestor );
        oniumTree->Branch( "onium.iMPI",            &onium.iMPI );
    }
    if( writeRegions ){
        oniumTree->Branch( "multRegion1",              &onium.multRegion1 );
        oniumTree->Branch( "multRegion2",              &onium.multRegion2 );
        oniumTree->Branch( "multRegion3",              &onium.multRegion3 );
        oniumTree->Branch( "multRegionRnd",            &onium.multRegionRnd );
        oniumTree->Branch( "multEta1Region1",          &onium.multEta1Region1 );
        oniumTree->Branch( "multEta1Region2",          &onium.multEta1Region2 );
        oniumTree->Branch( "multEta1Region3",          &onium.multEta1Region3 );
        oniumTree->Branch( "multEta1RegionRnd",        &onium.multEta1RegionRnd );
        oniumTree->Branch( "multV0ARegion1",           &onium.multV0ARegion1 );
        oniumTree->Branch( "multV0ARegion2",           &onium.multV0ARegion2 );
        oniumTree->Branch( "multV0ARegion3",           &onium.multV0ARegion3 );
        oniumTree->Branch( "multV0ARegionRnd",         &onium.multV0ARegionRnd );
        oniumTree->Branch( "multV0CRegion1",           &onium.multV0CRegion1 );
        oniumTree->Branch( "multV0CRegion2",           &onium.multV0CRegion2 );
        oniumTree->Branch( "multV0CRegion3",           &onium.multV0CRegion3 );
        oniumTree->Branch( "multV0CRegionRnd",         &onium.multV0CRegionRnd );
    }
   }


   HeavyProbe open;
   
  if(writeOpen) {
    openTree->Branch( "mult",                  &mult );
    openTree->Branch( "multEta1",              &multEta1 );
    openTree->Branch( "multV0A",               &multV0A );
    openTree->Branch( "multV0C",               &multV0C );
    openTree->Branch( "nMPI",                  &nMPI );
    openTree->Branch( "ptHat",                 &ptHat );
    openTree->Branch( "type",                  &type );
    openTree->Branch( "b",                     &b );
    openTree->Branch( "onium.pdg",             &open.pdg );
    openTree->Branch( "onium.pt",              &open.pt );
    openTree->Branch( "onium.y",               &open.y );
    if(traceBack){
        openTree->Branch( "onium.initialPdg",      &open.initialPdg );
        openTree->Branch( "onium.mother1process",  &open.mother1process );
        openTree->Branch( "onium.mother2process",  &open.mother2process );
        openTree->Branch( "onium.mother1mpi",      &open.mother1mpi );
        openTree->Branch( "onium.mother2mpi",      &open.mother2mpi );
    }
    if( writeRegions ){
        openTree->Branch( "multRegion1",              &open.multRegion1 );
        openTree->Branch( "multRegion2",              &open.multRegion2 );
        openTree->Branch( "multRegion3",              &open.multRegion3 );
        openTree->Branch( "multRegionRnd",            &open.multRegionRnd );
        openTree->Branch( "multEta1Region1",          &open.multEta1Region1 );
        openTree->Branch( "multEta1Region2",          &open.multEta1Region2 );
        openTree->Branch( "multEta1Region3",          &open.multEta1Region3 );
        openTree->Branch( "multEta1RegionRnd",        &open.multEta1RegionRnd );
        openTree->Branch( "multV0ARegion1",           &open.multV0ARegion1 );
        openTree->Branch( "multV0ARegion2",           &open.multV0ARegion2 );
        openTree->Branch( "multV0ARegion3",           &open.multV0ARegion3 );
        openTree->Branch( "multV0ARegionRnd",         &open.multV0ARegionRnd );
        openTree->Branch( "multV0CRegion1",           &open.multV0CRegion1 );
        openTree->Branch( "multV0CRegion2",           &open.multV0CRegion2 );
        openTree->Branch( "multV0CRegion3",           &open.multV0CRegion3 );
        openTree->Branch( "multV0CRegionRnd",         &open.multV0CRegionRnd );
    }
  }

  HardProbe highPtHadron;
   
  if(writeHard) {
    hardTree->Branch( "mult",                  &mult );
    hardTree->Branch( "multEta1",              &multEta1 );
    hardTree->Branch( "multV0A",               &multV0A );
    hardTree->Branch( "multV0C",               &multV0C );
    hardTree->Branch( "nMPI",                  &nMPI );
    hardTree->Branch( "ptHat",                 &ptHat );
    hardTree->Branch( "type",                  &type );
    hardTree->Branch( "onium.pdg",             &highPtHadron.pdg );
    hardTree->Branch( "onium.pt",              &highPtHadron.pt );
    hardTree->Branch( "onium.y",               &highPtHadron.y );
    hardTree->Branch( "b",                     &b );
    if(writeRegions){
        hardTree->Branch( "multRegion1",              &highPtHadron.multRegion1 );
        hardTree->Branch( "multRegion2",              &highPtHadron.multRegion2 );
        hardTree->Branch( "multRegion3",              &highPtHadron.multRegion3 );
        hardTree->Branch( "multRegionRnd",            &highPtHadron.multRegionRnd );
        hardTree->Branch( "multEta1Region1",          &highPtHadron.multEta1Region1 );
        hardTree->Branch( "multEta1Region2",          &highPtHadron.multEta1Region2 );
        hardTree->Branch( "multEta1Region3",          &highPtHadron.multEta1Region3 );
        hardTree->Branch( "multEta1RegionRnd",        &highPtHadron.multEta1RegionRnd );
        hardTree->Branch( "multV0ARegion1",           &highPtHadron.multV0ARegion1 );
        hardTree->Branch( "multV0ARegion2",           &highPtHadron.multV0ARegion2 );
        hardTree->Branch( "multV0ARegion3",           &highPtHadron.multV0ARegion3 );
        hardTree->Branch( "multV0ARegionRnd",         &highPtHadron.multV0ARegionRnd );
        hardTree->Branch( "multV0CRegion1",           &highPtHadron.multV0CRegion1 );
        hardTree->Branch( "multV0CRegion2",           &highPtHadron.multV0CRegion2 );
        hardTree->Branch( "multV0CRegion3",           &highPtHadron.multV0CRegion3 );
        hardTree->Branch( "multV0CRegionRnd",         &highPtHadron.multV0CRegionRnd );
    }
  }

  pythia.readFile( settings );
  pythia.readString("Random:setSeed = on");


  
  int seed = (time(NULL) * taskid * thread)  % 900000000;
  std::stringstream sstm;
  sstm <<  "Random:seed =" << seed;
  std::string seedString = sstm.str();
  pythia.readString(  seedString  );

//   cout << endl << "SEED=  " << seed <<endl << endl;
  
  pythia.init();


     for (int iev = 0; iev < nev; iev++) {
      if (!pythia.next()) continue;
      mult = 0; // mult in eta +- inf.
      multEta1 = 0; // mult in eta +-1.0
      multV0A = 0; // mult in V0A acceptance
      multV0C = 0; // mult in V0C acceptance
      
      multRegionRnd = 0; // mult in eta +- inf.
      multEta1RegionRnd = 0; // multEta1 
      multV0ARegionRnd = 0; // multV0A 
      multV0CRegionRnd = 0; // multV0C 
      
      type  = pythia.info.code();
      nMPI  =  pythia.info.nMPI();
      ptHat = 0;
      ptHat1 = 0;
    //if(type==105) pythia.event.list(0,0,0);
    //if(type==105) pythia.process.list(0,0,0);
    //cout << (int) type << "  " << pythia.info.pT2Hat() << endl;

      
      b = pythia.info.bMPI();
      if( nMPI>1){
        for(int iMPI=0; iMPI<nMPI; ++iMPI){
          // cout<< iMPI << "   " << pow(pythia.info.pTMPI(iMPI),2) << endl;  
          ptHat += pow(pythia.info.pTMPI(iMPI),2);
        }
      //ptHat *= ptHat;
      }
      else if(type ==103) ptHat = pythia.info.pT2Hat(1);
      else if(type ==104) ptHat = pythia.info.pT2Hat(2);
      else if(type ==105) ptHat = pythia.info.pT2Hat(1) + pythia.info.pT2Hat(2);
      else ptHat = pythia.info.pT2Hat();
    ptHat1 = pythia.info.pT2Hat();
    //if( i abs( ptHat - pythia.info.pT2Hat() ) > 1e-3  )
    //if(type==101 && ptHat < 1) cout << ptHat  << "    " << pythia.info.pT2Hat()  << "   " <<    pythia.info.pT2Hat(1)  << "  " << pythia.info.pT2Hat(2) << "  " <<   nMPI << "   " << (int)type <<  endl;

    //cout << endl;
      vector <Quarkonium> foundQuarkoniumPerEvent;
      vector <HeavyProbe> foundDPerEvent;
      vector <HardProbe> foundHighPtPerEvent;
        for ( int iPart = 0;  iPart < pythia.event.size(); iPart++) {
          Particle* part = &pythia.event[iPart];
          if( isPrimaryChargedALICE(iPart, pythia) ){
            ++mult;
            if( abs(  part->eta() ) < 1.0 ) ++ multEta1;
            else if( inV0Aacceptance( part->eta() ) ) ++multV0A;
            else if( inV0Cacceptance( part->eta() ) ) ++multV0C;
            if(writeRegions ){
              fillRegion( 
                multRegionRnd, multEta1RegionRnd, multV0ARegionRnd, multV0CRegionRnd,
                abs(part->phi()) / M_PI, part->eta()
              );
            }
            if(writeHard && part->pT() > 2){
              Quarkonium found;
              found.pt = part->pT();
              found.y = part->y();
              found.pdg = part->id();
              if( writeRegions ){
                fillRegionsForHardProbe( found, pythia,part->phi() / M_PI, -1. + 2. * gRandom->Rndm() );
              }
              foundHighPtPerEvent.push_back( found );
            }
          }
          else  if(
            isOnium( part->id()  )  || isDmeson(part->id())  || isBmeson(part->id() )  ){
            int pdg = part->id();
            // avoid double counting ("J/psi -> J/psi")
            if(
              !( part->daughter1() && pythia.event[ part->daughter1()  ].id() == pdg )
          && !( part->daughter2() && pythia.event[ part->daughter1()  ].id() == pdg )
            ) {
              if( writeOnia &&  isOnium(pdg) ){
      // we found a J/psi!
                totalCallsPerEvent = 0;
                if(verbose) cout <<endl<<endl<< "event " << iev << "  (code: " << pythia.info.code() <<")"<< endl;
                Quarkonium found;
                if( traceBack){
                    traceBackQuarkonium( found, iPart, pythia );
                    if( found.process == kUndefined || 
                        ( found.process > 3 && ( found.mother1process == kUndefined || found.mother2process == kUndefined ) ) 
                    ) {
                      cout << "UNDEFINED BEHAVIOR!!!!!"<<endl;
                      pythia.event.list(0,0,0);
                    }
                }
                found.pt = part->pT();
                found.y = part->y();
                found.pdg = pdg;
                int pdgDau1 = pythia.event[ part->daughter1()  ].idAbs();
                int pdgDau2 = pythia.event[ part->daughter2()  ].idAbs();
      //          cout << "pdg: " << pdg << endl;
      //          cout <<"  daughters: " << pdgDau1 << "  " <<pdgDau2 <<endl;

                if (      pdgDau1 == 11 && pdgDau2 == 11 ) found.decayChannel = 1;
                else if ( pdgDau1 == 13 && pdgDau2 == 13 ) found.decayChannel = -1;
                if( writeRegions ){
                  fillRegionsForHardProbe( found, pythia,part->phi() / M_PI, -1. + 2. *gRandom->Rndm() );
                }
                foundQuarkoniumPerEvent.push_back( found);
              }
              else if( writeOpen && ( isDmeson(pdg)  || isBmeson(pdg) ) ){
                  
                totalCallsPerEvent = 0;
                HeavyProbe found;
                if(traceBack){
                  traceBackOpenHeavyFlavor( found, iPart, pythia );
                  if(found.mother1process == kUndefined || ( found.mother1process != kFromOnium && found.mother2process == kUndefined ) ){
                    cout << "UNDEFINED BEHAVIOR!!!!!"<<endl;
                    pythia.event.list(0,0,0);
                  }
                }
                found.pdg = pdg;
                found.pt = part->pT();
                found.y =  part->y();
                if( writeRegions ){
                  fillRegionsForHardProbe( found, pythia, part->phi() / M_PI, -1. + 2. *gRandom->Rndm() );
                }
                foundDPerEvent.push_back( found );
              }
              
            }
          }
        }
          
        unsigned short downscaleFactor = 1;
        if(scaleDown){
          if( mult < 100) downscaleFactor *= 10;
          if ( noMPI && mult < 120 ) downscaleFactor *= 100;
          if ( !noMPI && !noCR   && mult < 350 ) downscaleFactor *= 100;
          if ( !noMPI && noCR   && mult < 550 ) downscaleFactor *= 100;
        }
        if( iev % downscaleFactor == 0 ) eventTree->Fill();
        if( foundQuarkoniumPerEvent.size() ){
          for ( std::vector<Quarkonium>::iterator iOnium = foundQuarkoniumPerEvent.begin(); iOnium != foundQuarkoniumPerEvent.end(); iOnium++ ) {
            onium = *iOnium;
            oniumTree->Fill();
          }
          foundQuarkoniumPerEvent.clear();
        }
        if( writeOpen && foundDPerEvent.size() ){
          for ( std::vector<HeavyProbe>::iterator iOpen = foundDPerEvent.begin(); iOpen != foundDPerEvent.end(); iOpen++ ) {
            open = *iOpen;
            openTree->Fill();
          }
          foundDPerEvent.clear();
        }
        if( writeHard && foundHighPtPerEvent.size() ){
          for ( std::vector<HardProbe>::iterator iHard = foundHighPtPerEvent.begin(); iHard != foundHighPtPerEvent.end(); iHard++ ) {
            highPtHadron = *iHard;
            hardTree->Fill();
          }
          foundHighPtPerEvent.clear();
        }
      }

         
  

       #pragma omp critical
    { 
     eventTree->Write();
    if(writeOnia) oniumTree->Write();
    if(writeOpen) openTree->Write();
    if(writeHard) hardTree->Write();
     fout->Close();
    }
   
  }
}

bool inV0Aacceptance( float eta){
  return eta > 2.8 && eta < 5.1;
}


bool inV0Cacceptance( float eta){
  return eta > -3.7 && eta < -1.7 ;
}



void traceBackQuarkonium( Quarkonium &ret, unsigned short nextParticle, Pythia & pythia ){
  if(verbose) cout << endl << endl << "Tracing back charmonium" << endl << endl;
  while( nextParticle && totalCallsPerEvent < maxCalls ){
    ++ totalCallsPerEvent;
    Particle* part = &pythia.event[ nextParticle ];

    if(verbose) printParticle( part );
    unsigned int          status, pdg, m1, m2, d1, d2, s, pdg_m1, pdg_m2, pdg_d1, pdg_d2, pdg_s;
    getInformation( part, pythia, status, pdg, m1, m2, d1, d2, s, pdg_m1, pdg_m2, pdg_d1, pdg_d2, pdg_s );
// carbon-copy
    if( m1 == m2 ){
      nextParticle = m1;
    }
// hard scattering
    else if( status == 33 || status == 23 ){
      if (verbose){
        cout << "mother1PDG : "<< pdg_m1 << endl;
        cout << "mother2PDG : "<< pdg_m2 << endl;
      }
      if     ( pdg_m1 == 21 && pdg_m2 == 21 ) ret.process = kgg2QQ;
      else if( ( pdg_m1 == 21 && pdg_m2 < 9 && pdg_s == pdg_m2 )
            || ( pdg_m2 == 21 && pdg_m1 < 9 && pdg_s == pdg_m1 )
      ) ret.process = kgq2QQq;
      else if( pdg_m1 < 9   && pdg_m2 < 9   ) ret.process = kqq2QQ;
      if( status == 33 ){
        unsigned int nMPI  = pythia.info.nMPI();
        for( int iMPI = 1; iMPI < nMPI; ++iMPI ){
          if(verbose) cout << iMPI <<" "<< pythia.info.codeMPI(iMPI) << endl;
          int codeMPI = pythia.info.codeMPI(iMPI);
          if( !ret.iMPI && 
            ( ( codeMPI > 400 && codeMPI < 423 ) || ( codeMPI > 500 && codeMPI < 523 ) ) 
          ) ret.iMPI = iMPI;
        }
        if( !ret.iMPI ) ret.iMPI = 1; // for diffractive events above method does not work, setting mpi process number by hand
      }
      ret.initialPdg = part->id();
      nextParticle = 0;
    }
// case 81-84 : string fragmentation
// case 85-89 : junction fragmentation
    else if( status > 80 && status < 90 ){
      unsigned short q1 = 0;
      unsigned short q2 = 0;
      for (int i = m1; i <= m2; ++i ){
        int pdgTmp = pythia.event[ i ].id();
        if( pythia.event[ i ].isDiquark() ) pdgTmp /= 1000;    // treat diquarks the same as quarks
        if ( verbose &&   abs(pdgTmp) == 21) cout << "~";
        if( abs(pdgTmp) != 21 ) {
          if(verbose)cout << "( " << i << " " << pythia.event[ i ].name() << " )";
          if( !isBhadron(pdg) ){
            if(      pdgTmp ==  4 ) q1 = i;
            else if( pdgTmp == -4 ) q2 = i;
          }
          else{
            short pdgSign = part->id() /part->idAbs();
            if( !q1 &&
              (
                ( isBmeson(pdg)  && pdgTmp * pdgSign == -5 )
                || ( isBbaryon(pdg) && pdgTmp * pdgSign == 5 )
              )
            )    q1 = i;
            else q2 = i;
          }
        }
      }
      Quark quark1       =  traceBackQuark( q1, pythia );
      ret.mother1process = quark1.process;
      ret.mother1mpi     = quark1.mpi;

      Quark quark2       =  traceBackQuark( q2, pythia );
      ret.mother2process = quark2.process;
      ret.mother2mpi     = quark2.mpi;
      ret.sameAncestor   = ( quark1.ancestor && quark2.ancestor && ( quark1.ancestor == quark2.ancestor ) );
      ret.process = kStringFragmentation;
      ret.initialPdg = part->id();
      nextParticle = 0;
      if( verbose ) cout << endl;
    }
// keep going back
    else{
      nextParticle = m1;
    }
  }

  if(verbose){  cout << "--------------------- " << endl << ">>> process = " << charmoniumProcessNames[ret.process] <<  "   " << (int) ret.iMPI  << endl;
    cout << " initial PDG " << ret.initialPdg << endl;
    if( ret.mother1process != kUndefined ) cout << "mother1 process = " << heavyQuarkProcessNames[ ret.mother1process] << (ret.mother1mpi ? "mpi" : "") << endl;
    if( ret.mother2process != kUndefined ) cout << "mother2 process = " << heavyQuarkProcessNames[ ret.mother2process] << (ret.mother2mpi ? "mpi" : "") << endl;
    if(ret.sameAncestor) cout << "same ancestor!!" << endl;
  }
//   verbose = false;
  return;
}

void traceBackOpenHeavyFlavor( HeavyProbe &ret, unsigned short nextParticle, Pythia &pythia ){
  if(verbose) cout << endl << endl << "Tracing back open heavy flavor" << endl << endl;
  while( nextParticle && totalCallsPerEvent < maxCalls ){
    ++ totalCallsPerEvent;
    Particle* part = &pythia.event[ nextParticle ];

    if(verbose) printParticle( part );
    unsigned int          status, pdg, m1, m2, d1, d2, s, pdg_m1, pdg_m2, pdg_d1, pdg_d2, pdg_s;
    getInformation( part, pythia, status, pdg, m1, m2, d1, d2, s, pdg_m1, pdg_m2, pdg_d1, pdg_d2, pdg_s );
// carbon-copy
    if( m1 == m2 ){
      nextParticle = m1;
    }
// from quarkonium above 2 open heavy flavor decay threshold
    else if( isOnium(pdg) || isOniumOctet(pdg) ){
      if(verbose) cout << "from onium." << endl;
      if( ( status == 33 || status == 23 ) ){
        ret.mother1process = kFromOnium;
        ret.initialPdg = part->id();
        ret.mother1mpi = (status == 33);
        nextParticle = 0;
      }
      else  nextParticle = m1;
    }
// case 81-84 : string fragmentation
// case 85-89 : junction fragmentation
    else if( status > 80 && status < 90 ){
      if(verbose) cout << "fragmentation." << endl;
      unsigned short q1 = 0;
      unsigned short q2 = 0;
      for (int i = m1;  i <= m2; ++i ){
        int pdgTmp = pythia.event[ i ].id();
        if( pythia.event[ i ].isDiquark() ) pdgTmp /= 1000;    // treat diquarks the same as quarks
        if ( verbose &&   abs(pdgTmp) == 21) cout << "~";
        if( abs(pdgTmp) != 21 ) {
          if(verbose)cout << "( " << i << " " << pythia.event[ i ].name() << " )";
          short pdgSign = part->id() /part->idAbs();
          if( !q1 &&
              ( 
		( isBmeson(pdg)  && pdgTmp * pdgSign == -5 ) 
		|| ( isBbaryon(pdg) && pdgTmp * pdgSign == 5 ) 
		|| ( isDmeson(pdg)  && pdgTmp * pdgSign == -4 ) 
		|| ( isCbaryon(pdg) && pdgTmp * pdgSign == 4 ) 

	       )
            )    q1 = i;
            else q2 = i;



          if( isChadron(pdg) &&  abs(pdgTmp) == 4 )    q1 = i;
          else if( isBhadron(pdg) &&  abs(pdgTmp) == 5 )    q1 = i;
          else q2 = i;
        }
      }
      Quark quark1       =  traceBackQuark( q1, pythia );
      ret.mother1process = quark1.process;
      ret.mother1mpi     = quark1.mpi;
      if(q2){
        Quark quark2       =  traceBackQuark( q2, pythia );
        ret.mother2process = quark2.process;
        ret.mother2mpi     = quark2.mpi;
      }
      
      ret.initialPdg = part->id();
      nextParticle = 0;
      if( verbose ) cout << endl;
    }
// keep going back
    else{
      nextParticle = m1;
    }
  }

  if(verbose){
    cout << " initial PDG " << ret.initialPdg << endl;
    if( ret.mother1process != kUndefined ) cout << "mother1 process = " << heavyQuarkProcessNames[ ret.mother1process] << (ret.mother1mpi ? "mpi" : "") << endl;
    if( ret.mother2process != kUndefined ) cout << "mother2 process = " << heavyQuarkProcessNames[ ret.mother2process] << (ret.mother2mpi ? "mpi" : "") << endl;
  }
//   verbose = false;
  return;
}



Quark traceBackQuark(  unsigned short nextParticle, Pythia &pythia ){
//    verbose = true;
  if(verbose) cout << endl << endl << "Tracing back quark" << endl << endl;

  Quark ret;
  bool initialHardProcess = false;
  bool mpiHardProcess = false;
  while( nextParticle && totalCallsPerEvent < maxCalls ){
    ++ totalCallsPerEvent;
    Particle* part = &pythia.event[ nextParticle ];
    if(verbose) printParticle(part);
    unsigned int          status, pdg, m1, m2, d1, d2, s, pdg_m1, pdg_m2, pdg_d1, pdg_d2, pdg_s;
    getInformation( part, pythia, status, pdg, m1, m2, d1, d2, s, pdg_m1, pdg_m2, pdg_d1, pdg_d2, pdg_s );
    if( part->isDiquark() ) pdg /= 1000;
    if( pythia.event[ m1 ].isDiquark() ) pdg_m1 /= 1000;
    if( pythia.event[ m2 ].isDiquark() ) pdg_m2 /= 1000;
// carbon-copy
    if( m1 == m2 ){
      nextParticle = m1;
    }
    else if( m1 && pdg_m1 == pdg ){
      if( status == 23 ) initialHardProcess = true;
      if( status == 33 ) mpiHardProcess = true;
      nextParticle = m1;
    }
    else if( m2 && pdg_m2 == pdg ){
      if( status == 23) initialHardProcess = true;
      if( status == 33 ) mpiHardProcess = true;
      nextParticle = m2;
    }
// origin found
    else if( s && pdg_s == pdg ){
      
      unsigned char status_s = pythia.event[s].statusAbs();
      if(verbose) cout << "mother1: " << m1 << " PDG: " << pdg_m1 << "  Status: " << pythia.event[m1].statusAbs()  << endl;
      if(verbose) cout << "mother2: " << m2 << " PDG: " << pdg_m2 << "  Status: " << (m2 ? pythia.event[m2].statusAbs() :0) << endl;
      if(verbose) cout << "sister : " << s  << " PDG: " << pdg_s  <<"  Status: " << (int) status_s << endl;
//       cout << "sisters mothers: " << (s ? pythia.event[s].mother1() : 0) << "  " << (s ? pythia.event[s].mother2() : 0)  << endl;
      
      if( status == status_s  && status == 33 || status == 23 ){
        if(      pdg_m1 == 21 && pdg_m2 == 21 ) ret.process = kGluonFusion;
        else if( pdg_m1 <   9 && pdg_m2 <   9 ) ret.process = kQuarkAnnihilation;
        ret.mpi = (status == 33);
      }
      else if( ( pdg_m1 == 21  || pdg_m1 == 22 )  && pdg_m2 == 0){
// gluon splitting
        bool photon = (pdg_m1 == 22);
        if ( photon ) cout << "photon splitting" << endl;
        if( initialHardProcess || mpiHardProcess ) {
          ret.process = kFlavorExcitation;
          ret.mpi = mpiHardProcess;
        }
        else{
          Gluon sister_heavy_quark= traceForwardGluon( s, pythia,  0, false);
          if( sister_heavy_quark.process == kGluonFromISR   ){
            ret.process = kFlavorExcitation;
            ret.mpi = sister_heavy_quark.mpi;
          }

          else{
            unsigned short m_original = pythia.event[m1].iTopCopy();
            unsigned char status_mother_original = pythia.event[ m_original ].statusAbs();
            if(verbose) cout << "original mother: " << m_original << "  status: " << (int) status_mother_original << endl;
            if(verbose) cout << "mothers: " <<  pythia.event[ m_original ].mother1() << "  " <<  pythia.event[ m_original ].mother2() << endl;
            
            if( status_mother_original == 23 || status_mother_original == 33 ){
              ret.process = photon ? kPhotonFromHardProcess : kGluonFromHardProcess;
              ret.mpi = ( status_mother_original == 33 );
            }
            else if( status_mother_original == 41 || status_mother_original == 42 || status_mother_original == 43 ){
              unsigned short mother_of_original_mother = pythia.event[ m_original ].mother1() ;
              Gluon final_ancestor = traceForwardGluon( mother_of_original_mother, pythia, m_original, false);
              if( final_ancestor.process == kGluonFromISR){
                ret.process = photon ? kPhotonFromIFSR : kGluonFromISR;
                ret.mpi = final_ancestor.mpi;
              }
              else{
                final_ancestor = traceForwardGluon( mother_of_original_mother, pythia, m_original, true);
                if( final_ancestor.process != kUndefined  ){
                  ret.process = photon ? kPhotonFromIFSR : kGluonFromIFSR;
                  ret.mpi = final_ancestor.mpi;
                }
              }
            }
            else if( status_mother_original == 51 || status_mother_original == 52  || status_mother_original == 53  || status_mother_original == 91 ){
              unsigned short mother_of_original_mother = pythia.event[ m_original ].mother1() ;
              
              Gluon final_ancestor = traceBackGluon( mother_of_original_mother, pythia, m_original, false);
              if( final_ancestor.process == kGluonFromFSR  ){
                if(status_mother_original == 91) ret.process = kGluonFromDecay;
                else ret.process = photon ? kPhotonFromFSR : kGluonFromFSR;
                ret.mpi = final_ancestor.mpi;
              }
              else{
                final_ancestor = traceBackGluon( mother_of_original_mother, pythia, m_original, true);
                if( final_ancestor.process != kUndefined  ){
                  if(status_mother_original == 91) ret.process = kGluonFromDecay;
                  else ret.process = photon ? kPhotonFromIFSR : kGluonFromIFSR;
                  ret.mpi = final_ancestor.mpi;
                }
              }
            }
            
            if(  ret.process == kUndefined ){
  // find out if gluon splitted off from heavy quark participates in hard collisions
              Gluon heavyQuark = traceForwardGluon( nextParticle, pythia, 0, true );
              
              if( heavyQuark.process == kGluonFromISR ){
                ret.mpi = heavyQuark.mpi;
                ret.process = kGluonFromHeavyQuark;
              }
              else {
                heavyQuark = traceForwardGluon( s, pythia, 0, true );
                if( heavyQuark.process == kGluonFromISR ){
                  ret.mpi = heavyQuark.mpi;
                  ret.process = kGluonFromHeavyQuark;
                }
              }
            }
          }
        }
      }
  
      else if(status == 91 && isOnium(m1)){
        ret.process = kFromOnium;
        Quarkonium motherQuarkonium;
        traceBackQuarkonium( motherQuarkonium, m1, pythia );
        ret.mpi = motherQuarkonium.iMPI;
      }
      ret.ancestor = m1;
      if( ret.process == kUndefined ) cout << endl << "!!!!!!!!!!!!!!!!!!!UNDEFINED!!!!!!!!!!!!!!!!" << endl << endl;
      nextParticle = 0;
    }
// beam remnant
    else if ( status == 61  ){
      if( initialHardProcess || mpiHardProcess ) {
        ret.process  =  kBeamRemnantToHardProcess;
        ret.mpi = mpiHardProcess;
      }
      else{
        Gluon heavyQuark = traceForwardGluon( nextParticle, pythia, 0, true );
        if( heavyQuark.process == kGluonFromISR ){
          ret.process = kBeamRemnantToHardProcess;
          ret.mpi = heavyQuark.mpi;
        }
      }
      nextParticle = 0;
    }
// beam remnant
    else if (status == 63 ){
      ret.process = kBeamRemnant;
      nextParticle = 0;
    }
  }
  if(verbose) cout << "    >>> process = " << heavyQuarkProcessNames[ret.process] << ( ret.mpi ? "mpi" : "") << endl;

  return ret;
}



Gluon traceBackGluon(  unsigned short nextParticle, Pythia &pythia, unsigned short dExcl, bool rigorously ){
  if(verbose) cout << endl << endl << "Tracing back gluon" << (rigorously ? " rigorously" :"") << endl << endl;
  Gluon ret;
  while( nextParticle && totalCallsPerEvent < maxCalls ){
    ++ totalCallsPerEvent;
    Particle* part = &pythia.event[ nextParticle ];
    if(verbose) printParticle( part );
    unsigned int          status, pdg, m1, m2, d1, d2, s, pdg_m1, pdg_m2, pdg_d1, pdg_d2, pdg_s;
    getInformation( part, pythia, status, pdg, m1, m2, d1, d2, s, pdg_m1, pdg_m2, pdg_d1, pdg_d2, pdg_s );
    if( status == 23 || status == 33){
      ret.process = kGluonFromFSR;
      ret.mpi = status == 33;
      nextParticle = 0;
    }
    
    else {
      if( rigorously ){
        if ( d1 && d1 != dExcl /*&& isParton(d1, isParton)*/ ){
          Gluon gluonTmp = traceForwardGluon( d1, pythia, dExcl, true );
          if( gluonTmp.process == kGluonFromISR ){
            ret.process = gluonTmp.process;
            ret.mpi = gluonTmp.mpi;
            nextParticle = 0;
          }
        }
        if( ret.process == kUndefined && d2 && d2 != d1 && d2 != dExcl/* && isParton(d2, isParton)*/ ){
          Gluon gluonTmp = traceForwardGluon( d2, pythia, dExcl, true );
          if( gluonTmp.process == kGluonFromISR ){
            ret.process = gluonTmp.process;
            ret.mpi = gluonTmp.mpi;
            nextParticle = 0;
          }
        }
      }
      if( ret.process == kUndefined ){
        if( m1 && pythia.event[m1].statusAbs() > 20  ){
          dExcl = nextParticle;
          nextParticle = m1;
        }
        else{
          nextParticle = 0;
        }
      }
    }
  }
  if(verbose) cout << " --> gluon origin: " << heavyQuarkProcessNames[ret.process] << endl;
  return ret;
}




Gluon traceForwardGluon(  unsigned short nextParticle, Pythia &pythia, unsigned short dExcl, bool rigorously ){
  if(verbose) cout << endl << endl << "Tracing forward gluon" << (rigorously ? " rigorously" :"") << endl << endl;
  Gluon ret;
  while( nextParticle && totalCallsPerEvent < maxCalls ){
    ++ totalCallsPerEvent;
    Particle* part = &pythia.event[ nextParticle ];
    if(verbose) printParticle(part);
    unsigned int          status, pdg, m1, m2, d1, d2, s, pdg_m1, pdg_m2, pdg_d1, pdg_d2, pdg_s;
    getInformation( part, pythia, status, pdg, m1, m2, d1, d2, s, pdg_m1, pdg_m2, pdg_d1, pdg_d2, pdg_s );

    if( status == 21 || status == 31  ){
      ret.process=kGluonFromISR;
      ret.mpi = status == 31;
      nextParticle = 0;
    }
    else if( d1 && d1 != dExcl && isParton(d1, pythia)  && ( rigorously || pdg_d1 == pdg )   ){
      if( rigorously && d2 &&  d2 != d1 && d2 != dExcl && isParton(d2, pythia) && ( rigorously || pdg_d2 == pdg ) ){
        Gluon gluonTmp = traceForwardGluon( d2, pythia, dExcl, true );
        if( gluonTmp.process == kGluonFromISR ){
          ret.process = gluonTmp.process;
          ret.mpi = gluonTmp.mpi;
          nextParticle = 0;
        }
      }
      if( ret.process == kUndefined ) nextParticle = d1;
    }
    else if( d2 && d2 != dExcl && isParton(d2, pythia) && ( rigorously || pdg_d2 == pdg ) ){
      nextParticle = d2;
    }
    else{
      nextParticle = 0;
    }
  }
  if(verbose) cout << " --> gluon origin: " << heavyQuarkProcessNames[ret.process] << endl;
  return ret;
}

void printParticle( Particle* part){
  cout << "        " << part->index() <<  "\t" <<part->idAbs() <<    "\t( "  << part->name() << " )\t" <<  part->statusAbs() << " -->   " <<  part->mother1() << "  " << part->mother2() <<  "\t/\t" <<  part->daughter1() << "  " << part->daughter2() << endl ; 
}


void getInformation( Particle *part,
                     Pythia &pythia,
                     unsigned int &status,
                     unsigned int &pdg,
                     unsigned int &m1,
                     unsigned int &m2,
                     unsigned int &d1,
                     unsigned int &d2,
                     unsigned int &s,
                     unsigned int &pdg_m1,
                     unsigned int &pdg_m2,
                     unsigned int &pdg_d1,
                     unsigned int &pdg_d2,
                     unsigned int &pdg_s  ){
  pdg = part->idAbs();
  status =  part->statusAbs() ;
  m1 = part->mother1();
  m2 = part->mother2();
  d1 = part->daughter1();
  d2 = part->daughter2();
  pdg_m1 = m1 ? pythia.event[ m1 ].idAbs() : 0;
  pdg_m2 = m2 ? pythia.event[ m2 ].idAbs() : 0;
  pdg_d1 = d1 ? pythia.event[ d1 ].idAbs() : 0;
  pdg_d2 = d2 ? pythia.event[ d2 ].idAbs() : 0;
  s = 0;
  pdg_s = 0;
  if( pdg_m1  ){
    if (     pythia.event[ m1 ].daughter1() == part->index() )
      s = pythia.event[ m1 ].daughter2();
    else if (pythia.event[ m1 ].daughter2() == part->index() )
      s = pythia.event[ m1 ].daughter1();
    if( s) pdg_s = pythia.event[s].idAbs();
  }
  

}


int firstQuark (int pdg){
  return ( abs(pdg) % 100 - abs(pdg) % 10 ) / 10;
}

int secondQuark (int pdg){
  return ( abs(pdg) % 1000 - abs(pdg) % 100 ) / 100;
}

int thirdQuark (int pdg){
  return ( abs(pdg) % 10000 - abs(pdg) % 1000 ) / 1000;
}

bool isCharmonium(int pdg){
  return ( firstQuark(pdg) == 4 && secondQuark(pdg) == 4 );
}

bool isBottomonium(int pdg){
  return ( firstQuark(pdg) == 5 && secondQuark(pdg) == 5 );
}

bool isOnium(int pdg){
  return (isCharmonium(pdg) || isBottomonium(pdg) );
}

bool isOniumOctet(int pdg){
  return abs(pdg) > 9900000;
}

bool isDmeson(int pdg){
  return   (firstQuark(pdg) != 4 && secondQuark(pdg) == 4);
}
bool isCbaryon(int pdg){
  return (thirdQuark(pdg) == 4); 
}
bool isChadron(int pdg){
  return isDmeson(pdg) || isCbaryon(pdg) || isCharmonium(pdg);
}

bool isBmeson(int pdg){
  return   (firstQuark(pdg) != 5 && secondQuark(pdg) == 5);
}
bool isBbaryon(int pdg){
  return (thirdQuark(pdg) == 5); 
}
bool isBhadron(int pdg){
  return isBmeson(pdg) || isBbaryon(pdg) || isBottomonium(pdg);
}
bool isParton( unsigned short idx, Pythia &pythia){
  return pythia.event[idx].idAbs() < 9 || pythia.event[idx].idAbs() == 21 || pythia.event[idx].idAbs() == 22  ||  pythia.event[idx].isDiquark() ;
}

bool isPrimaryChargedALICE( unsigned short idx, Pythia &pythia ){
  
  if( ! pythia.event[idx].isCharged() ) return false;
  
  int status = pythia.event[idx].statusHepMC();
  if( status==0 || status ==4 || (status>=11 && status<=200) ) return false;
  unsigned int pdg = pythia.event[idx].idAbs();
  if (!isLongLived(pdg)) return false;
  while( idx = pythia.event[idx].mother1() ){
    status = pythia.event[idx].statusHepMC();
   // if ( status == 4  ) return true;
//    if ( status == 4 || (status>=11 && status <20 )   ) return true;
    if ( status == 4 || status==13 ) return true;
    pdg = pythia.event[idx].idAbs();
    if ( isLongLived(pdg) ) return false;
  }
  return true;
}


bool isLongLived( unsigned int pdg ){
  if (pdg > 1000000000) return true;

  switch (pdg) {
    case 13:
    case 11:
    case 22:
    case 211:
    case 321:
    case 130:
    case 310:
    case 2212:
    case 2112:
    case 3122:
    case 3112:
    case 3222:
    case 3312:
    case 3322:
    case 3334:
    case 12:
    case 14:
    case 16:
    return true;
  }
  return false;
}
void fillRegions( unsigned short &multRegion1, unsigned short &multEta1Region1, unsigned short &multV0ARegion1, unsigned short &multV0CRegion1,
                  unsigned short &multRegion2, unsigned short &multEta1Region2, unsigned short &multV0ARegion2, unsigned short &multV0CRegion2,
                  unsigned short &multRegion3, unsigned short &multEta1Region3, unsigned short &multV0ARegion3, unsigned short &multV0CRegion3,
                  double phi, double eta){
 
  if(phi < 1./3.){
    ++multRegion1;
    if( abs(  eta ) < 1.0 ) ++ multEta1Region1;
    else if( inV0Aacceptance( eta ) ) ++ multV0ARegion1;
    else if( inV0Cacceptance( eta ) ) ++ multV0CRegion1;
  }
  else if(phi < 2./3){
    ++multRegion2;
    if( abs(  eta ) < 1.0 ) ++ multEta1Region2;
    else if( inV0Aacceptance( eta ) ) ++ multV0ARegion2;
    else if( inV0Cacceptance( eta ) ) ++ multV0CRegion2;
  }
  else{
    ++multRegion3;
    if( abs(  eta ) < 1.0 ) ++ multEta1Region3;
    else if( inV0Aacceptance( eta ) ) ++ multV0ARegion3;
    else if( inV0Cacceptance( eta ) ) ++ multV0CRegion3;
  }
}

void fillRegion( unsigned short &multRegion, unsigned short &multEta1Region, unsigned short &multV0ARegion,  unsigned short &multV0CRegion,
                  double phi, double eta){
  if(phi < 1./3.){
    ++multRegion;
    if( abs(  eta ) < 1.0 ) ++multEta1Region;
    else if( inV0Aacceptance( eta ) ) ++multV0ARegion;
    else if( inV0Cacceptance( eta ) ) ++multV0CRegion;
  }
}

void fillRegionsForHardProbe( HardProbe &found, Pythia& pythia, double phi, double phi_rnd){
  for ( int iPart2 = 0;  iPart2 < pythia.event.size(); iPart2++) {
    Particle* part2 = &pythia.event[iPart2];
    if(  isPrimaryChargedALICE(iPart2, pythia)  ){
      float phi_assoc = part2->phi()/M_PI;
      float deltaPhi = abs( phi_assoc - phi );
      float deltaPhi_rnd = abs( phi_assoc - phi_rnd );
      if(deltaPhi > 1) deltaPhi = 2 - deltaPhi;
      if(deltaPhi_rnd > 1) deltaPhi_rnd = 2 - deltaPhi_rnd;

      fillRegions( found.multRegion1, found.multEta1Region1, found.multV0ARegion1, found.multV0CRegion1,
        found.multRegion2, found.multEta1Region2, found.multV0ARegion2, found.multV0CRegion2,
        found.multRegion3, found.multEta1Region3, found.multV0ARegion3, found.multV0CRegion3,
        deltaPhi, part2->eta() );
      fillRegion( found.multRegionRnd, found.multEta1RegionRnd, found.multV0ARegionRnd, found.multV0CRegionRnd,
        deltaPhi_rnd, part2->eta() );
    }
  }
}


