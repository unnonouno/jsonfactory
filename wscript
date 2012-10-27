APPNAME = 'jsonfactory'
VERSION = '0.1.0'

def options(opt):
  opt.load('compiler_cxx')
  opt.load('unittest_gtest')

def configure(conf):
  conf.env.CXXFLAGS += ['-O2', '-Wall', '-g', '-pipe']
  conf.load('compiler_cxx')
  conf.load('unittest_gtest')
  conf.check_cfg(package = 'pficommon', args = '--cflags --libs')

def build(bld):
  bld.program(
    features = 'gtest',
    source = 'lib/jsonfactory_test.cpp',
    target = 'jsonfactory_test',
    use = 'PFICOMMON'
  )
