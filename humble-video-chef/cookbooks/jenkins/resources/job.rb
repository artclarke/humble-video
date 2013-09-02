#
# Cookbook Name:: jenkins
# Resource:: job
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

actions :create, :update, :delete, :build, :disable, :enable

attribute :url, :kind_of => String
attribute :job_name, :kind_of => String
attribute :config, :kind_of => String

def initialize(name, run_context=nil)
  super
  @action = :update
  @job_name = name
  jenkins_node = jenkins_attributes_from_node(run_context)
  @url = (jenkins_node[:server] && jenkins_node[:server][:url]) || "http://localhost:8080"
end

private

def jenkins_attributes_from_node(run_context)
  jenkins_attr = if run_context && run_context.node
    run_context.node[:jenkins]
  end
  jenkins_attr || {}
end
