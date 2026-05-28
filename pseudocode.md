# Networking Lab Pseudocode Reference

## 1. Congestion Control

### Leaky Bucket Algorithm
```python
bucket_content = 0

for tick in time_simulation:
    incoming_packet = get_random_packet_size()
    
    # 1. Bucket addition / Overflow check
    if incoming_packet + bucket_content > BUCKET_CAPACITY:
        print("Bucket overflow, packet dropped")
    else:
        bucket_content += incoming_packet
        print("Packet added to bucket")
        
    # 2. Leak at constant rate
    sent_data = min(bucket_content, OUTPUT_RATE)
    bucket_content -= sent_data
    
    print("Transmitted:", sent_data, "Remaining:", bucket_content)
    sleep(1_SECOND)
```

---

## 2. Flow Control

### Stop-and-Wait Basic
**Sender**
```python
while has_packets_to_send():
    packet = get_next_packet()
    sendto(packet, receiver)
    ack = recvfrom()
    if ack == expected_ack:
        move_to_next_packet()
```

**Receiver**
```python
while True:
    packet = recvfrom()
    save(packet)
    sendto("ACK", sender)
```

### Stop-and-Wait ARQ (With Timeout)
**Sender**
```python
set_socket_timeout(TIMEOUT_SECONDS)
while has_packets():
    sendto(current_packet, receiver)
    try:
        ack = recvfrom()
        move_to_next_packet()
    except TimeoutError:
        print("Timeout! Resending packet")
        # Loop continues, resending the same packet
```

### Go-Back-N ARQ
**Sender**
```python
set_socket_timeout(TIMEOUT_SECONDS)
window_start = 0
window_end = WINDOW_SIZE

def send_window():
    for packet in range(window_start, window_end):
        sendto(packet, receiver)

send_window() # Blast initial window

while window_start < TOTAL_PACKETS:
    try:
        ack = recvfrom()
        if ack == expected_ack:
            window_start += 1
            if window_end < TOTAL_PACKETS:
                sendto(packet[window_end], receiver)
                window_end += 1
        else:
            send_window() # Resend entire window on wrong ACK
    except TimeoutError:
        send_window() # Resend entire window on timeout
```

**Receiver**
```python
expected_packet = 0
while True:
    packet = recvfrom()
    if packet == expected_packet:
        sendto(f"ACK {expected_packet}", sender)
        expected_packet += 1
    else:
        # Silently drop out-of-order packets (triggers sender timeout)
        pass 
```

### Selective Repeat ARQ
**Sender**
```python
set_socket_timeout(TIMEOUT_SECONDS)
acked = array_of_zeros(TOTAL_PACKETS)

def send_unacked_in_window():
    for p in range(window_start, window_end):
        if acked[p] == 0:
            sendto(p, receiver)

send_unacked_in_window()

while window_start < TOTAL_PACKETS:
    try:
        msg, packet_id = recvfrom()
        if msg == "ACK":
            acked[packet_id] = 1
            # Slide window if possible (if oldest packet is acked)
            while acked[window_start] == 1:
                window_start += 1
                if window_end < TOTAL_PACKETS:
                    sendto(window_end, receiver)
                    window_end += 1
        elif msg == "NACK":
            acked[packet_id] = 0
            sendto(packet_id, receiver) # Resend ONLY this single packet
    except TimeoutError:
        send_unacked_in_window() # Resend all unacked packets in window
```

**Receiver**
```python
while True:
    packet = recvfrom()
    if not is_duplicate(packet):
        save(packet)
    
    # Simulate network reliability (1 in 3 chance of NACK)
    if random_chance(33%):
        sendto(f"NACK {packet.id}", sender)
    else:
        sendto(f"ACK {packet.id}", sender)
```

---

## 3. FTP (File Transfer Protocol)

### FTP Server
```python
server_sock = socket(TCP)
bind(server_sock, port)
listen(server_sock)
client_sock, addr = accept(server_sock)

while True:
    command = recv(client_sock)
    
    if command == "PUT":
        filename = recv(client_sock)
        file = open(filename, "w")
        while True:
            chunk = recv(client_sock)
            if chunk == "END$": break
            file.write(chunk)
        file.close()

    elif command == "GET":
        send(client_sock, server_pid)
        filename = recv(client_sock)
        if file_exists(filename):
            send(client_sock, "200")
            file = open(filename, "r")
            for line in file:
                send(client_sock, line)
            send(client_sock, "END$")
            file.close()
        else:
            send(client_sock, "404")

    elif command == "BYE":
        close(client_sock)
        exit()
```

### FTP Client
```python
client_sock = socket(TCP)
connect(client_sock, server_port)

while True:
    choice = get_user_menu_choice() # 1=PUT, 2=GET, 3=BYE
    
    if choice == 1: # Upload
        send(client_sock, "PUT")
        filename = get_input()
        send(client_sock, filename)
        
        file = open(filename, "r")
        for line in file:
            send(client_sock, line)
        send(client_sock, "END$")
        file.close()
        
    elif choice == 2: # Download
        send(client_sock, "GET")
        server_pid = recv(client_sock)
        filename = get_input()
        send(client_sock, filename)
        
        status = recv(client_sock)
        if status == "200":
            file = open(filename, "w")
            while True:
                chunk = recv(client_sock)
                if chunk == "END$": break
                file.write(chunk)
            file.close()
            
    elif choice == 3: # Disconnect
        send(client_sock, "BYE")
        close(client_sock)
        exit()
```

---

## 4. Routing

### Distance Vector Routing (Bellman-Ford)
```python
# Initialize distance matrix (dist) with direct links, else infinity
# next_hop[i][j] = j for all direct neighbors

# Main algorithm
for k in range(nodes):           # intermediate node
    for i in range(nodes):       # source node
        for j in range(nodes):   # destination node
            new_cost = dist[i][k] + dist[k][j]
            if new_cost < dist[i][j]:
                dist[i][j] = new_cost
                next_hop[i][j] = next_hop[i][k]

print_routing_tables(dist, next_hop)
```

### Link State Routing (Dijkstra)
```python
for start_node in all_nodes:
    distances = array_of_infinity()
    previous = empty_array()
    distances[start_node] = 0
    unvisited = all_nodes

    while unvisited is not empty:
        current = get_node_with_min_distance(unvisited, distances)
        unvisited.remove(current)

        for neighbor in neighbors(current):
            new_dist = distances[current] + edge_cost(current, neighbor)
            if new_dist < distances[neighbor]:
                distances[neighbor] = new_dist
                previous[neighbor] = current

    print_shortest_paths(start_node, distances, previous)
```

---

## 5. TCP Fundamentals

### TCP Multi-Client Chat
**Chat Server**
```python
server_sock = socket(TCP)
bind(server_sock, port)
listen(server_sock)

# Background thread per client
def client_handler(client_sock, client_id):
    while True:
        msg = recv(client_sock)
        for other_client in all_clients:
            if other_client.id != client_id:
                send(other_client.sock, msg)

# Main loop
while True:
    client_sock, addr = accept(server_sock)
    add_to_client_list(client_sock)
    spawn_thread(client_handler, client_sock, new_client_id)
```

**Chat Client**
```python
client_sock = socket(TCP)
connect(client_sock, server_port)

# Background receive thread
def receive_handler():
    while True:
        msg = recv(client_sock)
        print(msg)

spawn_thread(receive_handler)

# Main interface loop
while True:
    user_input = read_keyboard()
    if user_input == "SEND":
        data = read_keyboard()
        send(client_sock, data)
```

### Simple TCP
**Server**
```python
server_sock = socket(TCP); bind(); listen()
client_sock, addr = accept()
print(recv(client_sock))
send(client_sock, "Data received")
```

**Client**
```python
client_sock = socket(TCP); connect()
send(client_sock, "Hello")
print(recv(client_sock))
```

---

## 6. UDP Fundamentals

### Simple UDP
**Server**
```python
server_sock = socket(UDP); bind()
while True:
    data, client_addr = recvfrom(server_sock)
    sendto(server_sock, "Data received", client_addr)
```

**Client**
```python
client_sock = socket(UDP)
sendto(client_sock, "Hello", server_addr)
print(recvfrom(client_sock))
```

### UDP Time Protocol
**Time Server**
```python
server_sock = socket(UDP); bind()
while True:
    data, client_addr = recvfrom(server_sock)
    if data == "time":
        sendto(server_sock, format_time(get_system_time()), client_addr)
```

**Time Client**
```python
client_sock = socket(UDP)
sendto(client_sock, "time", server_addr)
print("Server time:", recvfrom(client_sock))
```