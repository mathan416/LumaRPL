#include "ntp_packet.h"
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <chrono>
int main()
{
    for(const char *host:{"time.cloudflare.com","time.google.com"})
    {
        addrinfo hints{},*addresses=nullptr;hints.ai_family=AF_INET;hints.ai_socktype=SOCK_DGRAM;if(getaddrinfo(host,"123",&hints,&addresses))continue;int fd=socket(AF_INET,SOCK_DGRAM,0);timeval timeout{4,0};setsockopt(fd,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof timeout);
        if(connect(fd,addresses->ai_addr,addresses->ai_addrlen)){close(fd);freeaddrinfo(addresses);continue;}freeaddrinfo(addresses);
        uint64_t cookie;arc4random_buf(&cookie,sizeof cookie);uint8_t request[48],reply[512];pico_ntp_packet::request(request,cookie|1);auto sent=std::chrono::steady_clock::now();int length=-1;if(send(fd,request,48,0)==48)length=recv(fd,reply,sizeof reply,0);close(fd);
        auto ms=std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()-sent).count();int64_t utc=0;
        if(length>=0&&pico_ntp_packet::reply(reply,length,request,uint32_t(ms),utc)){printf("PASS: actual %s SNTP reply, RTT %lld ms, UTC %lld, host difference %lld seconds\n",host,(long long)ms,(long long)utc,(long long)(utc-time(nullptr)));return 0;}
    }
    puts("UNAVAILABLE: no validated response from either public UDP/123 server on this host network; simulated transport tests are separate");return 2;
}
