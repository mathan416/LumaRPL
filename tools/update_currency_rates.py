#!/usr/bin/env python3
"""Download dated Frankfurter reference rates over verified HTTPS into a Pico cache.
No SD deployment is implicit: --output names the exact file to replace.
"""
import argparse
import datetime
from decimal import Decimal, InvalidOperation
import json
import os
from pathlib import Path
import re
import struct
import urllib.request

MAXIMUM = 32
SIZE = 48 + MAXIMUM * 40 + 4
DEFAULT_QUOTES = 'USD,CAD,GBP,JPY,CHF,AUD,NZD,CNY,INR,MXN,BRL,KRW,SEK,NOK,DKK,PLN,CZK,HUF,TRY,ZAR,SGD,HKD,ILS,AED,SAR,THB,IDR,PHP,MYR'

def checksum(data):
    result = 2166136261
    for byte in data:
        result = ((result ^ byte) * 16777619) & 0xffffffff
    return result

def field(text, size):
    encoded = text.encode('ascii')
    if not encoded or len(encoded) >= size:
        raise ValueError('Field is empty or too long')
    return encoded + bytes(size - len(encoded))

def valid_date(text):
    if not re.fullmatch(r'\d{4}-\d{2}-\d{2}', text):
        raise ValueError('Invalid rate date')
    day = datetime.date.fromisoformat(text)
    if not 1900 <= day.year <= 2199:
        raise ValueError('Rate date outside supported range')
    return text

def valid_rate(text):
    if not re.fullmatch(r'[0-9]+(?:\.[0-9]*)?|\.[0-9]+', text) or len(text) >= 24:
        raise ValueError('Rate must be a bounded positive decimal')
    value = Decimal(text)
    if not Decimal('1e-12') <= value <= Decimal('1e12'):
        raise ValueError('Rate outside supported range')
    return text

def encode(provider, rows):
    if not 0 < len(rows) <= MAXIMUM or not all(32 <= ord(c) <= 126 for c in provider):
        raise ValueError('Invalid rate table')
    data = bytearray(SIZE)
    data[:8] = b'PCFX17\0\0'
    struct.pack_into('<II', data, 8, 1, len(rows))
    data[16:48] = field(provider, 32)
    seen = set()
    for index, (code, date, rate) in enumerate(rows):
        if not re.fullmatch('[A-Z]{3}', code) or code in seen:
            raise ValueError('Invalid or duplicate currency')
        seen.add(code)
        valid_date(date)
        valid_rate(rate)
        if code == 'EUR' and Decimal(rate) != 1:
            raise ValueError('EUR base rate must equal 1')
        at = 48 + index * 40
        data[at:at+4] = field(code, 4)
        data[at+4:at+16] = field(date, 12)
        data[at+16:at+40] = field(rate, 24)
    struct.pack_into('<I', data, SIZE-4, checksum(data[:-4]))
    return bytes(data)

def decode(data):
    if len(data) != SIZE or data[:8] != b'PCFX17\0\0' or struct.unpack_from('<I', data, 8)[0] != 1 or struct.unpack_from('<I', data, SIZE-4)[0] != checksum(data[:-4]):
        raise ValueError('Invalid cache header/checksum')
    count = struct.unpack_from('<I', data, 12)[0]
    if not 0 < count <= MAXIMUM:
        raise ValueError('Invalid cache count')
    def text(start, size):
        raw = data[start:start+size]
        if b'\0' not in raw:
            raise ValueError('Unterminated cache field')
        return raw.split(b'\0', 1)[0].decode('ascii')
    provider = text(16, 32)
    rows = [(text(48+i*40, 4), text(52+i*40, 12), text(64+i*40, 24)) for i in range(count)]
    encode(provider, rows)  # Validate semantic constraints too.
    return provider, rows

def fetch(quotes, date=None):
    quotes = [s.upper() for s in quotes if s.upper() != 'EUR']
    if not quotes or len(quotes) >= MAXIMUM or len(set(quotes)) != len(quotes) or any(not re.fullmatch('[A-Z]{3}', s) for s in quotes):
        raise ValueError('Specify 1–31 distinct three-letter quote currencies')
    url = 'https://api.frankfurter.dev/v2/rates?base=EUR&quotes=' + ','.join(quotes)
    if date:url += '&date=' + valid_date(date)
    request = urllib.request.Request(url, headers={'User-Agent': 'DB48X-PicoCalc-port17/1.0'})
    with urllib.request.urlopen(request, timeout=25) as response:
        if response.geturl().split(':', 1)[0] != 'https':
            raise ValueError('Refusing non-HTTPS response')
        body = response.read(131073)
    if len(body) > 131072:
        raise ValueError('Response exceeds 128 KiB')
    payload = json.loads(body, parse_float=Decimal)
    if not isinstance(payload, list) or len(payload) != len(quotes):
        raise ValueError('Incomplete rate response')
    entries = {}
    for entry in payload:
        if not isinstance(entry, dict) or entry.get('base') != 'EUR' or entry.get('quote') not in quotes or entry['quote'] in entries:
            raise ValueError('Unexpected currency/base in response')
        number = entry.get('rate')
        if isinstance(number, bool) or not isinstance(number, (Decimal, int)):
            raise ValueError('Invalid numerical rate')
        rate = format(Decimal(number), 'f')
        observation=valid_date(entry['date'])
        if date and observation>date:raise ValueError('Provider returned observations newer than requested date')
        entries[entry['quote']] = (entry['quote'], observation, valid_rate(rate))
    # EUR is exact; use the oldest observation date to avoid implying newer data.
    date = min(row[1] for row in entries.values())
    rows = [('EUR', date, '1')] + [entries[code] for code in quotes]
    return encode('Frankfurter (daily reference)', rows)

def replace(output, data):
    decode(data)
    output = Path(output)
    output.parent.mkdir(parents=True, exist_ok=True)
    temporary = output.with_name(output.name + '.tmp')
    backup_temp = output.with_name(output.name + '.bak.tmp')
    try:
        with temporary.open('wb') as stream:
            stream.write(data)
            stream.flush()
            os.fsync(stream.fileno())
        # Do not rotate a corrupt primary over a valid backup.
        if output.exists():
            previous = output.read_bytes()
            try:
                decode(previous)
            except ValueError:
                pass
            else:
                with backup_temp.open('wb') as stream:
                    stream.write(previous)
                    stream.flush()
                    os.fsync(stream.fileno())
                os.replace(backup_temp, output.with_name(output.name + '.bak'))
        os.replace(temporary, output)
    finally:
        temporary.unlink(missing_ok=True)
        backup_temp.unlink(missing_ok=True)

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output', type=Path, required=True, help='Exact .rates output path; never inferred from mounted disks')
    parser.add_argument('--quotes', default=DEFAULT_QUOTES)
    parser.add_argument('--date', help='Optional historical rate date, YYYY-MM-DD')
    parser.add_argument('--csv-fixture', type=Path, help='Also save the provider CSV response for protocol tests')
    args = parser.parse_args()
    try:
        data = fetch(args.quotes.split(','), args.date)
        if args.csv_fixture:
            url='https://api.frankfurter.dev/v2/rates.csv?base=EUR&quotes='+args.quotes
            if args.date:url+='&date='+valid_date(args.date)
            with urllib.request.urlopen(url,timeout=25) as response:
                csv_body=response.read(8193)
                if len(csv_body)>8192:raise ValueError('CSV fixture too large')
            args.csv_fixture.write_bytes(csv_body)
        replace(args.output, data)
        provider, rows = decode(data)
        print(f'{args.output}: {len(rows)} currencies; {provider}; observations {min(r[1] for r in rows)} to {max(r[1] for r in rows)}')
    except (OSError, ValueError, InvalidOperation, KeyError, TypeError) as error:
        parser.exit(1, f'Rates not updated; previous cache retained: {error}\n')

if __name__ == '__main__':
    main()
