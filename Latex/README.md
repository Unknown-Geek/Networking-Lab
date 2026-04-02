# CSL 332 Networking Lab

Complete networking lab report for CSL 332 course with 19 experiments covering network programming, protocols, and network simulation.

## Student Information
- **Name:** Shravan Nander Pandala
- **Roll Number:** TVE23CS131
- **Course:** CSL 332 Network Lab

## Project Structure

```
CSL_332_Network_Lab/
├── main.tex                      # Main LaTeX document (84 pages)
├── main.pdf                      # Compiled PDF report (12.1 MB)
├── src/                          # C source files (27 programs)
│   ├── cycle1_*.c               # Cycle 1 programs
│   ├── cycle2_*.c               # Cycle 2 programs (TCP/UDP)
│   ├── cycle3_*.c               # Cycle 3 programs
│   ├── cycle4_*.c               # Cycle 4 programs
│   └── cycle5_*.c               # Cycle 5 programs
├── assets/
│   ├── cet.jpeg                 # CET logo for cover page
│   ├── screenshots/
│   │   ├── cycle1/              # 22 networking command screenshots
│   │   ├── cycle2/              # Cycle 2 program outputs
│   │   ├── cycle3/              # Cycle 3 program outputs
│   │   ├── cycle4/              # Cycle 4 program outputs
│   │   └── cycle5/              # Cycle 5 program outputs
│   └── images/
│       ├── exp15/               # Experiment 15 images
│       ├── exp16/               # Experiment 16 images
│       ├── exp17/               # Experiment 17 images
│       ├── exp18/               # Experiment 18 images
│       └── exp19/               # Experiment 19 images
└── docs/
    └── missing_cycle1_screenshots.md
```

## Experiments Covered

### Cycle 1: Networking Commands
1. **Basic Networking Commands** - 22 commands including ifconfig, ping, traceroute, netstat, etc.
2. **Socket Programming Basics** - UDP and TCP socket programming fundamentals

### Cycle 2: Client-Server Programming
3. UDP Echo Client-Server
4. TCP Echo Client-Server
5. TCP Concurrent Client-Server
6. TCP Iterative Client-Server
7. UDP Multiserver
8. TCP Multiserver
9. ARP Implementation
10. RARP Implementation

### Cycle 3: Advanced Protocols
11. Sliding Window Protocol
12. Stop-and-Wait Protocol
13. Go-Back-N Protocol
14. Selective Repeat Protocol

### Cycle 4: Network Simulation (NS2)
15. Network Topology Simulation
16. Congestion Control Simulation
17. Error Control Simulation

### Cycle 5: Routing Protocols (NS2)
18. Link State Routing (Dijkstra's Algorithm)
19. Distance Vector Routing (DSDV)

## Building the PDF

### Requirements
- LaTeX distribution (TeX Live, MacTeX, or MiKTeX)
- Required LaTeX packages: graphicx, geometry, fancyhdr, listings, xcolor, tocloft, hyperref, refcount

### Compilation

From the project root directory:

```bash
pdflatex main.tex
pdflatex main.tex  # Run twice for correct page references
```

Or use the full path (macOS with MacTeX):

```bash
/Library/TeX/texbin/pdflatex main.tex
/Library/TeX/texbin/pdflatex main.tex
```

### Output
- **Pages:** 84 pages
- **Size:** ~12.1 MB
- **Content:** Complete lab report with all experiments, code, screenshots, and results

## Compiling C Programs

All C source files compile cleanly with no errors or warnings:

```bash
cd src
gcc -Wall -Wextra -o program_name program_name.c
```

Example:
```bash
gcc -Wall -Wextra -o cycle2_TCP_client cycle2_TCP_client.c
gcc -Wall -Wextra -o cycle2_TCP_server cycle2_TCP_server.c
```

## Features

✅ Complete documentation of all 19 experiments  
✅ All C programs compile without errors/warnings  
✅ Comprehensive screenshots for all experiments  
✅ Properly formatted code listings with syntax highlighting  
✅ Detailed aims, algorithms, programs, and results  
✅ Professional title page and table of contents  
✅ Automatic page numbering and index generation  

## Notes

- The report uses custom macros for automatic page range calculation in the index
- All file paths are relative to the project root
- Screenshots are organized by cycle for easy reference
- All TCP/UDP programs follow standard socket programming practices
