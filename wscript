#! /usr/bin/env python
# encoding: utf-8

VERSION='0.0.1'
APPNAME='instrumentation-library'


def options(opt):
  opt.load('compiler_cxx')


def configure(conf):
  conf.load('compiler_cxx')
  conf.env.CXXFLAGS += ['-std=c++14']

  conf.check_cfg(
    package='rapidjson', args=['--cflags'], uselib_store='RAPIDJSON')

def build(bld):
  directories = ['include', 'source']

  bld.recurse(directories)
