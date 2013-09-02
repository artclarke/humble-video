
ssl_dir = node['jenkins']['http_proxy']['ssl']['dir']
ssl_req = "/C=US/ST=Several/L=Locality/O=Example/OU=Operations/CN=TEST/emailAddress=test@test.com"

directory ssl_dir do
  recursive true
end

execute "Create SSL Certificates" do
  cwd ssl_dir
  command <<-EOH
  umask 077
  openssl genrsa 2048 > jenkins.key
  openssl req -subj "#{ssl_req}" -new -x509 -nodes -sha1 -days 3650 -key jenkins.key > jenkins.cert
  cat jenkins.key jenkins.cert > jenkins.pem
  EOH
  not_if { ::File.exists?("#{ssl_dir}/jenkins.pem") }
end
