Wi-Fi and time setup (port 17.3)

Configure Wi-Fi through Ctrl+U > Wi-Fi setup. Saved WIFI.cfg credentials remain
readable on SD and are optional. Forget removes credential recovery copies.

Refresh over Wi-Fi / HTTPS automatically obtains current UTC over NTP, then
checks TLS and downloads reference rates. If UDP/123 is blocked, use Cached
currency rates > Update options / date > Clock > Manual current UTC date/time.
NTP is not authenticated; TLS chain, hostname and date checks remain enabled.

Clock timezone defaults to Toronto with automatic DST. Change it through Wi-Fi
setup > Clock timezone or the currency Update options. TIMEZONE.cfg stores this
preference separately, with checksum and recovery copies. Fixed offsets do not
change automatically for DST. Clock time is lost on reboot until another sync.

Historical rates date is separate from current UTC. It never backdates TLS or
the clock. Old rates are retained on network, time-sync or data-validation failure.
The radio turns off on every return. No credentials are shipped in this release.
