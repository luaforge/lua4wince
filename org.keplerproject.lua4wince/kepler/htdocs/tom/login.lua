local cgilua = require"cgilua"
local cookies = require"cgilua.cookies"
local auth = require"cgilua.authentication"
require"luasql.mysql"

cgilua.htmlheader()


auth.configure({
	tokenName="authorization";
	tokenPersistence="cookie";
	criptKey="secret";
	checkURL="/index.html";
	
	method="debug";
	
},
{
	mysql={
		check = function(user, passwd)
			
			local env = luasql.mysql()
			
			return false; 
		end
	};

	debug={
		check = function(user, passwd) 
			io.output"log"
			io.write"-------- checked! --------!" return true 
		end
	};
})

local checked, errmsg = auth.check(cgilua.POST.loginEmail, cgilua.POST.loginSenha)

if checked then

	cgilua.print"{ success: true }"    

else

	cgilua.print(string.format("{ success: false, errors: { reason: %q  }}", tostring(errmsg)))   

end




