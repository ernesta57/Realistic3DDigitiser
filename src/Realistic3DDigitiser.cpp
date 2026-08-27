#include "Realistic3DDigitiser.h"

#include "DD4hep/Detector.h"

Realistic3DDigitiser::Realistic3DDigitiser(const std::string& name,
                         ISvcLocator* svcLoc)
    : MultiTransformer(
          name,
          svcLoc,

          // Inputs
          {
              KeyValues("InputHits",
                        {"VertexBarrelCollection"}),

              KeyValues("HeaderName",
                        {"EventHeader"})
          },

          {
              KeyValues("OutputCollectionName",
                        {"VXDBarrelHits"}),

              KeyValues("RelationColName",
                        {"VXDBarrelHitsRelations"}),

              KeyValues("RawHitsLinkColName",
                        {"VXDBarrelRawHitsRelations"}),

              KeyValues("SimHitLocCollectionName",
                        {"VertexBarrelHits_Passed"})
          }) {

}
StatusCode Realistic3DDigitiser::initialize() {

    info() << "Initialization successful" << endmsg;

    return StatusCode::SUCCESS;
}
std::tuple<edm4hep::TrackerHitPlaneCollection, edm4hep::TrackerHitSimTrackerHitLinkCollection,
           edm4hep::TrackerHitSimTrackerHitLinkCollection, edm4hep::SimTrackerHitCollection>
Realistic3DDigitiser::operator()(
        const edm4hep::SimTrackerHitCollection& simHits,
        const edm4hep::EventHeaderCollection& headers) const {

    auto outputHits = edm4hep::TrackerHitPlaneCollection();
    auto relCol = edm4hep::TrackerHitSimTrackerHitLinkCollection();
    auto rawHitsCol = edm4hep::TrackerHitSimTrackerHitLinkCollection();
    auto simHitLocCol = edm4hep::SimTrackerHitCollection();

    debug() << "Processing event "
            << headers[0].getEventNumber()
            << endmsg;

    for (const auto& hit : simHits) {

        //------------------------------------
        // User analysis
        //------------------------------------

        if (hit.getEDep() < m_energyCut)
            continue;

        // TODO:
        //   - ProduceIonisationPoints: can be reused
        //   - ProduceSignalPoints: The planar version computes diffusion 
        //     sigma from 'DistanceToPlane'; the 3D equivalent needs
        //     DistanceToNearestColumn instead
        //   - ProduceHits: pixel thresholding/clustering logic can
        //     likely be reused
        auto outHit = outputHits.create();

        outHit.setCellID(hit.getCellID());
        outHit.setPosition(hit.getPosition());
        outHit.setTime(hit.getTime());
        outHit.setEDep(hit.getEDep());

    }

    return std::make_tuple(std::move(outputHits), std::move(relCol), std::move(rawHitsCol), std::move(simHitLocCol));
}
