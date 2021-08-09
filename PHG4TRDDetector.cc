#include "PHG4TRDDetector.h"

#include <g4main/PHG4Detector.h>                // for PHG4Detector

#include <Geant4/G4RotationMatrix.hh>
#include <Geant4/G4PVDivision.hh>
#include <Geant4/G4Tubs.hh>
#include <Geant4/G4Colour.hh>
#include <Geant4/G4LogicalVolume.hh>
#include <Geant4/G4Material.hh>
#include <Geant4/G4VPhysicalVolume.hh>          // for G4VPhysicalVolume
#include <Geant4/G4PVPlacement.hh>
#include <Geant4/G4SystemOfUnits.hh>
#include <Geant4/G4ThreeVector.hh>              // for G4ThreeVector
#include <Geant4/G4VisAttributes.hh>

#include <cmath>
#include <iostream>                             // for operator<<, endl, bas...

class PHCompositeNode;
class PHG4Subsystem;

using namespace std;


PHG4TRDDetector::PHG4TRDDetector(PHG4Subsystem *subsys, PHCompositeNode *Node, PHParameters *params, const std::string &dnam, const int lyr)
  : PHG4Detector(subsys, Node, dnam)
  ,Logic(nullptr)
  ,fLogicRadiator(nullptr)
  ,TRD_det_Logic(nullptr)
  , m_Params(params)
  ,m_Layer(lyr)
{

}

  //_______________________________________________________________
  //_______________________________________________________________
  int PHG4TRDDetector::IsInTRD(G4VPhysicalVolume *volume) const
  {
if (volume == fPhysicsRadiator || volume == TRD_det_Phys)
  {
return true;
}
return false;

}

    //_______________________________________________________________
    void PHG4TRDDetector::ConstructMe(G4LogicalVolume *logicWorld)
    {
//Is this going to be mother volume 
//Original gemetry says "Create a global volumes for your detector" 
G4Material *TRDMaterial = G4Material::GetMaterial(m_Params->get_string_param("material"));

double RIn = m_Params->get_double_param("RIn")*cm; // 20.* cm
double ROut = m_Params->get_double_param("ROut")*cm; // 200 * cm
double ThicknessZ = m_Params->get_double_param("ThicknessZ")*cm; // 30. *cm
double PosZ = m_Params->get_double_param("PosZ")*cm; // none defined


G4Tubs* Solid = new G4Tubs("TRD_GVol_Solid", RIn, ROut, ThicknessZ / 2., 0., 360 * deg);
Logic = new G4LogicalVolume(Solid, TRDMaterial, "TRD_GVol_Logic");
G4VisAttributes *attr_TRD_GVol = new G4VisAttributes(G4Color(0.3, 0.5, 0.9, 0.9));
attr_TRD_GVol->SetColor(G4Color::Green());
attr_TRD_GVol->SetForceSolid(true);
Logic->SetVisAttributes(attr_TRD_GVol);
Phys =  new G4VPlacement(0, G4ThreeVector(0, 0, PosZ), Logic, "H_CAP_TRD_Physics", logicWorld, 0, false, OverlapCheck() );


//Radiator construction ---------
double det_RIn = m_Params->get_double_param("det_RIn")*cm;
double det_ROut = m_Params->get_double_param("det_ROut")*cm;
//Phase space distribution of radiators -------------
double fGasGap = 0.600*mm ; // for ZEUS 300 publication
double fDetGap = 0.01 * mm;
double fRadThickness = 0.020 * mm;    // 16 um // ZEUS NIMA 323 (1992) 135-139, D=20um, dens.= 0.1 g/cm3
double fRadThick = 10. * cm - fGasGap + fDetGap;
int fFoilNumber = 0;
fFoilNumber = fRadThick / (fRadThickness + fGasGap);
double fRadZ /*= m_Params->get_double_param("fRadZ")*cm*/; // none defined
fRadZ = -ThicknessZ / 2. + fRadThick / 2. + 2 * cm;
double  foilGasRatio = fRadThickness / (fRadThickness + fGasGap);

// ------ Define materials for radiator----------
G4Material *Mylar = fMat->GetMaterial("Mylar");
G4Material *Air = fMat->GetMaterial("Air");
G4Material *Al = fMat->GetMaterial("Al");
G4Material *CH2 = fMat->GetMaterial("CH2");
G4Material *He = fMat->GetMaterial("He");

double foilDensity = 0.91 * g / cm3;  // CH2 1.39*g/cm3; // Mylar //  0.534*g/cm3; //Li
double gasDensity = 1.2928 * mg / cm3; // Air // 1.977*mg/cm3; // CO2 0.178*mg/cm3; // He
double totDensity = foilDensity * foilGasRatio + gasDensity * (1.0 - foilGasRatio);
double fractionFoil = foilDensity * foilGasRatio / totDensity;
double fractionGas = gasDensity * (1.0 - foilGasRatio) / totDensity;

double fractionFoil = foilDensity * foilGasRatio / totDensity;
double fractionGas = gasDensity * (1.0 - foilGasRatio) / totDensity;
G4Material *radiatorMat0 = new G4Material("radiatorMat0", totDensity, 2);
radiatorMat0->AddMaterial(CH2, fractionFoil);
radiatorMat0->AddMaterial(Air, fractionGas);
double NewDensity = 0.083 * (g / cm3);
G4Material *radiatorMat = new G4Material("radiatorMat", NewDensity, 1);
radiatorMat->AddMaterial(radiatorMat0, 1.);

// default materials of the detector and TR radiator
G4Material *fRadiatorMat = radiatorMat;
G4Material *fFoilMat = CH2; // Kapton; // Mylar ; // Li ; // CH2 ;
G4Material *fGasMat = Air; // CO2; // He; //
//------------------------end --material -------------------------------


//Define all sorts of G4 volumes 
G4Tubs *fSolidRadiator = new G4Tubs("TRD_Radiator_Solid", det_RIn, det_ROut, 0.5 *fRadThick, 0., 360 * deg);
fLogicRadiator = new G4LogicalVolume(fSolidRadiator, fRadiatorMat, "TRD_Radiator_Logic"); 

G4VisAttributes *TRD_rad_att = new G4VisAttributes();
TRD_rad_att->SetColour(G4Colour::Blue());
TRD_rad_att->SetVisibility(true);
TRD_rad_att->SetForceSolid(true);
fLogicRadiator->SetVisAttributes(TRD_rad_att);

fPhysicsRadiator =  new G4PVPlacement(0, G4ThreeVector(0, 0, fRadZ),  fLogicRadiator,  "TRD_Radiator_Phys", Phys, false, 0 , OverlapCheck() ); // Placing the Radiator in global detector volume "Phys". Phys has been placed in world volume earlier

G4Region *fRadRegion;
 delete fRadRegion;
 if (fRadRegion == nullptr) fRadRegion = new G4Region("XTRradiator");
 fRadRegion->AddRootLogicalVolume(fLogicRadiator);

 // Absorber (Xe gas with MPGD)


 double det_ThicknessZ =2.5*cm;
 double det_Pos_Z = fRadZ+ fRadThickness/2. + det_ThicknessZ/2. ;
 G4Material *det_Material = fMat->GetMaterial("Xe20CO2");

 G4Tubs *TRD_det_Solid = new G4Tubs("TRD_det_Solid", det_RIn, det_ROut, det_ThicknessZ / 2., 0., 360 * deg);;
 TRD_det_Logic =  new G4LogicalVolume(TRD_det_Solid, det_Material, "TRD_det_Logic");
 G4VisAttributes *TRD_det_att = new G4VisAttributes();
 TRD_det_att->SetColour(G4Colour::Grey());
 TRD_det_att->SetVisibility(true);
 TRD_det_att->SetForceSolid(true);
 TRD_det_Logic->SetVisAttributes(TRD_det_att);

 TRD_det_Phys =  new G4PVPlacement(0, G4ThreeVector(0, 0, det_Pos_Z),  TRD_det_Logic,  "TRD_det_Phys", Phys, false, 0 , OverlapCheck() ); // Placing the MPGD with Xe in global detector volume "Phys". Phys has been placed in world volume earlier

 G4Region *fRegGasDet;
 if (fRegGasDet != 0) delete fRegGasDet;
 if (fRegGasDet == 0) fRegGasDet = new G4Region("XTRdEdxDetector");
 fRegGasDet->AddRootLogicalVolume(TRD_det_Logic);

    }
