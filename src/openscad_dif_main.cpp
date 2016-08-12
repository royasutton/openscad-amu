/**************************************************************************//**

  \file   openscad_dif_main.cpp

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
    OpenSCAD Doxygen input filter main source.

  \todo add option to list JavaScript referenced by all input source. this
        would help construct makefile dependencies (--list-js). The larger
        point is that this filter needs to be responsible for installing
        and removing JavaScript dependencies (or at least communicating
        them to a responsible party). See Doxygen configuration tab
        HTML_EXTRA_FILES.
  \test make sure the filter can be defined in the doxygen input file with
        the --lip-path option specified. If this does not work, will need
        to turn to environment variable definition (OPENSCAD_AMU_LIB).
  \todo add option to create a histogram/map of amu command that exists
        in the input file.
  \todo add an option that echos macro to-variables and macro definitions
        to the scanner output file to help with macro definition coding.

  \ingroup openscad_dif_src
*******************************************************************************/

#include "openscad_dif_scanner.hpp"

#include "boost/program_options.hpp"
#include "boost/filesystem.hpp"

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

using namespace std;

//! \ingroup openscad_dif_src
//! @{

// return value constants.
namespace
{
  const size_t SUCCESS = 0;

  const size_t ERROR_UNHANDLED_EXCEPTION = 1;
  const size_t ERROR_IN_COMMAND_LINE = 2;
  const size_t ERROR_UNABLE_TO_OPEN_FILE = 3;
}


//! program main.
int
main(int argc, char** argv)
{
  try
  {
    ////////////////////////////////////////////////////////////////////////////
    // setup command line arguments
    ////////////////////////////////////////////////////////////////////////////
    string command_name = boost::filesystem::basename( argv[0] );

    string input;

    string lib_path       = __LIB_PATH__;

    namespace po = boost::program_options;

    po::options_description opts("Options");
    opts.add_options()
      ("input,i",
          po::value<string>(&input)->required(),
          "Input file containing annotated script(s) embedded within "
          "comments.\n")
      ("lib-path",
          po::value<string>(&lib_path)->default_value(lib_path),
          "Makefile script library path.")
      ("debug",
          "Run scanner in debug mode.\n")
      ("verbose,V",
          "Run in verbose mode.")
      ("version,v",
          "Report tool version.")
      ("help,h",
          "Print this help messages.")
    ;

    po::positional_options_description opts_pos;
    opts_pos.add("input", 1);


    ////////////////////////////////////////////////////////////////////////////
    // parse command line arguments
    ////////////////////////////////////////////////////////////////////////////
    po::variables_map vm;

    try
    {
      po::store(po::command_line_parser(argc, argv).options(opts)
                  .positional(opts_pos).run(), vm);

      // output help and exit
      if ( vm.count("help") || (argc==1) ) {
        cout << command_name << " v" << PACKAGE_VERSION << endl
             << endl
             <<
  "Doxygen input filter for OpenSCAD source files. Can be used in conjunction\n"
  "with Doxygen tags INPUT_FILTER and FILTER_*.\n"

             << endl
             << "Example:" << endl
             << "  FILTER_PATTERNS = *.scad=<prefix>/bin/" << command_name << endl
             << endl
             << opts
             << endl;

        exit( SUCCESS );
      }

      // output version and exit
      if ( vm.count("version")  ) {

        if ( vm.count("verbose")  )
          cout << command_name << " " << PACKAGE_VERSION << endl
               << endl
               << "       package: " << PACKAGE_NAME << endl
               << "       version: " << PACKAGE_VERSION << endl
               << "    bug report: " << PACKAGE_BUGREPORT << endl
               << "      site url: " << PACKAGE_URL << endl
               << endl
               << "      lib path: " << __LIB_PATH__ << endl
               << endl;
        else
          cout << PACKAGE_VERSION << endl;

        exit( SUCCESS );
      }

      po::notify(vm);
    }
    catch(po::required_option& e)
    {
      cerr << "ERROR: " << e.what() << endl;
      exit( ERROR_IN_COMMAND_LINE );
    }
    catch(po::error& e)
    {
      cerr << "ERROR: " << e.what() << endl;
      exit( ERROR_IN_COMMAND_LINE );
    }


    ////////////////////////////////////////////////////////////////////////////
    // setup and run scanner
    ////////////////////////////////////////////////////////////////////////////

    ODIF::ODIF_Scanner scanner( input, command_name + ": " );

    scanner.set_lib_path( lib_path );
    scanner.set_debug( vm.count("debug")>0 );

    while( scanner.scan() != 0 )
      ;
  }
  catch(exception& e)
  {
    cerr << "Unhandled exception reached the top of main:" << endl
         << e.what() << "," << endl
         << "exiting..." << endl;
    exit( ERROR_UNHANDLED_EXCEPTION );
  }

  exit( SUCCESS );
}

//! @}


/*******************************************************************************
// eof
*******************************************************************************/
