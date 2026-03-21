# Project Name
TARGET = SynthExpt

USE_DAISYSP_LGPL = 1


# Sources
CPP_SOURCES = SynthMain.cpp Synth.cpp MidiManager.cpp VoiceState.cpp

# Library Locations
LIBDAISY_DIR = ../../libDaisy/
DAISYSP_DIR = ../../DaisySP/

# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile
