iex "& { $(irm https://chocolatey.org/install.ps1) }"
$env:PATH="$env:PATH;$env:ALLUSERSPROFILE\chocolatey\bin"
choco sources add -name smingrepo -source 'https://www.myget.org/F/sming/'
choco install -y sming sming.esp32 --installargs 'ADD_CMAKE_TO_PATH=System' --applyargstodependencies

# Remove left-over installation files
Remove-Item "c:\*.7z" -Force
Remove-Item "c:\tools\esp32\dist\*" -Force
Remove-Item "$env:Temp\*" -Recurse -Force
Remove-Item "$env:LocalAppData\NuGet\Cache\*" -Force
