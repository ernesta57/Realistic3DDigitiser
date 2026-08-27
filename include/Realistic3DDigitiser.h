#ifndef REALISTIC3DDIGITISER_H
#define REALISTIC3DDIGITISER_H

#include "Gaudi/Property.h"
#include "k4FWCore/Transformer.h"

#include "edm4hep/EventHeaderCollection.h"
#include "edm4hep/SimTrackerHitCollection.h"
#include "edm4hep/TrackerHitPlaneCollection.h"
#include "edm4hep/TrackerHitSimTrackerHitLinkCollection.h"

struct Realistic3DDigitiser final
    : k4FWCore::MultiTransformer<
          std::tuple<edm4hep::TrackerHitPlaneCollection,
                     edm4hep::TrackerHitSimTrackerHitLinkCollection,
                     edm4hep::TrackerHitSimTrackerHitLinkCollection,
                     edm4hep::SimTrackerHitCollection>(
              const edm4hep::SimTrackerHitCollection&,
              const edm4hep::EventHeaderCollection&)> {

  Realistic3DDigitiser(const std::string& name,
              ISvcLocator* svcLoc);

  StatusCode initialize() override;

  std::tuple<edm4hep::TrackerHitPlaneCollection, edm4hep::TrackerHitSimTrackerHitLinkCollection,
             edm4hep::TrackerHitSimTrackerHitLinkCollection, edm4hep::SimTrackerHitCollection>
  operator()(const edm4hep::SimTrackerHitCollection& simHits,
             const edm4hep::EventHeaderCollection& headers) const override;

private:

  //---------------------------------------------
  // Configurable properties
  //---------------------------------------------

  Gaudi::Property<std::string> m_subDetName{
      this,
      "SubDetectorName",
      "VertexBarrel",
      "Subdetector name"};

  Gaudi::Property<double> m_energyCut{
      this,
      "EnergyCut",
      0.0,
      "Minimum deposited energy"};

  // TODO: For 3D, the equivalent needs a column pitch/pattern
  // to determine distance-to-nearest-column instead of distance-to-plane:
  
  // Gaudi::Property<double> m_columnPitch{this, "ColumnPitch", 0.05, "3D-sensor column spacing [mm]"};
  // Gaudi::Property<double> m_diffusionCoefficient{this, "DiffusionCoefficient", 0.05, "Charge diffusion coefficient"};

};

DECLARE_COMPONENT(Realistic3DDigitiser)

#endif
