import socket
import multiprocessing
import json
import time


def simple_hash(s):
    """Simple hash function using basic arithmetic operations."""
    hash_value = 0
    for char in s:
        hash_value = (hash_value * 31 + ord(char)) % (2**32)
    return hash_value


def receiver_proc(host, port, task_queue):
    """Gavėjas - receives data from C++ sender and puts into task_queue."""
    srv_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    srv_sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    srv_sock.bind((host, port))
    srv_sock.listen(1)
    print(f"[Receiver] Listening on {host}:{port}")

    conn, addr = srv_sock.accept()
    print(f"[Receiver] Connected by {addr}")

    buffer = ""
    while True:
        data = conn.recv(1024)
        if not data:
            continue
        buffer += data.decode("utf-8")
        print(f"[Receiver] Received raw data: {data}")
        try:
            movies = json.loads(buffer)
            if isinstance(movies, list):
                for movie in movies:
                    print(f"[Receiver] Processing movie: {movie}")
                    task_queue.put(movie)
                buffer = ""
                conn.sendall(b"OK\n")
        except json.JSONDecodeError:
            print("[Receiver] Incomplete JSON, waiting for more data")
            pass


def worker_proc(task_queue, result_queue, worker_id, second_filter_rating=8.9):
    """Darbininkai: compute second function (5+ seconds), filter by second criterion, send to siuntėjas."""
    start_time = None
    processed_count = 0

    while True:
        print(f"[Worker {worker_id}] Waiting for movie...")
        try:
            movie = task_queue.get(timeout=3.0)
            if start_time is None:
                start_time = time.time()
                print(f"[Worker {worker_id}] Received first movie at {
                      start_time}")
        except multiprocessing.queues.Empty:
            if start_time is not None:
                end_time = time.time()
                duration_ms = (end_time - start_time) * 1000
                print(f"[Worker {worker_id}] Processed {
                      processed_count} movies in {duration_ms:.2f} ms")
            break

        processed_count += 1
        movie_str = json.dumps(movie)
        print(f"[Worker {worker_id}] Processing movie: {movie_str}")
        for _ in range(1000):
            hash2 = 0
            for char in movie_str:
                hash2 = (hash2 * 31 + ord(char)) % (2**32)
        movie['hash2'] = hash2

        rating = float(movie.get("rating", 0.0))
        if rating > second_filter_rating:
            result_queue.put(movie)
            print(f"[Worker {worker_id}] Filter passed: {movie}")
        else:
            print(f"[Worker {worker_id}] Filter failed: {movie}")


def sender_proc(host, port, result_queue):
    """Siuntėjas: collects all filtered movies and sends as single JSON array."""
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    while True:
        try:
            s.connect((host, port))
            print(f"[Sender] Connected to C++ at {host}:{port}")
            break
        except OSError:
            time.sleep(1)

    filtered_movies = []
    while True:
        print("[Sender] Waiting for movie...")
        try:
            movie = result_queue.get(timeout=3.0)
        except multiprocessing.queues.Empty:
            movie = None
        print(f"[Sender] Got movie: {movie}")
        if movie is None:
            if filtered_movies:
                # Build JSON array string manually
                json_data = "[\n" + ",\n".join(json.dumps(movie)
                                               for movie in filtered_movies) + "\n]\n"
                data_bytes = json_data.encode("utf-8")
                size = len(data_bytes)
                print(f"[Sender] Sending {size} bytes to C++")
                s.sendall(size.to_bytes(4, byteorder='little'))

                s.sendall(data_bytes)
                print(f"[Sender] Sent {len(filtered_movies)} movies to C++")
            else:
                print("[Sender] Zero movies filtered.")
                s.sendall((0).to_bytes(4, byteorder='little'))
            print("[Sender] Exiting.")
            s.close()
            break

        filtered_movies.append(movie)


def main():
    task_queue = multiprocessing.Queue()
    result_queue = multiprocessing.Queue()

    receiver_p = multiprocessing.Process(
        target=receiver_proc,
        args=("0.0.0.0", 9999, task_queue),
        daemon=True
    )
    receiver_p.start()

    worker_count = 8
    workers = []
    for i in range(worker_count):
        p = multiprocessing.Process(
            target=worker_proc,
            args=(task_queue, result_queue, i),
            daemon=True
        )
        workers.append(p)
        p.start()

    sender_p = multiprocessing.Process(
        target=sender_proc,
        args=("127.0.0.1", 9998, result_queue),
        daemon=True
    )
    sender_p.start()

    print("[Main] Python helper system running. Press Ctrl+C to terminate.")
    try:
        receiver_p.join()
        for w in workers:
            w.join()
        result_queue.put(None)
        sender_p.join()
    except KeyboardInterrupt:
        print("[Main] Shutting down.")


if __name__ == "__main__":
    main()
