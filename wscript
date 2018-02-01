#! /usr/bin/env python
# encoding: utf-8

VERSION='0.0.1'
APPNAME='cpts571-project'


def options(opt):
  opt.load('compiler_cxx')
  opt.load('boost')
  opt.load('flex')
  opt.load('bison')


def configure(conf):
  conf.load('compiler_cxx')
  conf.env.CXXFLAGS += ['-std=c++14', '-g']

  conf.load('boost')
  # Using boost for command line arguments
  conf.check_boost('program_options')
  
  conf.check_cfg(
    package='rapidjson', args=['--cflags'], uselib_store='RAPIDJSON')

  conf.load('flex')
  conf.env.FLEXFLAG = ['-c++']
  conf.load('bison')

def build(bld):
  directories = ['include', 'source', 'tools' ]

  bld.recurse(directories)
