#pragma once
#include "http_response.h"
#include "network/ca_root.h"
#include "mbedtls/ssl.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"
#include <cstring>
// Shared verbatim by firmware and host TLS integration tests. Transport callbacks
// block only within the caller's cancellation/deadline policy.
template<class Client>
bool pico_https_get(Client &s,const char *host,const char *request,char *out,size_t capacity,int &tls_error,
                   int (*send)(void *,const unsigned char *,size_t),int (*receive)(void *,unsigned char *,size_t))
{
    int code=0;
    s.stage("Verifying TLS certificate / hostname");
    code=mbedtls_ctr_drbg_seed(&s.rng,mbedtls_entropy_func,&s.entropy,nullptr,0);
    if(!code)code=mbedtls_x509_crt_parse_der(&s.ca,pico_currency_ca,sizeof pico_currency_ca);
    if(!code)code=mbedtls_ssl_config_defaults(&s.config,MBEDTLS_SSL_IS_CLIENT,MBEDTLS_SSL_TRANSPORT_STREAM,MBEDTLS_SSL_PRESET_DEFAULT);
    if(!code){mbedtls_ssl_conf_authmode(&s.config,MBEDTLS_SSL_VERIFY_REQUIRED);mbedtls_ssl_conf_ca_chain(&s.config,&s.ca,nullptr);mbedtls_ssl_conf_rng(&s.config,mbedtls_ctr_drbg_random,&s.rng);code=mbedtls_ssl_setup(&s.ssl,&s.config);}
    if(!code)code=mbedtls_ssl_set_hostname(&s.ssl,host);
    mbedtls_ssl_set_bio(&s.ssl,&s,send,receive,nullptr);
    if(!code)code=mbedtls_ssl_handshake(&s.ssl);
    if(code||mbedtls_ssl_get_verify_result(&s.ssl)){tls_error=code;s.fail(7,"TLS verification/handshake failed; check UTC date/time");return false;}
    s.stage("Downloading dated currency rates");size_t sent=0;
    while(sent<strlen(request)){code=mbedtls_ssl_write(&s.ssl,reinterpret_cast<const unsigned char *>(request)+sent,strlen(request)-sent);if(code<=0){tls_error=code;s.fail(7,"HTTPS request failed");return false;}sent+=code;if(!s.pump())return false;}
    pico_http_response response(out,capacity);unsigned char chunk[256];
    while(!response.complete)
    {
        code=mbedtls_ssl_read(&s.ssl,chunk,sizeof chunk);
        if(code==0||code==MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY){if(!response.finish()){s.fail(8,"Incomplete HTTPS response; previous rates retained");return false;}break;}
        if(code<0){tls_error=code;s.fail(7,"HTTPS receive failed; previous rates retained");return false;}
        if(!response.feed(chunk,code)){s.fail(8,"Invalid/oversized HTTP response; previous rates retained");return false;}
        if(!s.pump())return false;
    }
    return response.finish();
}
