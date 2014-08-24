#
# Cookbook Name:: humble_development
# Recipe:: default
#
# Copyright 2014, Humble Software
#
# All rights reserved - Do Not Redistribute
#
#
apt_repository "saucy" do
   uri "http://ubuntu.mirror.cambrium.nl/ubuntu/" 
   distribution "saucy"
   components ["main"]
end

package "autoconf" do
  version "2.69-1.1"
  action :upgrade
end
package "automake" do   
  version "1:1.13.3-1.1ubuntu2"
  action :upgrade
end
apt_repository "saucy" do
   action :remove
end
package "libtool" do
  version "2.4.2-1ubuntu1"
  action :upgrade
end
package "pkg-config" do
  version "0.26-1ubuntu1"
  action :upgrade
end
package "gcc-multilib" do
  version "4:4.6.3-1ubuntu5"
  action :upgrade
end
package "g++-multilib" do
  version "4:4.6.3-1ubuntu5"
  action :upgrade
end
package "mingw-w64" do
  version "2.0.1-1"
  action :upgrade
end
package "g++-mingw-w64" do
  version "4.6.3-1ubuntu5+5ubuntu1"
  action :upgrade
end
package "gcc-mingw-w64" do
  version "4.6.3-1ubuntu5+5ubuntu1"
  action :upgrade
end
package "binutils-mingw-w64" do
  version "2.22-2ubuntu1+1"
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
  version "1.7.6.1-2ubuntu1"
  action :upgrade
end
package "valgrind" do
  version "1.3.7.0-0"
  action :upgrade
end
package "graphviz" do
  version "2.26.3-10"
  action :upgrade
end
package "maven" do
  version "3.0.4-2"
  action :upgrade
end
package "libpcre3-dev" do
  version "8.12-4"
  action :upgrade
end
