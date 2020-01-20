from setuptools import setup,Extension
import os, sys

#############################################################################################
#
# The file locations in this tuple are searched in order to find the boost library. The first
# valid location is used. The location of the boost library used can be modified by changing 
# this tuple
#

boost_library_search_locations = (
                                    "/usr/include/boost",
                                    "./local_boost"
                                 )
#############################################################################################

def check_valid_boost_path(path):
      '''
      Returns True if the path entered gives the location of the boost library
      '''
      #TODO perform more extensive checks that path is valid
      return os.path.isdir(path)

def set_boost_path():
      for path in boost_library_search_locations:
            if check_valid_boost_path(path):
                  return path
      
      while True:
            path = input("Unable to locate boost library headers. Please enter file path to their location. Press Enter to exit")
            if not path:
                  sys.exit()

            if check_valid_boost_path(path):
                  return path
            
            print("The directory path you entered is not a valid path to the boost library")
            

boost_path = set_boost_path()
print(f"Using {boost_path} as boost location")

src = 'source/'

setup(name='Kumquat',
      version='0.2',
      author='Conor Jackson',
      author_email='conorgjackson@gmail.com',
      url='https://github.com/CGJackson/Kumquat',
      ext_modules=[Extension('kumquat',[src+f for f in ('kumquat.cpp',
                                            'GaussKronrod.cpp',
                                            'IntegrandFunctionWrapper.cpp')],

                                       include_dirs=[boost_path]
                            )],
      test_suite='tests.tests'
     )
