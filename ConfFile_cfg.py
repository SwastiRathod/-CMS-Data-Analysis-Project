
import FWCore.ParameterSet.Config as cms

process = cms.Process("Demo")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.load("Configuration.EventContent.EventContent_cff")
process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')

#Why this line- write it here once u read
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string("QCD_Combined.root"))

#shud change it to -1 once the code works
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(-1))

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring([
                                'root://eospublic.cern.ch//eos/opendata/cms/mc/RunIISummer20UL16MiniAODv2/QCD_Pt_300to470_TuneCP5_13TeV_pythia8/MINIAODSIM/106X_mcRun2_asymptotic_v17-v1/120000/92A432E3-31F8-4549-ADA6-D2F7C14E709E.root',
                                'root://eospublic.cern.ch//eos/opendata/cms/mc/RunIISummer20UL16MiniAODv2/QCD_Pt_300to470_TuneCP5_13TeV_pythia8/MINIAODSIM/106X_mcRun2_asymptotic_v17-v1/130000/07A778BD-1363-F740-A9A9-27FE78D771B3.root',
                                'root://eospublic.cern.ch//eos/opendata/cms/mc/RunIISummer20UL16MiniAODv2/QCD_Pt_470to600_TuneCP5_13TeV_pythia8/MINIAODSIM/106X_mcRun2_asymptotic_v17-v1/120000/10325CC6-EC2F-F142-B732-BC7693B00E48.root',
                                'root://eospublic.cern.ch//eos/opendata/cms/mc/RunIISummer20UL16MiniAODv2/QCD_Pt_470to600_TuneCP5_13TeV_pythia8/MINIAODSIM/106X_mcRun2_asymptotic_v17-v1/130000/06D647E8-AF59-A244-BBFA-9DF55CBABE1B.root'
                     ]))

process.demo = cms.EDAnalyzer('MiniAnalyzer',
                              vertices = cms.InputTag("offlineSlimmedPrimaryVertices"),
                              secondaryVertices = cms.InputTag("slimmedSecondaryVertices"))

process.p = cms.Path(process.demo)

# Schedule definition shud be added for this why - to define schedule othws by default this will be executed
process.schedule = cms.Schedule(process.p)
