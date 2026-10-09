$f = 'd:\AFSoftware - AllInOne\_源码TXT\AF\pch.h.txt'
# 删除被加密的文件
Remove-Item -Force $f -ErrorAction SilentlyContinue
# 用 .NET ReadAllBytes + WriteAllBytes 重新写入
$src = 'd:\AFSoftware - AllInOne\AF\pch.h'
$data = [System.IO.File]::ReadAllBytes($src)
[System.IO.File]::WriteAllBytes($f, $data)
Write-Host ("Written: " + $data.Length + " bytes")
# 立即读回验证
$check = [System.IO.File]::ReadAllBytes($f)
Write-Host ("Read back: " + $check.Length + " bytes")
$head = ($check[0..5] | ForEach-Object { [char]$_ }) -join ''
Write-Host ("First chars: $head")
