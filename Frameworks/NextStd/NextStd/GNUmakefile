include "$(GNUSTEP_MAKEFILES)/common.make"

LIBRARY_NAME = NextStd
NextStd_HEADER_FILES_DIR = $(GNUSTEP_LOCAL_ROOT)/Include/NextStd

#import additional header files
NextStd_HEADER_FILES += $(NextStd_HEADER_FILES_DIR)/NextStd.h \
$(NextStd_HEADER_FILES_DIR)/NSTypes.h \
$(NextStd_HEADER_FILES_DIR)/NSObject.h \
$(NextStd_HEADER_FILES_DIR)/NSString.h \
$(NextStd_HEADER_FILES_DIR)/NSArray.h \
$(NextStd_HEADER_FILES_DIR)/NSEnumerator.h \
$(NextStd_HEADER_FILES_DIR)/NSDictionary.h \
$(NextStd_HEADER_FILES_DIR)/NSData.h \
$(NextStd_HEADER_FILES_DIR)/NSValue.h

#import additional source files
NextStd_SOURCE_FILES += Sources/NSObject.m \
Sources/NSString.m \
Sources/NSArray.m \
Sources/NSEnumerator.m \
Sources/NSData.m \
Sources/NSDictionary.m

include "$(GNUSTEP_MAKEFILES)/library.make"