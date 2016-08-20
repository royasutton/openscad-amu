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
  \todo add option to create a histogram/map of amu command that exists
        in the input file.

  \ingroup openscad_dif_src
*******************************************************************************/

#include "openscad_dif_scanner.hpp"

#include "boost/program_options.hpp"
#include "boost/filesystem.hpp"

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

//! \ingroup openscad_dif_src
//! @{

using namespace std;
namespace po = boost::program_options;


// return value constants.
namespace
{
  const size_t SUCCESS = 0;

  const size_t ERROR_UNHANDLED_EXCEPTION = 1;
  const size_t ERROR_IN_COMMAND_LINE = 2;
  const size_t ERROR_UNABLE_TO_OPEN_FILE = 3;
}


//! check for opt2 if opt1 is set and throws exception if not.
void
option_depend(
  const po::variables_map& vm,
  const char* opt1,
  const char* opt2)
{
  if ( vm.count(opt1) && !vm[opt1].defaulted() )
    if ( vm.count(opt2) == 0 || vm[opt2].defaulted() )
      throw std::logic_error( std::string("Option '--" ) + opt1
            + "' requires option '--" + opt2 + "'" );
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
    string scope;
    string joiner         = "_";

    string output_prefix;

    string lib_path       = __LIB_PATH__;
    string make_path      = __MAKE_PATH__;

    string makefile_ext   = ".makefile";

    string config;

    po::options_description opts("Options");
    opts.add_options()
      ("input,i",
          po::value<string>(&input)->required(),
          "Input file containing annotated script(s) embedded within "
          "comments.\n")
      ("scope,s",
          po::value<string>(&scope),
          "Scope root name. When not specified, the input file stem "
          "name is used.")
      ("joiner,j",
          po::value<string>(&joiner)->default_value(joiner),
          "Scope joiner. String used to conjoin scope hierarchies.\n")
      ("output-prefix,o",
          po::value<string>(&output_prefix),
          "Output prefixed used by seam for extracted auxiliary scripts.\n")
      ("lib-path",
          po::value<string>(&lib_path)->default_value(lib_path),
          "Makefile script library path.")
      ("make-path",
          po::value<string>(&make_path)->default_value(make_path),
          "GNU Make executable path.\n")
      ("makefile-ext",
          po::value<string>(&makefile_ext)->default_value(makefile_ext),
          "Makefile file extension.\n")
      ("config,c",
          po::value<string>(&config),
          "Configuration file with one or more option value pairs.\n")
      ("debug-scanner",
          "Run scanner in debug mode.")
      ("debug-filter",
          "Turn on filter debugging.\n")
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
        cout << command_name << " " << PACKAGE_VERSION << endl
             << endl
             <<
  "Doxygen input filter for OpenSCAD source files. Can be used in conjunction\n"
  "with Doxygen tags INPUT_FILTER and FILTER_*.\n"

             << endl
             << "Example:" << endl
             << "  FILTER_PATTERNS = *.scad=<prefix>/bin/" << command_name << endl
             << "  FILTER_PATTERNS = *.scad=\"<prefix>/bin/" << command_name
             << " --config <config>\"" <<endl
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
               << "    build date: " << __BUILD_DATE__ << endl
               << "  architecture: " << __BUILD_ARCH__ << endl
               << endl
               << "      lib path: " << __LIB_PATH__ << endl
               << endl;
        else
          cout << PACKAGE_VERSION << endl;

        exit( SUCCESS );
      }

      // parse configuration file options
      if ( vm.count("config")  ) {
        // notify not called to allow require options to be parsed
        // from configuration file... manually retrieve specified
        // configuration file name
        config = vm["config"].as<string>();

        if ( vm.count("verbose")  )
          cout << "reading configuration file: " << config << endl;

        ifstream config_file ( config.c_str() );

        if ( config_file.good() ) {
          po::store(po::parse_config_file(config_file, opts, true), vm);
        } else {
          cerr << "ERROR: unable to open configuration file [" << config
               << "]" << endl;

          exit( ERROR_UNABLE_TO_OPEN_FILE );
        }
        config_file.close();
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
    // validate arguments
    ////////////////////////////////////////////////////////////////////////////

     option_depend( vm, "verbose", "version");


    ////////////////////////////////////////////////////////////////////////////
    // show configuration
    ////////////////////////////////////////////////////////////////////////////

    // iff --debug-filter is set
    if ( vm.count("debug-filter")  ) {
      cout << "\n\\if __INCLUDE_FILTER_DEBUG__\n\\verbatim\n";
      for(po::variables_map::const_iterator it = vm.begin(); it != vm.end(); ++it)
      {
        // skip some options
        if (  !it->first.compare("debug-filter") |
              !it->first.compare("debug-scanner") ) continue;

        cout << "# " << it->first;
        if ( it->second.defaulted() ) cout << " (defaulted)";
        cout << endl;

        if        (((boost::any)it->second.value()).type() == typeid(int)) {
          cout << it->first << "=" << it->second.as<int>();
        } else if (((boost::any)it->second.value()).type() == typeid(bool)) {
          cout << it->first << "=" << it->second.as<bool>();
        } else if (((boost::any)it->second.value()).type() == typeid(string)) {
          cout << it->first << "=" << it->second.as<string>();
        } else if (((boost::any)it->second.value()).type() == typeid(vector<string>)) {
          vector<string> v = it->second.as<vector<string> >();
          for ( vector<string>::iterator vit=v.begin(); vit != v.end(); ++vit) {
            cout << it->first << "=" << *vit;
            if ( (vit+1) != v.end() ) cout << endl;
          }
        } else {
          cout << it->first << "=unknown-value-type";
        }

        cout << endl << endl;
      }
      cout << "\n\\endverbatim\n\\endif\n";
    }

    ////////////////////////////////////////////////////////////////////////////
    // setup and run scanner
    ////////////////////////////////////////////////////////////////////////////

    ODIF::ODIF_Scanner scanner( input, command_name + ": " );

    scanner.set_rootscope( scope );
    scanner.set_scopejoiner( joiner );
    scanner.set_output_prefix( output_prefix );

    scanner.set_lib_path( lib_path );
    scanner.set_make_path( make_path );

    scanner.set_makefile_ext( makefile_ext );

    scanner.set_debug( vm.count("debug-scanner")>0 );
    scanner.set_debug_filter( vm.count("debug-filter")>0 );

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
