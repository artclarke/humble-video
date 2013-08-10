name             "xuggleswig"
maintainer       "Humble Software"
maintainer_email "art@humble.io"
license          "All Rights Reserved"
description      "Installs/Configures xuggleswig"
long_description IO.read(File.join(File.dirname(__FILE__), 'README.md'))
version          "0.0.3"

%w{ debian ubuntu centos redhat fedora scientific amazon }.each do |os|
supports os
end

depends "build-essential"
depends "git"

recipe "xuggleswig", "Installs xuggleswig."
recipe "xuggleswig::source", "Installs xuggleswig from source."
