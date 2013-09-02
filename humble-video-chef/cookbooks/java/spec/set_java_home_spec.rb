require 'spec_helper'

describe 'java::set_java_home' do
  let(:chef_run) do
    runner = ChefSpec::ChefRunner.new
    runner.node.set['java']['java_home'] = '/opt/java'
    runner.converge('java::set_java_home')
  end
  it 'it should set the java home environment variable' do
    expect(chef_run).to execute_ruby_block('set-env-java-home')
  end

  it 'should create the profile.d directory' do
    expect(chef_run).to create_directory('/etc/profile.d')
  end

  it 'should create jdk.sh with the java home environment variable' do
    expect(chef_run).to create_file_with_content(
      '/etc/profile.d/jdk.sh',
      'export JAVA_HOME=/opt/java'
    )
  end

end
