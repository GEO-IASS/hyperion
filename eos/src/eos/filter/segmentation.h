#ifndef EOS_FILTER_SEGMENTATION_H
#define EOS_FILTER_SEGMENTATION_H
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


/// \file segmentation.h
/// Provides a system to group image channels based on similarity, so it creates
/// segments where each segment has similar values throughout.

#include "eos/types.h"
#include "eos/mem/alloc.h"
#include "eos/ds/arrays.h"
#include "eos/ds/arrays2d.h"
#include "eos/svt/var.h"
#include "eos/svt/field.h"
#include "eos/time/progress.h"

namespace eos
{
 namespace filter
 {
//------------------------------------------------------------------------------
/// This outputs a segmentation based on iterativly grouping items based on there
/// eucledian distance in feature space. You give it a number of fields,
/// an output field for the segment numbers to be written into and input fields 
/// with scalars to provide the features, and finally a distance cutoff. It 
/// creates segments from every pixel in the image to start with, it then merges
/// segments that are less than the given distance from each other, each merge 
/// setting the segments features to the weighted average of the segments that 
/// went into it. The final output will be such that no neighbouring segments
/// are within the given distance of each other using there mean feature vector.
/// Implimented as a class, you create an instance, add the relevent fields,
/// call the run method, extract results and then delete it; an instance can not
/// be recycled.
class EOS_CLASS Segmenter
{
 public:
  /// &nbsp;
   Segmenter():cutoff(1.0),minMerge(0),minKill(0),avs(0),doWeight(false),segs(null<nat32*>()) {}

  /// &nbsp;
   ~Segmenter() {mem::Free(segs);}
  
  /// Sets the distance cutoff between segments merging/not merging. If not called
  /// it defaults to 1.
   void SetCutoff(real32 dist) {cutoff = dist;}

  /// This sets the minimum segment size, segments smaller than this size will be 
  /// merged with there closest segment, regardless of distance. Defaults to 0.
   void SetMinimum(nat32 min) {minMerge = min;}
   
  /// Any segment smaller than this value will be 'deleted'. Ushally left less 
  /// than minimum, as as to have no effect, but if larger than the value given 
  /// to SetMinimum then this mode kicks in. As all pixels must have a segment 
  /// assignment all such segments are merged into a segment 0.
  /// The special status of segment 0 allways exists if this is larger than 
  /// SetMinimum's value, regardless to if it contains any actual segments.
   void SetKillMin(nat32 min) {minKill = min;}

  /// This sets the number of averaging steps to do, the algorithm first merges
  /// all pixels that are within the given distance of each other, it then sets 
  /// each segment to its average feature vector and then merges all segments 
  /// within the given distance of each other using the averaged feature vector.
  /// This sets the number of averaging steps done, defaults to 0 so it only 
  /// does a basic merge.
   void SetAverageSteps(nat32 a) {avs = a;}


  /// This enables an extra constraint on merging two segments, that the average
  /// of a given field where the two segments meet should be greater than a given 
  /// value. This allows you to provide a weighting field that has high values 
  /// where discontinuities in the segmentation are likelly/wanted
  /// etc.
   void SetMergeWeight(const svt::Field<real32> & weight,real32 cutoff);

  /// This adds a field to the feature vector to be used, all entrys to the field
  /// will be multiplied by the given scale value before use. At least one field
  /// must be provided and they must all be 2D with the same sizes.
   void AddField(const svt::Field<real32> & in,real32 scale = 1.0);


  /// Calculates the result. After calling this do not call it again or any setup 
  /// methods, just output methods.
   void Run(time::Progress * prog = null<time::Progress*>());


  /// Returns the number of Segments generated by Run().
   nat32 Segments() const {return segments;}

  /// Outputs the calculated segment field, the input field must be 2D and the same size
  /// as the input fields.
   void GetOutput(svt::Field<nat32> & out) const;

  /// Outputs the calculated segment field, resizes as needed.
   void GetOutput(ds::Array2D<nat32> & out) const;


  /// &nbsp;
   inline cstrconst TypeString() const {return "eos::filter::Group";}


 private:
  // Input...
   real32 cutoff;
   nat32 minMerge;
   nat32 minKill;
   nat32 avs;

   bit doWeight;
   real32 weightMax;
   svt::Field<real32> weight;

   struct Feature
   {
    svt::Field<real32> field;
    real32 scale;
   };
   ds::Array<Feature> feat; // Array of all the features.

  // Output...
   nat32 segments; // How many segments were created.
   nat32 * segs; // Array of all the segment numbers, malloc'ed.


 // Internal stuff...
  // Data structure used throughout...
   struct Node
   {
    Node * Head() // Gets the head of the segment for which this node is a member, does path shortening.
    {
     if (parent==null<Node*>()) return this;
     parent = parent->Head();
     return parent;
    }

    Node *& Last() // For adding to the linked list.
    {
     if (next==null<Node*>()) return next;
                         else return next->Last();
    }

    Node * parent;
    Node * newParent; // Used in the two phases of merging, to store a new parent for a pixel base on the closest neighbour.
    real32 distance; // Used in the final merging proccess, to remove small clusters.
    bit stop; // Set true when all pixels arround this pixel are of the same segment, i.e. no work is possible so break early.
    nat32 segment; // The segment number+1, used during the segmentation phase, 0 indicates unassigned.
    nat32 weight; // Number of pixels that have gone into creating this.
    real32 edge; // The weight term I keep refering to in regards to edge enforcment. Yes, I designed this class originally without this feature, the names suck I know.
    Node * next; // The linked list of node members, only used when weighting is enabled.
    nat32 xPos; // The x and y position of the node, only used when weighting is enabled.
    nat32 yPos; // "
    real32 fv[1]; // First entry of the feature vector, we pad the structure with data as neccesary to allow for the correct sized vector.
   };

   Node * forest;

  // Useful numbers to know...
   nat32 width;
   nat32 height;
   nat32 nodeCount;
   nat32 nodeSize;


  // This method merges all segments in the data structure that are within the 
  // cutoff distance of each other for pixels at the edges...
   void MergeBasic();

  // Identical to MergeBasic, except it also does the weighting requirement,
  // an optional code path to replace MergeBasic when weighting is enabled.
   void MergeWeighted();

  // This method sets each segment to its average feature vector...
   void AverageFeatures();

  // This method merges in any small segments to the larger segments...
   void MergeSmallOnce();
   
  // This method merges all small segments into one, returning a pointer to the
  // head of this super-segment...
   Node * AggregateSmallOnce();
};

//------------------------------------------------------------------------------
 };
};
#endif
