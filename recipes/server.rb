#
# Author:: AJ Christensen <aj@junglist.gen.nz>
# Author:: Doug MacEachern <dougm@vmware.com>
# Author:: Fletcher Nichol <fnichol@nichol.ca>
# Author:: Seth Chisamore <schisamo@opscode.com>
# Author:: Guilhem Lettron <guilhem.lettron@youscribe.com>
#
# Cookbook Name:: jenkins
# Recipe:: server
#
# Copyright 2010, VMware, Inc.
# Copyright 2012, Opscode, Inc.
# Copyright 2013, Youscribe.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

include_recipe "java"

user node['jenkins']['server']['user'] do
  home node['jenkins']['server']['home']
end

home_dir = node['jenkins']['server']['home']
plugins_dir = File.join(home_dir, "plugins")
log_dir = node['jenkins']['server']['log_dir']
ssh_dir = File.join(home_dir, ".ssh")

[
  home_dir,
  plugins_dir,
  log_dir,
  ssh_dir
].each do |dir_name|
  directory dir_name do
    owner node['jenkins']['server']['user']
    group node['jenkins']['server']['group']
    mode '0700'
    recursive true
  end
end

execute "ssh-keygen -f #{File.join(ssh_dir, "id_rsa")} -N ''" do
  user node['jenkins']['server']['user']
  group node['jenkins']['server']['group']
  not_if { File.exists?(File.join(ssh_dir, "id_rsa")) }
  notifies :create, "ruby_block[store_server_ssh_pubkey]", :immediately
end

ruby_block "store_server_ssh_pubkey" do
  block do
    node.set['jenkins']['server']['pubkey'] = IO.read(File.join(ssh_dir, "id_rsa.pub"))
    node.save unless Chef::Config[:solo]
  end
  action :nothing
end

include_recipe "jenkins::_server_#{node['jenkins']['server']['install_method']}"

node['jenkins']['server']['plugins'].each do |plugin|
  version = 'latest'
  if plugin.is_a?(Hash)
    name = plugin['name']
    version = plugin['version'] if plugin['version']
  else
    name = plugin
  end

  # Plugins installed from the Jenkins Update Center are written to disk with
  # the `*.jpi` extension. Although plugins downloaded from the Jenkins Mirror
  # have an `*.hpi` extension we will save the plugins with a `*.jpi` extension
  # to match Update Center's behavior.
  remote_file File.join(plugins_dir, "#{name}.jpi") do
    source "#{node['jenkins']['mirror']}/plugins/#{name}/#{version}/#{name}.hpi"
    owner node['jenkins']['server']['user']
    group node['jenkins']['server']['group']
    backup false
    action :create_if_missing
    notifies :restart, "service[jenkins]"
    notifies :create, "ruby_block[block_until_operational]"
  end
end

ruby_block "block_until_operational" do
  block do
    Chef::Log.info "Waiting until Jenkins is listening on port #{node['jenkins']['server']['port']}"
    until JenkinsHelper.service_listening?(node['jenkins']['server']['port']) do
      sleep 1
      Chef::Log.debug(".")
    end

    Chef::Log.info "Waiting until the Jenkins API is responding"
    test_url = URI.parse("#{node['jenkins']['server']['url']}/api/json")
    until JenkinsHelper.endpoint_responding?(test_url) do
      sleep 1
      Chef::Log.debug(".")
    end
  end
  action :nothing
end

log "ensure_jenkins_is_running" do
  notifies :start, "service[jenkins]", :immediately
  notifies :create, "ruby_block[block_until_operational]", :immediately
end
