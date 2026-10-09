$root = 'd:\AFSoftware - AllInOne'
$dst = Join-Path $root '_源码TXT'

# 清除旧的加密目录
if (Test-Path $dst) { Remove-Item -Recurse -Force $dst }
New-Item -ItemType Directory -Path $dst -Force | Out-Null
New-Item -ItemType Directory -Path (Join-Path $dst 'AF') -Force | Out-Null
New-Item -ItemType Directory -Path (Join-Path $dst 'CSharpUseC++AF_Dll') -Force | Out-Null
New-Item -ItemType Directory -Path (Join-Path $dst 'CSharpUseC++AF_Dll\Properties') -Force | Out-Null
Write-Host "Directories created"
