require 'spec_helper'

describe 'java::default' do
  let(:chef_run) do
    runner = ChefSpec::ChefRunner.new(
      :platform => 'debian',
      :version => '7.0'
    )
    runner.converge('java::default')
  end
  it 'should include the openjdk recipe by default' do
    expect(chef_run).to include_recipe('java::openjdk')
  end

  context 'windows' do
    let(:chef_run) do
      runner = ChefSpec::ChefRunner.new(
        'platform' => 'windows',
        'version' => '2008R2'
      )
      runner.node.set['java']['install_flavor'] = 'windows'
      runner.node.set['java']['windows']['url'] = 'http://example.com/windows-java.msi'
      runner.converge('java::default')
    end

    it 'should include the windows recipe' do
      expect(chef_run).to include_recipe('java::windows')
    end
  end

  context 'oracle' do
    let(:chef_run) do
      runner = ChefSpec::ChefRunner.new
      runner.node.set['java']['install_flavor'] = 'oracle'
      runner.converge('java::default')
    end

    it 'should include the oracle recipe' do
      expect(chef_run).to include_recipe('java::oracle')
    end
  end

  context 'oracle_i386' do
    let(:chef_run) do
      runner = ChefSpec::ChefRunner.new
      runner.node.set['java']['install_flavor'] = 'oracle_i386'
      runner.converge('java::default')
    end

    it 'should include the oracle_i386 recipe' do
      expect(chef_run).to include_recipe('java::oracle_i386')
    end
  end

  context 'ibm' do
    let(:chef_run) do
      runner = ChefSpec::ChefRunner.new
      runner.node.set['java']['install_flavor'] = 'ibm'
      runner.node.set['java']['ibm']['url'] = 'http://example.com/ibm-java.bin'
      runner.converge('java::default')
    end

    it 'should include the ibm recipe' do
      expect(chef_run).to include_recipe('java::ibm')
    end
  end

end
