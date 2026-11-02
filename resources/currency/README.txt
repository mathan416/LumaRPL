Port 17 cached currency rates

REFERENCE.rates contains dated Frankfurter reference observations downloaded
when this release was assembled. It is not automatically current after that date.
Each currency has its own observation date. These are reference values, excluding
bank fees and spreads, and not intraday market quotations.

On the calculator: Ctrl+U > Cached currency rates.
Use Refresh over Wi-Fi / HTTPS for an on-device update after Wi-Fi setup. Current UTC is obtained
automatically over NTP; a manual UTC override is in Update options / date. The radio turns off on every return.
Alternatively use Import SD rate table.
Select REFERENCE.rates, choose currencies, then Convert cached rates.
Choose Save current cache to restore your chosen rate table after restarting.

To download a new table with a computer and an internet connection:
  python3 update_currency_rates.py --output LATEST.rates

Copy LATEST.rates into /db48x/currency/ on the SD card. Import it on the
calculator and Save current cache. The updater checks HTTPS certificates using
the computer's trusted CA store, validates every row, and replaces files only
once the complete download has been validated. No API key is required.

The updater is included here and in the matching source archive under tools/.
Source and API documentation: https://frankfurter.dev/

Cache files use port 17's versioned, FNV-1a checksummed format. The checksum
detects accidental corruption; it does not authenticate externally supplied files.

Port 17.3 Update options / date also supports a historical rate date and a saved
clock timezone (Toronto by default). Historical rates never backdate the clock
or TLS. The computer updater accepts --date YYYY-MM-DD. A historical download
replaces the active table; inspect the observation dates shown with conversions.
