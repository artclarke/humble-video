#
# Cookbook Name:: humbleswig
# Recipe:: default
#
# Copyright 2012-2013, Escape Studios
#

case node[:humbleswig][:install_method]
when :source
  include_recipe "humbleswig::source"
end
