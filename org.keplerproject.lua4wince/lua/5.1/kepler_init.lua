
--
-- Kepler bootstrap file
-- Defines the default directories for Kepler
--

-- Kepler applications directory
KEPLER_APPS = [[/usr/local/kepler/apps]]

-- Kepler configuration directory
KEPLER_CONF = [[/usr/local/kepler/etc]]

-- Kepler logs directory
KEPLER_LOG = [[/usr/local/kepler/log]]

-- Kepler temporary directory
KEPLER_TMP = [[/tmp]]

-- Kepler default web directory
KEPLER_WEB  = [[/usr/local/kepler/htdocs]]

-- CGILua globals (usually defined using KEPLER globals)
CGILUA_APPS = KEPLER_APPS.."/cgilua"
CGILUA_CONF = KEPLER_CONF.."/cgilua"
CGILUA_TMP = KEPLER_TMP
CGILUA_ISDIRECT = true

RINGS_CGILUA_GLOBALS = {
    "KEPLER_APPS", "KEPLER_CONF", "KEPLER_LIB", 
    "KEPLER_LOG", "KEPLER_LUA", "KEPLER_TMP", "KEPLER_WEB",
    "CGILUA_APPS", "CGILUA_CONF", "CGILUA_TMP", "CGILUA_ISDIRECT",
}

