#!/bin/bash

# variables
raspberry_pi_ip="172.19.172.252"
raspberry_pi_user="pi"
file_to_transfer="/home/smita236-erau.edu/Documents/largefile.txt"
transfer_destination_path="/home/pi/"
output_file="performance_metrics.csv"

# ensure output file exists and the header is written
echo "Test,Result,Unit" > $output_file

# ping test
ping_result=$(ping -c 4 $raspberry_pi_ip | tail -1| awk '{print $4}' | cut -d '/' -f 2)
echo "Ping Average Latency,$ping_result,ms" >> $output_file

# ssh connection time
ssh_time=$( { time ssh -o ConnectTimeout=10 $raspberry_pi_user@$raspberry_pi_ip "exit"; } 2>&1 | grep '^real' | awk '{print $2}')
echo "SSH Connection Time,$ssh_time,s" >> $output_file

# scp transfer rate (upload)
start_time=$(date +%s)
scp $file_to_transfer $raspberry_pi_user@$raspberry_pi_ip:$transfer_destination_path
end_time=$(date +%s)
transfer_time=$((end_time-start_time))
file_size=$(du -k "$file_to_transfer" | cut -f1)
transfer_rate=$(echo "scale=5; $file_size / $transfer_time" | bc)
echo "SCP Upload Rate,$transfer_rate,KB/s" >> $output_file

# scp transfer rate (download)
scp $raspberry_pi_user@$raspberry_pi_ip:$transfer_destination_path"/$(basename $file_to_transfer)" /tmp
end_time=$(date +%s)
transfer_time=$((end_time-start_time))
echo "SCP Download Rate,$transfer_rate,KB/s" >> $output_file

# bandwidth test
ssh $raspberry_pi_user@$raspberry_pi_ip 'iperf3 -s' # start iperf3 service on pi lol
iperf3 -c $raspberry_pi_ip -J | jq '.end.sum_received.bits_per_second' | {
  read rate;
  rate_mbps=$(echo "scale=2; $rate / 1000000" | bc);
  echo "Iperf3 Bandwidth,$rate_mbps,Mbps" >> $output_file;
}

# netcat throughput test 
# ssh $raspberry_pi_user@$raspberry_pi_ip 'nc -l -v -p 12345' # ensure nc is in listening mode on pi
start_time=$(date +%s.%N)
pv $file_to_transfer | nc $raspberry_pi_ip 12345
end_time=$(date +%s.%N)
elapsed=$(echo "$end_time - $start_time" | bc)
rate=$(echo "scale=7; $file_size / ($elapsed/1000)" | bc)
echo "Netcat Throughput,$rate,KB/s" >> $output_file

# tcp transfer rate test (using iperf3)
# result_tcp=$(iperf3 -c $raspberry_pi_ip -J | jq '.end.sum_received.bits_per_second')
# rate_tcp=$(echo "scale=2; $result_tcp / 1000000" | bc)
# echo "TCP,$rate_tcp,Mbps" >> $output_file

# udp transfer rate test (using iperf3)
# result_udp=$(iperf3 -c $raspberry_pi_ip -u -J | jq '.end.streams[0].udp.bits_per_second')
# rate_udp=$(echo "scale=2; $result_udp / 1000000" | bc)
# echo "UDP,$rate_udp,Mbps" >> $output_file

file_size_bytes=$(stat -c %s "$file_to_transfer") # get file size in bytes
file_size_bits=$((file_size_bytes * 8)) # convert file size to bits

# tcp transfer rate test
start_time=$(date +%s.%N)
nc $raspberry_pi_ip 12345 < "$file_to_transfer"
end_time=$(date +%s.%N) 
transfer_time=$(echo "$end_time - $start_time" | bc)
rate_mbps=$(echo "scale=5; ($file_size_bits / $transfer_time) / 1000000" | bc)
if [ $? -eq 0 ]; then
    echo "TCP,$rate_mbps,Mbps" >> $output_file
else
    echo "TCP,Failure"
fi

start_time=$(date +%s.%N)
ncat -u $raspberry_pi_ip 12345 < "$file_to_transfer"
end_time=$(date +%s.%N) 
transfer_time=$(echo "$end_time - $start_time" | bc)
rate_mbps=$(echo "scale=5; ($file_size_bits / $transfer_time) / 1000000" | bc)
if [ $? -eq 0 ]; then
    echo "UDP,$rate_mbps,Mbps" >> $output_file
else
    echo "UDP,Failure"
fi

# display results
echo "Test results stored in $output_file:"
cat $output_file
