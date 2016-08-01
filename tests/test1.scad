/*******************************************************************************

  \file       test1.scad

*******************************************************************************/


//! (decimal) Epsilon, small distance to deal with overlapping shapes.
ep = 0.02;


//! Return the feild number for given feild name.
/*******************************************************************************
  \param    feild name (string).
  \returns  (integer) Feild number.

  \public
*******************************************************************************/
function bearing_lbb_tablef(feild) =
  feild == "dr" ? 0 :
  feild == "d"  ? 1 :
  feild == "l"  ? 2 :
  feild == "b"  ? 3 :
  feild == "w"  ? 4 :
  feild == "d1" ? 5 :
-1;


//! Return the table row for a given index.
/*******************************************************************************
  \param    feild name (string).
  \returns  (vector) Table row.

  \public

  \note data not available for {b, w, and d1} for bearings lm{3|4}uu.
        Their values were extrapolated.
*******************************************************************************/
function bearing_lbb_tabler(size) =
  size == "lm3uu"   ? [  3,   7,  10,   7.3, 0.90,   6.7] :
  size == "lm4uu"   ? [  4,   8,  12,   8.8, 0.90,   7.6] :
  size == "lm5uu"   ? [  5,  10,  15,  10.2, 1.10,   9.6] :
  size == "lm6uu"   ? [  6,  12,  19,  13.5, 1.10,  11.5] :
  size == "lm8suu"  ? [  8,  15,  17,  11.5, 1.10,  14.3] :
  size == "lm8uu"   ? [  8,  15,  24,  17.5, 1.10,  14.3] :
  size == "lm10uu"  ? [ 10,  19,  29,  22.0, 1.30,  18.0] :
-1;


//! Return the record feild value for a given index and feild name.
/*******************************************************************************
  \param    table row (string).
  \param    feild name (string).
  \returns  (vector) feild value.

  \public
*******************************************************************************/
function bearing_lbb_table(row, feild) =
  bearing_lbb_tabler(row)[bearing_lbb_tablef(feild)];


//! linear ball-bearing.
/*******************************************************************************
  \param    bearing size/name (string).
  \param    show shell only (boolean).
  \param    show bearing detail (boolean).

  \public
*******************************************************************************/
module bearing_lbb(
    size,
    shell=false,
    detail=true)
{
  if (bearing_lbb_tabler(size) == -1)
    echo(str("ERROR:bearing_lbb() - size of '",size,"' is undefined."));

  dr = bearing_lbb_table(size, "dr");
  d  = bearing_lbb_table(size, "d");
  l  = bearing_lbb_table(size, "l");
  b  = bearing_lbb_table(size, "b");
  w  = bearing_lbb_table(size, "w");
  d1 = bearing_lbb_table(size, "d1");

  dp = 0.94 * d;  // diameter of plastic interior
  lp = 0.96 * l;  // length of plastic interior

  // remove external library dependancy
  //$fn=mget_fn_r(d/2);

  // bearing shell
  if (shell == true)
  {
    color("silver")
    cylinder(d=d, h=l, center=true);
  }
  else
  {
    if (detail == true) {
      color("silver")
      difference() {
        // bearing shell
        cylinder(d=d, h=l, center=true);

        // bearing shell thinkness
        cylinder(d=dp, h=l+ep*2, center=true);

        // mounting band recess
        for (i = [-1, +1] ) {
          translate([0, 0, b/2 * i])
          difference() {
            cylinder(d=d+ep, h=w, center=true);
            cylinder(d=d1, h=w, center=true);
          }
        }
      }

      // mounting bands detail
      color("black")
      for (i = [-1, +1] ) {
        translate([0, 0, b/2 * i])
        difference() {
          cylinder(d=d1+ep, h=w, center=true);
          cylinder(d=d1, h=w+ep*2, center=true);
        }
      }

      // plastic bearing housing detail
      color("black")
      difference() {
        cylinder(d=dp, h=lp, center=true);
        cylinder(d=dr, h=lp+ep*2, center=true);
      }

      // plastic bearing housing rim
      color("black")
      difference() {
        cylinder(d=(dr+d-dp), h=l-ep*2, center=true);
        cylinder(d=dr, h=l, center=true);
      }
    }
    else
    {
      // no detail
      color("silver")
      difference() {
        cylinder(d=d, h=l, center=true);
        cylinder(d=dr, h=l+ep*2, center=true);
      }
    } // detail
  } // shell
}


//! linear ball-bearing parallel inset mount with mounting holes
/*******************************************************************************
  \param    size (string) bearing size/name.
  \param    material_depth (float) material thickness.
  \param    inset_depth (float) depth to inset bearing.
  \param    mh_size (float) size of mouting holes.
  \param    mh_square (boolean) use square mounting holes.
  \param    thru (boolean) thru-hole mount.
  \param    shell (boolean) show shell only.

  \public
*******************************************************************************/
module bearing_lbb_pim(
    size,
    material_depth,
    inset_depth,
    mh_size   = 3.175,
    mh_square = true,
    thru      = false,
    shell     = false)
{
  if (bearing_lbb_tabler(size) == -1)
    echo(str("ERROR:bearing_lbb_pim() size of '",size,"' is undefined."));

  dr = bearing_lbb_table(size, "dr");
  d  = bearing_lbb_table(size, "d");
  l  = bearing_lbb_table(size, "l");
  b  = bearing_lbb_table(size, "b");
  w  = bearing_lbb_table(size, "w");
  d1 = bearing_lbb_table(size, "d1");

  dp = 0.94 * d;  // diameter of plastic interior
  lp = 0.96 * l;  // length of plastic interior

  if (thru == true)
  {
    hull()
    for (i = [0:1])
      mirror([0, i, 0])
      // intersection
      intersection() {
        cube([d+ep*2, material_depth+ep*2, l+ep*2], center=true);
        translate([0, d/2 + material_depth/2 - inset_depth, 0])
          bearing_lbb(size=size, shell=shell);
      }
  }
  else
  {
      // identical copy of "intersection" code above (with axle fill-in)
      intersection() {
        cube([d+ep*2, material_depth+ep*2, l+ep*2], center=true);
        translate([0, d/2 + material_depth/2 - inset_depth, 0])
        union() {
          bearing_lbb(size=size, shell=shell);
          cylinder(d=(dr+d-dp), h=l-ep*2, center=true); // fill in axle
        }
      }
  }

  // add mount holes
  for (i = [ [-1, -1], [-1, +1], [+1, -1], [+1, +1] ])
    translate( [(d+mh_size)/2 * i[0], 0, b/2 * i[1]] )
    if (mh_square == true)
      cube([mh_size, material_depth+ep*2, mh_size], center=true);
    else
      rotate([90, 0, 0])
      cylinder(d=mh_size, h=material_depth+ep*2, center=true, $fn=10 );
}


//! linear ball-bearing axial insert mount
/*******************************************************************************
  \param    size (string) bearing size/name.
  \param    shell (boolean) show shell only.

  \public
*******************************************************************************/
module bearing_lbb_aim(
    size,
    shell = false)
{
  if (bearing_lbb_tabler(size) == -1)
    echo(str("ERROR:bearing_lbb_aim() size of '",size,"' is undefined."));

  dr = bearing_lbb_table(size, "dr");
  d  = bearing_lbb_table(size, "d");
  l  = bearing_lbb_table(size, "l");
  b  = bearing_lbb_table(size, "b");
  w  = bearing_lbb_table(size, "w");
  d1 = bearing_lbb_table(size, "d1");

  dp = 0.94 * d;  // diameter of plastic interior
  lp = 0.96 * l;  // length of plastic interior

  bearing_lbb(size=size, shell=shell);
  cylinder(d=(dr+d-dp), h=l-ep*2, center=true); // fill in axle
}

////////////////////////////////////////////////////////////////////////////////
// library end
////////////////////////////////////////////////////////////////////////////////


/*******************************************************************************
Begin_Scope _doc;

  //
  // Openscad documentation code
  //

  Begin_OpenSCAD;
    use <test_01.scad>;

    //
    // parallel inset mount demo
    //
    module bearing_pim_demo(b="lm8uu") {
      m  = 6.5;
      d  = bearing_lbb_table(b, "d");
      l  = bearing_lbb_table(b, "l");
      i  = d / 15;

      difference() {
        cube([d*2, l*1.5, m], center=true);
        rotate([90, 0, 0])
        bearing_lbb_pim(b, m, i, thru=false);
      }
      translate([0, 0, d/2+m/2-i + d*1])
      rotate([90, 0, 0])
      bearing_lbb(b, shell=false, detail=true);

    }

    //
    // axial inset mount demo
    //
    module bearing_aim_demo(b="lm8uu") {
      m  = 6.5;
      d  = bearing_lbb_table(b, "d");
      l  = bearing_lbb_table(b, "l");
      i  = d / 15;

      translate([0, 0, 0])
      difference() {
        cube([d*2, l*1.5, m], center=true);
        rotate([0, 0, 0])
        bearing_lbb_aim(b, shell=false);
      }
      translate([0, 0, d/2+m/2-i + d*1])
      rotate([0, 0, 0])
      bearing_lbb(b, shell=false, detail=true);
    }

    module render_bearing_demo(b="lm8uu") {
      d  = bearing_lbb_table(b, "d") * 1.25;
      l  = bearing_lbb_table(b, "l") * 1.00;

      translate([-d, 0, 0])
      bearing_aim_demo(b);

      translate([+d, 0, 0])
      bearing_pim_demo(b);
    }

    render_bearing_demo();
  End_OpenSCAD;


  //
  // Makefile script to generate documentation
  //

  Begin_MFScript;

    #---------------------------------------------------------------------------
    # Options
    #---------------------------------------------------------------------------
    views   Name "view" Translate "0,0,12" Distance "200" Views "all";

    images  Name "size"
            Types "pdf html"
            Aspect  "4:3" Hsizes "480 768"
            Aspect "16:9" Wsizes "640 1280";

    defines Name "part" Define "render_part" Strings "cover base";

    defines Name "mode" Define "render_mode" Strings "mode1 mode2";

    #---------------------------------------------------------------------------
    # Script
    #---------------------------------------------------------------------------
    variables
      set_Makefile "${__MAKE_FILE__}"  add_Depend "${__MAKE_FILE__}"
      set_Source "${__SCOPE_FILE__}"   set_Prefix "${__PREFIX__}"

      set_Ext "png"
      set_Convert_Exts "png eps jpeg tiff"  set_Convert_Opts "-verbose"
      set_Opts "--preview --projection=o --viewall --colorscheme=Cornfield"
      set_Opts_Combine "view size";

    script
      Begin_Makefile_New
        Include_Copy "${__INCLUDE_PATH__}/parallel_jobs.mk"
        Summary  Tables  Targets  Menu_Ext
      End_Makefile;

    variables
      Set_Ext "stl"
      Clear_Convert
      Set_Opts "--render"
      Set_Opts_Combine "part mode";

    script
      Begin_Makefile_Append
        Summary  Tables  Targets  Menu_Ext
        Menu_Src
        Menu_All
      End_Makefile;

  End_MFScript;

End_Scope;
*******************************************************************************/

////////////////////////////////////////////////////////////////////////////////
// eof
////////////////////////////////////////////////////////////////////////////////

