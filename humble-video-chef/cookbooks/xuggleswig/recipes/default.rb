#
# Cookbook Name:: xuggle-swig
# Recipe:: default
#
# Copyright 2012-2013, Escape Studios
#

case node[:xuggleswig][:install_method]
when :source
  include_recipe "xuggleswig::source"
end
