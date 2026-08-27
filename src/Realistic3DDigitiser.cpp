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

          // Outputs
          {
              KeyValues("OutputHits",
                        {"VXDBarrelHits"})
          }) {

}
StatusCode Realistic3DDigitiser::initialize() {

    m_geoSvc = serviceLocator()->service(m_geoSvcName);

    if (!m_geoSvc) {
        error() << "Cannot retrieve GeoSvc" << endmsg;
        return StatusCode::FAILURE;
    }

    // TODO: look up the 3D-sensor geometry/readout parameters for
    // m_subDetName from m_geoSvc here 

    info() << "Initialization successful" << endmsg;

    return StatusCode::SUCCESS;
}
edm4hep::TrackerHitPlaneCollection
Realistic3DDigitiser::operator()(
        const edm4hep::SimTrackerHitCollection& simHits,
        const edm4hep::EventHeaderCollection& headers) const {

    auto output = edm4hep::TrackerHitPlaneCollection();

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
        //   - group/cluster SimTrackerHits using a 3D sensor charge-
        //     collection model
        //   - apply realistic position smearing based on the 3D-sensor
        //     resolution model
        auto outHit = output.create();

        outHit.setCellID(hit.getCellID());
        outHit.setPosition(hit.getPosition());
        outHit.setTime(hit.getTime());
        outHit.setEDep(hit.getEDep());

    }

    return output;
}
