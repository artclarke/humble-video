#
# Cookbook Name:: jenkins
# Recipe:: _node_ssh
#
# Author:: Doug MacEachern <dougm@vmware.com>
# Author:: Fletcher Nichol <fnichol@nichol.ca>
#
# Copyright 2010, VMware, Inc.
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

unless Chef::Config[:solo]
  unless node['jenkins']['server']['pubkey']
    host = node['jenkins']['server']['host']
    if host == node['fqdn']
      host = URI.parse(node['jenkins']['server']['url']).host
    end
    jenkins_node = search('node', "fqdn:#{host}").first
    node.set['jenkins']['server']['pubkey'] = jenkins_node['jenkins']['server']['pubkey']
  end
end

group node['jenkins']['node']['group']

user node['jenkins']['node']['user'] do
  comment "Jenkins CI node (ssh)"
  gid node['jenkins']['node']['group']
  home node['jenkins']['node']['home']
  shell node['jenkins']['node']['shell']
end

directory node['jenkins']['node']['home'] do
  owner node['jenkins']['node']['user']
  group node['jenkins']['node']['group']
  action :create
end

directory "#{node['jenkins']['node']['home']}/.ssh" do
  owner node['jenkins']['node']['user']
  group node['jenkins']['node']['group']
  mode '0700'
  action :create
end

file "#{node['jenkins']['node']['home']}/.ssh/authorized_keys" do
  content node['jenkins']['server']['pubkey']
  owner node['jenkins']['node']['user']
  group node['jenkins']['node']['group']
  mode '0600'
  action :create
end

jenkins_node node['jenkins']['node']['name'] do
  description  node['jenkins']['node']['description']
  executors    node['jenkins']['node']['executors']
  remote_fs    node['jenkins']['node']['home']
  labels       node['jenkins']['node']['labels']
  mode         node['jenkins']['node']['mode']
  launcher     "ssh"
  availability node['jenkins']['node']['availability']
  env          node['jenkins']['node']['env']
  #ssh options
  host         node['jenkins']['node']['ssh_host']
  port         node['jenkins']['node']['ssh_port']
  username     node['jenkins']['node']['ssh_user']
  password     node['jenkins']['node']['ssh_pass']
  private_key  node['jenkins']['node']['ssh_private_key']
  jvm_options  node['jenkins']['node']['jvm_options']
end
