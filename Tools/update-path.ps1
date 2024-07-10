param (
    [Parameter(Mandatory=$true)][array]$dirs
)

$syspath = [Environment]::GetEnvironmentVariable("PATH", [System.EnvironmentVariableTarget]::Machine)
$syspathdirs = ($syspath -split ";").where({ "" -ne $_ })
$userpath = [Environment]::GetEnvironmentVariable("PATH", [System.EnvironmentVariableTarget]::User)
$userpathdirs = ($userpath -split ";").where({ "" -ne $_ })
$newdirs = @()
foreach ($dir in $dirs) {
    if ($userpathdirs.ToLower().Contains($dir.ToLower())) {
        Write-Host "$dir is already in user PATH"
    } elseif ($syspathdirs.ToLower().Contains($dir.ToLower())) {
        Write-Host "$dir is already in system PATH"
    } else {
        $newdirs += $dir
    }
}
if ($newdirs) {
    $userpath = ($newdirs + $userpathdirs) -join ";"
    [Environment]::SetEnvironmentVariable("PATH", $userpath, [EnvironmentVariableTarget]::User)
    Write-Host "** $newdirs added to user PATH"
}
