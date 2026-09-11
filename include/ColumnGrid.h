#ifndef ColumnGrid_h
#define ColumnGrid_h 1

#include <cmath>

// ATLAS ITk 3D design: one readout electrode at the centre
// of each pixel cell (arXiv:2605.25670, PoS(VERTEX2023)072).

// readout electrode pitch = pixel pitch (one electrode per pixel)

// Default values, which can be overriden via the Gaudi::Property in the
// digitiser (ColumnRadius, ColumnPitchX/Y)

struct ColumnGrid {

  double pitchX      = 0.025;   // distance between columns along x (same direction as PixelSizeX) in mm
  double pitchY       = 0.025;   // distance between columns along y (same direction as PixelSizeY) in mm
  double columnRadius = 0.0025;  // size of the electrode in mm
  double offsetX       = 0.0;     // offset in mm, if not centred on (0,0)
  double offsetY       = 0.0;     // offset in mm, if not centred on (0, 0)

  // Nearest readout-column position to a given (x,y)
  void NearestColumn(double x, double y, double& colX, double& colY) const {
    int ix = static_cast<int>(std::floor((x - offsetX) / pitchX + 0.5));
    int iy = static_cast<int>(std::floor((y - offsetY) / pitchY + 0.5));
    colX = offsetX + ix * pitchX;
    colY = offsetY + iy * pitchY;
  }

  // Lateral drift distance from (x,y) to the nearest readout column
  double DistanceToNearestColumn(double x, double y) const {
    double colX, colY;
    NearestColumn(x, y, colX, colY);
    return std::sqrt((x - colX) * (x - colX) + (y - colY) * (y - colY));
  }

  // True if (x,y) falls inside the electrode material
  bool InColumnDeadZone(double x, double y) const {
    return DistanceToNearestColumn(x, y) < columnRadius;
  }
};

#endif
