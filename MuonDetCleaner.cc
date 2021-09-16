#include "TauAnalysis/MCEmbeddingTools/plugins/MuonDetCleaner.h"

#include "TrackingTools/TrackAssociator/plugins/MuonDetIdAssociator.h"

#include "FWCore/Framework/interface/ESHandle.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"

#include "Geometry/CSCGeometry/interface/CSCGeometry.h"
#include "Geometry/DTGeometry/interface/DTGeometry.h"
#include "Geometry/RPCGeometry/interface/RPCGeometry.h"

#include "DataFormats/MuonDetId/interface/DTLayerId.h"
#include "DataFormats/DTRecHit/interface/DTSLRecCluster.h"
#include "DataFormats/DTRecHit/interface/DTRecHit1DPair.h"

#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/MuonDetId/interface/CSCDetId.h"
#include "DataFormats/CSCRecHit/interface/CSCRecHit2D.h"
#include "DataFormats/MuonDetId/interface/RPCDetId.h"
#include "DataFormats/RPCRecHit/interface/RPCRecHit.h"


typedef MuonDetCleaner<CSCDetId, CSCRecHit2D> CSCRecHitColCleaner;
typedef MuonDetCleaner<DTLayerId, DTRecHit1DPair> DTRecHitColCleaner;
typedef MuonDetCleaner<RPCDetId, RPCRecHit> RPCRecHitColCleaner;

typedef edm::ESHandle<CSCGeometry> CSCGeo;
typedef edm::ESHandle<DTGeometry> DTGeo;
typedef edm::ESHandle<RPCGeometry> RPCGeo;

//-------------------------------------------------------------------------------
// define 'getDetIds' functions used for different types of recHits
//-------------------------------------------------------------------------------


template <typename T1, typename T2>
uint32_t MuonDetCleaner<T1,T2>::getRawDetId(const T2& recHit)
{
  assert(0); // CV: make sure general function never gets called;
             //     always use template specializations
}

template <>
uint32_t MuonDetCleaner<CSCDetId, CSCRecHit2D>::getRawDetId(const CSCRecHit2D& recHit)
{
  return recHit.cscDetId().rawId();
}

template <>
uint32_t MuonDetCleaner<DTLayerId, DTRecHit1DPair>::getRawDetId(const DTRecHit1DPair& recHit)
{
  return recHit.geographicalId().rawId();
}

template <>
uint32_t MuonDetCleaner<RPCDetId, RPCRecHit>::getRawDetId(const RPCRecHit& recHit)
{
  return recHit.rpcId().rawId();
}


//-------------------------------------------------------------------------------
// return geometry of detector
//-------------------------------------------------------------------------------

template <typename T1, typename T2>
auto MuonDetCleaner<T1,T2>::getGeometry(const edm::EventSetup& iSetup)
{
  assert(0);
}

template<>
auto MuonDetCleaner<CSCDetId, CSCRecHit2D>::getGeometry(const edm::EventSetup& iSetup)
{
  CSCGeo theGeometry;
  iSetup.get<MuonGeometryRecord>().get(theGeometry);
  return theGeometry;
}

template<>
auto MuonDetCleaner<DTLayerId, DTRecHit1DPair>::getGeometry(const edm::EventSetup& iSetup)
{
  DTGeo theGeometry;
  iSetup.get<MuonGeometryRecord>().get(theGeometry);
  return theGeometry;
}

template<>
auto MuonDetCleaner<RPCDetId, RPCRecHit>::getGeometry(const edm::EventSetup& iSetup)
{
  RPCGeo theGeometry;
  iSetup.get<MuonGeometryRecord>().get(theGeometry);
  return theGeometry;
}


//-------------------------------------------------------------------------------
// return global position of recHits
//-------------------------------------------------------------------------------


template <typename T1, typename T2>
GlobalPoint MuonDetCleaner<T1,T2>::getglobalPosition(const T2& recHit, const edm::EventSetup& iSetup)
{ 
  assert(0); 
}

template <>
GlobalPoint MuonDetCleaner<CSCDetId, CSCRecHit2D>::getglobalPosition(const CSCRecHit2D& recHit, const edm::EventSetup& iSetup)
{
  CSCGeo geom = getGeometry(iSetup); 
  const CSCLayer *layer = dynamic_cast<const CSCLayer *>(geom->layer(recHit.cscDetId()));
  const Plane surface = layer->surface();
  return surface.toGlobal(recHit.localPosition());
}

template <>
GlobalPoint MuonDetCleaner<DTLayerId, DTRecHit1DPair>::getglobalPosition(const DTRecHit1DPair& recHit, const edm::EventSetup& iSetup)
{
  DTGeo geom = getGeometry(iSetup);
  const DTChamber *chamber = dynamic_cast<const DTChamber *>(geom->chamber(recHit.geographicalId()));
  const Plane surface = chamber->surface();
  return surface.toGlobal(recHit.localPosition());  
}

template <>
GlobalPoint MuonDetCleaner<RPCDetId, RPCRecHit>::getglobalPosition(const RPCRecHit& recHit, const edm::EventSetup& iSetup)
{
  RPCGeo geom = getGeometry(iSetup);
  const RPCRoll *roll = dynamic_cast<const RPCRoll *>(geom->roll(recHit.rpcId()));
  const Plane surface = roll->surface();
  return surface.toGlobal(recHit.localPosition());
}

//-------------------------------------------------------------------------------
// check delta R between outer hit from track and recHit
//-------------------------------------------------------------------------------

template <typename T1, typename T2>
bool MuonDetCleaner<T1,T2>::checkTrackVicinity(const GlobalPoint& recHit, const reco::Track& track)
{
  assert(0);
}

template <>
bool MuonDetCleaner<CSCDetId, CSCRecHit2D>::checkTrackVicinity(const GlobalPoint& recHit, const reco::Track& track)
{
  double deta = recHit.eta() - track.outerEta();
  double dphi = recHit.phi() - track.outerPhi();
  double dR = TMath::Sqrt(deta*deta + dphi*dphi);
  if(dR<0.4) return true;
  else return false;
}

template <>
bool MuonDetCleaner<DTLayerId, DTRecHit1DPair>::checkTrackVicinity(const GlobalPoint& recHit, const reco::Track& track)
{
  double deta = recHit.eta() - track.outerEta();
  double dphi = recHit.phi() - track.outerPhi();
  double dR = TMath::Sqrt(deta*deta + dphi*dphi);
  if(dR<0.4) return true;
  else return false;
}

template <>
bool MuonDetCleaner<RPCDetId, RPCRecHit>::checkTrackVicinity(const GlobalPoint& recHit, const reco::Track& track)
{
  double deta = recHit.eta() - track.outerEta();
  double dphi = recHit.phi() - track.outerPhi();
  double dR = TMath::Sqrt(deta*deta + dphi*dphi);
  if(dR<0.4) return true;
  else return false;
}

//-------------------------------------------------------------------------------
// find out what the kind of RecHit used by imput muons rechit
//-------------------------------------------------------------------------------


template <typename T1, typename T2>
bool MuonDetCleaner<T1,T2>::checkrecHit(const TrackingRecHit& recHit)
{
  edm::LogError("TauEmbedding")<<"!!!! Please add the checkrecHit for the individual class templates "
  assert(0);
}


template <>
bool MuonDetCleaner<CSCDetId, CSCRecHit2D>::checkrecHit(const TrackingRecHit& recHit)
{	    
   const std::type_info &hit_type = typeid(recHit);
   if (hit_type == typeid(CSCSegment))  {return true;}  // This should be the default one (which are included in the global (outer) muon track)
   else if (hit_type == typeid(CSCRecHit2D)) {return true;}
   //else {std::cout<<"else "<<hit_type.name()<<std::endl;}    
   return false;
}


template <>
bool MuonDetCleaner<DTLayerId, DTRecHit1DPair>::checkrecHit(const TrackingRecHit& recHit)
{	    
   const std::type_info &hit_type = typeid(recHit);
   if (hit_type == typeid(DTRecSegment4D))  {return true;}  // This should be the default one (which are included in the global (outer) muon track)
   else if (hit_type == typeid(DTRecHit1DPair)) {return true;}
   else if (hit_type == typeid(DTSLRecCluster)) {return true; }
   else if (hit_type == typeid(DTSLRecSegment2D)) {return true; }
  // else {std::cout<<"else "<<hit_type.name()<<std::endl;}	    
   return false;
}


template <>
bool MuonDetCleaner<RPCDetId, RPCRecHit>::checkrecHit(const TrackingRecHit& recHit)
{	    
   const std::type_info &hit_type = typeid(recHit);
   if (hit_type == typeid(RPCRecHit))  {return true;}  // This should be the default one (which are included in the global (outer) muon track)
   //else {std::cout<<"else "<<hit_type.name()<<std::endl;}	    
   return false;
}



DEFINE_FWK_MODULE(CSCRecHitColCleaner);
DEFINE_FWK_MODULE(DTRecHitColCleaner);
DEFINE_FWK_MODULE(RPCRecHitColCleaner);
