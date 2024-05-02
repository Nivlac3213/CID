#!/bin/bash

network="172.19.172.0/24"
target_macs=("D8:3A:DD:62:0B:4C" "D8:3A:DD:62:0A:51") 
raspberry_pi_ips=()

# scan the network for devices
echo "Scanning the network, please wait..."
map_output=$(sudo nmap -sn $network)

for target_mac in "${target_macs[@]}"; do
    # search for MAC address and extract the IP
    ip_address=$(echo "$map_output" | awk -v mac="$target_mac" '
        /Nmap scan report for/ { ip=$NF }
        /MAC Address:/ && $0 ~ mac { print ip; exit }
    ')

    if [ -n "$ip_address" ]; then
        echo "The IP address for MAC $target_mac is: $ip_address"
        raspberry_pi_ips+=($ip_address)
    else
        echo "MAC address $target_mac not found."
    fi
done

# ssh into pi and read file
for ip in ${raspberry_pi_ips[@]}; do
    echo "Reading data from Raspberry Pi at $ip"
    # TODO replace with file containing SDR info
    ssh pi@$ip 'cat /home/pi/temp.txt'
done