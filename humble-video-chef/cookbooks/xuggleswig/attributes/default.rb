#
# Cookbook Name:: xuggleswig
# Attributes:: default
#
# Copyright 2012-2013, Escape Studios
#

default[:xuggleswig][:install_method] = :source
default[:xuggleswig][:prefix] = "/usr/local"
default[:xuggleswig][:git_repository] = "git://github.com/artclarke/xuggle-swig"
default[:xuggleswig][:git_revision] = "HEAD"
default[:xuggleswig][:compile_flags] = []
