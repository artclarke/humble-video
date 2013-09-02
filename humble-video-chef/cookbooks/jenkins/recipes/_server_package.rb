#
# Author:: Guilhem Lettron <guilhem.lettron@youscribe.com>
# Cookbook Name:: jenkins
# Recipe:: _server_package
#
# Copyright 2013, Youscribe
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
when "debian"
  include_recipe "apt"

  apt_repository "jenkins" do
    uri "http://pkg.jenkins-ci.org/debian"
    distribution "binary/"
    components [""]
    key "http://pkg.jenkins-ci.org/debian/jenkins-ci.org.key"
    action :add
  end

  config_path = "/etc/default/jenkins"
  config_template = "default.erb"

when "rhel"
  include_recipe "yum"

  yum_key "RPM-GPG-KEY-jenkins-ci" do
    url "http://pkg.jenkins-ci.org/redhat/jenkins-ci.org.key"
    action :add
  end

  yum_repository "jenkins-ci" do
    url "http://pkg.jenkins-ci.org/redhat"
    key "RPM-GPG-KEY-jenkins-ci"
    action :add
  end

  config_path = "/etc/sysconfig/jenkins"
  config_template = "sysconfig.erb"
end

package "jenkins" do
  unless node['jenkins']['server']['version'].nil?
    version node['jenkins']['server']['version']
  end
end

template config_path do
  source config_template
  variables node['jenkins']['server'].to_hash
  owner "root"
  group "root"
  mode "0644"
  notifies :restart, "service[jenkins]", :immediately
  notifies :create, "ruby_block[block_until_operational]", :immediately
end

service "jenkins" do
  supports :status => true, :restart => true, :reload => true
  action [ :enable, :start ]
end
