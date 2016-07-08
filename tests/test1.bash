#!/bin/bash

#/
#  \file       test.bash
#/

########## \mainpage My Personal Index Page
## \section intro_sec Introduction
## This is the introduction.
## \afn This is the introduction.
## @aparamo  opta  is the introduction.
## \section install_sec Installation
## \subsection step1 Step 1: Opening the box 1 2 3


## this is a varable
    ## \section variable qe
i=2 ## \var i

q=3

v="jljl"

x=tyutuy

l_l=8

declare q=12

#/#######################################################################
# \afn _test()
# \aparami <string> \--opta know is the time
# \aparami <string> optb for all good men
# \aparamo optc for all good men
# \retval 0 when good
# \retval 1 when bad
# \brief this is what it does
########################################################################/

#
#
# ignore normal comments
#
#

test1 (      )
{

  declare local v1=1

  echo $v1
}

function test2() {

  declare local v2=2

  echo $v2
}

function test3 {

  declare local v3=3

  echo $v3
}

test1
test2
test3

#/#######################################################################
#
# eof
#
########################################################################/
