from setuptools import setup,Extension

src = 'source/'

boost_path = './local_boost'

setup(name='Kumquat',
      version='0.1',
      author='Conor Jackson',
      author_email='conrgjackson@gmail.com',
      ext_modules=[Extension('kumquat',[src+f for f in ('kumquat.cpp',
                                            'GaussKronrod.cpp',
                                            'IntegrandFunctionWrapper.cpp')],
                                       include_dirs=[boost_path]
                            )]
     )
