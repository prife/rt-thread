# toolchains options
ARCH='sim'
#CPU='win32'
#CPU='posix'
CPU='posix'
CROSS_TOOL='gcc' #msvc # gcc

# lcd panel options
# 'FMT0371','ILI932X', 'SSD1289'
# RT_USING_LCD_TYPE = 'SSD1289'

# cross_tool provides the cross compiler
# EXEC_PATH is the compiler execute path, for example, CodeSourcery, Keil MDK, IAR
if  CROSS_TOOL == 'gcc':
	PLATFORM 	= 'gcc'
	EXEC_PATH 	= '/usr/bin/gcc'

if  CROSS_TOOL == 'msvc':
	PLATFORM 	= 'cl'
	EXEC_PATH = ''

BUILD = 'debug'
#BUILD = ''

if PLATFORM == 'gcc':
    # toolchains
    PREFIX = ''
    CC = PREFIX + 'gcc'
    AS = PREFIX + 'gcc'
    AR = PREFIX + 'ar'
    LINK = PREFIX + 'gcc'
    TARGET_EXT = 'axf'
    SIZE = PREFIX + 'size'
    OBJDUMP = PREFIX + 'objdump'
    OBJCPY = PREFIX + 'objcopy'

    DEVICE = ' -ffunction-sections -fdata-sections'
    DEVICE = '  '
    CFLAGS = DEVICE + ' -I/usr/include -w -D_REENTRANT'
    AFLAGS = ' -c' + DEVICE + ' -x assembler-with-cpp'
    #LFLAGS = DEVICE + ' -Wl,--gc-sections,-Map=rtthread-linux.map,-cref,-u,Reset_Handler -T stm32_rom.ld'
    #LFLAGS = DEVICE + ' -Wl,--gc-sections,-Map=rtthread-linux.map -lpthread'
    #LFLAGS = DEVICE + ' -Wl,--gc-sections,-Map=rtthread-linux.map -pthread'
    LFLAGS = DEVICE + ' -Wl,-Map=rtthread-linux.map -pthread -T gcc.ld'

    CPATH = ''
    LPATH = ''

    if BUILD == 'debug':
        CFLAGS += ' -g -O0 -gdwarf-2'
        AFLAGS += ' -gdwarf-2'
    else:
        CFLAGS += ' -O2'

    POST_ACTION = ''

elif PLATFORM == 'cl':
    # toolchains
    PREFIX = ''
    TARGET_EXT = 'exe'
    AS = PREFIX + 'cl'
    CC = PREFIX + 'cl'
    AR = PREFIX + 'cl'
    LINK = PREFIX + 'cl'
    AFLAGS = ''
    CFLAGS = ''
    LFLAGS = ''

    if BUILD == 'debug':
        CFLAGS += ' /MTd'
        LFLAGS += ' /DEBUG'
    else:
        CFLAGS += ' /MT'
        LFLAGS += ''

    CFLAGS += ' /ZI /Od /W 3 /WL '
    LFLAGS += ' /SUBSYSTEM:CONSOLE /MACHINE:X86 '

    CPATH = ''
    LPATH = ''

    POST_ACTION = ''
