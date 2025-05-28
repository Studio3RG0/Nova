import os 

cwd = os.getcwd()
operatingSystem = os.name

if operatingSystem == 'nt':
    os.system(cwd + '/external/vcpkg/bootstrap-vcpkg.bat')
else:
    os.system(cwd + '/external/vcpkg/bootstrap-vcpkg.sh')

os.system(cwd + '/external/vcpkg/vcpkg install --x-install-root=./external/vcpkg_installed')