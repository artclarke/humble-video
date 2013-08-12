#
# Cookbook Name:: humbleswig
# Attributes:: default
#
# Copyright 2012-2013, Escape Studios
#

default[:humbleswig][:install_method] = :source
default[:humbleswig][:prefix] = "/usr/local"
default[:humbleswig][:git_repository] = "git://github.com/artclarke/humble-swig"
default[:humbleswig][:git_revision] = "HEAD"
default[:humbleswig][:compile_flags] = []
