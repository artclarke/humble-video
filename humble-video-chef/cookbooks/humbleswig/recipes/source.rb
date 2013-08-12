#
# Cookbook Name:: humbleswig
# Recipe:: source
#

include_recipe "build-essential"
include_recipe "git"

creates_humbleswig = "#{node[:humbleswig][:prefix]}/bin/swig"

file "#{creates_humbleswig}" do
    action :nothing
end

git "#{Chef::Config[:file_cache_path]}/humbleswig" do
    repository node[:humbleswig][:git_repository]
    reference node[:humbleswig][:git_revision]
    action :sync
    notifies :delete, "file[#{creates_humbleswig}]", :immediately
end

#write the flags used to compile to disk
template "#{Chef::Config[:file_cache_path]}/humbleswig-compiled_with_flags" do
    source "compiled_with_flags.erb"
    owner "root"
    group "root"
    mode 0600
    variables(
        :compile_flags => node[:humbleswig][:compile_flags]
    )
    notifies :delete, "file[#{creates_humbleswig}]", :immediately
end

bash "compile_humbleswig" do
    cwd "#{Chef::Config[:file_cache_path]}/humbleswig"
    code <<-EOH
        ./autogen.sh
        ./configure --prefix=#{node[:humbleswig][:prefix]} #{node[:humbleswig][:compile_flags].join(' ')}
        make clean && make && make install
    EOH
    creates "#{creates_humbleswig}"
end
