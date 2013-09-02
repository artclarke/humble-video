#
# Cookbook Name:: jenkins
# Attributes:: node
#
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

case node['platform_family']
when "windows"
  default['jenkins']['node']['home'] = "C:/jenkins"
  default['jenkins']['node']['log_dir']  = "C:/jenkins"
  default['jenkins']['node']['agent_type'] = "windows"
when "mac_os_x"
  default['jenkins']['node']['home'] = "/Users/jenkins"
  default['jenkins']['node']['log_dir']  = "/var/log/jenkins"
  default['jenkins']['node']['agent_type'] = "jnlp"
else
  default['jenkins']['node']['home'] = "/home/jenkins"
  default['jenkins']['node']['log_dir']  = "/var/log/jenkins"
  default['jenkins']['node']['agent_type'] = "jnlp"
end

default['jenkins']['node']['user'] = "jenkins-node"
default['jenkins']['node']['group'] = "jenkins-node"
default['jenkins']['node']['shell'] = "/bin/sh"
default['jenkins']['node']['name'] = node['fqdn']
default['jenkins']['node']['description'] =
  "#{node['platform']} #{node['platform_version']} " <<
  "[#{node['kernel']['os']} #{node['kernel']['release']} #{node['kernel']['machine']}] " <<
  "slave on #{node['hostname']}"
default['jenkins']['node']['labels'] = (node['tags'] || [])

default['jenkins']['node']['env'] = nil
default['jenkins']['node']['jvm_options'] = nil
default['jenkins']['node']['executors'] = 1
default['jenkins']['node']['in_demand_delay'] = 0
default['jenkins']['node']['idle_delay'] = 1

# Usage
#    normal - Utilize this slave as much as possible
#    exclusive - Leave this machine for tied jobs only
default['jenkins']['node']['mode'] = "normal"

# Availability
#    always - Keep this slave on-line as much as possible
#    demand - Take this slave on-line when in demand and off-line when idle
default['jenkins']['node']['availability'] = "always"

# SSH options
default['jenkins']['node']['ssh_host'] = node['fqdn']
default['jenkins']['node']['ssh_port'] = 22
default['jenkins']['node']['ssh_user'] = default['jenkins']['node']['user']
default['jenkins']['node']['ssh_pass'] = nil
default['jenkins']['node']['ssh_private_key'] = nil
