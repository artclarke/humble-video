#
# Cookbook Name:: jenkins
# Resource:: execute
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

actions :run

attribute :command, :kind_of => String
attribute :cwd, :kind_of => String
attribute :timeout, :kind_of => Integer
attribute :block, :kind_of => Proc

def initialize(name, run_context=nil)
  super
  @action = :run
  @command = name
end

def block(&block)
  if block_given? and block
    @block = block
  else
    @block
  end
end
