import subprocess
import time

# Function to start a command in a new command prompt window
def start_cmd(command):
    print(command)
    return subprocess.Popen(['cmd', '/C'] + command)

# Starting the server
print("Starting script...")
server_command = [r'bin\debug\game.exe', 'Server', 'tcp://localhost:5555', 'tcp://localhost:5556', '0']
subprocess.Popen(['start', 'cmd', '/K'] + server_command, shell=True)

time.sleep(3)
total_time = 0

# Number of clients to run
clientNos = 4
for times in range(5):  # Number of experiments
    print("NEW EXPERIMENT")
    process_queue = []
    start_time = time.time()
    for i in range(1, clientNos + 1):
        print("CLIENT :", i)
        client_command = [r'bin\debug\game.exe', 'Client', 'tcp://localhost:5555', 'tcp://localhost:5556', str(i)]
        process = start_cmd(client_command)
        process_queue.append(process)
    
    # Wait for all clients to finish
    for proc in process_queue:
        proc.wait()
    end_time = time.time()
    it_time = (end_time - start_time) * 1000
    print("Time: ", it_time)

