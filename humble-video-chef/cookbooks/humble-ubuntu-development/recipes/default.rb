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
package "gcc-mingw32" do
  action :upgrade
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
