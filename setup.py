from setuptools import setup,Extension

src = 'source/'

setup(name='Kumquat'
      version='0.1'
      author='Conor Jackson'
      ext_package='kumquat'
      ext_modules=[Extension('kumquat',[src+f for f in ('kumquat.cpp',
                                            'GaussKronrod.cpp',
                                            'IntegrandFunctionWrapper.cpp')]
                            )]
     )
