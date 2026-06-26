ifndef GDK
$(error GDK is not set. Point it to your SGDK directory.)
endif

PROJECT_NAME = sgdk-tetris

include $(GDK)/makefile.gen
