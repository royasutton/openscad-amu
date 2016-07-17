/***************************************************************************//**

  \file   vehicle.scad

  \author Roy Allen Sutton
  \date   2016

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

    Simple vehicle library to demonstrate model design with embedded documentation.

  \details

    This Parameterized model consists of three simple OpenSCAD modules
    that generate very basic blocks that resemble cars, trucks, and vans.
    The library consists of a vehicle module, a cabin module, and a wheel
    module. Each module supports basic parameterization and is documented.

*******************************************************************************/

/***************************************************************************//**

  \mainpage Library Documentation

  \tableofcontents

  \section overview Overview

    \dot
      graph g { subgraph cluster_model { graph[style=solid];
          label="Example Vehicle Library\n(select a module)";
          node [shape=Mrecord];
          a [label="wheel" URL="\ref wheel"];
          b [label="cabin" URL="\ref cabin"];
          c [label="vehicle" URL="\ref vehicle"];
      } }
     \enddot

    With this library you will be able to make simple models that almost
    resemble vehicles. It does not matter if you prefer cars, trucks, or
    vans, you can instantiate whichever you prefer. You can even choose
    your wheel size and change your vehicles color.

    \warning  Vehicles constructed with this library __should not be
              constructed without additional design refinement!__

  \section modules OpenSCAD Library Modules

    - \ref wheel "Wheel Module Documentation".
    - \ref cabin "Cabin Module Documentation".
    - \ref vehicle "Vehicle Module Documentation".

  \section using Using the Library

    The basic vehicle is constructed with seven parameters as shown
    in the script segment below. Please review the \ref vehicle module
    documentation for more details on each configuration option.

    \dontinclude vehicle_test.scad
    \skip use
    \until vehicle_wheels );

    \note This library does not use global variables and therefore may be
          included using the \c use OpenSCAD command.

  \section views Vehicle Views

    \htmlonly
      <table> <caption id="Views">View from all angles</caption>
      <tr><th>Front <th>Right Side <th>Left Side
      <tr><td><img src=../png/vehicle_test_car_12_green_left_320x240.png>
          <td><img src=../png/vehicle_test_car_12_green_front_320x240.png>
          <td><img src=../png/vehicle_test_car_12_green_back_320x240.png></tr>
      <tr><th>Top <th>Bottom <th>Rear
      <tr><td><img src=../png/vehicle_test_car_12_green_top_320x240.png>
          <td><img src=../png/vehicle_test_car_12_green_bottom_320x240.png>
          <td><img src=../png/vehicle_test_car_12_green_right_320x240.png></tr>
      </table>
    \endhtmlonly

    \image latex vehicle_test_car_12_green_left_320x240.png "Front" width=2.5in
    \image latex vehicle_test_car_12_green_front_320x240.png "Right Side" width=2.5in
    \image latex vehicle_test_car_12_green_back_320x240.png "Left Side" width=2.5in
    \image latex vehicle_test_car_12_green_top_320x240.png "Top" width=2.5in
    \image latex vehicle_test_car_12_green_bottom_320x240.png "Bottom" width=2.5in
    \image latex vehicle_test_car_12_green_right_320x240.png "Rear" width=2.5in

  \section examples Examples

    Here are a few examples. All of these images are rendered from the
    design library scripts in the same file that also created this
    documentation. The images and Stereo Lithography (STL) models have been
    generated using OpenSCAD via the auxiliary scripts at the end of
    this file.

  \subsection example_12 Vehicles with 12in Wheels

    \htmlonly
      <table> <caption id="table">Green</caption>
      <tr><th>Cars <th>Trucks <th>Vans
      <tr><td><img src=../png/vehicle_test_car_12_green_diag_320x240.png>
          <td><img src=../png/vehicle_test_truck_12_green_diag_320x240.png>
          <td><img src=../png/vehicle_test_van_12_green_diag_320x240.png></tr>
      </table>
      <table> <caption id="table">Blue</caption>
      <tr><th>Cars <th>Trucks <th>Vans
      <tr><td><img src=../png/vehicle_test_car_12_blue_diag_320x240.png>
          <td><img src=../png/vehicle_test_truck_12_blue_diag_320x240.png>
          <td><img src=../png/vehicle_test_van_12_blue_diag_320x240.png></tr>
      </table>
    \endhtmlonly

    \image latex vehicle_test_car_12_green_diag_320x240.png "Green Car with 12in Wheels" width=2.5in
    \image latex vehicle_test_truck_12_green_diag_320x240.png "Green Truck with 12in Wheels" width=2.5in
    \image latex vehicle_test_van_12_green_diag_320x240.png "Green Van with 12in Wheels" width=2.5in
    \image latex vehicle_test_car_12_blue_diag_320x240.png "Blue Car with 12in Wheels" width=2.5in
    \image latex vehicle_test_truck_12_blue_diag_320x240.png "Blue Truck with 12in Wheels" width=2.5in
    \image latex vehicle_test_van_12_blue_diag_320x240.png "Blue Van with 12in Wheels" width=2.5in

  \subsection example_17 Vehicles with 17in Wheels

    \htmlonly
      <table> <caption id="table">Green</caption>
      <tr><th>Cars <th>Trucks <th>Vans
      <tr><td><img src=../png/vehicle_test_car_17_green_diag_320x240.png>
          <td><img src=../png/vehicle_test_truck_17_green_diag_320x240.png>
          <td><img src=../png/vehicle_test_van_17_green_diag_320x240.png></tr>
      </table>
      <table> <caption id="table">Blue</caption>
      <tr><th>Cars <th>Trucks <th>Vans
      <tr><td><img src=../png/vehicle_test_car_17_blue_diag_320x240.png>
          <td><img src=../png/vehicle_test_truck_17_blue_diag_320x240.png>
          <td><img src=../png/vehicle_test_van_17_blue_diag_320x240.png></tr>
      </table>
    \endhtmlonly

    \image latex vehicle_test_car_17_green_diag_320x240.png "Green Car with 17in Wheels" width=2.5in
    \image latex vehicle_test_truck_17_green_diag_320x240.png "Green Truck with 17in Wheels" width=2.5in
    \image latex vehicle_test_van_17_green_diag_320x240.png "Green Van with 17in Wheels" width=2.5in
    \image latex vehicle_test_car_17_blue_diag_320x240.png "Blue Car with 17in Wheels" width=2.5in
    \image latex vehicle_test_truck_17_blue_diag_320x240.png "Blue Truck with 17in Wheels" width=2.5in
    \image latex vehicle_test_van_17_blue_diag_320x240.png "Blue Van with 17in Wheels" width=2.5in

  \section downloads Downloads

  \subsection models STL Models

    - <a href="../stl/vehicle_test_car_12.stl">Car with 12in wheels</a>
    - <a href="../stl/vehicle_test_car_17.stl">Car with 17in wheels</a>
    - <a href="../stl/vehicle_test_truck_12.stl">Truck with 12in wheels</a>
    - <a href="../stl/vehicle_test_truck_17.stl">Truck with 17in wheels</a>
    - <a href="../stl/vehicle_test_van_12.stl">Van with 12in wheels</a>
    - <a href="../stl/vehicle_test_van_17.stl">Van with 17in wheels</a>

  \subsection refman  PDF Reference Manual

    Doxygen generates documentation if numerous formats. The Doxygen
    configuration file for this example has been setup to generate this
    HTML output as well as Latex generated PDF reference manual.

    Here is the Doxygen/Latex generated <a href="../latex/refman.pdf">
    PDF version</a> of this library documentation.

    \latexonly
      This is the PDF Reference manual.
    \endlatexonly

  \note There are also UNIX man page documentation that has been generated
        via Doxygen. It can be found at: <tt>@builddir@/share/examples/build/man</tt>.

  \todo Improve the window detail.
  \todo Fix all the typos and spelling mistakes.

*******************************************************************************/

/***************************************************************************//**

  \example vehicle_document.scad using the wheel library module.
  \example vehicle_test.scad using the vehicle library module.

*******************************************************************************/

//! Parameterized wheel model.
/***************************************************************************//**

  \param diameter   <integer> wheel rim diameter size in inches.
  \param width      <integer> wheel width in inches.
  \param wallheight <integer> wheel size tire wall height.
  \param tirecolor  <string> wire color.
  \param rimcolor   <string> rim color.

  \details

    For a complete list of color names see the World Wide Web consortium's
    [SVG color list] (http://www.w3.org/TR/css3-color/).

    Here is an example on how to use this module:

    \dontinclude vehicle_document.scad
    \skip use
    \until 10 );

    Sample 17in wheels:

    \image html  vehicle_document_17_320x240_front.png "17in Wheel Top View"
    \image html  vehicle_document_17_320x240_top.png "17in Wheel Front View"
    \image html  vehicle_document_17_320x240_diag.png "17in Wheel Diagonal View"

    \image latex  vehicle_document_17_320x240_front.eps "17in Wheel Top View" width=2.5in
    \image latex  vehicle_document_17_320x240_top.eps "17in Wheel Front View" width=2.5in
    \image latex  vehicle_document_17_320x240_diag.eps "17in Wheel Diagonal View" width=2.5in

  \protected

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

//! Parameterized vehicle cabin design module.
/***************************************************************************//**

  \param size       <integer[x, y, z]> Overall size of cabin
  \param cabcolor   <string>  Color for vehicle cabin.

  \details
    For a complete list of color names see the World Wide Web consortium's
    [SVG color list] (http://www.w3.org/TR/css3-color/).

  \protected

*******************************************************************************/
module cabin( size, cabcolor ) {
  color( cabcolor ) cube([size[0], size[1], size[2]], center=true);
  for (x=[-1,1]) translate(x*[size[0]/2, 0, 0])
  color( "whitesmoke" ) cube([1/2, size[1]*90/100, size[2]*90/100], center=true);
}

//! Parameterized vehicle design module.
/***************************************************************************//**

  \param type       <string>  Type of vehicle desired. One of {car, truck, van}.
  \param bodycolor  <string>  Color for body of vehicle.
  \param cabcolor   <string>  Color for vehicle cabin.
  \param width      <integer> Overall width of vehicle in inches.
  \param height     <integer> Overall height of vehicle in inches.
  \param length     <integer> Overall length of vehicle in inches.
  \param wheels     <integer> Vehicle rim diameter in inches.

  \details
    For a complete list of color names see the World Wide Web consortium's
    [SVG color list] (http://www.w3.org/TR/css3-color/).

    Here is an example on how to use this module:

    \dontinclude vehicle_test.scad
    \skip use
    \until vehicle_wheels );

  \test Determine the minimum and maximum wheel size that work for each
        vehicle and add to this documentation.

  \public

*******************************************************************************/
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

Begin_Scope _test;

  Begin_OpenSCAD;

    use <vehicle.scad>;

    vehicle_type="van";
    vehicle_body="Tan";
    vehicle_cab="Wheat";
    vehicle_wheels=15;

    vehicle( type=vehicle_type, bodycolor=vehicle_body, cabcolor=vehicle_cab,
             width=50, height=40, length=100, wheels=vehicle_wheels );

  End_OpenSCAD;

  Begin_MFScript;

    views   Name "views" Distance "100" Views "all";
    images  Name "sizes" Aspect "4:3" Wsizes "320";
    defines Name "wheel" Define "vehicle_wheels" Integers "12 17";
    defines Name "type" Define "vehicle_type" Strings "car truck van";
    defines Name "body" Define "vehicle_body" Strings "green blue";

    variables
      set_Makefile "${__MAKE_FILE__}"  add_Depend "${__MAKE_FILE__}"
      set_Source "${__SCOPE_FILE__}"   set_Prefix "${__PREFIX__}"

      set_Ext "png"
      set_Convert_Exts "eps jpeg"  set_Convert_Opts "-verbose"
      set_Opts "--preview --projection=o --viewall"
      set_Opts_Combine "type wheel body views sizes";

    script
      Begin_Makefile_New
        Include_Copy "${__LIBPATH__}/mk/parallel_jobs.mk"
        Summary  Tables  Targets  Menu_Ext
      End_Makefile;

    variables
      Set_Ext "stl"
      Clear_Convert
      Set_Opts "--render"
      Set_Opts_Combine "type wheel";

    script
      Begin_Makefile_Append
        Summary  Tables  Targets  Menu_Ext
        Menu_Src
        Menu_All
      End_Makefile;

  End_MFScript;

End_Scope;

Begin_Scope _document;

  Begin_OpenSCAD;

    use <vehicle.scad>;

    size = 15;

    wheel( diameter=size, width=size*6/10, wallheight=size*7/10 );

  End_OpenSCAD;

  Begin_MFScript;

    views   Name "views" Distance "150" Views "top front diag";
    images  Name "sizes" Aspect "4:3" Wsizes "320";
    defines Name "wheel" Define "size" Integers "12 17";

    variables
      set_Makefile "${__MAKE_FILE__}"  add_Depend "${__MAKE_FILE__}"
      add_Depend "${__LIBPATH__}/mk/parallel_jobs.mk"
      set_Source "${__SCOPE_FILE__}"   set_Prefix "${__PREFIX__}"

      set_Ext "png"
      set_Convert_Exts "eps jpeg"  set_Convert_Opts "-verbose"
      set_Opts "--preview --projection=o --viewall"
      set_Opts_Combine "wheel sizes views";

    script
      Begin_Makefile
        Include "${__LIBPATH__}/mk/parallel_jobs.mk"
        Summary  Tables  Targets  Menu
      End_Makefile;

  End_MFScript;

End_Scope;

*******************************************************************************/


////////////////////////////////////////////////////////////////////////////////
// end of file
////////////////////////////////////////////////////////////////////////////////
