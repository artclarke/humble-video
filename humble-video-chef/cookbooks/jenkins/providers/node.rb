#
# Cookbook Name:: jenkins
# Provider:: node
#
# Author:: Doug MacEachern <dougm@vmware.com>
# Author:: Fletcher Nichol <fnichol@nichol.ca>
#
# Copyright:: 2010, VMware, Inc.
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

def load_current_resource
  @current_resource = Chef::Resource::JenkinsNode.new(@new_resource.name)
  # Inject some useful platform labels
  @new_resource.labels((new_resource.labels + platform_labels).uniq)
  @current_resource
end

def action_update
  action_create
end

def action_create

  gscript = "#{new_resource.remote_fs}/manage_#{new_resource.name}.groovy"

  file gscript do
    action :delete
    backup false
  end

  cookbook_file "#{node['jenkins']['node']['home']}/node_info.groovy" do
    source "node_info.groovy"
  end

  jenkins_cli "groovy node_info.groovy #{new_resource.name}" do
    block do |stdout|
      current_node = JSON.parse(stdout)
      node_exists = current_node.keys.size > 0
      if !node_exists && new_resource.action.to_s == "update"
        Chef::Application.fatal! "Cannot update #{new_resource} - node does not exist!"
      end
      new_node = new_resource.to_hash
      if !node_exists || jenkins_node_compare(current_node, new_node)
        ::File.open(gscript, "w") {|f| f.write jenkins_node_manage(new_node) }
      end
    end
  end

  jenkins_cli "groovy #{gscript}" do
    only_if { ::File.exists?(gscript) }
  end

  ruby_block "new_resource.updated" do
    block { new_resource.updated_by_last_action(true) }
    only_if { ::File.exists?(gscript) }
  end
end

def action_delete
  jenkins_cli "delete-node #{new_resource.name}"
end

def action_connect
  jenkins_cli "connect-node #{new_resource.name}"
end

def action_disconnect
  jenkins_cli "disconnect-node #{new_resource.name}"
end

def action_online
  jenkins_cli "online-node #{new_resource.name}"
end

def action_offline
  jenkins_cli "offline-node #{new_resource.name}"
end

private

def platform_labels
  platform_labels = []
  platform_labels << node['platform'] # ubuntu
  platform_labels << node['platform_family'] # debian
  platform_labels << node['platform_version'] # 10.04
  platform_labels << "#{node['platform']}-#{node['platform_version']}" # ubuntu-10.04
  platform_labels << node['kernel']['machine'] # x86_64
  platform_labels << node['os'] # linux
  platform_labels << node['os_version'] # 2.6.32-38-server
  platform_labels
end
