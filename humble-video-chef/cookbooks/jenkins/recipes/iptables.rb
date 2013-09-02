#
# Cookbook Name:: jenkins
# Recipe:: iptables
#
# Author:: Fletcher Nichol <fnichol@nichol.ca>
#
# Copyright 2011, Fletcher Nichol.
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

if platform_family?("debian", "rhel")
  include_recipe "iptables"
  iptables_rule "port_jenkins" do
    if node['jenkins']['iptables_allow'] == "enable"
      enable true
    else
      enable false
    end
  end
end
