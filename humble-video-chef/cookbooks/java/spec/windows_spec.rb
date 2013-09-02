require 'chefspec'

describe 'java::windows' do
  let(:chef_run) { ChefSpec::ChefRunner.new.converge 'java::windows' }
  it 'should do something' do
    pending 'Your recipe examples go here.'
  end
end
