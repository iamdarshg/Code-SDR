<#
.SYNOPSIS
Explicitly starts a fresh no-timeout protected Freerouting pass.
#>

[CmdletBinding()]
param()

& (Join-Path $PSScriptRoot 'freerouting_start_forever.ps1') -Restart
