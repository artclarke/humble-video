name             "humbleswig"
maintainer       "Humble Software"
maintainer_email "art@humble.io"
license          "All Rights Reserved"
description      "Installs/Configures humbleswig"
long_description IO.read(File.join(File.dirname(__FILE__), 'README.md'))
version          "0.0.3"

%w{ debian ubuntu centos redhat fedora scientific amazon }.each do |os|
supports os
end

depends "build-essential"
depends "git"

recipe "humbleswig", "Installs humbleswig."
recipe "humbleswig::source", "Installs humbleswig from source."
