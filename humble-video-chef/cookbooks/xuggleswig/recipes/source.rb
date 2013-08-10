#
# Cookbook Name:: xuggleswig
# Recipe:: source
#

include_recipe "build-essential"
include_recipe "git"

creates_xuggleswig = "#{node[:xuggleswig][:prefix]}/bin/swig"

file "#{creates_xuggleswig}" do
    action :nothing
end

git "#{Chef::Config[:file_cache_path]}/xuggleswig" do
    repository node[:xuggleswig][:git_repository]
    reference node[:xuggleswig][:git_revision]
    action :sync
    notifies :delete, "file[#{creates_xuggleswig}]", :immediately
end

#write the flags used to compile to disk
template "#{Chef::Config[:file_cache_path]}/xuggleswig-compiled_with_flags" do
    source "compiled_with_flags.erb"
    owner "root"
    group "root"
    mode 0600
    variables(
        :compile_flags => node[:xuggleswig][:compile_flags]
    )
    notifies :delete, "file[#{creates_xuggleswig}]", :immediately
end

bash "compile_xuggleswig" do
    cwd "#{Chef::Config[:file_cache_path]}/xuggleswig"
    code <<-EOH
        ./autogen.sh
        ./configure --prefix=#{node[:xuggleswig][:prefix]} #{node[:xuggleswig][:compile_flags].join(' ')}
        make clean && make && make install
    EOH
    creates "#{creates_xuggleswig}"
end
