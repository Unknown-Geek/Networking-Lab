# CSL 332 — Networking Lab
**College of Engineering Trivandrum | Semester 4 | Dec 2025 – Mar 2026**

---

## Table of Contents
- [Cycle I — System Calls](#cycle-i--system-calls)
- [Cycle II — Socket Programming](#cycle-ii--socket-programming)
  - [2.1 TCP](#21-tcp)
  - [2.2 UDP](#22-udp)
  - [2.3 FTP](#23-ftp)
- [Cycle III — Flow Control](#cycle-iii--flow-control)
- [Cycle IV — Routing](#cycle-iv--routing)
- [Cycle V — Congestion Control](#cycle-v--congestion-control)
- [Quick Reference — Key Socket Functions](#quick-reference--key-socket-functions)

---

## Cycle I — System Calls

> Familiarize with `fork()`, process IDs, and basic file I/O system calls in Linux.

| # | File | Description |
|---|------|-------------|
| 1.1 | [`system_calls/fork_odd_even_sum.c`](system_calls/fork_odd_even_sum.c) | `fork()` — child computes odd sum, parent computes even sum from the same input array |
| 1.2 | [`system_calls/fork_parent_child.c`](system_calls/fork_parent_child.c) | Basic `fork()` — child prints "Child", parent calls `wait()` then prints "Parent" |
| 1.3 | [`system_calls/fork_uppercase.c`](system_calls/fork_uppercase.c) | Child converts a string to uppercase (`char - 32`); parent waits with `wait(NULL)` |
| 1.4 | [`system_calls/fork_prime.c`](system_calls/fork_prime.c) | Parent checks whether an input number is prime after `fork()` |
| 1.5 | [`system_calls/fork_hello.c`](system_calls/fork_hello.c) | Child prints "Hello" 5 times with `sleep(1)`; parent waits with `wait(NULL)` |
| 1.6 | [`system_calls/fork_getpid.c`](system_calls/fork_getpid.c) | Both child and parent print `getpid()` and `getppid()` after `fork()` |
| 1.7 | [`system_calls/file_io.c`](system_calls/file_io.c) | Write a line to `sample.txt` with `fprintf`, read it back with `fscanf` |

---

## Cycle II — Socket Programming

### 2.1 TCP

> Client-server communication over **TCP** (`SOCK_STREAM`) — connection-oriented, reliable, ordered.

**Server flow:** `socket` → `bind` → `listen` → `accept` → `recv`/`send` → `close`  
**Client flow:** `socket` → `connect` → `send`/`recv` → `close`

| File | Role | Description |
|------|------|-------------|
| [`TCP/server.c`](TCP/server.c) | Server | Binds to port 4321, accepts one client, receives a message, replies `"Data received"` |
| [`TCP/client.c`](TCP/client.c) | Client | Connects to server on port 4321, sends user input, prints the server's reply |
| [`TCP/multi_server.c`](TCP/multi_server.c) | Chat server | Spawns a `pthread` per client; routes `SEND <id> <msg>` to all other connected clients |
| [`TCP/multi_client.c`](TCP/multi_client.c) | Chat client | Background thread prints incoming messages; main loop reads user input and sends `SEND` commands |

> **Compile:** `gcc server.c -o server` / `gcc client.c -o client`  
> **Multi:** `gcc -pthread multi_server.c -o server` / `gcc -pthread multi_client.c -o client`

---

### 2.2 UDP

> Client-server communication over **UDP** (`SOCK_DGRAM`) — connectionless, no handshake, faster.

**Server flow:** `socket` → `bind` → `recvfrom` → `sendto`  
**Client flow:** `socket` → `sendto` → `recvfrom` *(no bind, no connect)*

| File | Role | Description |
|------|------|-------------|
| [`UDP/server.c`](UDP/server.c) | Server | Binds to port 5431, receives a datagram, replies `"Data received"` |
| [`UDP/client.c`](UDP/client.c) | Client | Sends user input via `sendto`, receives reply via `recvfrom` |
| [`UDP/time_server.c`](UDP/time_server.c) | Time server | Loops waiting for `"time"` requests; responds with `time()` + `strftime()` formatted string |
| [`UDP/time_client.c`](UDP/time_client.c) | Time client | Sends `"time"` to server on port 8776, prints the received timestamp |

> **Key difference from TCP:** No `listen`/`accept`. Every `sendto`/`recvfrom` carries the destination/source address explicitly.

---

### 2.3 FTP

> File transfer over TCP using a simple command protocol: `PUT`, `GET`, `BYE`.

| File | Role | Description |
|------|------|-------------|
| [`FTP/server.c`](FTP/server.c) | FTP Server | `PUT` → receive and write file; `GET` → send file (200 OK / 404 not found); `BYE` → disconnect |
| [`FTP/client.c`](FTP/client.c) | FTP Client | Menu-driven: **1** upload (PUT), **2** download (GET), **3** exit (BYE) |

> **Protocol detail:** File data is sent line-by-line; the sentinel string `"END$"` signals end of transfer.

---

## Cycle III — Flow Control

> Simulate sliding window protocols using UDP sockets. The server randomly drops ACKs to simulate an unreliable network channel.

### 3.1.1 Stop-and-Wait

> Sender sends **one packet at a time** and waits for ACK before proceeding. Three-message exchange per packet.

| File | Role | Description |
|------|------|-------------|
| [`flow_control/stop_and_wait_server.c`](flow_control/stop_and_wait_server.c) | Receiver | `recvfrom(packet N)` → `sendto(ACK N)` → `recvfrom(final ACK)` → repeat |
| [`flow_control/stop_and_wait_client.c`](flow_control/stop_and_wait_client.c) | Sender | `sendto(packet N)` → `recvfrom(ACK)` → `sendto(final ACK)` → `packet_count++` |

---

### 3.1.2 Stop-and-Wait ARQ

> Stop-and-Wait with **automatic retransmission** on timeout. Uses `setsockopt(SO_RCVTIMEO)`.

| File | Role | Description |
|------|------|-------------|
| [`flow_control/stop_and_wait_ARQ_server.c`](flow_control/stop_and_wait_ARQ_server.c) | Receiver | Receives packets; randomly drops 2/3 of ACKs (`rand()%3==0`) to force client retransmits |
| [`flow_control/stop_and_wait_ARQ_client.c`](flow_control/stop_and_wait_ARQ_client.c) | Sender | Sets 5s receive timeout; if `recvfrom` returns `<0` (timeout) → retry same packet; else advance |

---

### 3.1.3 Go-Back-N ARQ

> Sender keeps a **window** of multiple in-flight packets. On any error, the **entire window is retransmitted**.

| File | Role | Description |
|------|------|-------------|
| [`flow_control/gobackn_server.c`](flow_control/gobackn_server.c) | Receiver | Tracks received packets in `recd[]`; randomly drops ACKs; logs duplicates |
| [`flow_control/gobackn_client.c`](flow_control/gobackn_client.c) | Sender | `sendWindowPackets()` blasts the full window; timeout or wrong ACK → resend whole window; correct ACK → slide `windowStart++`, `windowEnd++`, send next packet |

---

### 3.1.4 Selective Repeat ARQ

> Only **individual missing packets** are retransmitted. One `pthread` per packet handles retries independently.

| File | Role | Description |
|------|------|-------------|
| [`flow_control/selective_ARQ_server.c`](flow_control/selective_ARQ_server.c) | Receiver | Sends `"ACK id"` or `"NACK id"` per packet; silently drops some; marks received in `packets[]` struct |
| [`flow_control/selective_ARQ_client.c`](flow_control/selective_ARQ_client.c) | Sender | `send_packet` thread per packet retries until `packet.sent==1`; `receive_packets` thread processes ACK/NACK and advances the sliding window |

---

## Cycle IV — Routing

### 4.1 Distance Vector Routing — [`routing/DVR.c`](routing/DVR.c)

> Each router shares its distance table with neighbours and updates using the **Bellman-Ford** equation:  
> `route[i].dist[j] = min(route[i].dist[j], route[i].dist[k] + route[k].dist[j])`

**Steps:**
1. Input number of nodes and distance matrix
2. Initialize `route[i].dist[j] = dm[i][j]`, `route[i].from[j] = j`
3. Loop until no update: for all `i, j, k` — relax via intermediate node `k`
4. Print routing table (destination, next hop, cost) for every router

---

### 4.2 Link State Routing — [`routing/LSR.c`](routing/LSR.c)

> Each router has the complete topology and independently computes shortest paths using **Dijkstra's algorithm**.

**Steps:**
1. Input nodes and links → build `graph[src][dest] = cost`
2. Run `dijkstra(start)` for each node:
   - `distance[all] = INF`, `distance[start] = 0`
   - Repeat: pick unvisited node with min distance → mark visited → relax neighbours
3. Print routing table per node using `distance[]` and `previous[]`

---

## Cycle V — Congestion Control

### 5.1 Leaky Bucket — [`congestion/leaky_bucket.c`](congestion/leaky_bucket.c)

> Models a fixed-size network buffer with a **constant drain rate** — bursts are absorbed, overflow is dropped.

**Steps:**
1. Input output `rate` and `bucket_size`
2. Generate random packet sizes
3. For each packet:
   - If `packet + remaining > bucket_size` → drop (overflow)
   - Else `remaining += packet`
4. Each clock tick (`sleep(1)`): transmit `min(remaining, rate)` bytes

> **Concept:** Output is smoothed to a steady rate regardless of how bursty the input is.

---

## Quick Reference — Key Socket Functions

| Function | TCP | UDP | Notes |
|----------|:---:|:---:|-------|
| `socket(AF_INET, SOCK_STREAM, 0)` | ✅ | — | Create TCP socket |
| `socket(AF_INET, SOCK_DGRAM, 0)` | — | ✅ | Create UDP socket |
| `bind(sock, &addr, sizeof(addr))` | Server | Server | Assign local IP + port |
| `listen(sock, backlog)` | Server | — | TCP only — queue incoming connections |
| `accept(sock, &client_addr, &size)` | Server | — | TCP only — blocks until a client connects |
| `connect(sock, &addr, sizeof(addr))` | Client | — | TCP only — initiate connection |
| `send(sock, buf, len, 0)` | ✅ | — | Send over connected TCP socket |
| `recv(sock, buf, len, 0)` | ✅ | — | Receive from connected TCP socket |
| `sendto(sock, buf, len, 0, &addr, size)` | — | ✅ | UDP send — specify destination each call |
| `recvfrom(sock, buf, len, 0, &addr, &size)` | — | ✅ | UDP receive — also captures sender's address |
| `setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv))` | — | ✅ | Set receive timeout (used in ARQ) |
| `pthread_create(&tid, NULL, fn, arg)` | ✅ | ✅ | Spawn thread (multi-client / selective ARQ) |
| `close(sock)` | ✅ | ✅ | Close socket |
