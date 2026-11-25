# Karachi Transit Route Planner

A C++11 console project that calculates routes on an offline sample network of 10 Karachi stops and 10 two-way connections. Line numbers, times, and fares are demonstration data.

## Run on Windows

Open PowerShell in this project folder and run:

```powershell
.\transit_planner.exe
```

Rebuild it with MinGW g++:

```powershell
g++ -std=c++11 -Wall -Wextra -pedantic -static src/main.cpp -o transit_planner.exe
```

On Linux or macOS, build with `g++ -std=c++11 -Wall -Wextra -pedantic src/main.cpp -o transit_planner` and run `./transit_planner` from this folder.

## Karachi dataset demo

Run the 30-stop Green Line and People's Bus Service Route 04 model from the project folder:

```powershell
.\transit_planner.exe --data-dir datasets/karachi
```

The Karachi dataset uses published stops and stop orders with demonstration weights for fare and time. See its [sources and demo](datasets/karachi/README.md). The command without arguments opens the original sample.

## Original sample demo

1. Choose **1** and enter **Numaish** and **Surjani Town**. The fastest route takes **46 minutes**, costs **Rs 90**, and changes lines **once**.
2. Choose **7**, rank by time, and enter the same stops. The alternative through Tibet Centre takes **55 minutes** and costs **Rs 120**.
3. Choose **8 → 4** and block connection **8** between Saddar and North Nazimabad. Run the fastest search again to see the detour. Choose **11** to undo.
4. Choose **3** and search **Tibet Centre → Surjani Town**. The result stays on line 2 and has zero transfers. Choose **6** and enter **nazim** to find North Nazimabad.
5. Choose **12** to show the minimum spanning tree. The original connected dataset has nine selected connections and a total connection fare of **Rs 185**.

Windows console stop prompts show suggestions as you type. Press **Tab** to complete the first suggestion, then **Enter** to submit. Menu option **4** also offers prefix suggestions, including with redirected input. Search and lookup are case-insensitive for the bundled stop names.

Network edits include adding, removing, blocking, and reopening stops or connections. Back and forward replay earlier route searches against the current network. Undo reverts the latest network edit in the current session. Option **13** saves the edited network and its status flags into `data/network.txt`. The planner loads that file on its next run. Keep the two original data files beside it to retain the original dataset. Option **14** displays a simple circular line when the selected line contains a loop. The bundled network has no single-line loop.

## Code and tests

- `src/main.cpp` runs the menu and search history.
- `include/Graph.h` implements adjacency lists, route modes, alternative paths, and Prim's algorithm.
- `include/TransitNetwork.h` handles stop indexes, edits, and text files.

Windows PowerShell:

```powershell
Get-ChildItem tests/test_*.cpp | ForEach-Object {
    $out = Join-Path $env:TEMP ("transit-" + $_.BaseName + ".exe")
    g++ -std=c++11 -Wall -Wextra -pedantic $_.FullName -o $out
    if ($LASTEXITCODE -ne 0) { throw "Build failed for $($_.Name)" }
    & $out
    if ($LASTEXITCODE -ne 0) { throw "Test failed for $($_.Name)" }
    Write-Output "PASS $($_.Name)"
}
```

Run `bash tests/run_tests.sh` on Linux, macOS, or Git Bash.

## Data and implementation choices

`data/stops.txt` gives one stop name per nonempty line, starting at ID zero. `data/routes.txt` gives five integers per row in the order `from_id to_id minutes fare_rupees line_id`. Each row produces a two-way connection. A stored closure affects both directions.

Fastest routing uses the custom min-heap. Cheapest routing uses a custom sorted linked-list priority queue with the same Dijkstra logic. Fewest transfers uses a double-ended queue in 0–1 BFS. Routes with equal cost under the selected metric may have different time or fare. Ranked alternatives are simple paths. On larger networks, the search caps its work and reports when the ranking covers only the paths it found.

All data structures and algorithms used for the planner are implemented in the project. The program uses standard C++ strings and streams for text and console I/O.
