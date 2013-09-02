#
# Cookbook Name:: jenkins
# Recipe:: _proxy_apache2
#
# Author:: Fletcher Nichol <fnichol@nichol.ca>
#
# Copyright 2011, Fletcher Nichol
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

include_recipe "apache2"

www_redirect = (node['jenkins']['http_proxy']['www_redirect'] == "enable")
host_name = node['jenkins']['http_proxy']['host_name'] || node['fqdn']

if node['jenkins']['http_proxy']['cas_validate_server'] == "cas"
  apache_module "mod_auth_cas"
end

if node['jenkins']['http_proxy']['ssl']['enabled']
  include_recipe "apache2::mod_ssl"
end

apache_module "proxy"
apache_module "proxy_http"
apache_module "vhost_alias"

if www_redirect || node['jenkins']['http_proxy']['ssl']['redirect_http']
  apache_module "rewrite"
end

template "#{node['apache']['dir']}/htpasswd" do
  variables( :username => node['jenkins']['http_proxy']['basic_auth_username'],
             :password => node['jenkins']['http_proxy']['basic_auth_password'])
  owner node['apache']['user']
  group node['apache']['user']
  mode '0600'
end

template "#{node['apache']['dir']}/sites-available/jenkins" do
  source      "apache_jenkins.erb"
  owner       'root'
  group       'root'
  mode        '0644'
  variables(
    :host_name        => host_name,
    :host_aliases     => node['jenkins']['http_proxy']['host_aliases'],
    :www_redirect     => www_redirect,
    :jenkins_port     => node['jenkins']['server']['port'],
    :redirect_http    => node['jenkins']['http_proxy']['ssl']['redirect_http'],
    :ssl_enabled      => node['jenkins']['http_proxy']['ssl']['enabled'],
    :ssl_listen_ports => node['jenkins']['http_proxy']['ssl']['ssl_listen_ports']
  )

  if File.exists?("#{node['apache']['dir']}/sites-enabled/jenkins")
    notifies  :restart, 'service[apache2]'
  end
end

apache_site "000-default" do
  enable  false
end

apache_site "jenkins" do
  enable true
end
