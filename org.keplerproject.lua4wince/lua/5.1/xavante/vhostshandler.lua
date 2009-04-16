-----------------------------------------------------------------------------
-- Xavante virtual hosts handler
--
-- Authors: Javier Guerra
-- Copyright (c) 2006-2007 Kepler Project
--
-- $Id: vhostshandler.lua,v 1.2 2009-04-27 13:56:36 jasonsantos Exp $
-----------------------------------------------------------------------------

function xavante.vhostshandler (vhosts)
	return function (req, res)
		local h = vhosts [req.headers.host] or vhosts [""]
		return h (req, res)
	end
end
