# j2534-tcp
Virtual J2534 driver over TCP/IP

## Instructions (server)

1. Put `j2534-tcp.dll` at `C:\obdlabs\j2534-tcp.dll` (create directory if needed)
1. Run `j2534-tcp.reg`
1. Configure DTS.Monaco or Xentry PassThru to use `j2534-tcp` device.
1. Get your external IPv4 (not IPv6) from `https://www.whatismyip.com/`
1. Make sure your IP is accessible on TCP port `30000` (check firewall/router port forwarding settings. Use UPnP if needed). Check port is open with `https://www.yougetsignal.com/tools/open-ports/`. If the port isn't open, the client will not be able to connect and it will not work.

## Instructions (client)

1. Connect to remote computer with AnyDesk/similar.
1. Download + extract flasher.
1. Open `flasher.exe`
1. Go to `USB -> TCP Remote Diagnostics Proxy (CAN)` module
1. Pick `Mode: Client`
1. Enter the external IPv4 of the server
1. Leave port at `30000`
1. You will need to figure out request + reply arbitration IDs for the modules you want to work on. You may also need to check the `Diagnostic firewall` box.
1. Click `Connect.
