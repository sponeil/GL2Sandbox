# NOTE: Included Android.mk files seem to mess things up when they set this:
# And we have to assume that the "current" directory is actually the parent.
#LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := libpng
LOCAL_SRC_FILES :=\
    libpng/png.c \
    libpng/pngerror.c \
    libpng/pngget.c \
    libpng/pngmem.c \
    libpng/pngpread.c \
    libpng/pngread.c \
    libpng/pngrio.c \
    libpng/pngrtran.c \
    libpng/pngrutil.c \
    libpng/pngset.c \
    libpng/pngtrans.c \
    libpng/pngwio.c \
    libpng/pngwrite.c \
    libpng/pngwtran.c \
    libpng/pngwutil.c 
LOCAL_LDLIBS := -lz
include $(BUILD_STATIC_LIBRARY)

