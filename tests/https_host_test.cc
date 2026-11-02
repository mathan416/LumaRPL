#include "https_client.h"
#include "tls_budget.h"
#include "currency_csv.h"
#include "mbedtls/platform_time.h"
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <algorithm>
struct client
{
    int socket=-1,failure=0;const char *message="";bool cancelled=false;
    mbedtls_ssl_context ssl;mbedtls_ssl_config config;mbedtls_x509_crt ca;mbedtls_ctr_drbg_context rng;mbedtls_entropy_context entropy;
    client(){mbedtls_ssl_init(&ssl);mbedtls_ssl_config_init(&config);mbedtls_x509_crt_init(&ca);mbedtls_ctr_drbg_init(&rng);mbedtls_entropy_init(&entropy);}
    ~client(){mbedtls_ssl_free(&ssl);mbedtls_ssl_config_free(&config);mbedtls_x509_crt_free(&ca);mbedtls_ctr_drbg_free(&rng);mbedtls_entropy_free(&entropy);if(socket>=0)close(socket);}
    void fail(int code,const char *text){failure=code;message=text;}
    void stage(const char *){}
    bool pump(){return !cancelled;}
};
static int send_bytes(void *context,const unsigned char *data,size_t length)
{auto &c=*static_cast<client *>(context);if(c.cancelled)return MBEDTLS_ERR_SSL_INTERNAL_ERROR;int n=send(c.socket,data,std::min<size_t>(length,512),0);return n>0?n:MBEDTLS_ERR_SSL_INTERNAL_ERROR;}
static int receive_bytes(void *context,unsigned char *data,size_t length)
{auto &c=*static_cast<client *>(context);if(c.cancelled)return MBEDTLS_ERR_SSL_INTERNAL_ERROR;int n=recv(c.socket,data,std::min<size_t>(length,512),0);return n>=0?n:MBEDTLS_ERR_SSL_INTERNAL_ERROR;}
static int connect_provider()
{
    addrinfo hints{},*addresses=nullptr;hints.ai_family=AF_INET;hints.ai_socktype=SOCK_STREAM;
    if(getaddrinfo("api.frankfurter.dev","443",&hints,&addresses))return -1;int fd=-1;
    for(auto *a=addresses;a;a=a->ai_next){fd=socket(a->ai_family,a->ai_socktype,a->ai_protocol);if(fd<0)continue;timeval timeout{20,0};setsockopt(fd,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof timeout);setsockopt(fd,SOL_SOCKET,SO_SNDTIMEO,&timeout,sizeof timeout);if(!connect(fd,a->ai_addr,a->ai_addrlen))break;close(fd);fd=-1;}
    freeaddrinfo(addresses);return fd;
}
extern "C" int mbedtls_hardware_poll(void *,unsigned char *out,size_t length,size_t *actual)
{arc4random_buf(out,length);*actual=length;return 0;}
extern "C" mbedtls_ms_time_t mbedtls_ms_time(void){timespec value;clock_gettime(CLOCK_MONOTONIC,&value);return int64_t(value.tv_sec)*1000+value.tv_nsec/1000000;}
static time_t invalid_time(time_t *out){time_t value=1577836800;if(out)*out=value;return value;}
int main(int argc,char **argv)
{
    if(argc<2)return 2;
    if(atoi(argv[1])==5){client c;int code=mbedtls_x509_crt_parse_der(&c.ca,pico_currency_ca,sizeof pico_currency_ca);printf("Root parse code %d\n",code);return code!=0;}int mode=atoi(argv[1]);bool ok=false;int code=0;size_t peak;
    if(!pico_tls_budget_reset())return 3;
    {
        client c;c.socket=connect_provider();if(c.socket<0){fprintf(stderr,"Could not connect provider\n");return 4;}
        if(mode==2)mbedtls_platform_set_time(invalid_time);if(mode==3)c.cancelled=true;
        if(mode==4){void *pressure=pico_tls_calloc(1,39000);if(!pressure)return 7;char out[2048];ok=pico_https_get(c,"api.frankfurter.dev","GET / HTTP/1.1\r\nHost: api.frankfurter.dev\r\n\r\n",out,sizeof out,code,send_bytes,receive_bytes);pico_tls_free(pressure);}
        else
        {
            char out[2048];const char *host=mode==1?"wrong.invalid":"api.frankfurter.dev";
            const char *request="GET /v2/rates.csv?base=EUR&quotes=USD,CAD,GBP,JPY,CHF,AUD,NZD,CNY,INR,MXN,BRL,KRW,SEK,NOK,DKK,PLN,CZK,HUF,TRY,ZAR,SGD,HKD,ILS,AED,SAR,THB,IDR,PHP,MYR HTTP/1.1\r\nHost: api.frankfurter.dev\r\nAccept-Encoding: identity\r\nConnection: close\r\n\r\n";
            ok=pico_https_get(c,host,request,out,sizeof out,code,send_bytes,receive_bytes);
            if(ok){pico_currency_rates rates;if(!pico_currency_csv(out,strlen(out),rates)){fprintf(stderr,"CSV failed: %s\n",out);return 5;}printf("Validated %u dated currencies\n",rates.count);}
        }
        peak=pico_tls_peak();fprintf(stderr,"mode %d success %d TLS %d failure %d (%s), peak %zu / 40960\n",mode,ok,code,c.failure,c.message,peak);
    }
    if(pico_tls_used()){fprintf(stderr,"TLS leak: %zu\n",pico_tls_used());return 6;}
    return mode==0?!ok:ok;
}
