# Polls the user's Downloads folder for the Lattice Diamond 3.14 Windows package
# every 5 minutes for 2 hours, and logs what it finds. Launched detached from the
# opencode session so it survives between turns.
$ErrorActionPreference = 'SilentlyContinue'

$root    = Split-Path -Parent $PSScriptRoot
$log     = Join-Path $PSScriptRoot 'diamond_watch.log'
$dl      = Join-Path $env:USERPROFILE 'Downloads'
$deadline = (Get-Date).AddHours(2)
$seen    = @{}

function Log($m) {
    "$(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')  $m" | Add-Content -Path $log
}

Log "watch start; scanning $dl every 5 min until $($deadline.ToString('HH:mm'))"

while ((Get-Date) -lt $deadline) {
    $hits = Get-ChildItem -LiteralPath $dl -File -ErrorAction SilentlyContinue |
            Where-Object {
                ($_.Name -match 'diamond|lattice|lscc|diamond_3|Diamond_3') -and
                ($_.Length -gt 50MB)
            }
    foreach ($f in $hits) {
        if (-not $seen.ContainsKey($f.FullName) -or $seen[$f.FullName] -ne $f.Length) {
            $seen[$f.FullName] = $f.Length
            Log ("FOUND {0}  size={1:N1} MB  modified={2}" -f `
                 $f.FullName, ($f.Length/1MB), $f.LastWriteTime)
        }
    }

    # also report any large archive/installer that appeared, even if unnamed oddly
    $big = Get-ChildItem -LiteralPath $dl -File -ErrorAction SilentlyContinue |
           Where-Object { $_.Length -gt 300MB -and $_.LastWriteTime -gt (Get-Date).AddMinutes(-5) }
    foreach ($f in $big) { Log ("BIG  {0}  size={1:N1} MB" -f $f.FullName, ($f.Length/1MB)) }

    # partial-download detection
    Get-ChildItem -LiteralPath $dl -File -ErrorAction SilentlyContinue |
        Where-Object { $_.Name -match '\.(crdownload|part|tmp|partial)$' } |
        ForEach-Object { Log ("DOWNLINK-IN-PROGRESS {0} {1:N1} MB" -f $_.Name, ($_.Length/1MB)) }

    Start-Sleep -Seconds 300
}
Log "watch end"
