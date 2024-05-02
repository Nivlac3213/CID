#!/bin/bash

network="172.19.172.0/24"
target_macs=("D8:3A:DD:62:0B:4C" "D8:3A:DD:62:0A:51" "DC:A6:32:04:55:C6") 
raspberry_pi_ips=()

# scan network for devices
echo "Scanning the network, please wait..."
map_output=$(sudo nmap -sn $network)

for target_mac in "${target_macs[@]}"; do
    # search for MAC address and extract IP
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

# pass IP addy to serverApp
# for ip in ${raspberry_pi_ips[@]}; do
#    echo $ip | tr -d '()'
# done