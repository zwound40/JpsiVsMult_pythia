/**
 *
 * SIMPLIFIED VERSION OF MACRO
 *
 * This macro generates events with PYTHIA, looks for quarkonia in the events,
 * and stores the information about the events and probes in TTree's that it writes into a root file.
 * The trees can be later used to determine e.g. the multiplicity dependence of J/psi production.
 *
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
 *
 */



#include <time.h>
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TRandom3.h"
#include "Pythia8/Pythia.h"

using namespace Pythia8;


/**
 *
 * This is the information stored for each Quarkonium in the tree:
 *  - the pdg code (see http://pdg.lbl.gov/2019/reviews/rpp2019-rev-monte-carlo-numbering.pdf )
 *  - the pT and rapidity
 *  - the multiplicities in the towards ("Region1"), transverse ("Region2") and away ("Region3") region
 *    these multiplicities are counted either
 *    - for all etas
 *    - for |eta| < 1
 *    - in the V0A acceptance
 *    - in the V0C acceptance
 *
 */

struct Quarkonium {
    int pdg;
    float pt,y;
    int initialPdg;
    int decayChannel;
    unsigned short multRegion1;
    unsigned short multRegion2;
    unsigned short multRegion3;
    unsigned short multEta1Region1;
    unsigned short multEta1Region2;
    unsigned short multEta1Region3;
    unsigned short multV0ARegion1;
    unsigned short multV0ARegion2;
    unsigned short multV0ARegion3;
    unsigned short multV0CRegion1;
    unsigned short multV0CRegion2;
    unsigned short multV0CRegion3;
    Quarkonium():
        pdg(0),
        pt(0.),
        y(0.),
        initialPdg(0),
        decayChannel(0),
        multRegion1(0),
        multRegion2(0),
        multRegion3(0),
        multEta1Region1(0),
        multEta1Region2(0),
        multEta1Region3(0),
        multV0ARegion1(0),
        multV0ARegion2(0),
        multV0ARegion3(0),
        multV0CRegion1(0),
        multV0CRegion2(0),
        multV0CRegion3(0)
    {};
};

void traceBackQuarkonium( Quarkonium &found, unsigned short particle, Pythia &pythia);
bool isOnium(int pdg);



void fillRegions( unsigned short &multRegion1, unsigned short &multEta1Region1, unsigned short &multV0ARegion1, unsigned short &multV0CRegion1,
                  unsigned short &multRegion2, unsigned short &multEta1Region2, unsigned short &multV0ARegion2, unsigned short &multV0CRegion2,
                  unsigned short &multRegion3, unsigned short &multEta1Region3, unsigned short &multV0ARegion3, unsigned short &multV0CRegion3,
                  double phi, double eta);
void fillRegionRandom( unsigned short &multRegion, unsigned short &multEta1Region, unsigned short &multV0ARegion, unsigned short &multV0CRegion,
                       double phi, double eta);
void fillRegionsWrtQuarkonium( HardProbe &found, Pythia& pythia, double phi, double phi_rnd);


bool isPrimaryChargedALICE( unsigned short idx, Pythia &pythia);
bool isLongLived( unsigned int pdg);

bool inV0Aacceptance( float eta );
bool inV0Cacceptance( float eta );

/**
 *
 * The main function.
 *
 * - processes the command line arguments
 * - starts parallel threads using openMP
 * - created the output trees
 * - initialized pythia
 * - runs the event loop
 *   - here it calculates the multplicities, looks for the hard probes, and calls helper function
 *     to get the multiplicities in the different regions, and information about the hard probes.
 *
 *
 */
int main( int argc, char** argv ) {

    int nev            = argc > 1 ? std::stoi(argv[1]) : 10000;
    string settings    = argc > 2 ? argv[4]            : "settings.cmnd";
    string outfilename = argc > 3 ? argv[5]            : "out";
    int taskid         = argc > 4 ? std::stoi(argv[6]) : 1;

    // prepare output file

    outfilename += ".root";

    TFile *fout;
    fout = TFile::Open(  outfilename.c_str(),"RECREATE");
    fout->cd();



    // prepare trees

    unsigned short mult, multEta1, multV0A, multV0C, nMPI;
    unsigned char type;
    unsigned short multRegionRnd;
    unsigned short multEta1RegionRnd;
    unsigned short multV0ARegionRnd;
    unsigned short multV0CRegionRnd;


    TTree* eventTree = new TTree( "eventTree", "event information");



    // define the tree for minimum bias events
    eventTree->Branch( "mult",                &mult );
    eventTree->Branch( "multEta1",            &multEta1 );
    eventTree->Branch( "multV0A",             &multV0A );
    eventTree->Branch( "multV0C",             &multV0C );
    eventTree->Branch( "type",                &type );
    eventTree->Branch( "nMPI",                &nMPI );
    eventTree->Branch( "multRegionRnd",              &multRegionRnd );
    eventTree->Branch( "multEta1RegionRnd",          &multEta1RegionRnd );
    eventTree->Branch( "multV0ARegionRnd",           &multV0ARegionRnd );
    eventTree->Branch( "multV0CRegionRnd",           &multV0CRegionRnd );




    TTree* oniumTree = new TTree( "oniumTree", "Onia information");
    Quarkonium onium;
    oniumTree->Branch( "mult",                  &mult );
    oniumTree->Branch( "multEta1",              &multEta1 );
    oniumTree->Branch( "multV0A",               &multV0A );
    oniumTree->Branch( "multV0C",               &multV0C );
    oniumTree->Branch( "nMPI",                  &nMPI );
    oniumTree->Branch( "type",                  &type );
    oniumTree->Branch( "onium.pdg",             &onium.pdg );
    oniumTree->Branch( "onium.decayChannel",    &onium.decayChannel );
    oniumTree->Branch( "onium.pt",              &onium.pt );
    oniumTree->Branch( "onium.y",               &onium.y );
    oniumTree->Branch( "onium.initialPdg",      &onium.initialPdg );
    oniumTree->Branch( "multRegion1",              &onium.multRegion1 );
    oniumTree->Branch( "multRegion2",              &onium.multRegion2 );
    oniumTree->Branch( "multRegion3",              &onium.multRegion3 );
    oniumTree->Branch( "multEta1Region1",          &onium.multEta1Region1 );
    oniumTree->Branch( "multEta1Region2",          &onium.multEta1Region2 );
    oniumTree->Branch( "multEta1Region3",          &onium.multEta1Region3 );
    oniumTree->Branch( "multV0ARegion1",           &onium.multV0ARegion1 );
    oniumTree->Branch( "multV0ARegion2",           &onium.multV0ARegion2 );
    oniumTree->Branch( "multV0ARegion3",           &onium.multV0ARegion3 );
    oniumTree->Branch( "multV0CRegion1",           &onium.multV0CRegion1 );
    oniumTree->Branch( "multV0CRegion2",           &onium.multV0CRegion2 );
    oniumTree->Branch( "multV0CRegion3",           &onium.multV0CRegion3 );


    // start pythia
    gRandom = new TRandom3();
    Pythia pythia;
    pythia.readFile( settings );

    pythia.readString("Random:setSeed = on");
    int seed = time(NULL) * taskid % 900000000;
    std::stringstream sstm;
    sstm <<  "Random:seed =" << seed;
    std::string seedString = sstm.str();
    pythia.readString(  seedString  );

    pythia.init();



    // Start the event loop

    for (int iev = 0; iev < nev; iev++) {

        if (!pythia.next()) continue;
        mult = 0; // mult in eta +- inf.
        multEta1 = 0; // mult in eta +-1.0
        multV0A = 0; // mult in V0A acceptance
        multV0C = 0; // mult in V0C acceptance


        // multiplicity in a randomly chosen 120 degree phi region
        multRegionRnd = 0;
        multEta1RegionRnd = 0;
        multV0ARegionRnd = 0;
        multV0CRegionRnd = 0;


        // collision type:
        // single, double, central or non-diffractive, see http://home.thep.lu.se/~torbjorn/pythia82html/QCDProcesses.html
        type  = pythia.info.code();

        nMPI  =  pythia.info.nMPI();

        vector <Quarkonium> foundQuarkoniumPerEvent;

        // here the loop over the particles starts

        for ( int iPart = 0;  iPart < pythia.event.size(); iPart++) {
            Particle* part = &pythia.event[iPart];
            if( isPrimaryChargedALICE(iPart, pythia) ) {
                ++mult;
                if( abs(  part->eta() ) < 1.0 ) ++ multEta1;
                else if( inV0Aacceptance( part->eta() ) ) ++multV0A;
                else if( inV0Cacceptance( part->eta() ) ) ++multV0C;
                fillRegionRandom(
                    multRegionRnd, multEta1RegionRnd, multV0ARegionRnd, multV0CRegionRnd,
                    abs(part->phi()) / M_PI, part->eta()
                );
            }

            // we found a quarkonium
            else  if( isOnium( part->id()  ) {
            int pdg = part->id();
                int pdgDau1 = pythia.event[ part->daughter1()  ].idAbs();
                int pdgDau2 = pythia.event[ part->daughter2()  ].idAbs();
                // avoid double counting ("J/psi -> J/psi")
                if( !( pdgDau1 == pdg ) && !( pdgDau2 == pdg ) ) {

                    Quarkonium found;
                    found.pt = part->pT();
                    found.y = part->y();
                    found.pdg = pdg;
                    if (      pdgDau1 == 11 && pdgDau2 == 11 ) found.decayChannel = 1;
                    else if ( pdgDau1 == 13 && pdgDau2 == 13 ) found.decayChannel = -1;
                    // get information on where the J/psi came from, i.e. prompt or non-prompt
                    traceBackQuarkonium( found, iPart, pythia );

                    // fill in the multiplicity values in the 3 regions
                    fillRegionsWrtQuarkonium( found, pythia, part->phi() / M_PI );

                    // add the found quarkonium to the list of found quarkonia in this event
                    foundQuarkoniumPerEvent.push_back( found);
                }
            }
        }


        eventTree->Fill();
        // we found some quarkonia in this event -> fill them in the tree
        if( foundQuarkoniumPerEvent.size() ) {
            for ( std::vector<Quarkonium>::iterator iOnium = foundQuarkoniumPerEvent.begin(); iOnium != foundQuarkoniumPerEvent.end(); iOnium++ ) {
                onium = *iOnium;
                oniumTree->Fill();
            }
            foundQuarkoniumPerEvent.clear();
        }
    }

    // here the event loop ends

    eventTree->Write();
    oniumTree->Write();
    fout->Close();

}

bool inV0Aacceptance( float eta) {
    return eta > 2.8 && eta < 5.1;
}


bool inV0Cacceptance( float eta) {
    return eta > -3.7 && eta < -1.7 ;
}
/**
 *
 * Function that finds out, where the quarkonia comes from. In this simplified version it only checks
 * if it is a prompt or non-prompt J/psi (i.e. if one of it's ancestor's was a b-meson or baryon)
 * The function gets the mother of the quarkonium, checks it's id and status code, and decides if it has to
 * go further back to find the initially produced partcile
 * Check http://home.thep.lu.se/~torbjorn/pythia81html/ParticleProperties.html for the meaning of the status codes.
 *
 */


void traceBackQuarkonium( Quarkonium &ret, unsigned short nextParticle, Pythia & pythia ) {

    while( nextParticle  ) {
        Particle* part = &pythia.event[ nextParticle ];
        unsigned int status = part->statusAbs() ;
        unsigned int m1 = part->mother1();
        unsigned int m2 = part->mother2();

// particle is carbon-copy of mother -> keep going back
        if( m1 == m2 ) {
            nextParticle = m1;
        }
// particle was produced in hard scattering -> we found the initially produced particle!
        else if( status == 33 || status == 23 ) {
            ret.initialPdg = part->id();
            nextParticle = 0;
        }

// particle was produced in string fragmentation step -> we found the initially produced particle!
        else if( status > 80 && status < 90 ) {
            ret.initialPdg = part->id();
            nextParticle = 0;
        }
// we are at some intermediate state (particle is rescattering or decaying) -> keep going back
        else {
            nextParticle = m1;
        }
    }

    return;
}


// Find out the quark content of a particle from it's PDG identifier
// see http://pdg.lbl.gov/2019/reviews/rpp2019-rev-monte-carlo-numbering.pdf


int firstQuark (int pdg) {
    return ( abs(pdg) % 100 - abs(pdg) % 10 ) / 10;
}

int secondQuark (int pdg) {
    return ( abs(pdg) % 1000 - abs(pdg) % 100 ) / 100;
}

int thirdQuark (int pdg) {
    return ( abs(pdg) % 10000 - abs(pdg) % 1000 ) / 1000;
}

bool isCharmonium(int pdg) {
    return ( firstQuark(pdg) == 4 && secondQuark(pdg) == 4 && thirdQuark(pdg) == 0 );
}

bool isBottomonium(int pdg) {
    return ( firstQuark(pdg) == 5 && secondQuark(pdg) == 5 && thirdQuark(pdg) == 0 );
}

bool isOnium(int pdg) {
    return (isCharmonium(pdg) || isBottomonium(pdg) );
}




/**
 *
 * Find out if given partilce is a primary charged particle according to ALICE definition,
 * see https://cds.cern.ch/record/2270008
 *
 */

bool isPrimaryChargedALICE( unsigned short idx, Pythia &pythia ) {

    if( ! pythia.event[idx].isCharged() ) return false;

    int status = pythia.event[idx].statusHepMC();
    if( status==0 || status ==4 || (status>=11 && status<=200) ) return false;
    unsigned int pdg = pythia.event[idx].idAbs();
    if (!isLongLived(pdg)) return false;
    while( idx = pythia.event[idx].mother1() ) {
        status = pythia.event[idx].statusHepMC();
        if ( status == 4 || status==13 ) return true;
        pdg = pythia.event[idx].idAbs();
        if ( isLongLived(pdg) ) return false;
    }
    return true;
}


bool isLongLived( unsigned int pdg ) {
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
                  double phi, double eta) {

    if(phi < 1./3.) {
        ++multRegion1;
        if( abs(  eta ) < 1.0 ) ++ multEta1Region1;
        else if( inV0Aacceptance( eta ) ) ++ multV0ARegion1;
        else if( inV0Cacceptance( eta ) ) ++ multV0CRegion1;
    }
    else if(phi < 2./3) {
        ++multRegion2;
        if( abs(  eta ) < 1.0 ) ++ multEta1Region2;
        else if( inV0Aacceptance( eta ) ) ++ multV0ARegion2;
        else if( inV0Cacceptance( eta ) ) ++ multV0CRegion2;
    }
    else {
        ++multRegion3;
        if( abs(  eta ) < 1.0 ) ++ multEta1Region3;
        else if( inV0Aacceptance( eta ) ) ++ multV0ARegion3;
        else if( inV0Cacceptance( eta ) ) ++ multV0CRegion3;
    }
}

void fillRegionRandom( unsigned short &multRegion, unsigned short &multEta1Region, unsigned short &multV0ARegion,  unsigned short &multV0CRegion,
                       double phi, double eta) {
    if(phi < 1./3.) {
        ++multRegion;
        if( abs(  eta ) < 1.0 ) ++multEta1Region;
        else if( inV0Aacceptance( eta ) ) ++multV0ARegion;
        else if( inV0Cacceptance( eta ) ) ++multV0CRegion;
    }
}

void fillRegionsWrtQuarkonium( HardProbe &found, Pythia& pythia, double phi_quarkonium ) {
    for ( int iPart2 = 0;  iPart2 < pythia.event.size(); iPart2++) {
        Particle* part2 = &pythia.event[iPart2];
        if(  isPrimaryChargedALICE(iPart2, pythia)  ) {
            float phi_assoc = part2->phi()/M_PI;
            float deltaPhi = abs( phi_assoc - phi_quarkonium );
            if(deltaPhi > 1) deltaPhi = 2 - deltaPhi;
            fillRegions( found.multRegion1, found.multEta1Region1, found.multV0ARegion1, found.multV0CRegion1,
                         found.multRegion2, found.multEta1Region2, found.multV0ARegion2, found.multV0CRegion2,
                         found.multRegion3, found.multEta1Region3, found.multV0ARegion3, found.multV0CRegion3,
                         deltaPhi, part2->eta() );
        }
    }
}
