AUDIO_ENABLE = no

ifeq ($(strip $(AUDIO_ENABLE)), yes)
    SRC += muse.c
endif
