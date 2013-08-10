Description
===========

This cookbook provides an easy way to install the Xuggle version of Swig.

Requirements
============

## Cookbooks:

* build-essential

## Platforms:

* Debian
* Ubuntu
* Amazon

Attributes
==========

* `node['xuggleswig']['install_method']` - Installation method, ':source' or ':package' - default ':source'
* `node['xuggleswig']['prefix']` - Location prefix of where the installation files will go if installing via ':source'
* `node['xuggleswig']['git_repository']` - Location of the source git repository
* `node['xuggleswig']['git_revision']` - Revision of the git repository to install
* `node['xuggleswig']['compile_flags']` - Array of flags to use in compilation process

Usage
=====

1) include `recipe[xuggleswig]` in a run list
2) tweak the attributes via attributes/default.rb
    --- OR ---
    override the attribute on a higher level (http://wiki.opscode.com/display/chef/Attributes#Attributes-AttributesPrecedence)

References
==========

* [Xuggle Swig home page] (http://github.com/artclarke/xuggle-swig)

License and Authors
===================

Author: Art Clarke <art@humble.io>
Copyright: 2013, Humble Software
All Rights Reserved

