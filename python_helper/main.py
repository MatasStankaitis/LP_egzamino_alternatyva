import socket
import multiprocessing
import json
import sys

SENTINEL_VALUE = {"title": "SENTINEL", "rating": 0.0, "year": 0}


def is_sentinel(movie):
    """Check if movie is a sentinel value."""
    return movie.get("title") == "SENTINEL" and movie.get("rating") == 0.0 and movie.get("year") == 0


def simple_hash(s):
    """Simple hash function using basic arithmetic operations."""
    hash_value = 0
    for char in s:
        hash_value = (hash_value * 31 + ord(char)) % (2**32)
    return hash_value


def receiver_proc(conn, task_queue):
    """Gavėjas - receives data from C++ sender and puts into task_queue."""

    buffer = ""
    while True:
        data = conn.recv(1024)
        if not data:
            continue
        buffer += data.decode("utf-8")
        try:
            movies = json.loads(buffer)
            if isinstance(movies, list):
                for movie in movies:
                    print(f"[Receiver] Processing movie: {movie}")
                    task_queue.put(movie)
                task_queue.put(SENTINEL_VALUE)
                print("[Receiver] All movies queued. Exiting receiver.")
                break
        except json.JSONDecodeError:
            print("[Receiver] Incomplete JSON, waiting for more data")


def sender_proc(conn, result_queue):
    """Siuntėjas - sends filtered movies back to C++ on the same socket."""
    filtered_movies = []
    while True:
        try:
            movie = result_queue.get()
        except multiprocessing.queues.Empty:
            continue
        if movie["title"] == "SENTINEL":
            print("[Sender] Received SENTINEL, sending final response.")
            break
        filtered_movies.append(movie)

    response_data = json.dumps(filtered_movies).encode("utf-8")
    size = len(response_data)
    print(f"[Sender] Sending {size} bytes of data.")
    conn.sendall(size.to_bytes(4, byteorder='little'))
    conn.sendall(response_data)
    conn.close()


def worker_proc(task_queue, result_queue, worker_id, second_filter_rating=8.9):
    """Darbininkai: computes a function and filters by second criterion."""
    while True:
        print(f"[Worker {worker_id}] Waiting for movie...")
        try:
            movie = task_queue.get()
        except multiprocessing.queues.Empty:
            continue
        if is_sentinel(movie):
            print(f"[Worker {worker_id}] Received SENTINEL, exiting.")
            task_queue.put(SENTINEL_VALUE)
            break
        movie_str = json.dumps(movie)
        print(f"[Worker {worker_id}] Processing movie: {movie_str}")
        for _ in range(1000):
            hash2 = simple_hash(movie_str)
        movie['hash2'] = hash2

        rating = float(movie.get("rating", 0.0))
        if rating > second_filter_rating:
            result_queue.put(movie)
            print(f"[Worker {worker_id}] Filter passed: {movie}")
        else:
            print(f"[Worker {worker_id}] Filter failed: {movie}")


def main():
    print("[Main] Python helper system running. Press Ctrl+C to terminate.")
    srv_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    srv_sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    srv_sock.bind(("0.0.0.0", 9999))
    srv_sock.listen(1)
    print("[Main] Listening on 0.0.0.0:9999")
    conn, addr = srv_sock.accept()
    print(f"[Main] Connected by {addr}")

    task_queue = multiprocessing.Queue()
    result_queue = multiprocessing.Queue()

    receiver_p = multiprocessing.Process(
        target=receiver_proc,
        args=(conn, task_queue),
        daemon=True
    )

    sender_p = multiprocessing.Process(
        target=sender_proc,
        args=(conn, result_queue),
        daemon=True
    )

    receiver_p.start()
    sender_p.start()

    worker_count = 1
    if len(sys.argv) > 1:
        worker_count = int(sys.argv[1])
    workers = []
    for i in range(worker_count):
        p = multiprocessing.Process(
            target=worker_proc,
            args=(task_queue, result_queue, i),
            daemon=True
        )
        workers.append(p)
        p.start()

    try:
        for w in workers:
            w.join()
        print("[Main] All workers finished.")
        result_queue.put(SENTINEL_VALUE)
        receiver_p.join()
        sender_p.join()

    except KeyboardInterrupt:
        print("[Main] Shutting down.")


if __name__ == "__main__":
    main()
