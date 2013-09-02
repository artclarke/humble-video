Description
===========

Installs and configures Jenkins CI server & node slaves. Resource providers to 
support automation via jenkins-cli, including job create/update.

Requirements
============

Chef 0.10.10+ and Ohai 6.10+ for platform_family use.

## Platform:

### Server (Master) Recipe

* Ubuntu
* RHEL/CentOS

### Node (Slave) Recipe

Agent Flavor:

* `ssh` - Any Unix platform that is running `sshd`.
* `jnlp` - Most Unix platforms.
* `windows` - Windows platforms only. Depends on .NET Framework.

Attributes
==========

### Common Attributes

* `node['jenkins']['mirror']` - Base URL for downloading all code (WAR file and 
  plugins).
* `node['jenkins']['java_home']` - Java install path, used for for cli commands.
* `node['jenkins']['iptables_allow']` - If iptables is enabled, add a rule 
  passing `node['jenkins']['server']['port']`.

### Master/Server related Attributes

* `node['jenkins']['server']['install_method']` - Whether Jenkins is installed 
  from packages or run from a WAR file.
* `node['jenkins']['server']['home']` - Location of `JENKINS_HOME` directory.
* `node['jenkins']['server']['user']` - User the Jenkins server runs as.
* `node['jenkins']['server']['group']` - Jenkins user primary group.
* `node['jenkins']['server']['port']` - TCP port Jenkins server listens on.
* `node['jenkins']['server']['url']` - Base URL of the Jenkins server.
* `node['jenkins']['server']['plugins']` - Download the latest version of 
  plugins in this Array, bypassing update center. The members of the Array can 
  either be strings if the latest version desired OR a Hash of the form 
`{'name' => 'git', 'version' => '1.4.0'}` if a specific version is required.
* `node['jenkins']['server']['jvm_options']` - Additional tuning parameters 
  to pass the underlying JVM process.
* `node['jenkins']['http_proxy']['variant']` - use `nginx` or `apache2` to 
  proxy traffic to jenkins backend (`nginx` by default)
* `node['jenkins']['http_proxy']['www_redirect']` - add a redirect rule for 
  'www.*' URL requests ("disable" by default)
* `node['jenkins']['http_proxy']['listen_ports']` - list of HTTP ports for the 
  HTTP proxy to listen on ([80] by default).
* `node['jenkins']['http_proxy']['host_name']` - primary vhost name for the 
  HTTP proxy to respond to (`node['fqdn']` by default).
* `node['jenkins']['http_proxy']['host_aliases']` - optional list of other host 
  aliases to respond to (empty by default).
* `node['jenkins']['http_proxy']['client_max_body_size']` - max client upload 
  size ("1024m" by default, nginx only). 
* `node['jenkins']['http_proxy']['server_auth_method']` - Authentication with
  the server can be done with cas (using `apache2::mod_auth_cas`), or htauth
  (basic). The default is no authentication.
* `node['jenkins']['http_proxy']['basic_auth_username']` - Username to use for
  HTTP Basic Authenitcation.
* `node['jenkins']['http_proxy']['basic_auth_password']` - Password to use with
  HTTP Basic Authenitcation.
* `node['jenkins']['http_proxy']['cas_login_url']` - Login url for cas if using
  cas authentication.
* `node['jenkins']['http_proxy']['cas_validate_url']` - Validation url for cas
  if using cas authentication.
* `node['jenkins']['http_proxy']['cas_validate_server']` - Whether to validate
  the server cert. Defaults to off.
* `node['jenkins']['http_proxy']['cas_root_proxy_url']` - If set, sets the url
  that the cas server redirects to after auth.

### Node/Slave related Attributes

* `node['jenkins']['node']['agent_type']` - Type of agent to communicate with 
  this slave/node. Valid values include `jnlp`, `ssh` and `windows`. (default 
  is `jnlp`)
* `node['jenkins']['node']['name']` - Name of the node within Jenkins.
* `node['jenkins']['node']['description']` - Jenkins node description.
* `node['jenkins']['node']['executors']` - Number of node executors.
* `node['jenkins']['node']['home]` - Home directory ("Remote FS root") of the node.
* `node['jenkins']['node']['labels']` - Node labels.
* `node['jenkins']['node']['mode']` - Node usage mode, `normal` or `exclusive`
  (tied jobs only).
* `node['jenkins']['node']['availability']` - `always` keeps node on-line, 
  `demand` off-lines when idle.
* `node['jenkins']['node']['in_demand_delay']` - number of minutes for which 
  jobs must be waiting in the queue before attempting to launch this slave.
* `node['jenkins']['node']['idle_delay']` - number of minutes that this slave 
  must remain idle before taking it off-line. 
* `node['jenkins']['node']['env']` - "Node Properties" -> "Environment 
  Variables".
* `node['jenkins']['node']['user']` - user the slave runs as.
* `node['jenkins']['node']['ssh_host']` - Hostname or IP Jenkins Master should 
  connect to when launching an SSH slave.
* `node['jenkins']['node']['ssh_port']` - SSH port Jenkins Master should 
  connect to when launching a slave.
* `node['jenkins']['node']['ssh_user']` - SSH slave user name (only required if 
  Jenkins server and slave user is different).
* `node['jenkins']['node']['ssh_pass']` - SSH slave password (not required when 
  server is installed via `jenkins::server` recipe).
* `node['jenkins']['node']['ssh_private_key']` - Jenkins Master defaults to: 
  `JENKINS_HOME/.ssh/id_rsa` (created by the `jenkins::server` recipe).
* `node['jenkins']['node']['jvm_options']` - Additional tuning parameters to 
  pass the underlying JVM process.

Recipes
=======

server
------

Creates all required directories, installs Jenkins and generates an ssh private 
key and stores the ssh public key in the `node['jenkins']['server']['pubkey']` 
attribute for use by the node recipes. The installation method is controlled by 
the `node['jenkins']['server']['install_method']` attribute. The following 
install methods are supported:

* __package__ - Installs Jenkins from the official jenkins-ci.org packages.
* __war__ - Downloads the latest version of the Jenkins WAR file from 
  http://jenkins-ci. The server process is configured to run as a runit 
  service.

node
----

The type of agent that is used to communicate with the slave is determined by 
the attribute `node['jenkins']['node']['agent_type']`. The following agent 
types are supported:

* __ssh__ - Creates the user and group for the Jenkins slave to run as and sets 
`.ssh/authorized_keys` to the `node['jenkins']['server']['pubkey']` attribute.  
The [jenkins-cli.jar](http://wiki.jenkins-ci.org/display/JENKINS/Jenkins+CLI) 
is downloaded from the Jenkins server and used to manage the nodes via the 
[groovy](http://wiki.jenkins-ci.org/display/JENKINS/Jenkins+Script+Console) cli 
command. Jenkins is configured to launch a slave agent on the node using it's 
[SSH slave plugin](http://wiki.jenkins-ci.org/display/JENKINS/SSH+Slaves+plugin).
* __jnlp__ - Creates the user and group for the Jenkins slave to run as and 
`/jnlpJars/slave.jar` is downloaded from the Jenkins server. The slave process 
is configured to run as a runit service.
* __windows__ - Creates the home directory for the node slave and sets `JENKINS_HOME` and 
`JENKINS_URL` system environment variables.  The 
[winsw](http://weblogs.java.net/blog/2008/09/29/winsw-windows-service-wrapper-less-restrictive-license) 
Windows service wrapper will be downloaded and installed, along with generating 
`jenkins-slave.xml` from a template.  Jenkins is configured with the node as a 
[jnlp](http://wiki.jenkins-ci.org/display/JENKINS/Distributed+builds) slave and 
`/jnlpJars/slave.jar` is downloaded from the Jenkins server.  The 
`jenkinsslave` service will be started the first time the recipe is run or if 
the service is not running.  The 'jenkinsslave' service will be restarted if 
`/jnlpJars/slave.jar` has changed.  The end results is functionally the same 
had you chosen the option to 
[Let Jenkins control this slave as a Windows service](http://wiki.jenkins-ci.org/display/JENKINS/Installing+Jenkins+as+a+Windows+service).

proxy
-----

Installs a proxy and creates a vhost to route traffic to the installed Jenkins 
server. The type of HTTP proxy that is installed and configured is determined 
by the `node['jenkins']['http_proxy']['variant']` attribute. The following HTTP
proxy variants are supported:

* __apache2__
* __nginx__

Resource/Provider
=================

jenkins_cli
-----------

This resource can be used to execute the Jenkins cli from your recipes.  For 
example, install plugins via update center and restart Jenkins:

    %w{ git URLSCM build-publisher }.each do |plugin|
      jenkins_cli "install-plugin #{plugin}"
      jenkins_cli "safe-restart"
    end

jenkins_node
------------

This resource can be used to configure nodes as the `node_ssh` and 
`node_windows` recipes do or "Launch slave via execution of command on the 
Master":

    jenkins_node node['fqdn'] do
      description  "My node for things, stuff and whatnot"
      executors 5
      remote_fs "/var/jenkins"
      launcher "command"
      command "ssh -i my_key #{node[:fqdn]} java -jar #{remote_fs}/slave.jar"
      env "ANT_HOME" => "/usr/local/ant", "M2_REPO" => "/dev/null"
    end

jenkins_job
-----------

This resource manages jenkins jobs, supporting the following actions:

    :create, :update, :delete, :build, :disable, :enable

The `:create` and `:update` actions require a jenkins job config.xml.  Example:

    git_branch = 'master'
    job_name = "sigar-#{branch}-#{node[:os]}-#{node[:kernel][:machine]}"

    job_config = File.join(node[:jenkins][:node][:home], "#{job_name}-config.xml")

    jenkins_job job_name do
      action :nothing
      config job_config
    end

    template job_config do
      source "sigar-jenkins-config.xml"
      variables :job_name => job_name, :branch => git_branch, :node => node[:fqdn]
      notifies :update, resources(:jenkins_job => job_name), :immediately
      notifies :build, resources(:jenkins_job => job_name), :immediately
    end

Jenkins Node Authentication
===========================

If your Jenkins instance requires authentication, you'll either need to embed 
user:pass in `node['jenkins']['server']['url']` or issue a jenkins-cli.jar 
login command prior to using the `jenkins::node_*` recipes.  For example, 
define a role like so:

    name "jenkins_ssh_node"
    description "cli login & register ssh slave with Jenkins"
    run_list %w{ mycompany-jenkins::jenkins_login jenkins::node_ssh }

Where the jenkins_login recipe is simply:

    jenkins_cli "login --username #{node['jenkins']['username']} --password #{node['jenkins']['password']}"

License and Author
==================

|                      |                                          |
|:---------------------|:-----------------------------------------|
| **Original Author**  | Doug MacEachern (<dougm@vmware.com>)     |
| **Contributor**      | AJ Christensen <aj@junglist.gen.nz>      |
| **Contributor**      | Fletcher Nichol <fnichol@nichol.ca>      |
| **Contributor**      | Roman Kamyk <rkj@go2.pl>                 |
| **Contributor**      | Darko Fabijan <darko@renderedtext.com>   |
| **Contributor**      | Seth Chisamore <schisamo@opscode.com>    |
|                      |                                          |
| **Copyright**        | Copyright (c) 2010 VMware, Inc.          |
| **Copyright**        | Copyright (c) 2011 Fletcher Nichol       |
| **Copyright**        | Copyright (c) 2013 Opscode, Inc.         |

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
