#
# Cookbook Name:: jenkins
# Provider:: cli
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

def action_run
  url = @new_resource.url || node['jenkins']['server']['url']
  home = @new_resource.home || node['jenkins']['node']['home']
  username = @new_resource.username ||  node['jenkins']['cli']['username']
  password = @new_resource.password ||  node['jenkins']['cli']['password']
  password_file = @new_resource.password_file ||  node['jenkins']['cli']['password_file']
  key_file = @new_resource.key_file || node['jenkins']['cli']['key_file']
  jvm_options = @new_resource.jvm_options || node['jenkins']['cli']['jvm_options']

  #recipes will chown to jenkins later if this doesn't already exist
  directory "home for jenkins-cli.jar" do
    action :create
    path node['jenkins']['node']['home']
  end

  cli_jar = ::File.join(home, "jenkins-cli.jar")
  remote_file cli_jar do
    source "#{url}/jnlpJars/jenkins-cli.jar"
    not_if { ::File.exists?(cli_jar) }
  end

  java_home = node['jenkins']['java_home'] || (node.attribute?('java') ? node['java']['jdk_dir'] : nil)
  if java_home == nil
    java = "java"
  else
    java = ::File.join(java_home, "bin", "java")
  end
  if jvm_options
    java << " #{jvm_options}"
  end

  if key_file
    command = "#{java} -jar #{cli_jar} -i #{key_file} -s #{url} #{@new_resource.command}"
  else
    command = "#{java} -jar #{cli_jar} -s #{url} #{@new_resource.command}"
  end

  if username
    command << " --username #{username}"
  end
  if password
    command << " --password #{password}"
  end
  if password_file
    command << " --password_file #{password_file}"
  end

  je = jenkins_execute command do
        cwd home
        block { |stdout| new_resource.block.call(stdout) } if new_resource.block
      end

  new_resource.updated_by_last_action(je.updated?)
end
