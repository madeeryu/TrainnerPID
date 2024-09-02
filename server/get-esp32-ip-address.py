import nmap
import time
# Using the ESP32Cam stream URL
def esp32_ip_address(target_mac):
    nm = nmap.PortScanner()
    # nm.scan(hosts='192.168.0.0/16', arguments='-sP --max-retries 1 --host-timeout 10ms')
    nm.scan(hosts='192.168.1.0/24', arguments='-sP')
    '''
    I'm still confused in this part. Should I use /24 instead /16?
    If I use /16, I can scanned the IP Address from 192.168.0.0 until 192.168.255.255 and it will very slow. Because it will scanning 65,536 IP address.
    But, if I use /24, I only scanned the IP Address from 192.168.1.0 until 192.168.1.255, or just 256 IP Address and it will be faster.
    The advantage I use /16, I can cover wide range of dynamic IP Address.
    Well, maybe this program depends on the router we will use. For now, we use Fiberhome router and the IP address will started from 192.168.1.0.
    '''
    host_list = nm.all_hosts()
    for host in host_list:
        if 'mac' in nm[host]['addresses']:
            # print(host+' : '+nm[host]['addresses']['mac'])
            if target_mac == nm[host]['addresses']['mac']:
                # print(target_mac)
                return host

esp32_mac_address = 'A8:42:E3:48:39:70' # ESP32 MAC Address
ip_address = esp32_ip_address(esp32_mac_address)
print(ip_address)