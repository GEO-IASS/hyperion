#ifndef EOS_STEREO_PLANE_SEG_H
#define EOS_STEREO_PLANE_SEG_H
//------------------------------------------------------------------------------
// Copyright 2006 Tom Haines

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.


/// \file plane_seg.h
/// Given a disparity map with validity information and segments this 
/// fits a plane to each segment, providing an interface to this structure.
/// At any time you can then request the disparity map as generated by the
/// planes.

#include "eos/types.h"
#include "eos/svt/var.h"
#include "eos/svt/field.h"
#include "eos/ds/arrays.h"
#include "eos/ds/lists.h"
#include "eos/bs/geo3d.h"
#include "eos/alg/fitting.h"

namespace eos
{
 namespace stereo
 {
//------------------------------------------------------------------------------
/// A class designed to suport plane-based stereo algorithms. You give it a
/// disparity map, segementation and validity information for the disparity map,
/// it then fits planes to each segment. You can then alter the object in
/// various ways, but ultimatly you can finally extract a disparity map 
/// generated directly from the planes.
class EOS_CLASS PlaneSeg
{
 public:
  /// &nbsp;
   PlaneSeg();
   
  /// &nbsp;
   ~PlaneSeg();
   
  
  /// Sets the disparity map, must exist for the duration of the class, so avoid
  /// Commit()-ing any changes to this, and other, maps. Must be provided.
   void SetDisparity(const svt::Field<real32> & disp);
   
  /// Sets the validty map for the disparity data. Must be provided.
   void SetValidity(const svt::Field<bit> & valid);
   
  /// Sets the segmentation map and the number of segments, must be provided.
   void SetSegs(nat32 segCount,const svt::Field<nat32> & segs);
  
  
  /// Generates the planes fitted to segments based on the data that must be in
  /// the class when this is called, see the setters for details. Each segment
  /// is fitted with outlier detection capability, so noise can be handled quite
  /// well. The details of how this is done are in the paper 'A layered stereo
  /// matching algorithm using image segmentation and global visibility constraints'
  /// by Michael Bleyer & Margrit Gelautz.
   void MakePlanes();
   
  /// Extracts a disparity map based on the planes assigned to each segment.
   void Extract(svt::Field<real32> & disp);
   
  
  /// Returns how many segments are avaliable from the getter methods.
   nat32 Segments() {return sed.Size();}
 
  /// Allows you to extract the plane for a given segment.
   void GetPlane(nat32 seg,bs::PlaneABC & out);
   
  /// Allows you to set the plane for a given segment. (So you can edit the segments
  /// before extracting a disparity map that uses them.)
   void SetPlane(nat32 seg,const bs::PlaneABC & in);
 
  /// Allows you to extract the center of mass for a given segment.
   void GetCenter(nat32 seg,real32 & outX,real32 & outY);

  /// Allows you to extract the area, in pixels, of a given segment.
   void GetArea(nat32 seg,nat32 & out);

  /// Given a LayerSeg this fills in the layerSeg from this, so its 
  /// working with the same data set.
   void PrepLayerSeg(class LayerSeg & out);


  /// &nbsp;
   inline cstrconst TypeString() const {return "eos::stereo::PlaneSeg";}


 private:
  // Constant parameters...
   static const real32 outlier = 1.0;
   static const real32 convergence = 10e-6;

  // Inputs...
   svt::Field<real32> disp;
   svt::Field<bit> valid;
   svt::Field<nat32> segs;
  
  // Outputs...
   struct Sed
   {
    bs::PlaneABC plane; // Plane fitted to segment.
    real32 x,y; // Center of segment.
    nat32 pix; // Number of valid pixels in segment.
    nat32 area; // Number of pixels in segment, regardless of validity.
   };
   ds::Array<Sed> sed; // Size of this is number of segments.

 // Internal...
  struct SedInt // Structure used during calculation in conjunction with above.
  {
   public:
    alg::PlaneFit planeFit;
    bit done; // true once we are satisfied with the quality of the fit.
  };
};

//------------------------------------------------------------------------------
/// A variation on the PlaneSeg object, the LayerSeg object is given a set of 
/// segments, plus the same data as LayerSeg, and then fits a plane to those 
/// segments, with the same outlier managment techneque. Essentially 'just'
/// a simpler version, but very useful for certain classes of algorithm when
/// used in conjunction with PlaneSeg.
class EOS_CLASS LayerSeg
{
 public:
  /// &nbsp;
   LayerSeg();

  /// &nbsp;
   ~LayerSeg();


  /// Sets the disparity map, must exist for the duration of the class, so avoid
  /// Commit()-ing any changes to this, and other, maps. Must be provided.
   void SetDisparity(svt::Field<real32> & disp);
   
  /// Sets the validty map for the disparity data. Must be provided.
   void SetValidity(svt::Field<bit> & valid);
   
  /// Sets the segmentation map and the number of segments, must be provided.
   void SetSegs(nat32 segCount, svt::Field<nat32> & segs);

  /// Adds a segment to the list used to obtain the plane, do not call this until
  /// after SetSegs has been called.
   void AddSegment(nat32 seg);

  /// Removes a segment from the list used to obtain the plane. Do not call before
  /// SetSegs has been called.
   void RemSegment(nat32 seg);


  /// Calculates the result. You can call this at any time, so you can edit the
  /// segment list then recalculate the plane. There must be at least one segment
  /// contained however.
   void Run();


  /// Obtains the result - a simple plane.
   void Result(bs::PlaneABC & plane);


  /// &nbsp;
   inline cstrconst TypeString() const {return "eos::stereo::LayerSeg";} 


 private:
  // Constant parameters...
   static const real32 outlier = 1.0;
   static const real32 convergence = 10e-6;

  // Inputs...
   svt::Field<real32> disp;
   svt::Field<bit> valid;
   svt::Field<nat32> segs;

   ds::Array<bit> gas; // A flag for each segment, indicating if it gives a shit or not.

  // Outputs...
   bs::PlaneABC plane; // The fitted plane.
};

//------------------------------------------------------------------------------
 };
};
#endif
