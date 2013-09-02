@test "check for IBM java in JAVA_HOME" {
  source /etc/profile.d/jdk.sh
  $JAVA_HOME/bin/java -version 2>&1 | grep IBM
}
