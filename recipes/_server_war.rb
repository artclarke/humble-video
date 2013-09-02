#
# Cookbook Name:: jenkins
# Recipe:: _server_war
#
# Author:: AJ Christensen <aj@junglist.gen.nz>
# Author:: Doug MacEachern <dougm@vmware.com>
# Author:: Fletcher Nichol <fnichol@nichol.ca>
# Author:: Seth Chisamore <schisamo@opscode.com>
#
# Copyright 2010, VMware, Inc.
# Copyright 2012, Opscode, Inc.
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

include_recipe "runit"

war_version = node['jenkins']['server']['version'].nil? ? "latest" : node['jenkins']['server']['version']

remote_file File.join(node['jenkins']['server']['home'], "jenkins.war") do
  source "#{node['jenkins']['mirror']}/war/#{war_version}/jenkins.war"
  checksum node['jenkins']['server']['war_checksum'] unless node['jenkins']['server']['war_checksum'].nil?
  owner node['jenkins']['server']['user']
  group node['jenkins']['server']['group']
  notifies :restart, "service[jenkins]"
  notifies :create, "ruby_block[block_until_operational]"
end

runit_service "jenkins"
