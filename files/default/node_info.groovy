/*
  Cookbook Name:: jenkins
  File:: node_info

  Author:: Doug MacEachern <dougm@vmware.com>
  Author:: Fletcher Nichol <fnichol@nichol.ca>

  Copyright 2010, VMware, Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/

import jenkins.slaves.*
import jenkins.model.*
import hudson.slaves.*
import hudson.model.*

def toJSON(node) {
  if (node instanceof Map) {
     return "{" + node.collect { k,v -> "\"${k}\":" + toJSON(v) }.join(", ") + "}"
  }
  else if (node instanceof String) {
     return "\"${node}\""
  }
  else {
     return node
  }
}

slave = Jenkins.instance.getNode(this.args[0]) as Slave

if (slave == null) {
  println "{}"
}
else {
  node = [
    "name" : slave.name,
    "description" : slave.nodeDescription,
    "remote_fs" : slave.remoteFS,
    "executors" : slave.numExecutors.toInteger(),
    "mode" : slave.mode.toString(),
    "labels" : slave.labelString,
    "availability" : slave.retentionStrategy.class.name.tokenize('$').get(1),
  ]

  if ((env = slave.nodeProperties.get(EnvironmentVariablesNodeProperty.class)?.envVars)) {
    node["env"] = env
  }

  if (slave.retentionStrategy instanceof RetentionStrategy.Demand) {
    retention = slave.retentionStrategy as RetentionStrategy.Demand
    node["in_demand_delay"] = retention.inDemandDelay
    node["idle_delay"] = retention.idleDelay
  }

  launcher = slave.launcher
  if (launcher instanceof CommandLauncher) {
    node["launcher"] = "command"
    node["command"] = launcher.command
  }
  else if (launcher instanceof JNLPLauncher) {
    node["launcher"] = "jnlp"
    if (jenkins.slaves.JnlpSlaveAgentProtocol.declaredFields.find { it.name == 'SLAVE_SECRET' }) {
      node["secret"] = jenkins.slaves.JnlpSlaveAgentProtocol.SLAVE_SECRET.mac( slave.name )
    }
  }
  else {
    node["launcher"] = "ssh"
    node["host"] = launcher.host
    node["port"] = launcher.port
    node["username"] = launcher.username
    if (launcher.password != null) {
      node["password"] = launcher.password
    }
    node["private_key"] = launcher.privatekey
    node["jvm_options"] = launcher.jvmOptions
  }

  println toJSON(node)
}
