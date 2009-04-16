-- Lua for Windows Mobile initializer
package.path = '?;./?.lua;/usr/local/share/lua/5.1/?.lua;/usr/local/share/lua/5.1/?/init.lua;;;'
assert(lfs.chdir'/usr/local/share/lua/5.1')

--- log function
function log(...)
	local f = io.open('logfile.txt', 'a+')
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

print = log

require'lfs'
require'socket'

table.foreach(lfs, print)

--[[ testing Lua
package.path = '?;./?.lua;/usr/local/share/lua/5.1/?.lua;/usr/local/share/lua/5.1/testsuite/lua/?.lua;;'
assert(lfs.chdir'/usr/local/share/lua/5.1/testsuite/lua')
dofile'all.lua'
]]

--[[ testing LuaFileSystem
assert(lfs.chdir'/usr/local/share/lua/5.1/testsuite/lfs')
dofile'test.lua'
]]

--[[ testing LuaSocket
assert(lfs.chdir'/usr/local/share/lua/5.1/testsuite/socket')
dofile'testsrvr.lua'
]]

--[[
assert(lfs.chdir'/usr/local/share/lua/5.1/testsuite/copas')
dofile'test.lua'
]]

dofile'xavante_start'