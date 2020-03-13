#!/usr/bin/python

from scapy.all import *
from time import sleep

sip = "157.0.0.10" # spoofed source IP address
dip = "127.0.0.1" # destination IP address
sport = 11223 # source port
dport = 22222 # destination port
payload = "yaba daba doo" # packet payload

i = 1
while True:
    print("count=" + str(i))
    i=i+1
    spoofed_packet=IP(src=sip, dst=dip) / UDP(sport=sport, dport=dport) / payload
    send(spoofed_packet)
    sleep(0.3)


