[ req ]
default_bits            = $ENV::KEY_SIZE
prompt                  = no
distinguished_name      = req_dn
x509_extensions         = v3_ca

[ req_dn ]
commonName              = %CN%
organizationName	= %ORG%

[ server ]
nsComment="FS Server Cert"
basicConstraints=CA:FALSE
subjectKeyIdentifier=hash
authorityKeyIdentifier=keyid,issuer:always
subjectAltName=%ALTNAME%
nsCertType=server
extendedKeyUsage=serverAuth

[ client ]
nsComment="FS Client Cert"
basicConstraints=CA:FALSE
subjectKeyIdentifier=hash
authorityKeyIdentifier=keyid,issuer:always
subjectAltName=%ALTNAME%
nsCertType=client
extendedKeyUsage=clientAuth

[ v3_ca ]
subjectKeyIdentifier=hash
authorityKeyIdentifier=keyid:always,issuer
basicConstraints=CA:TRUE

