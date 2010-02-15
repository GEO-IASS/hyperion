#ifndef CYCLOPS_CROP_H
#define CYCLOPS_CROP_H
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


#include "cyclops/main.h"

//------------------------------------------------------------------------------
// Allows you to open an image and its intrinsic calibration, it then turns the 
// camera into a 2D protractor...
class Crop
{
 public:
   Crop(Cyclops & cyclops);
  ~Crop();


 private:
  Cyclops & cyclops;

  gui::Window * win;
  gui::Canvas * canvas;

  svt::Var * imgVar;
  svt::Field<bs::ColRGB> img;

  svt::Var * imageVar;
  svt::Field<bs::ColourRGB> image;


  math::Vect<2,real32> pMin;
  math::Vect<2,real32> pMax;


  void Quit(gui::Base * obj,gui::Event * event);

  void Resize(gui::Base * obj,gui::Event * event);
  void Click(gui::Base * obj,gui::Event * event);
  void Move(gui::Base * obj,gui::Event * event);

  void LoadImage(gui::Base * obj,gui::Event * event);

  void SaveImage(gui::Base * obj,gui::Event * event);
  void UpdateIntrinsic(gui::Base * obj,gui::Event * event);

  
  void RenderLine(const math::Vect<2,real32> & a,const math::Vect<2,real32> & b,const bs::ColourRGB & col);
};

//------------------------------------------------------------------------------
#endif
