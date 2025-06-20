# auser
handles VDV AUS subscription and offers the received updates for data consumers


## Start service
Specify client attributes and to which VDV AUS server you want to subscribe.
```shell
./auser --client_name client --client_ip X.X.X.X --client_port 80 --server_name server --server_addr http://Y.Y.Y.Y:80
```

## Fetch updates
Send GET Request to
```
X.X.X.X:80/auser/fetch?since=23
```
to fetch all updates that arrived since update with ID = 23. Use `since=0` for initial fetch.

Response:
```xml
<?xml version="1.0" encoding="iso-8859-1"?>
<AUSNachricht auser_id="42">
    <IstFahrt> ... </IstFahrt>
    <IstFahrt> ... </IstFahrt>
    <IstFahrt> ... </IstFahrt>
    ...
</AUSNachricht>
```
The `auser_id` attribute of the `AUSNachricht` node is the ID of the most recent update available at the `auser` service and should be used as the value for `since` in the next fetch.