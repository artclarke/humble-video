name             "jenkins"
maintainer       "Opscode, Inc."
maintainer_email "cookbooks@opscode.com"
license          "Apache 2.0"
description      "Installs and configures Jenkins CI server & slaves"
long_description IO.read(File.join(File.dirname(__FILE__), 'README.md'))
version          "0.8.0"

recipe 'default', 'Does nothing'
recipe 'iptables', 'Configures iptables to allow incoming connections to the jenkins instance'
recipe 'node', 'Installs a Jenkins node'
recipe 'proxy', 'Installs a web server proxy for communication with the jenkins instance'
recipe 'server', 'Installs Jenkins server'

depends "java"
depends "runit", ">= 1.0.0"
depends "apt"
depends "yum"

depends "apache2"
depends "nginx"
depends "iptables"
