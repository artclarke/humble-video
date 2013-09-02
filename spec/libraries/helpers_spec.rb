#
# Author:: Joshua Timberman <joshua@opscode.com>
# Copyright:: Copyright (c) 2013, Opscode, Inc. <legal@opscode.com>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

$:.unshift(File.join(File.dirname(__FILE__), '..'))
require 'spec_helper'

describe Opscode::OpenJDK do
  let(:node) do
    {
      'java' => {
        'java_home' => '/usr/lib/jvm/default-java',
        'jdk_version' => 6
      },
      'kernel' => {
        'machine' => 'x86_64'
      },
      'platform_family' => 'debian',
      'platform' => 'ubuntu',
      'platform_version' => '12.04'
    }
  end

  let(:subject) { described_class.new(node) }

  describe '.initialize' do
    it 'sets the instance variable' do
      expect(subject.instance_variable_get(:@node)).to be_a(Hash)
    end

    it 'sets java_home' do
      expect(subject.java_home).to eq(node['java']['java_home'])
    end

    it 'sets jdk_version' do
      expect(subject.jdk_version).to eq(node['java']['jdk_version'])
    end
  end

  describe '#java_location' do
    before do
      subject.stub(:openjdk_path).and_return('foo')
    end

    it 'sets the java location' do
      expected_path = '/usr/lib/jvm/foo/bin/java'
      expect(subject.java_location).to eq(expected_path)
    end
  end

  context 'ubuntu 10.04 32 bit' do
    before do
      node['platform'] = 'ubuntu'
      node['platform_version'] = '10.04'
      node['platform_family'] = 'debian'
      node['kernel']['machine'] = 'i386'
    end

    it 'sets the java location for JDK 6' do
      expected_path = '/usr/lib/jvm/java-6-openjdk/jre/bin/java'
      expect(subject.java_location).to eq(expected_path)
    end
  end

  context 'ubuntu 10.04 64 bit' do
    before do
      node['platform'] = 'ubuntu'
      node['platform_version'] = '10.04'
      node['platform_family'] = 'debian'
      node['kernel']['machine'] = 'x86_64'
    end

    it 'sets the java location for 64 bit and JDK 6' do
      expected_path = '/usr/lib/jvm/java-6-openjdk/jre/bin/java'
      expect(subject.java_location).to eq(expected_path)
    end
  end

  context 'ubuntu 12.04 32 bit' do
    before do
      node['platform'] = 'ubuntu'
      node['platform_version'] = '12.04'
      node['platform_family'] = 'debian'
      node['kernel']['machine'] = 'i386'
    end

    it 'sets the java location for JDK 6' do
      expected_path = '/usr/lib/jvm/java-6-openjdk-i386/jre/bin/java'
      expect(subject.java_location).to eq(expected_path)
    end

    it 'sets the java location for JDK 7' do
      node['java']['jdk_version'] = '7'
      expected_path = '/usr/lib/jvm/java-7-openjdk-i386/jre/bin/java'
      expect(subject.java_location).to eq(expected_path)
    end
  end

  context 'ubuntu 12.04 64 bit' do
    before do
      node['platform'] = 'ubuntu'
      node['platform_version'] = '12.04'
      node['platform_family'] = 'debian'
      node['kernel']['machine'] = 'x86_64'
    end

    it 'sets the java location for JDK 6' do
      expected_path = '/usr/lib/jvm/java-6-openjdk-amd64/jre/bin/java'
      expect(subject.java_location).to eq(expected_path)
    end

    it 'sets the java location for JDK 7' do
      node['java']['jdk_version'] = '7'
      expected_path = '/usr/lib/jvm/java-7-openjdk-amd64/jre/bin/java'
      expect(subject.java_location).to eq(expected_path)
    end
  end

  context 'debian 6 32 bit' do
    before do
      node['platform'] = 'debian'
      node['platform_version'] = '6.0.7'
      node['platform_family'] = 'debian'
      node['kernel']['machine'] = 'i386'
    end

    it 'sets the java location for JDK 6' do
      expected_path = '/usr/lib/jvm/java-6-openjdk/jre/bin/java'
      expect(subject.java_location).to eq(expected_path)
    end
  end

  context 'debian 6 64 bit' do
    before do
      node['platform'] = 'debian'
      node['platform_version'] = '6.0.7'
      node['platform_family'] = 'debian'
      node['kernel']['machine'] = 'x86_64'
    end

    it 'sets the java location for 64 bit and JDK 6' do
      expected_path = '/usr/lib/jvm/java-6-openjdk/jre/bin/java'
      expect(subject.java_location).to eq(expected_path)
    end
  end

  context 'debian 7 32 bit' do
    before do
      node['platform'] = 'debian'
      node['platform_version'] = '7.0.0'
      node['platform_family'] = 'debian'
      node['kernel']['machine'] = 'i386'
    end

    it 'sets the java location for JDK 6' do
      expected_path = '/usr/lib/jvm/java-6-openjdk-i386/jre/bin/java'
      expect(subject.java_location).to eq(expected_path)
    end

    it 'sets the java location for JDK 7' do
      node['java']['jdk_version'] = '7'
      expected_path = '/usr/lib/jvm/java-7-openjdk-i386/jre/bin/java'
      expect(subject.java_location).to eq(expected_path)
    end
  end

  context 'debian 7 64 bit' do
    before do
      node['platform'] = 'debian'
      node['platform_version'] = '7.0.0'
      node['platform_family'] = 'debian'
      node['kernel']['machine'] = 'x86_64'
    end

    it 'sets the java location for JDK 6' do
      expected_path = '/usr/lib/jvm/java-6-openjdk-amd64/jre/bin/java'
      expect(subject.java_location).to eq(expected_path)
    end

    it 'sets the java location for JDK 7' do
      node['java']['jdk_version'] = '7'
      expected_path = '/usr/lib/jvm/java-7-openjdk-amd64/jre/bin/java'
      expect(subject.java_location).to eq(expected_path)
    end
  end

  context 'centos 5 32 bit' do
    before do
      node['platform'] = 'centos'
      node['platform_version'] = '5.9'
      node['platform_family'] = 'rhel'
      node['kernel']['machine'] = 'i386'
    end

    it 'sets the java location for JDK 6' do
      expected_path = '/usr/lib/jvm/jre-1.6.0-openjdk/bin/java'
      expect(subject.java_location).to eq(expected_path)
    end

    it 'sets the java location for JDK 7' do
      node['java']['jdk_version'] = '7'
      expected_path = '/usr/lib/jvm/jre-1.7.0-openjdk/bin/java'
      expect(subject.java_location).to eq(expected_path)
    end
  end

  context 'centos 5 64 bit' do
    before do
      node['platform'] = 'centos'
      node['platform_version'] = '5.9'
      node['platform_family'] = 'rhel'
      node['kernel']['machine'] = 'x86_64'
    end

    it 'sets the java location for JDK 6' do
      expected_path = '/usr/lib/jvm/jre-1.6.0-openjdk.x86_64/bin/java'
      expect(subject.java_location).to eq(expected_path)
    end

    it 'sets the java location for JDK 7' do
      node['java']['jdk_version'] = '7'
      expected_path = '/usr/lib/jvm/jre-1.7.0-openjdk.x86_64/bin/java'
      expect(subject.java_location).to eq(expected_path)
    end
  end

  context 'centos 6 32 bit' do
    before do
      node['platform'] = 'centos'
      node['platform_version'] = '6.4'
      node['platform_family'] = 'rhel'
      node['kernel']['machine'] = 'i386'
    end

    it 'sets the java location for JDK 6' do
      expected_path = '/usr/lib/jvm/jre-1.6.0-openjdk/bin/java'
      expect(subject.java_location).to eq(expected_path)
    end

    it 'sets the java location for JDK 7' do
      node['java']['jdk_version'] = '7'
      expected_path = '/usr/lib/jvm/jre-1.7.0-openjdk/bin/java'
      expect(subject.java_location).to eq(expected_path)
    end
  end

  context 'centos 6 64 bit' do
    before do
      node['platform'] = 'centos'
      node['platform_version'] = '6.4'
      node['platform_family'] = 'rhel'
      node['kernel']['machine'] = 'x86_64'
    end

    it 'sets the java location for JDK 6' do
      expected_path = '/usr/lib/jvm/jre-1.6.0-openjdk.x86_64/bin/java'
      expect(subject.java_location).to eq(expected_path)
    end

    it 'sets the java location for JDK 7' do
      node['java']['jdk_version'] = '7'
      expected_path = '/usr/lib/jvm/jre-1.7.0-openjdk.x86_64/bin/java'
      expect(subject.java_location).to eq(expected_path)
    end
  end
end
