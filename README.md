# auser

handles VDV AUS subscription and offers the received updates for data consumers

## Start service

Specify client attributes and which VDV AUS server to subscribe to.

```shell
./auser --client_name client --client_ip X.X.X.X --client_port 80 --server_name server --server_addr http://Y.Y.Y.Y:80
```

## Fetch updates

As a downstream data consumer, send GET Request to the `auser` instance

```
X.X.X.X:80/auser/fetch?since=23
```

to fetch all updates that arrived since the update with the `auser_id = 23`. Use `since=0` for initial fetch.

Response:

```
<AUSNachricht auser_id="42">
<IstFahrt>...</IstFahrt>
<IstFahrt>...</IstFahrt>
<IstFahrt>...</IstFahrt>
...
<IstFahrt>...</IstFahrt>
</AUSNachricht>
```

The response contains an attribute `auser_id` in the `AUSNachricht` XML node. This integer encodes the state of the data
consumer after processing the update. The `auser_id` should be used as the value for `since` in the next fetch.

Depending on the fetch frequency updates can get very large. The `body_limit` parameter can be used to define a maximum
size in Bytes for the response body. The default value is 8 MB.