.PHONY: execute install clean-mobile install-exe install-lua

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

TARGETEXE=$(TARGETDIR)/lua.exe

BASE_SRC=$(wildcard source/*.c)
LFS_SRC=$(wildcard source/lfs/*.c)
EXTS_SRC=$(wildcard source/ext/*.c)

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

LUA_FILES:=$(shell find lua/5.1 -type f)

SRC=source/main.c $(LFS_SRC) $(SOCKET_SRC) $(OS_SOCKS) $(EXTS_SRC) source/lua.c
OBJ=$(subst .c,.o,$(SRC))
	
LIBS=-lws2 -lm 

define mobile_mkdir
$(foreach FILE,$(LUA_FILES),\
synce-pmkdir /usr/local/share/$(dir $(FILE))|echo;)
endef

define mobile_cpfiles
$(foreach FILE,$(LUA_FILES),\
synce-pcp $(FILE) :/usr/local/share/$(FILE);)
endef

define mobile_rmfiles
$(foreach FILE,$(LUA_FILES),\
synce-prm /usr/local/share/$(FILE)|echo;)
endef


all: $(TARGETEXE)
	echo 'Build successful'
	
source/%.o: source/%.c
	echo $<
	$(CC) $(CFLAGS) -c -o $@ $< 
	
$(TARGETEXE): $(OBJ) 
	mkdir -p $(TARGETDIR)
	$(CC) $(LDFLAGS) -o $@ $(OBJ) $(LIBS)
	
clean:
	rm -Rf $(OBJ)
	rm -Rf bin/*
	
	


install: clean-mobile install-exe install-lua
ifneq "$(TARGETNAME)" "linux"
	synce-pcp $(TARGETEXE)
	$(mobile_mkdir)
	$(mobile_cpfiles)
	echo 'Deploy successful'

endif
	
clean-mobile:
	synce-prm lua.exe  | echo
	$(mobile_rmfiles)
	
execute:
ifneq "$(TARGETNAME)" "linux"
	synce-prun My\ Documents/lua.exe
	echo 'Run successful'
else
	$(TARGETDIR)/lua.exe
	echo 'Run successful'
endif
		