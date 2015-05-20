## US Postal Service API Module

_currently in development_

This module is an API interface only and requires additional coding or module to
interact with the data.

### How To Use

1. Find your _USERID_ provided by your registration notice email sent from the
United States Postal Service.
2. Enter your _USERID_ in the settings page: `/admin/config/services/usps_api`.
3. Create custom code to interact with this module api.



### USPS API Services
The USPS api provides three services:

1. Address Standardization Verificaton
2. Zip Code Lookup Tool
3. City/State Lookup Tool

> Note:
> This module currently only provides Address Stanardization Verification
> service.

### USPS Web Tools API Guide:
https://www.usps.com/business/web-tools-apis/address-information-api.htm

- - -

**Developers**

Instantiate a new `UspsApi` object with an array.

The array will be the minimum required address fields to return a response.

Array index `'error'` will be `true` is an error occurs.
