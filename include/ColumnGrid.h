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

  // Bias/ohmic electrodes at the four corners of each pixel cell
  bool biasElectrodesEnabled = false;
  double biasColumnRadius   = 0.0025;  // mm

  // the readout column may not span the full sensor thickness.
  // columnGap is the portion of the thickness measured from
  // z = -halfThickness that the readout column does not reach
  double columnGap = 0.0;  // mm

  // Nearest readout-column position to a given (x,y)
  void NearestColumn(double x, double y, double& colX, double& colY) const {
    int ix = static_cast<int>(std::floor((x - offsetX) / pitchX + 0.5));
    int iy = static_cast<int>(std::floor((y - offsetY) / pitchY + 0.5));
    colX = offsetX + ix * pitchX;
    colY = offsetY + iy * pitchY;
  }

  // Lateral (x,y-plane only) drift distance from (x,y) to the nearest
  // readout column
  double DistanceToNearestColumn(double x, double y) const {
    double colX, colY;
    NearestColumn(x, y, colX, colY);
    return std::sqrt((x - colX) * (x - colX) + (y - colY) * (y - colY));
  }

  // drift distance to the nearest readout column, for a point
  // at (x,y,z) in a layer of the given halfThickness. If z falls in the gap 
  // the point must additionally travel the depth distance to reach the column's end
  double DistanceToNearestColumn3D(double x, double y, double z, double halfThickness) const {
    double lateral = DistanceToNearestColumn(x, y);
    double zBottom = -halfThickness + columnGap;
    double zClamped = (z < zBottom) ? zBottom : z; // column top is always at +halfThickness
    double dz = z - zClamped;
    return std::sqrt(lateral * lateral + dz * dz);
  }

  // Nearest bias/ohmic electrode to a given (x,y).
  // Readout columns sit at cell centres (offsetX + ix*pitchX); the cell
  // corners are half a pitch away from that
  void NearestBiasElectrode(double x, double y, double& biasX, double& biasY) const {
    int ix = static_cast<int>(std::floor(x / pitchX + 0.5));
    int iy = static_cast<int>(std::floor(y / pitchY + 0.5));
    biasX = ix * pitchX;
    biasY = iy * pitchY;
  }

  double DistanceToNearestBiasElectrode(double x, double y) const {
    double biasX, biasY;
    NearestBiasElectrode(x, y, biasX, biasY);
    return std::sqrt((x - biasX) * (x - biasX) + (y - biasY) * (y - biasY));
  }

  // True if (x,y) falls inside any electrode's physical material
  // readout column or a corner bias column.
  bool InColumnDeadZone(double x, double y) const {
    if (DistanceToNearestColumn(x, y) < columnRadius) return true;
    if (biasElectrodesEnabled && DistanceToNearestBiasElectrode(x, y) < biasColumnRadius) return true;
    return false;
  }

  // a point beyond the readout column's reach (z < -halfThickness + columnGap) 
  // is NOT inside column material, so that volume is normal active silicon.
  bool InColumnDeadZone3D(double x, double y, double z, double halfThickness) const {
    double zBottom = -halfThickness + columnGap;
    bool insideReadoutColumn = (z >= zBottom) && (DistanceToNearestColumn(x, y) < columnRadius);
    if (insideReadoutColumn) return true;
    if (biasElectrodesEnabled && DistanceToNearestBiasElectrode(x, y) < biasColumnRadius) return true;
    return false;
  }
};

#endif
