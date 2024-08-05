// -*- C++ -*-
//
// Package:    Demo/DemoAnalyzer
// Class:      DemoAnalyzer
//
/**\class DemoAnalyzer DemoAnalyzer.cc Demo/DemoAnalyzer/plugins/MiniAnalyzer.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author: Swasti
//         Created:  Thu, 01 Feb 2024 02:16:09 GMT
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/Utilities/interface/InputTag.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "TH1.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/Candidate/interface/VertexCompositePtrCandidate.h"
#include "DataFormats/Candidate/interface/LeafCandidate.h"
#include "DataFormats/Math/interface/LorentzVector.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"

#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"

// ROOT includes
#include "TTree.h"
#include <TFile.h>
#include <TROOT.h>
#include "TBranch.h"
#include <string>
#include <vector>
#include "TSystem.h"
#include "TVector3.h"
#include "TLorentzVector.h"

//
// class declaration
//

class MiniAnalyzer : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
   public:
      explicit MiniAnalyzer(const edm::ParameterSet&);
      ~MiniAnalyzer();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   private:
      virtual void beginJob() override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;

      // ----------member data ---------------------------
      edm::EDGetTokenT<reco::VertexCollection> vtxToken_;
      edm::EDGetTokenT<std::vector<reco::VertexCompositePtrCandidate>> secondaryVertexToken_;

      TTree *tree;

      edm::RunNumber_t irun;
      edm::EventNumber_t ievent;
      edm::LuminosityBlockNumber_t ilumiblock;
      edm::Timestamp itime;

      size_t run, event, lumi, time;

      unsigned int npv;
      unsigned int nsv;

      float svPt;
      float svX;
      float svY;
      float svZ;
      float svNormalizedChi2;
      float svChi2;
      float svEta;
      float svMass;
      int svNumDaughters;
      float svDxyError;
      float svDxy;
      float svNdf;
};

//
// constructors and destructor
//
MiniAnalyzer::MiniAnalyzer(const edm::ParameterSet& iConfig)
    : vtxToken_(consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("vertices"))),
      secondaryVertexToken_(consumes<std::vector<reco::VertexCompositePtrCandidate>>(iConfig.getParameter<edm::InputTag>("secondaryVertices")))
{
   usesResource("TFileService");
   edm::Service<TFileService> file;

   tree = file->make<TTree>("tree","For QCD File");
   tree->Branch("npv", &npv, "npv/i");
   tree->Branch("nsv", &nsv, "nsv/i");

   tree->Branch("svPt", &svPt, "svPt/F");
   tree->Branch("svX", &svX, "svX/F");
   tree->Branch("svY", &svY, "svY/F");
   tree->Branch("svZ", &svZ, "svZ/F");
   tree->Branch("svNormalizedChi2", &svNormalizedChi2, "svNormalizedChi2/F");
   tree->Branch("svChi2", &svChi2, "svChi2/F");
   tree->Branch("svEta", &svEta, "svEta/F");
   tree->Branch("svMass", &svMass, "svMass/F");
   tree->Branch("svNumDaughters", &svNumDaughters, "svNumDaughters/I");
   tree->Branch("svDxyError", &svDxyError, "svDxyError/F");
   tree->Branch("svDxy", &svDxy, "svDxy/F");
   tree->Branch("svNdf", &svNdf, "svNdf/F");
}

MiniAnalyzer::~MiniAnalyzer() {}

//
// member functions
//

void MiniAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;

  edm::Handle<reco::VertexCollection> vertices;
  iEvent.getByToken(vtxToken_, vertices);
  if (vertices->empty()) return;

  edm::Handle<std::vector<reco::VertexCompositePtrCandidate>> secondaryVertices;
  iEvent.getByToken(secondaryVertexToken_, secondaryVertices);

  irun       = iEvent.id().run();
  ievent     = iEvent.id().event();
  ilumiblock = iEvent.id().luminosityBlock();
  itime      = iEvent.time();

  run   = static_cast<size_t>(irun);
  event = static_cast<size_t>(ievent);
  lumi  = static_cast<size_t>(ilumiblock);
  time  = static_cast<size_t>((iEvent.time().value()) >> 32);

  npv = vertices->size();
  nsv = secondaryVertices->size();

  if (nsv == 0) return;

  const reco::Vertex& primaryVertex = vertices->front(); #to get only the frst vertex
  const auto& sv = secondaryVertices->front();

  svPt = sv.pt();
  svX = sv.vertex().x();
  svY = sv.vertex().y();
  svZ = sv.vertex().z();
  svNormalizedChi2 = sv.vertexNormalizedChi2();
  svChi2 = sv.vertexChi2();
  svEta = sv.eta();
  svMass = sv.mass();
  svNumDaughters = sv.numberOfDaughters();
  svDxyError = sv.dxyError();
  svNdf = sv.vertexNdof();

  // Calculating dxy
  GlobalPoint svPosition(sv.vertex().x(), sv.vertex().y(), sv.vertex().z());
  GlobalPoint pvPosition(primaryVertex.position().x(), primaryVertex.position().y(), primaryVertex.position().z());

  double dx = svPosition.x() - pvPosition.x();
  double dy = svPosition.y() - pvPosition.y();
  svDxy = std::sqrt(dx * dx + dy * dy);

  tree->Fill();
}

void MiniAnalyzer::beginJob() {}

void MiniAnalyzer::endJob() {}

void MiniAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("vertices", edm::InputTag("offlinePrimaryVertices"));
  desc.add<edm::InputTag>("secondaryVertices", edm::InputTag("inclusiveSecondaryVertices"));
  descriptions.add("miniAnalyzer", desc);
}

DEFINE_FWK_MODULE(MiniAnalyzer);
