-----------------------------------------------------------------------------
-- Defines a class for a document-to-table mapper on top of Versium.
--
--
-- (c) 2007, 2008  Yuri Takhteyev (yuri@freewisdom.org)
-- License: MIT/X, see http://sputnik.freewisdom.org/en/License
-----------------------------------------------------------------------------

module(..., package.seeall)

require("saci.node")
local Saci = {}
local Saci_mt = {__metatable={}, __index=Saci}

-----------------------------------------------------------------------------
-- Creates a new instance of Saci.
-- 
-- @param module_name    versium module name to use for storage.
-- @param versium_params parameters to use when creating the storage module
--                       instance.
-- @param root_prototype [optional] the id of the node to be used as the root
--                       prototype (defaults to "@Root").
-- @return               an instance of "Saci".
-----------------------------------------------------------------------------
function new(module_name, versium_params, root_prototype_id)
   local repo = setmetatable({}, Saci_mt)
   repo.root_prototype_id = root_prototype_id or "@Root"
   assert(module_name)
   module_name = module_name
   local versium_module = require(module_name)
   repo.versium = versium_module.new(versium_params)
   repo.permission_groups = {
      all_users     = function(user)   return true end,
      Authenticated = function(user)   return user ~= nil and user:len() > 0 end,
      Anonymous     = function(user)   return not user end,
      all_actions   = function(action) return true end,
   }
   repo:reset_cache()
   return repo
end


function Saci:reset_cache()
   self.cache = {}
   self.cache_stub = {}
end
-----------------------------------------------------------------------------
-- Returns true if the node exists and false otherwise.
-- 
-- @param id             an id of an node.
-- @return               true or false.
-----------------------------------------------------------------------------
function Saci:node_exists(id)
   return self.versium:node_exists(id)
end

--[--------------------------------------------------------------------------
-- Prepares a Lua value for serialization into a stored saci node.  This
-- function will only output boolean, numeric, and number values.
--
-- @param data           The data to be serialized
-- @return               The string representation of the data
-----------------------------------------------------------------------------
local function serialize(data)
	local data_type = type(data)
	if data_type == "boolean" or data_type == "number" then
		return tostring(data)
	elseif data_type ~= "string" then
      return string.format("nil -- Could not serialize '%s'", tostring(data))
	end

	-- if the string contains any newlines, find a version of long quotes that will work
	if data:find("\n") then
		local count = 0
		local open = string.format("[%s[", string.rep("=", count))
		local close = string.format("]%s]", string.rep("=", count))

		while data:find(open, nil, true) or data:find(close, nil, true) do
			open = string.format("[%s[", string.rep("=", count))
			close = string.format("]%s]", string.rep("=", count))
			count = count + 1
		end

		return string.format("%s%s%s", open, data, close)
	else
		return string.format("%q", data)
	end
end

-----------------------------------------------------------------------------
-- Turns a node represented as a Lua table into a string representation which
-- could later be inflated again.
--
-- @param node           A versium node as a table.
-- @return               The string representation of the versium node.
-----------------------------------------------------------------------------
function Saci:deflate(node, fields)
   local buffer = ""
   local keysort = {}

   -- Sort the keys of the node so output is consistent
   for k,v in pairs(node) do
      if k ~= "__index" then
         table.insert(keysort, k)
      end
   end
   table.sort(keysort, function(x, y)
                          if fields and fields[x] and fields[y] then
                             return (fields[x][1] or 1000) < (fields[y][1] or 1000)
                          else
                             return x < y
                          end
                       end)

   for idx,key in ipairs(keysort) do
      local value = serialize(node[key])
      local padded_key = key
      if key:len() < 15 then
         padded_key = (key.."               "):sub(1,15)
      end
      buffer = string.format("%s\n%s= %s", buffer, padded_key, value)
   end

   return buffer
end

-----------------------------------------------------------------------------
-- Retrieves data from Versium and creates a Saci node from it.  If Versium
-- returns nil then Saci will check if it has a method get_fallback_node()
-- (which must be set by the client) and will use it to retrieve a fallback
-- node if it is defined.  If not, it will just return nil.
--
-- @param id             the id of the desired node.
-- @param version        the desired version of the node (defaults to latest).
-- @return               (1) a newly created instance of saci.node.Node,
--                       (2) 'true' if the node returned is a stub (nil
--                           otherwise).
-----------------------------------------------------------------------------
function Saci:get_node(id, version)
   --assert(id)
   --assert(type(id)=="string")

   -- first, check if we have this node in cache
   local cache_key = id
   if version then
      cache_key = cache_key.."."..version
   end
   if self.cache[cache_key] then
      return self.cache[cache_key], self.cache_stub[cache_key]
   end

   -- second, check if we have it in versium
   local data = self.versium:get_node(id, version)
   if data then
      local node = self:make_node(data, id)
      self.cache[cache_key] = node
      return node
   end

   -- third, check with the node's parent
   local parent_id, rest = string.match(id, "^(.+)/(.-)$")
   if parent_id then
      local parent = self:get_node(parent_id)
      if parent then
         local node = parent:get_child(rest)
         if node then
            self.cache[cache_key] = node
            return node
         end
      end
   end

   -- no luck, check if we have a fallback function
   if self.get_fallback_node then
      local prototype = nil
      local node, stub = self:get_fallback_node(id, version)
      self.cache[cache_key] = node
      self.cache_stub[cache_key] = stub
      return node, stub
   end
end

-----------------------------------------------------------------------------
-- Returns revision information for the specified version of the node with a
-- given id, or for the latest version.
--
-- @param id             the id of the desired node
-- @param version        [optional] the id of the revision that we want to
--                       know about (defaults to latest version).
-- @return               a table with revision metadata, just like versium's
--                       get_node_info()
-----------------------------------------------------------------------------
function Saci:get_node_info(id, version)
   return self.versium:get_node_info(id, version)
end

-----------------------------------------------------------------------------
-- Creates a node from a data string.
--
-- @param data           data for the node
-- @param id             the id of the desired node
-- @return               the version tag for the latest version of the node
-----------------------------------------------------------------------------
function Saci:make_node(data, id)
   return saci.node.new{data=data, id=id, repository=self}
end

-----------------------------------------------------------------------------
-- Saves a node.
--
-- @param node           the node to be saved.
-- @param author         the .user name associated with the change (required).
-- @param comment        a comment associated with this change (optional).
-- @param extra          extra params (optional).
-- @return               nothing
-----------------------------------------------------------------------------
function Saci:save_node(node, author, comment, extra, timestamp)
   assert(node.id)
   self.versium:save_version(node.id, self:deflate(node.raw_values, node.fields),
                             author, comment, extra, timestamp)
end

-----------------------------------------------------------------------------
-- Returns the history of edits to the node, optionally filtered by time
-- prefix (e.g., "2007-12") and/or capped at a certain number.
--
-- @param id             the id of the node.
-- @param date_prefix    [optional] a date prefix (e.g., "2007-12").
-- @param limit          [optional] a maxium number of records to return.
-- @return               history as a table.
-----------------------------------------------------------------------------
function Saci:get_node_history(id, date_prefix, limit)
   assert(id)
   return self.versium:get_node_history(id, date_prefix, limit) or {}
end

function Saci:get_complete_history(id_prefix, date_prefix, limit)
   local id_prefix = id_prefix or ""
   if self.versium.get_complete_history then
      return self.versium:get_complete_history(id_prefix, date_prefix, limit)
   end

   local edits = {}
   local preflen = id_prefix:len()
   local ids = self.versium:get_node_ids(id_prefix or nil, limit or nil)
   for i, id in ipairs(ids) do
      if id:sub(1, preflen) == id_prefix then
         for i, edit in ipairs(self:get_node_history(id, date_prefix, limit)) do
            edit.id = id
            table.insert(edits, edit)
         end
      end
   end
   table.sort(edits, function(e1, e2) return e1.timestamp > e2.timestamp end)
   if limit then
      local another_table = {}
      for i=1,limit do
         table.insert(another_table, edits[i])
      end
      edits = another_table
   end   
   return edits
end



-----------------------------------------------------------------------------
-- Retrieves multiple data nodes from Versium and returns Saci nodes created
-- from them.  If Versium returns an empty table, then Saci will also return
-- an empty table.  This function always returns the most recent version of 
-- each node
--
-- @param prefix         the desired node prefix
-- @return               a table containing the returned Saci nodes, indexed
--                       by node name.
-----------------------------------------------------------------------------
function Saci:get_nodes_by_prefix(prefix, limit, visible, id_filter)
   local versium_nodes = self:get_versium_nodes_by_prefix(prefix, limit)
   local nodes = {}
   local ids = {}
   local cache = self.cache
   local num_hidden = 0
   for id, vnode in pairs(versium_nodes) do
      if (not id_filter) or id_filter(id) then
         local node
         if cache[id] then
            node = cache[id]
         else
            node = self:make_node(vnode, id) 
            cache[id] = node
         end
         if visible then
            if node:check_permissions(user, "show") then
               nodes[id] = node
               ids[#ids+1] = id
            else
               num_hidden = num_hidden + 1
            end
         else
            nodes[id] = node
            ids[#ids+1] = id            
         end
      end
   end
   table.sort(ids)
   return nodes, ids, num_hidden
end

-----------------------------------------------------------------------------
-- Retrieves multiple data nodes from Versium, and returns them _without_
-- creating Saci nodes from them.
--
-- @param prefix         the desired node prefix
-- @return               a table containing the returned versium nodes,
--                       indexed by node name.
-----------------------------------------------------------------------------
function Saci:get_versium_nodes_by_prefix(prefix, limit)
   -- Get the nodes, either all at once, of one by one   
   if self.versium.get_nodes_by_prefix then
      return self.versium:get_nodes_by_prefix(prefix, limit)
   else
      local versium_nodes = {}
      for i, id in ipairs(self.versium:get_node_ids(prefix, limit)) do
         versium_nodes[id] = self.versium:get_node(id)
      end
      return versium_nodes
   end
end


-----------------------------------------------------------------------------
-- Returns a list of Saci nodes with fields matching the query.
--
-- @param query          a query as a string, with implicit "or" and with
--                       negatable terms, e.g., "lua git -storage"
-- @param fields         a list of fields to search.
-- @param prefix         a prefix within which to search.
-----------------------------------------------------------------------------
function Saci:find_nodes(fields, patterns, prefix)
   assert(fields)
   assert(patterns)
   if type(fields) == "string" then fields = {fields} end
   if type(patterns) == "string" then patterns = {patterns} end
   local nodes = {}
   -- find nodes matching the patterns
   local basic_match, matched, node, value

   local function basic_match(vnode) -- check if the pattern matches raw node
      for _, pattern in ipairs(patterns) do
         if vnode:lower():match(pattern) then return true end
      end
      return false
   end

   for id, vnode in pairs(self:get_versium_nodes_by_prefix(prefix)) do
      if basic_match(vnode) then -- ok, the pattern is somewhere there, let's look
         node = self:make_node(vnode, id)
         if node:multimatch(fields, patterns) then
            table.insert(nodes, node)
         end
      end
   end

   return nodes
end      

function Saci:query_nodes(fields, query, prefix)   
   query = " "..query.." "
   fields = fields or {"content"}
   local positive_terms = {}
   local negative_terms = {}
   for term in query:gmatch("%S+") do
      if term:match("%:") then
         local key, value = term:match("^(%w+):(.+)")
         if key == "prefix" then
            prefix=value
         end
      elseif term:sub(1,1)=="-" then
         table.insert(negative_terms, "%W("..term:sub(2)..")%W")
      else
         table.insert(positive_terms, "%W("..term..")%W")
      end
   end

   local nodes = self:find_nodes(fields, positive_terms, prefix)
   local nodes_without_negatives = {}
   for i, node in ipairs(nodes) do
      if not node:multimatch(fields, negative_terms) then
         table.insert(nodes_without_negatives, node)
      end
   end
   return nodes_without_negatives
end


-- vim:ts=3 ss=3 sw=3 expandtab
