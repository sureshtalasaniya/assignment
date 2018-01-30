# Update System Basic Packages:
apt-get update
apt-get install wget
apt-get install linux-headers-$(uname -r)
ldconfig

# Install FreeSWITCH dependency libraries:
apt-get install git-core build-essential autoconf automake libtool libncurses5 libncurses5-dev make libjpeg-dev pkg-config unixodbc unixodbc-dev zlib1g-dev libcurl4-openssl-dev libexpat1-dev libssl-dev libtiff5-dev libx11-dev unixodbc-dev zlib1g-dev libzrtpcpp-dev libasound2-dev libogg-dev libvorbis-dev libperl-dev libgdbm-dev libdb-dev python-dev uuid-dev bison autoconf g++ libncurses-dev speex libspeexdsp-dev libedit-dev libpcre3-dev

apt-get install sqlite3 libsqlite3-dev libtool-bin yasm
apt-get install lua5.2
apt-get install liblua5.2-dev
apt-get install libopus-dev
apt-get install libsndfile-dev

# For mod_flite
git clone https://freeswitch.org/stash/scm/sd/libflite.git 
cd libflite
./configure --enable-shared && make && make install


# Download Freeswitch Source Code.
cd /usr/local/src/
wget http://files.freeswitch.org/releases/freeswitch/freeswitch-1.6.19.tar.gz
tar -xvzf freeswitch/freeswitch-1.6.19.tar.gz
cd freeswitch-1.6.19

# The -j argument spawns multiple threads to speed the build process 
 ./rebootstrap.sh-j

# if you want to add or remove modules from the build, edit modules.conf
vim modules.conf

# add a module by removing '#' comment character at the beginning of the line
# remove a module by adding the '#' comment character at the beginning of the line containing the name of the module to be skipped

./configure --enable-core-pgsql-support --enable-shared
./configure --enable-core-pgsql-support  && make && make install

make && make install 
make cd-sounds-install cd-moh-install

# create user 'freeswitch'
# add it to group 'daemon'
# change owner and group of the freeswitch installation

cd /usr/local/ 
adduser --disabled-password  --quiet --system --home /usr/local/freeswitch --gecos "FreeSWITCH Voice Platform" --ingroup daemon freeswitch

chown -R freeswitch:daemon /usr/local/freeswitch/ 
chmod -R ug=rwX,o= /usr/local/freeswitch/
chmod -R u=rwx,g=rx /usr/local/freeswitch/bin/*

# link config files from source build to their expected F.H.S. locations to keep init script happy
ln /usr/local/freeswitch/bin/freeswitch /usr/bin/freeswitch
ln /usr/local/freeswitch/bin/fs_cli  /usr/bin/fs_cli

mkdir /etc/freeswitch
ln /usr/local/freeswitch/conf/freeswitch.xml /etc/freeswitch/freeswitch.xml

chown freeswitch:daemon /etc/freeswitch
chmod ug=rwx,o= /etc/freeswitch

mkdir /var/lib/freeswitch
chown freeswitch:daemon /var/lib/freeswitch
chmod -R ug=rwX,o= /var/lib/freeswitch

cp /usr/local/src/freeswitch-1.6.19/debian/freeswitch-sysvinit.freeswitch.default /etc/default/freeswitc

chown freeswitch:daemon /etc/default/freeswitch
chmod ug=rw,o= /etc/default/freeswitch
 
cp /usr/local/src/freeswitch-1.6.19/debian/freeswitch-sysvinit.freeswitch.init /etc/init.d/freeswitch
 
chown freeswitch:daemon /etc/init.d/freeswitch

chmod u=rwx,g=rx,o= /etc/init.d/freeswitch
update-rc.d freeswitch defaults

# optionally, to make manual start-up and shutdown of FreeSWITCH a bit easier
ln /etc/init.d/freeswitch /usr/local/bin/fs


#Load modify /usr/local/freeswitch/conf/autoload_configs/modules.conf.xml
To Load modules in below order
## Vim modules.conf.xml

<load module="mod_console"/>
<load module="mod_commands"/>
<load module="mod_lua"/>
<load module="mod_sofia"/>
<load module="mod_logfile"/>
<load module="mod_expr"/>
<load module="mod_spandsp"/>
<load module="mod_dptools"/>
<load module="mod_event_socket"/>
<load module="mod_native_file"/>
<load module="mod_sndfile"/>
<load module="mod_say_en"/>
<load module="mod_local_stream"/>
<load module="mod_tone_stream"/>
<load module="mod_opus"/>
<load module="mod_g729"/>
<load module="mod_esf"/>
<load module="mod_fsv"/>

# assignment

<include>
  <X-PRE-PROCESS cmd="set" data="default_password=zxsder45"/>
<!-- Codecs -->
<!-- Codecs -->
<X-PRE-PROCESS cmd="set" data="global_codec_prefs=OPUS,G722,PCMU,PCMA,VP8"/>
<X-PRE-PROCESS cmd="set" data="outbound_codec_prefs=OPUS,G722,PCMU,PCMA,VP8"/>
<X-PRE-PROCESS cmd="set" data="media_mix_inbound_outbound_codecs=true" />

<!-- Defaults -->
<X-PRE-PROCESS cmd="set" data="call_debug=false" />
<X-PRE-PROCESS cmd="set" data="console_loglevel=info" />
<X-PRE-PROCESS cmd="set" data="default_areacode=208" />
<X-PRE-PROCESS cmd="set" data="sit=%(274,0,913.8);%(274,0,1370.6);%(380,0,1776.7)" />
<X-PRE-PROCESS cmd="set" data="use_profile=internal" />
<X-PRE-PROCESS cmd="set" data="default_language=en" />
<X-PRE-PROCESS cmd="set" data="default_dialect=us" />
<X-PRE-PROCESS cmd="set" data="default_voice=callie" />
<X-PRE-PROCESS cmd="set" data="ajax_refresh_rate=3000" />
<X-PRE-PROCESS cmd="set" data="xml_cdr_archive=dir" />
<X-PRE-PROCESS cmd="set" data="ringback=$${us-ring}" />
<X-PRE-PROCESS cmd="set" data="transfer_ringback=$${us-ring}" />
<X-PRE-PROCESS cmd="set" data="record_ext=wav" />

<!-- Dingaling -->
<!-- xmpp_client_profile and xmpp_server_profile xmpp_client_profile can be any string. xmpp_server_profile is appended to "dingaling_" to form the database name containing the "subscriptions" table. used by: dingaling.conf.xml enum.conf.xml -->
<X-PRE-PROCESS cmd="set" data="xmpp_client_profile=xmppc" />
<X-PRE-PROCESS cmd="set" data="xmpp_server_profile=xmpps" />
<X-PRE-PROCESS cmd="set" data="bind_server_ip=auto" />

<!-- Domain -->
<X-PRE-PROCESS cmd="set" data="domain_name=$${domain}" />

<!-- IP Address -->
<X-PRE-PROCESS cmd="set" data="external_rtp_ip=autonat:PUBLIC_IP"/>
<X-PRE-PROCESS cmd="set" data="external_sip_ip=autonat:PUBLIC_IP"/>

<!-- Music on Hold -->
<X-PRE-PROCESS cmd="set" data="hold_music=local_stream://default" />

<!-- Ringtones -->
<!-- Default ringtone (US) -->
<X-PRE-PROCESS cmd="set" data="us-ring=%(2000,4000,440,480)" />
<X-PRE-PROCESS cmd="set" data="au-ring=%(400,200,383,417);%(400,2000,383,417)" />
<X-PRE-PROCESS cmd="set" data="be-ring=%(1000,3000,425)" />
<X-PRE-PROCESS cmd="set" data="ca-ring=%(2000,4000,440,480)" />
<X-PRE-PROCESS cmd="set" data="cn-ring=%(1000,4000,450)" />
<X-PRE-PROCESS cmd="set" data="cy-ring=%(1500,3000,425)" />
<X-PRE-PROCESS cmd="set" data="cz-ring=%(1000,4000,425)" />
<X-PRE-PROCESS cmd="set" data="de-ring=%(1000,4000,425)" />
<X-PRE-PROCESS cmd="set" data="dk-ring=%(1000,4000,425)" />
<X-PRE-PROCESS cmd="set" data="dz-ring=%(1500,3500,425)" />
<X-PRE-PROCESS cmd="set" data="eg-ring=%(2000,1000,475,375)" />
<X-PRE-PROCESS cmd="set" data="fi-ring=%(1000,4000,425)" />
<X-PRE-PROCESS cmd="set" data="fr-ring=%(1500,3500,440)" />
<X-PRE-PROCESS cmd="set" data="pt-ring=%(1000,5000,400)" />
<X-PRE-PROCESS cmd="set" data="hk-ring=%(400,200,440,480);%(400,3000,440,480)" />
<X-PRE-PROCESS cmd="set" data="hu-ring=%(1250,3750,425)" />
<X-PRE-PROCESS cmd="set" data="il-ring=%(1000,3000,400)" />
<X-PRE-PROCESS cmd="set" data="in-ring=%(400,200,425,375);%(400,2000,425,375)" />
<X-PRE-PROCESS cmd="set" data="jp-ring=%(1000,2000,420,380)" />
<X-PRE-PROCESS cmd="set" data="ko-ring=%(1000,2000,440,480)" />
<X-PRE-PROCESS cmd="set" data="pk-ring=%(1000,2000,400)" />
<X-PRE-PROCESS cmd="set" data="pl-ring=%(1000,4000,425)" />
<X-PRE-PROCESS cmd="set" data="ro-ring=%(1850,4150,475,425)" />
<X-PRE-PROCESS cmd="set" data="rs-ring=%(1000,4000,425)" />
<X-PRE-PROCESS cmd="set" data="it-ring=%(1000,4000,425)" />
<X-PRE-PROCESS cmd="set" data="ru-ring=%(800,3200,425)" />
<X-PRE-PROCESS cmd="set" data="sa-ring=%(1200,4600,425)" />
<X-PRE-PROCESS cmd="set" data="tr-ring=%(2000,4000,450)" />
<X-PRE-PROCESS cmd="set" data="uk-ring=%(400,200,400,450);%(400,2000,400,450)" />
<X-PRE-PROCESS cmd="set" data="bong-ring=v=-7;%(100,0,941.0,1477.0);v=-7;>=2;+=.1;%(1400,0,350,440)" />

<!-- SIP -->
<!-- Used to turn on sip loopback unrolling. -->
<X-PRE-PROCESS cmd="set" data="unroll_loops=true" />
<X-PRE-PROCESS cmd="set" data="sip_tls_version=tlsv1" />

<!-- SIP Profile: External -->
<X-PRE-PROCESS cmd="set" data="external_auth_calls=false" />
<X-PRE-PROCESS cmd="set" data="external_sip_port=5080" />
<X-PRE-PROCESS cmd="set" data="external_tls_port=5081" />
<X-PRE-PROCESS cmd="set" data="external_ssl_enable=true" />
<X-PRE-PROCESS cmd="set" data="external_ssl_dir=/usr/local/freeswitch/certs" />

<!-- SIP Profile: Internal -->
<X-PRE-PROCESS cmd="set" data="internal_auth_calls=true" />
<X-PRE-PROCESS cmd="set" data="internal_sip_port=5060" />
<X-PRE-PROCESS cmd="set" data="internal_tls_port=5061" />
<X-PRE-PROCESS cmd="set" data="internal_ssl_enable=true" />
<X-PRE-PROCESS cmd="set" data="internal_ssl_dir=/usr/local/freeswitch/certs" />

<!-- Sound -->
<!-- Sets the sound directory. -->
<X-PRE-PROCESS cmd="set" data="sound_prefix=$${sounds_dir}/music/en/us/callie" />

<!-- Tones -->
<!-- Imported from vars.xml -->
<X-PRE-PROCESS cmd="set" data="vacant-us-tone=%(274,0,913.8);%(274,0,1370.6);%(380,0,1776.7)" />
<X-PRE-PROCESS cmd="set" data="vacant-uk-tone=%(330,15,950);%(330,15,1400);%(330,1000,1800)" />
<X-PRE-PROCESS cmd="set" data="busy-us-tone=%(500,500,480,620)" />
<X-PRE-PROCESS cmd="set" data="busy-au-tone=v=-13;%(375,375,420);v=-23;%(375,375,420)" />
<X-PRE-PROCESS cmd="set" data="bong-us-tone=v=-7;%(100,0,941.0,1477.0);v=-7;>=2;+=.1;%(1400,0,350,440)" />
</include>



