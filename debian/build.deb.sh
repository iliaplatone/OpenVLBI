#!/bin/bash
	user=1000
	deb=$1
	version=$2
	arch=$3
	size=$(sudo du -sk ${deb}/ | sed -e "s:${deb}/::")
	mkdir -p ${deb}/DEBIAN
	sudo chown $user:$user ${deb} -R
	sed -e "s:VERSION:$version:g" control.${deb} > ${deb}/DEBIAN/control
	sudo sed -i "s/XXX/$size/g" ${deb}/DEBIAN/control
	find ${deb}/ | while read line; do md5sum $line; done 2>/dev/null > ${deb}/DEBIAN/md5sums
	sed -i "s:${deb}/::g" ${deb}/DEBIAN/md5sums
	sed -i "/DEBIAN/d" ${deb}/DEBIAN/md5sums

	echo "Setting permissions..."
	sudo chown root:root ${deb} -R || true
	sudo chmod 755 ${deb} -R || true
	sudo chmod 600 ${deb}/var/spool/ -R || true
	sudo chmod 700 ${deb}/lib/modules -R || true
	sudo chmod 600 ${deb}/etc/sudoers || true
	sudo chmod 755 ${deb}/etc/network/*.d/* || true
	sudo chmod 755 ${deb}/etc/NetworkManager/dispatcher.d/*.d/* || true
	sudo chmod 600 ${deb}/etc/NetworkManager/system-connections/* || true
	sudo chmod 440 ${deb}/etc/sudoers || true
	sudo chmod 755 ${deb}/bin/* || true
	sudo chmod 755 ${deb}/sbin/* || true
	sudo chmod 755 ${deb}/usr/bin/* || true
	sudo chmod 755 ${deb}/usr/sbin/* || true
	sudo chmod 700 ${deb}/usr/share/inova || true
	sudo chmod 755 ${deb}/usr/share/applications/* || true
	sudo chmod 755 ${deb}/DEBIAN/*inst || true
	sudo chown root:root ${deb} -R || true
	sudo chown www-data:www-data ${deb}/var/www -R || true
	mkdir -p packages
	TMPDIR=/tmp dpkg-deb --build ${deb}/ packages/${deb}_${version}_${arch}.deb
	sudo chown ${user}:${user} ${deb} -R
	sudo chmod 755 ${deb} -R
	echo "Done."
