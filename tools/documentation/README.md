# Generate the code reference

From the repository root, using PowerShell 7:

```powershell
# First run: download the official portable archive and verify its SHA-256.
./tools/documentation/Generate.ps1 -InstallTool
# Subsequent runs use the same workspace-local tool.
./tools/documentation/Generate.ps1
```

Alternatively, pass `-DoxygenPath /path/to/doxygen` for an existing **1.18.0** binary. The official Windows archive/version/checksum are pinned in [tool-lock.json](tool-lock.json), using [Doxygen's download checksums](https://www.doxygen.nl/download.html). No global installation, SDK changes or Graphviz dependency is required.

Open [the generated HTML](../../build/documentation/html/index.html) after a successful run. HTML/XML, warnings and `generation.json` live under ignored `build/documentation/`; portable tools stay under ignored `.tools/documentation/`. A failed run removes the success marker. Older generated files may remain; only a successful regeneration establishes current output.

[Doxyfile](Doxyfile) explicitly selects original Nordic C, coordinator UI/board C and project Python helpers, plus the two maintained code-reference notes. It excludes vendor SDKs, the coordinator upstream checkout, the `xiao_source` board-settings reference and runtime state. Source browsing retains conditional code; this is source indexing, not a compiled target's API inventory. PowerShell and CommonJS are mapped in the maintained notes rather than parsed as C APIs. Undocumented implementation symbols remain browsable through `EXTRACT_ALL`; this is not a full comment-coverage claim.

The [official configuration reference](https://www.doxygen.nl/manual/config.html) documents input selection, extraction and warnings. Documentation warnings fail generation. Firmware builds and hardware tests are separate validation.

## Firmware diagram export

Native draw.io sources live beside each firmware architecture note:

- [Nordic](../../docs/Firmware/Nordic%20Zigbee/assets/architecture.drawio).
- [Coordinator](../../docs/Firmware/ESP32-C6%20Coordinator/assets/architecture.drawio).

With draw.io desktop installed on Windows, choose one source and export from the repository root:

```powershell
$diagramPath = (Resolve-Path 'docs/Firmware/Nordic Zigbee/assets/architecture.drawio').Path
$pngPath = Join-Path (Split-Path $diagramPath) architecture.png
$exportArgs = '-x -f png -s 1.5 -b 20 -o "{0}" "{1}"' -f $pngPath, $diagramPath
Start-Process -FilePath 'C:/Program Files/draw.io/draw.io.exe' -WindowStyle Hidden -Wait -ArgumentList $exportArgs
```

Inspect the PNG after editing; keep source and export beside their firmware architecture note. Diagrams describe written code and existing interfaces.
