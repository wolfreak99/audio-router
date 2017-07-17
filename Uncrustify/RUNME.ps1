# Insert all the desired paths you wish to parse below (sub-directories are included!)
$parsePaths = @(
"..\audio-router\*.cpp", 
"..\audio-router\*.h", 
"..\audio-router-gui\*.cpp", 
"..\audio-router-gui\*.h", 
"..\bootstrapper\*.cpp",
"..\bootstrapper\*.h"
)

# The configuration file to use
$configName = "config.cfg"
# The file the new code is saved to (before overriding previous file)
$tempName = "output.tmp"

# Do not edit anything below unless you know what you are doing!

$configFile = Get-ChildItem $configName | % { $_.FullName }
$tempFile = Get-Location | % { $_.Path + "\" + $tempName }
$params = @("-f", "PLACEHOLDER", "-c", $configFile, "-o", $tempFile)

Get-ChildItem -Path $parsePaths -Recurse -Force |
ForEach-Object {
    $name = $_.FullName
    $params[1] = $name
    & .\uncrustify.exe $params
    Move-Item $tempFile $name -Force
}
