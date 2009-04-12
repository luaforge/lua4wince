

function log(...)
	local f = io.open('logfile.txt', 'a+')
	local t = {...}
	local s = ''

	f:write(string.format("%s %s\n", os.date("%x %X"), table.foreach(t, 	function(_, o)
																				return ': ' .. tostring(s) .. tostring(o) 
																			end)))
	f:close()
end

print = log

require'lfs'
require'socket'

log(loadstring"function a(b) return 'asd'  end")

-- testing Lua
package.path = '?;./?.lua;/usr/local/share/lua/5.1/?.lua;/usr/local/share/lua/5.1/testsuite/lua/?.lua;;'
assert(lfs.chdir'/usr/local/share/lua/5.1/testsuite/lua')
dofile'all.lua'

