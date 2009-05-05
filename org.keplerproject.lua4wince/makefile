.PHONY: execute install install-lua install-exe

TARGETNAME=mingw32ce

ifeq "$(TARGETNAME)" "linux"
  CC=gcc-3.4
  INCDIR=
  LIBDIR=
  CFLAGS=-O0 -g3
  LDFLAGS=
else
  CC=/opt/${TARGETNAME}/bin/arm-wince-${TARGETNAME}-gcc
  INCDIR=-Isource/ext -Isource/lua/socket -Isource/lua/lfs -Isource/lua -I/opt/${TARGETNAME}/arm-wince-${TARGETNAME}/include -I/opt/${TARGETNAME}/include 
  LIBDIR=/opt/mingw32ce/lib
  ifeq "$(TARGETNAME)" "mingw32ce"
    CFLAGS=-g -D_PTRDIFF_T -D_WIN32_IE=0x0400 -DNO_STRERROR -DWIN32 -D_WIN32 -DNDEBUG -D_WINDOWS -D__USE_W32_SOCKETS $(INCDIR)
  else
    CFLAGS=-g -DNDEBUG $(INCDIR)
  endif
  LDFLAGS=-fPIC -L$(LIBDIR) 
endif

TARGETDIR=bin/$(TARGETNAME)

EXENAME=lua.exe

TARGETEXE=$(TARGETDIR)/$(EXENAME)

RECEIPTDIR=.sendreceipt

BASE_SRC=$(wildcard source/*.c)
LFS_SRC=$(wildcard source/lfs/*.c)
EXTS_SRC=$(wildcard source/ext/*.c)
XVNT_SRC=$(wildcard source/luasystray/*.c)
RNGS_SRC=$(wildcard source/rings/*.c)
LPEG_SRC=$(wildcard source/lpeg/*.c)
LMD5_SRC=$(wildcard source/md5/*.c)
LB64_SRC=$(wildcard source/lbase64/*.c)
LMOB_SRC=$(wildcard source/luamobile/*.c)

SOCKET_SRC:= \
	source/socket/luasocket.c \
	source/socket/timeout.c \
	source/socket/buffer.c \
	source/socket/io.c \
	source/socket/auxiliar.c \
	source/socket/options.c \
	source/socket/inet.c \
	source/socket/tcp.c \
	source/socket/udp.c \
	source/socket/except.c \
	source/socket/select.c 
ifneq "$(TARGETNAME)" "mingw32ce"
OS_SOCKS:=source/socket/usocket.c
else 
OS_SOCKS:=source/socket/wsocket.c
endif	
MIME_SRC=source/socket/mime.c


LUA_FILES:=$(filter-out CVS% %CVS %CVS/Entries %CVS/Repository %CVS/Root %CVS/Template,$(shell find lua/5.1 -type f))
LUA_DIRS:=$(sort $(foreach FILE, $(LUA_FILES), $(dir $(FILE))))

KEPLER_FILES:=$(filter-out CVS% %CVS %CVS/Entries %CVS/Repository %CVS/Root %CVS/Template,$(shell find kepler -type f))
KEPLER_DIRS:=$(sort $(foreach FILE, $(KEPLER_FILES), $(dir $(FILE))))

SRC=source/main.c $(LFS_SRC) $(MIME_SRC) $(SOCKET_SRC) $(OS_SOCKS) $(LPEG_SRC) $(EXTS_SRC) $(RNGS_SRC) $(LMD5_SRC) $(LB64_SRC) $(XVNT_SRC) $(LMOB_SRC) source/lua.c 
OBJ=$(subst .c,.o,$(SRC))
	
LIBS=-lwininet -lws2 -lm 

define lua_mkdir
synce-pmkdir /usr | echo 
synce-pmkdir /usr/local | echo 
synce-pmkdir /usr/local/share | echo 
synce-pmkdir /usr/local/share/lua | echo 
synce-pmkdir /usr/local/share/lua/5.1 | echo 
endef

define kepler_mkdir
synce-pmkdir /usr/local/kepler | echo 
synce-pmkdir /usr/local/kepler/apps | echo 
synce-pmkdir /usr/local/kepler/etc | echo 
synce-pmkdir /usr/local/kepler/htdocs | echo 
synce-pmkdir /usr/local/kepler/log | echo 
endef

define mobile_mkdir
$(lua_mkdir)
$(kepler_mkdir)
endef


all: $(TARGETEXE)
	echo 'Build successful'

mobile-mkdir:
	$(mobile_mkdir)
	
$(RECEIPTDIR)/lua/%.rpt: lua/%
	echo $<
	synce-pmkdir $(dir /usr/local/share/$<)|echo
	synce-prm /usr/local/share/$<|echo
	synce-pcp $< :/usr/local/share/$<
	mkdir -p $(dir $@)
	touch $@
	
$(RECEIPTDIR)/kepler/%.rpt: kepler/%
	echo $<
	synce-pmkdir $(dir /usr/local/$<)|echo
	synce-prm /usr/local/$<|echo
	synce-pcp $< :/usr/local/$<
	mkdir -p $(dir $@)
	touch $@

$(RECEIPTDIR)/$(TARGETEXE).rpt: $(TARGETEXE) 
	echo $<
	synce-prm $(EXENAME)|echo
	synce-pcp $<
	mkdir -p $(dir $@)
	touch $@
	
install-lua: $(foreach FILE,$(LUA_FILES), $(RECEIPTDIR)/${FILE}.rpt) $(foreach FILE,$(KEPLER_FILES), $(RECEIPTDIR)/${FILE}.rpt)
	echo 'Install lua files successful'
	
install-exe: $(RECEIPTDIR)/$(TARGETEXE).rpt
	echo 'Install executable files successful'
	
source/%.o: source/%.c
	echo $<
	$(CC) $(CFLAGS) -c -o $@ $< 
	
$(TARGETEXE): $(OBJ) 
	mkdir -p $(TARGETDIR)
	$(CC) $(LDFLAGS) -o $@ $(OBJ) $(LIBS)
	
clean:
	rm -Rf $(OBJ)
	rm -Rf bin/*
	rm -Rf $(RECEIPTDIR)/*


install: mobile-mkdir install-lua install-exe 
	echo 'Deploy successful'
	
	
execute:
ifneq "$(TARGETNAME)" "linux"
	synce-prun My\ Documents/$(EXENAME)
	echo 'Run successful'
	epiphany http://169.254.2.1:8080
else
	$(TARGETDIR)/$(EXENAME)
	echo 'Run successful'
endif
		