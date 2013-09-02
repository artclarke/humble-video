require 'minitest/spec'
require File.expand_path('../support/helpers', __FILE__)

describe_recipe 'java::oracle_rpm' do
  include Helpers::Java

  it "installs the correct version of the jre/jdk" do
    java_version = shell_out("java -version")
    version_line = java_version.stderr
    version_line.must_match /"#{node['java']['oracle_rpm']['version']}"/
  end

  it "properly sets JAVA_HOME environment variable" do
    env_java_home = shell_out("echo $JAVA_HOME")
    java_home = env_java_home.stdout.chomp
    java_home.must_equal node['java']['java_home']
  end

end
