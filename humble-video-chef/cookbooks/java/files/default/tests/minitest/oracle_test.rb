require 'minitest/spec'
require File.expand_path('../support/helpers', __FILE__)

describe_recipe 'java::oracle' do
  include Helpers::Java

  it "installs the correct version of the jdk" do
    java_version = shell_out("java -version")
    version_line = java_version.stderr
    jdk_version = version_line.scan(/\.([678])\./)[0][0]
    assert_equal node['java']['jdk_version'], jdk_version
  end

  it "properly sets JAVA_HOME environment variable" do
    env_java_home = shell_out("echo $JAVA_HOME")
    java_home = env_java_home.stdout.chomp
    assert_equal node['java']['java_home'], java_home
  end

end
