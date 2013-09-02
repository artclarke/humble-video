#
# Cookbook Name:: jenkins
# Resource:: node
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

actions :create, :update, :delete, :connect, :disconnect, :offline, :online

attribute :description, :kind_of => String
attribute :remote_fs, :kind_of => String
attribute :executors, :kind_of => Integer
attribute :mode, :equal_to => ["normal", "exclusive"]
attribute :labels, :kind_of => Array
attribute :launcher, :equal_to => ["jnlp", "command", "ssh"]
attribute :availability, :equal_to => ["always", "demand"]
attribute :in_demand_delay, :kind_of => Integer
attribute :idle_delay, :kind_of => Integer
attribute :env, :kind_of => Hash

#XXX LWRPs cannot be subclassed?
#case launcher
#when jnlp
#when command
attribute :command, :kind_of => String
#when ssh
attribute :host, :kind_of => String
attribute :port, :kind_of => Integer
attribute :username, :kind_of => String
attribute :password, :kind_of => String
attribute :private_key, :kind_of => String
attribute :jvm_options, :kind_of => String

def initialize(name, run_context=nil)
  super
  @action = :create
end
