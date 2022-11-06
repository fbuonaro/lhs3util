%global __requires_exclude ^libs3\\.so.*$

Name:       libs3-lhdist
Version:    1
Release:    1
Summary:    libs3 packaged in an RPM
License:    See bji/libs3

%description
libs3 packaged in an RPM

%prep
git clone https://github.com/bji/libs3.git

%build
cd libs3
make DESTDIR=/usr

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}/usr
cd libs3
make install DESTDIR=%{buildroot}/usr
cd %{buildroot}
find ./ -type f -or -type l | grep usr | cut -d '.' -f 2- > %{buildroot}/../libs3_install_files.txt
find ./ -type f -or -type l | grep usr | xargs -I{} chmod 777 {}

%files -f %{buildroot}/../libs3_install_files.txt

%changelog