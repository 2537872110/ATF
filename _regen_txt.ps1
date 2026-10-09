$ErrorActionPreference = 'Stop'
$root = 'd:\AFSoftware - AllInOne'
$dst = Join-Path $root '_源码TXT'

# 清除旧的加密目录
if (Test-Path $dst) { Remove-Item -Recurse -Force $dst }
New-Item -ItemType Directory -Path $dst -Force | Out-Null

# 源文件清单
$files = @(
    '.gitignore',
    'AF_C++.sln',
    'AF\AF_Main.cpp','AF\AF_Main.h',
    'AF\AF_PID.cpp','AF\AF_PID.h',
    'AF\AfLensConfigManager.cpp','AF\AfLensConfigManager.h',
    'AF\FocusMode.h',
    'AF\HaiKangCamera.cpp','AF\HaiKangCamera.h',
    'AF\LensParameterStorage_CPP14.h',
    'AF\TTT_AutoFocus.cpp','AF\TTT_AutoFocus.h',
    'AF\XmlFile.cpp','AF\XmlFile.h',
    'AF\dllmain.cpp','AF\framework.h',
    'AF\pch.cpp','AF\pch.h',
    'AF\AF.vcxproj','AF\AF.vcxproj.filters',
    'CSharpUseC++AF_Dll\AFNative.cs',
    'CSharpUseC++AF_Dll\App.config',
    'CSharpUseC++AF_Dll\CSharpUseC++AF_Dll.csproj',
    'CSharpUseC++AF_Dll\Form1.Designer.cs',
    'CSharpUseC++AF_Dll\Form1.cs',
    'CSharpUseC++AF_Dll\Form1.resx',
    'CSharpUseC++AF_Dll\MakeZeroForm.Designer.cs',
    'CSharpUseC++AF_Dll\MakeZeroForm.cs',
    'CSharpUseC++AF_Dll\NativeInt32.cs',
    'CSharpUseC++AF_Dll\Program.cs',
    'CSharpUseC++AF_Dll\StatusLamp.cs',
    'CSharpUseC++AF_Dll\packages.config',
    'CSharpUseC++AF_Dll\Properties\AssemblyInfo.cs',
    'CSharpUseC++AF_Dll\Properties\Resources.Designer.cs',
    'CSharpUseC++AF_Dll\Properties\Resources.resx',
    'CSharpUseC++AF_Dll\Properties\Settings.Designer.cs',
    'CSharpUseC++AF_Dll\Properties\Settings.settings'
)

$count = 0
foreach ($f in $files) {
    $srcPath = Join-Path $root $f
    $dstPath = Join-Path $dst ($f + '.txt')
    $dstDir = Split-Path $dstPath -Parent
    if (-not (Test-Path $dstDir)) { New-Item -ItemType Directory -Path $dstDir -Force | Out-Null }
    $data = [System.IO.File]::ReadAllBytes($srcPath)
    [System.IO.File]::WriteAllBytes($dstPath, $data)
    $count++
}
Write-Host "Done: $count files copied"
