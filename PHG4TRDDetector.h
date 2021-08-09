#ifndef G4DETECTORS_PHG4TRDDETECTOR_H
#define G4DETECTORS_PHG4TRDDETECTOR_H

#include <g4main/PHG4Detector.h>

#include <cmath>
#include <set>
#include <string>

class G4LogicalVolume;
class G4VPhysicalVolume;
class PHCompositeNode;
class PHG4Subsystem;
class PHParameters;

class PHG4TRDDetector : public PHG4Detector
{
public: 
//!constructor
  PHG4TRDDetector(PHG4Subsystem *subsys, PHCompositeNode *Node, PHParameters *params, const std::string &dnam = "TRD");

 //! destructor
  ~PHG4TRDDetector() override
  {
  }

  //! construct TRD
  void ConstructMe(G4LogicalVolume *world) override;
  
  bool IsInTRD(const G4VPhysicalVolume *) const;
  void SuperDetector(const std::string &name) { m_SuperDetector = name; }
  const std::string SuperDetector() const { return m_SuperDetector; }
  int get_Layer() const { return m_Layer; }
  

 private:
  PHParameters *m_Params;

  G4LogicalVolume *Logic;
  G4LogicalVolume *fLogicRadiator;
  G4LogicalVolume *TRD_det_Logic;
  
  G4VPhysicalVolume *Phys ;
  G4VPhysicalVolume *TRD_det_Phys;
  G4VPhysicalVolume *fPhysicsRadiator;
 

  
  std::string m_SuperDetector;
};

#endif
