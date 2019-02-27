This is test for 'channel access failure' status during data send via MAC.
Scenario is:
- run disturber at other device. Maybe, can use disturber/, but in our experiments its noise is not enough.
- mlme-reset
- mlme-set (set short address)
- mlme-start (as pan coordinator)
- mcps-data.request
- mcps-data.confirm must return CHANNEL_ACCESS_FAILURE status - check it using trace.
Check for the trace like ">>mcps_data_confirm param 3 handle 0 status 2", where param
may vary.

Entire test created using NWK and lower layers.
There will be some side effects at NWK layer - it should not interest us.