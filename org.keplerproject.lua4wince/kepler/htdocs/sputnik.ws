require('sputnik')
return sputnik.new_wsapi_run_fn{
   VERSIUM_PARAMS = { '/usr/local/kepler/wiki-data/' },
   BASE_URL       = '/sputnik.ws',
   PASSWORD_SALT  = 'QsbvM7j7waVmKGSqaJ9r3OWwY5V32E7JW94z7ext',
   TOKEN_SALT     = 'fiGPPo5PY47SJUexP0rHvpghobHlg4Fldb0TpWze',
}
