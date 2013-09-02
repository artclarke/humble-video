require 'spec_helper'

describe 'java::openjdk' do
  platforms = {
    'ubuntu' => {
      'packages' => ['openjdk-6-jdk', 'default-jre-headless'],
      'versions' => ['10.04', '12.04'],
      'update_alts' => true
    },
    'centos' => {
      'packages' => ['java-1.6.0-openjdk', 'java-1.6.0-openjdk-devel'],
      'versions' => ['5.8', '6.3'],
      'update_alts' => true
    },
    'smartos' => {
      'packages' => ['sun-jdk6', 'sun-jre6'],
      'versions' => ['joyent_20130111T180733Z'],
      'update_alts' => false
    }
  }

  # Regression test for COOK-2989
  context 'update-java-alternatives' do
    let(:chef_run) do
      ChefSpec::ChefRunner.new(:platform => 'ubuntu', :version => '12.04').converge('java::openjdk')
    end

    it 'executes update-java-alternatives with the right commands' do
      # We can't use a regexp in the matcher's #with attributes, so
      # let's reproduce the code block with the heredoc + gsub:
      code_string = <<-EOH.gsub(/^\s+/, '')
      update-alternatives --install /usr/bin/java java /usr/lib/jvm/java-6-openjdk-amd64/jre/bin/java 1061 && \
      update-alternatives --set java /usr/lib/jvm/java-6-openjdk-amd64/jre/bin/java
      EOH
      expect(chef_run).to execute_bash_script('update-java-alternatives').with(:code => code_string)
    end
  end

  platforms.each do |platform, data|
    data['versions'].each do |version|
      context "On #{platform} #{version}" do
        let(:chef_run) do
          ChefSpec::ChefRunner.new(:platform => platform, :version => version).converge('java::openjdk')
        end

        data['packages'].each do |pkg|
          it "installs package #{pkg}" do
            expect(chef_run).to install_package(pkg)
          end

          it 'sends notification to update-java-alternatives' do
            expectation = data['update_alts'] ? :to : :not_to
            expect(chef_run.package(pkg)).send(expectation, notify("bash[update-java-alternatives]", :run))
          end
        end
      end
    end
  end

  describe 'license acceptance file' do
    {'centos' => '6.3','ubuntu' => '12.04'}.each_pair do |platform, version|
      context platform do
        let(:chef_run) do
          ChefSpec::ChefRunner.new(:platform => platform, :version => version).converge('java::openjdk')
        end

        it 'does not write out license file' do
          expect(chef_run).not_to create_file("/opt/local/.dlj_license_accepted")
        end
      end
    end

    context 'smartos' do
      let(:chef_run) do
        ChefSpec::ChefRunner.new(:platform => 'smartos', :version => 'joyent_20130111T180733Z', :evaluate_guards => true)
      end

      context 'when auto_accept_license is true' do
        it 'writes out a license acceptance file' do
          chef_run.node.set['java']['accept_license_agreement'] = true
          expect(chef_run.converge('java::openjdk')).to create_file("/opt/local/.dlj_license_accepted")
        end
      end

      context 'when auto_accept_license is false' do
        it 'does not write license file' do
          chef_run.node.set['java']['accept_license_agreement'] = false
          expect(chef_run.converge('java::openjdk')).not_to create_file("/opt/local/.dlj_license_accepted")
        end
      end
    end

  end
end
