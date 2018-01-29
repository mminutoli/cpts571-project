#! /usr/bin/env python
# encoding: utf-8

VERSION='0.0.1'
APPNAME='instrumentation-library'


top = '.'
out = 'build'


def options(opt):
  opt.load('compiler_cxx')


def configure(conf):
  conf.load('compiler_cxx')


def build(bld):
  directories = ['include', 'source']

  bld.recurse(directories)
