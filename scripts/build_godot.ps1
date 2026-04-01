# 解析命令行参数，debug 或 release
param(
    [string]$buildType = "debug"
)

# 获取脚本所在目录的完整路径
$scriptPath = $PSScriptRoot

$workspacePath = Split-Path -Path $scriptPath -Parent
$buildPath = Join-Path $workspacePath "build"
$buildCachePath = Join-Path $workspacePath "build/cache"

Write-Host "Workspace Path: $workspacePath"

$godotPath = Join-Path $workspacePath "3rd/godot"
$godotCppPath = Join-Path $workspacePath "3rd/godot-cpp"

Write-Host "Godot Path: $godotPath"
Write-Host "Godot Cpp Path: $godotCppPath"


Write-Host "Build Type: $buildType"

# build godot
# goto $godotPath

Set-Location $godotPath

scons platform=windows `
    target=template_$buildType `
    library_type=shared_library `
    accesskit=no `
    dev_build=yes `
    debug_crt=yes `
    disable_path_overrides=no `
    cache_path=$buildCachePath `
    d3d12=no 

# 创建目标目录（如果不存在）
$sourceDir = Join-Path $godotPath "bin/"
$targetDir = Join-Path $workspacePath "build/godot/lib/"
New-Item -ItemType Directory -Path $targetDir -Force | Out-Null

Write-Host "Copying files from $sourceDir to $targetDir"

# 拷贝 .dll, .lib, .pdb 文件
Get-ChildItem -Path $sourceDir -Filter "*.dll" | ForEach-Object {
    Write-Host "Copying $($_.Name) to $targetDir"
    Copy-Item -Path $_.FullName -Destination $targetDir -Force
}
Get-ChildItem -Path $sourceDir -Filter "*.dll" | ForEach-Object {
    Write-Host "Copying $($_.Name) to $buildPath"
    Copy-Item -Path $_.FullName -Destination $buildPath -Force
}
Get-ChildItem -Path $sourceDir -Filter "*.lib" | ForEach-Object {
    Write-Host "Copying $($_.Name) to $targetDir"
    Copy-Item -Path $_.FullName -Destination $targetDir -Force
}
Get-ChildItem -Path $sourceDir -Filter "*.pdb" | ForEach-Object {
    Write-Host "Copying $($_.Name) to $targetDir"
    Copy-Item -Path $_.FullName -Destination $targetDir -Force
}

# 创建目标目录（如果不存在）
$sourceDir = Join-Path $godotPath "bin/"
$targetDir = Join-Path $workspacePath "build/godot/exe/"
New-Item -ItemType Directory -Path $targetDir -Force | Out-Null

scons platform=windows `
    accesskit=no `
    cache_path=$buildCachePath `
    disable_path_overrides=no `
    debug_crt=yes `
    d3d12=no 

# 拷贝 .dll, .lib, .pdb 文件
Get-ChildItem -Path $sourceDir -Filter "*.exe" | ForEach-Object {
    Write-Host "Copying $($_.Name) to $targetDir"
    Copy-Item -Path $_.FullName -Destination $targetDir -Force
}

Set-Location $targetDir
./godot.windows.editor.x86_64.exe --dump-extension-api

$extensionApiPath = Join-Path $targetDir "extension_api.json"
$extensionApiTargetPath = Join-Path $godotCppPath "gdextension"
Copy-Item -Path $extensionApiPath -Destination $extensionApiTargetPath

Set-Location $godotCppPath

scons use_static_cpp=no debug_crt=yes