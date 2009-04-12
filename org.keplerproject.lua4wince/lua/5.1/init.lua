require'lfs'
function log(...)
	f = io.open('logfile.txt', 'a+')
	f:write(string.format("%s -- %s\n", os.date("%x %X"), table.concat({...}, ' / ')))
	f:close()
end
log'before'
t = lfs.attributes('logfile.txt')
table.foreach(t, log)
log'after'

print = log

package.path = '?;./?.lua;/usr/local/share/lua/5.1/?.lua;/usr/local/share/lua/5.1/tests/?.lua;;'
assert(lfs.chdir'/usr/local/share/lua/5.1/tests')
dofile'all.lua'