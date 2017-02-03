/***************************************************************************//**

  \file   animate.scad

  \author Roy Allen Sutton
  \date   2016-2017

  \copyright

    This file is part of OpenSCAD AutoMake Utilities ([openscad-amu]
    (https://github.com/royasutton/openscad-amu)).

    openscad-amu is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    openscad-amu is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    [GNU General Public License] (https://www.gnu.org/licenses/gpl.html)
    for more details.

    You should have received a copy of the GNU General Public License
    along with openscad-amu.  If not, see <http://www.gnu.org/licenses/>.

  \brief

    Animation demonstration.

  \details

*******************************************************************************/

module wheel( diameter=15, width=7, wallheight=6, tirecolor="black", rimcolor="white" ) {
  color( rimcolor ) cylinder(h=width, d=diameter, center=true);
  color( tirecolor ) difference() {
    cylinder(h=width-0.02, d=diameter+wallheight*2, center=true);
    for (o=[-1,0,1]) translate([0,0,width/4*o])
    cylinder(h=width/10, d=(diameter+wallheight*2)*101/100, center=true);
  }
  color("white") for (o=[-1,0,1]) translate([0,0,width/4*o])
  cylinder(h=width/10, d=(diameter+wallheight*2)*98/100, center=true);
}

module cabin( size, cabcolor ) {
  color( cabcolor ) cube([size[0], size[1], size[2]], center=true);
  for (x=[-1,1]) translate(x*[size[0]/2, 0, 0])
  color( "whitesmoke" ) cube([1/2, size[1]*90/100, size[2]*90/100], center=true);
}

module vehicle( type="car", bodycolor="yellow", cabcolor="lightgray",
                width=50, height=40, length=100, wheels=15 ) {
  body_hp = 55/100;

  color( bodycolor ) cube([length, width, height*body_hp], center=true);
  for (l=[-1,1]) for (w=[-1,1])
    translate([l*(length/2-wheels), w*width/2, -height*body_hp*50/200])
    rotate([90, 0, 0])
    wheel(diameter=wheels, width=wheels/2, wallheight=wheels/2);

  color( "yellow" ) for (w=[-1,1])
    translate([-length/2, w*width*70/200, 0]) rotate([0,90,0])
    cylinder(h=5, d=10, center=true);

  color( "red" ) for (w=[-1,1])
    translate([+length/2, w*width*60/200, 0]) rotate([0,90,0])
    cube([5, 15, 2], center=true);

  if (type == "car") {
    translate([length/40, 0, height/2])
    cabin([length*50/100, width, height*(1-body_hp)], cabcolor);
  }

  if (type == "truck") {
    translate([-length/5, 0, height/2])
    cabin([length*35/100, width, height*(1-body_hp)], cabcolor);
  }

  if (type == "van") {
    translate([length*5/200, 0, height/2])
    cabin([length*90/100, width, height*(1-body_hp)], cabcolor);
  }
}


////////////////////////////////////////////////////////////////////////////////
// end of library
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**

Begin_Scope move;
  Begin_OpenSCAD;
    use <animate.scad>;
    position=0;
    translate([position,0,0]) vehicle();
  End_OpenSCAD;

  Begin_MFScript;
    images  name "size" aspect "4:3" Wsizes "640";
    views   name "view" distance "200" Views "front";
    defines name "move" define "position" sequence 150 -1 -150;

    variables
      set_Makefile "${__MAKE_FILE__}"  add_Depend "${__MAKE_FILE__}"
      set_Source "${__SCOPE_FILE__}"   set_Prefix "${__PREFIX__}"

      set_Ext "png"
      set_Opts "--preview --projection=o"
      set_Opts_Combine "size view move";

    script
      Begin_Makefile
        Include_Copy "${__AMU_INCLUDE_PATH__}/parallel_jobs.mk"
        Summary  Tables  Targets  Menu
      End_Makefile;
  End_MFScript;
End_Scope;

Begin_Scope rotate;
  Begin_OpenSCAD;
    use <animate.scad>;
    vehicle();
  End_OpenSCAD;

  Begin_MFScript;
    images name "size" aspect "4:3" Wsizes "640";
    views_rotate name "view" distance "300"
                 elevation above angle0 0 angle1 360 delta 1 number;

    variables
      set_Makefile "${__MAKE_FILE__}"  add_Depend "${__MAKE_FILE__}"
      set_Source "${__SCOPE_FILE__}"   set_Prefix "${__PREFIX__}"

      set_Ext "png"
      set_Opts "--preview --projection=o"
      set_Opts_Combine "size view";

    script
      Begin_Makefile
        Include_Copy "${__AMU_INCLUDE_PATH__}/parallel_jobs.mk"
        Summary  Tables  Targets  Menu
      End_Makefile;
  End_MFScript;
End_Scope;

*******************************************************************************/


////////////////////////////////////////////////////////////////////////////////
// end of file
////////////////////////////////////////////////////////////////////////////////
