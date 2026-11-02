#include "tls_budget.h"
#include "../provider_chain_fixture.h"
#include "network/ca_root.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/platform_time.h"
#include <cstdlib>
#include <cstring>
#include <cstdint>
static void print(const char *s){register int op asm("r0")=4;register const char *text asm("r1")=s;asm volatile("bkpt 0xab":"+r"(op):"r"(text):"memory");}
extern "C" void exit(int result){unsigned parameters[]={0x20026,unsigned(result)};register unsigned operation asm("r0")=0x20;register void *data asm("r1")=parameters;asm volatile("bkpt 0xab":"+r"(operation):"r"(data):"memory");for(;;){}}
static void check(bool condition){static unsigned stage;++stage;if(!condition){char report[]="FAIL: Arm TLS stage 0000\n";const char *digits="0123456789abcdef";for(int i=0;i<4;++i)report[20+i]=digits[(stage>>((3-i)*4))&15];print(report);exit(1);}}
extern "C" int mbedtls_hardware_poll(void *,unsigned char *out,size_t length,size_t *actual){memset(out,0x7b,length);*actual=length;return 0;} // Test-only deterministic entropy.
extern "C" mbedtls_ms_time_t mbedtls_ms_time(void){return 0;}
static time_t fixture_time(time_t *out){time_t value=provider_fixture_utc;if(out)*out=value;return value;}
int main()
{
    mbedtls_platform_set_time(fixture_time);
    // Model persisted document buffers, the complete network session and CSV body
    // concurrently. Every allocation still uses production _sbrk's 96KiB ceiling.
    void *documents=malloc(8192),*session=malloc(8192),*body=malloc(2048);check(documents&&session&&body);
    for(int iteration=0;iteration<20;++iteration)
    {
        check(pico_tls_budget_reset());mbedtls_ssl_context ssl;mbedtls_ssl_config config;mbedtls_x509_crt root,peer;mbedtls_entropy_context entropy;mbedtls_ctr_drbg_context rng;
        mbedtls_ssl_init(&ssl);mbedtls_ssl_config_init(&config);mbedtls_x509_crt_init(&root);mbedtls_x509_crt_init(&peer);mbedtls_entropy_init(&entropy);mbedtls_ctr_drbg_init(&rng);
        check(!mbedtls_ctr_drbg_seed(&rng,mbedtls_entropy_func,&entropy,nullptr,0));{int code=mbedtls_x509_crt_parse_der(&root,pico_currency_ca,sizeof pico_currency_ca);if(code){char value[10];const char *digits="0123456789abcdef";for(int i=0;i<8;++i)value[i]=digits[(unsigned(code)>>((7-i)*4))&15];value[8]='\n';value[9]=0;print(value);}check(!code);}
        check(!mbedtls_x509_crt_parse_der(&peer,provider_certificate_0,sizeof provider_certificate_0));check(!mbedtls_x509_crt_parse_der(&peer,provider_certificate_1,sizeof provider_certificate_1));uint32_t flags=0;
        check(!mbedtls_x509_crt_verify(&peer,&root,nullptr,"api.frankfurter.dev",&flags,nullptr,nullptr)&&!flags);
        check(mbedtls_x509_crt_verify(&peer,&root,nullptr,"wrong.invalid",&flags,nullptr,nullptr)!=0&&(flags&MBEDTLS_X509_BADCERT_CN_MISMATCH));
        check(!mbedtls_ssl_config_defaults(&config,MBEDTLS_SSL_IS_CLIENT,MBEDTLS_SSL_TRANSPORT_STREAM,MBEDTLS_SSL_PRESET_DEFAULT));mbedtls_ssl_conf_ca_chain(&config,&root,nullptr);mbedtls_ssl_conf_authmode(&config,MBEDTLS_SSL_VERIFY_REQUIRED);mbedtls_ssl_conf_rng(&config,mbedtls_ctr_drbg_random,&rng);
        check(!mbedtls_ssl_setup(&ssl,&config)&&!mbedtls_ssl_set_hostname(&ssl,"api.frankfurter.dev"));check(pico_tls_peak()<40960);
        mbedtls_ssl_free(&ssl);mbedtls_ssl_config_free(&config);mbedtls_x509_crt_free(&root);mbedtls_x509_crt_free(&peer);mbedtls_ctr_drbg_free(&rng);mbedtls_entropy_free(&entropy);check(!pico_tls_used());
        if(iteration%5==4)print("Five crypto/setup cycles complete\n");
        if(iteration==9)
        {
            void *pressure[128];int n=0;while(n<128&&(pressure[n]=malloc(1024)))++n;check(n>0&&n<128);check(!pico_tls_calloc(1,20000));while(n)free(pressure[--n]);
            check(!pico_tls_used()); // The next ten full crypto/setup cycles must still succeed.
        }
    }
    {char value[10];size_t peak=pico_tls_peak();const char *digits="0123456789abcdef";for(int i=0;i<8;++i)value[i]=digits[(peak>>((7-i)*4))&15];value[8]='\n';value[9]=0;print("Arm TLS peak (hex): ");print(value);}
    free(body);free(session);free(documents);check(!pico_tls_used());print("PASS: Arm newlib 96KiB heap, 8KiB documents + 8KiB network + 2KiB body, repeated real TLS setup/provider-chain+hostname verification/free and exhaustion\n");return 0;
}
