TARGET = GalagaPSP
OBJS = main.o

BUILD_PRX = 1
PSP_FW_VERSION = 500

CFLAGS = -O2 -G0 -Wall
CXXFLAGS = -O2 -G0 -Wall -std=c++11 -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

INCDIR = $(PSPDEV)/psp/include/SDL2

LIBDIR =
LDFLAGS =
LIBS = -lSDL2main -lSDL2 -lGL -lpspvram -lpspaudio -lpspvfpu -lpspgu -lpspge -lpsphprm -lpspdisplay -lpspctrl -lpsppower -lpspdebug -lstdc++ -lm

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = Galaga (Arcade 1981) PSP
PSP_EBOOT_ICON = ICON0.PNG
PSP_EBOOT_PIC1 = PIC1.PNG

PSPSDK = $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

package: EBOOT.PBP
	mkdir -p PSP/GAME/GalagaPSP
	cp EBOOT.PBP PSP/GAME/GalagaPSP/
	zip -r GalagaPSP_PSP.zip PSP/
