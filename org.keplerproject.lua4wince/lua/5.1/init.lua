-- Lua for Windows Mobile initializer
package.path = '?;./?.lua;/usr/local/share/lua/5.1/?.lua;/usr/local/share/lua/5.1/?/init.lua;;;'
assert(lfs.chdir'/usr/local/share/lua/5.1')

LOGFILE = '/usr/local/kepler/log/sputnik.log'

--- log function
function log(...)
	local f = io.open(LOGFILE, 'a+')
	local t = {...}
	local s = ''

	f:write(string.format("%s %s\n", os.date("%x %X"), table.foreach(t, 	function(_, o)
																				return ': ' .. tostring(s) .. tostring(o) 
																			end)))
	f:close()
end

oldrequire = require

function require(...)
	log(...)
	return oldrequire(...)
end

print = function(...)
	local f = io.open(LOGFILE, 'a+')
	if f then
		local t = {...}
		for k,v in pairs(t) do
			f:write(tostring(v)) 
			f:write'\t' 
		end
		f:write('\n')
		f:close()
	end
end

require'lfs'
require'socket'
require'lpeg'
require'md5'
require're'
require'base64'

local mobile  = require'luamobile'
local systray = require'luasystray'

--[[ testing Lua
package.path = '?;./?.lua;/usr/local/share/lua/5.1/?.lua;/usr/local/share/lua/5.1/testsuite/lua/?.lua;;'
assert(lfs.chdir'/usr/local/share/lua/5.1/testsuite/lua')
dofile'all.lua'
]]

--[[ testing LuaFileSystem
assert(lfs.chdir'/usr/local/share/lua/5.1/testsuite/lfs')
dofile'test.lua'
]]

--[[
assert(lfs.chdir'/usr/local/share/lua/5.1/testsuite/md5')
dofile'test.lua'
]]

--[[
assert(lfs.chdir'/usr/local/share/lua/5.1/testsuite/lpeg')
dofile'test.lua'
]]

--[[
assert(lfs.chdir'/usr/local/share/lua/5.1/testsuite/lbase64')
dofile'test.lua'
]]

--dofile'xavante_start'

cli = require'sputnik.cli.start-xavante'

cli.execute{webdir='/usr/local/kepler/htdocs'}
