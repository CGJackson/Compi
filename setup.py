from setuptools import setup,Extension
import os, sys, re

import readline # makes terminal input nice

#############################################################################################
#
# The file locations in this tuple are searched in order to find the boost library. The first
# valid location is used. The location of the boost library used can be modified by changing 
# this tuple
#

boost_library_search_locations = (
                                    "/usr/include/boost",
                                    "local_boost"
                                 )
#############################################################################################

def check_valid_boost_path(path):
      '''
      Returns True if the path entered gives the location of the boost library, including,
      at a minimum, all files required by Kumquat
      '''

      include_regex = re.compile(r'#include <(boost/\S*)>') # matches a c++ include for a boost header file

      quad = "boost/math/quadrature/"
      files_required = {quad+"gauss_kronrod.hpp","boost/math/tools/precision.hpp"}

      # Recusively checks that a file exists, searches it for boost includes
      # and then checks those files

      files_checked = set()
      while files_required:
            f = files_required.pop()
            try:
                  for line in open(path+'/'+f,'r'):
                        include = include_regex.match(line)
                        if include is not None and include.group(1) not in files_checked:
                              files_required.add(include.group(1))
                        

            except FileNotFoundError:
                  return False
            files_checked.add(f)


      return True

def set_boost_path():
      for path in boost_library_search_locations:
            if check_valid_boost_path(path):
                  return path
      
      print("Unable to locate C++ boost library headers")
      while True:
            path = input("Please enter file path to boost library location. Press Enter to exit\n").strip('/')
            if not path:
                  sys.exit()
            
            

            if check_valid_boost_path(path):
                  return path
            
            print("The directory path you entered is not a valid path to the boost library")
            

boost_path = set_boost_path()
print("Using " + boost_path + " as boost location\n")

src = 'source/'

setup(name='Kumquat',
      version='0.3',
      author='Conor Jackson',
      author_email='conorgjackson@gmail.com',
      url='https://github.com/CGJackson/Kumquat',
      ext_modules=[Extension('kumquat',[src+f for f in ('kumquat.c',
                                            'GaussKronrod.cpp',
                                            'tanh_sinh.cpp',
                                            'sinh_sinh.cpp',
                                            'trapezoid.cpp',
                                            'IntegrandFunctionWrapper.cpp')],

                                       include_dirs=[boost_path],extra_compile_args=["-std=c++17"]
                            )],
     )
