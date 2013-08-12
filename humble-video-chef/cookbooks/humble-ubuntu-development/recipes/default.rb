#
# Cookbook Name:: humble_development
# Recipe:: default
#
# Copyright 2013, Humble Software
#
# All rights reserved - Do Not Redistribute
#

package "autoconf" do
  action :upgrade
end
package "automake" do
  action :upgrade
end
package "libtool" do
  action :upgrade
end
package "pkg-config" do
  action :upgrade
end
package "gcc-multilib" do
  action :upgrade
end
package "g++-multilib" do
  action :upgrade
end
package "mingw-w64" do
  action :upgrade
end
package "g++-mingw-w64" do
  action :upgrade
end
package "gcc-mingw-w64" do
  action :upgrade
end
package "binutils-mingw-w64" do
  action :upgrade
end
# There is a bug in virtualbox that cause 'strip' of a '.dll' file
# that resides on the shared folder to fail with a weird protocol error.
# See: https://www.virtualbox.org/ticket/8463
# This code works around that by replacing the strips provided in
# the mingw packages with a custom strip that copies the file to be stripped
# first to local storage, then runs strip, then copies it back.
bash 'create-correct-windows-strip' do
  code <<-EOH
    for strip in /usr/bin/x86_64-w64-mingw32-strip /usr/bin/i686-w64-mingw32-strip; do mv $strip $strip-orig; done
  EOH
  not_if {File.exists?("/usr/bin/x86_64-w64-mingw32-strip-orig")}
end
template '/usr/bin/x86_64-w64-mingw32-strip' do
  source 'mingw32-strip.erb'
  owner "root"
  group "root"
  mode 00755
  variables({
     :strip => '/usr/bin/x86_64-w64-mingw32-strip'
  })
  not_if {File.exists?("/usr/bin/x86_64-w64-mingw32-strip")}
end
template '/usr/bin/i686-w64-mingw32-strip' do
  source 'mingw32-strip.erb'
  owner "root"
  group "root"
  mode 00755
  variables({
     :strip => '/usr/bin/i686-w64-mingw32-strip'
  })
  not_if {File.exists?("/usr/bin/i686-w64-mingw32-strip")}
end
package "doxygen" do
  action :upgrade
end
package "valgrind" do
  action :upgrade
end
package "graphviz" do
  action :upgrade
end
package "openjdk-6-jdk" do
  action :upgrade
end
package "maven" do
  action :upgrade
end
package "libpcre3-dev" do
  action :upgrade
end
