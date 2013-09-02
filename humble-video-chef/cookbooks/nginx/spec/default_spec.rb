require 'chefspec'

class ChefSpec::ChefRunner
  def append(recipe)
    runner = Chef::Runner.new(recipe.run_context)
    runner.converge
    self
  end
end

module ChefSpec::Matchers
  RSpec::Matchers.define :enable_nginx_site do |site|
    match do |chef_run|
      chef_run.resources.any? do |resource|
        resource.resource_name == :execute and
            resource.name =~ /.*nxensite.*#{site}/
      end
    end
  end

  RSpec::Matchers.define :disable_nginx_site do |site|
    match do |chef_run|
      chef_run.resources.any? do |resource|
        resource.resource_name == :execute and
          resource.name =~ /.*nxdissite.*#{site}/
      end
    end
  end
end

def fake_recipe(run, &block)
  recipe = Chef::Recipe.new("nginx_spec", "default", run.run_context)
  recipe.instance_eval(&block)
end

describe 'nginx::default' do
  before do
    # stub out ohai
    Chef::Config[:config_file] = '/dev/null'
  end
  let(:runner) do
    ChefSpec::ChefRunner.new(
      :platform => 'debian',
      :version  => '7.0'
    )
  end
  let(:chef_run) do
    runner.converge 'nginx::default'
  end

  it "loads the ohai plugin" do
    expect(chef_run).to include_recipe 'nginx::ohai_plugin'
  end

  it "builds from source when specified" do
    runner.node.set['nginx']['install_method'] = 'source'
    expect(chef_run).to include_recipe 'nginx::source'
  end

  context "configured to install by package" do
    context "in a redhat-based platform" do
      let(:redhat) do
        ChefSpec::ChefRunner.new(
          :platform => 'redhat',
          :version  => '6.3'
        )
      end
      let(:redhat_run) do
        redhat.converge 'nginx::default'
      end
      it "includes the yum::epel recipe if the source is epel" do
        redhat.node.set['nginx']['repo_source'] = 'epel'
        expect(redhat_run).to include_recipe 'yum::epel'
      end

      it "includes the nginx::repo recipe if the source is not epel" do
        redhat.node.set['nginx']['repo_source'] = 'nginx'
        expect(redhat_run).to include_recipe 'nginx::repo'
      end
    end

    it "installs the package" do
      package_name = chef_run.node['nginx']['package_name']
      expect(chef_run).to install_package package_name
    end

    it "enables the service" do
      expect(chef_run).to enable_service 'nginx'
    end

    it "executes common nginx configuration" do
      expect(chef_run).to include_recipe 'nginx::commons'
    end
  end

  it "starts the service" do
    expect(chef_run).to start_service 'nginx'
  end

  context "#nginx_site (definition)" do
    context "#enable => true (default)" do
      let(:run) do
        recipe = fake_recipe(chef_run) do
          nginx_site "foo"
        end
        chef_run.append(recipe)
      end

      it { expect(run).to execute_command "/usr/sbin/nxensite foo" }

      it do
        expect(run.execute("nxensite foo"))
            .to notify("service[nginx]", "reload")
      end
    end

    context "#enable => false" do
      let(:run) do
        recipe = fake_recipe(chef_run) do
          nginx_site "foo" do
            enable false
          end
        end
        chef_run.append(recipe)
      end

      it { expect(run).to execute_command "/usr/sbin/nxdissite foo" }

      it do
        expect(run.execute("nxdissite foo"))
            .to notify("service[nginx]", "reload")
      end
    end
  end

  context "spec helpers" do
    def run(enabled=true)
      recipe = fake_recipe(chef_run) do
        nginx_site "foo" do
          enable enabled
        end
      end
      chef_run.append(recipe)
    end

    it "#enable_nginx_site is provided" do
      expect(run).to enable_nginx_site "foo"
    end

    it "#disable_nginx_site is provided" do
      expect(run false).to disable_nginx_site "foo"
    end
  end
end
