.PATH: ${.CURDIR}
.PATH: ${MAKEOBJDIR}

FRAMEWORK=UIKit
MK_AUTO_OBJ=yes
NO_FMWK_COMMON=yes
.include "../framework.common.mk"

SRCS= \
    UIResponder.m \
    UIView.m \
    UIWindow.m \
    UIApplication.m \
    UIViewController.m \
    UIScreen.m \
    UIDevice.m \
    UIEvent.m \
    UIColor.m \
    UIFont.m \
    UIImage.m \
    UIImageView.m \
    UILabel.m \
    UIButton.m \
    UIGestureRecognizer.m \
    UITableView.m \
    UITableViewCell.m \
    UIStackView.m \
    UIAccessibility.m

INCS= \
    UIKit.h \
    UIKitExport.h \
    UIGeometry.h \
    UIResponder.h \
    UIView.h \
    UIWindow.h \
    UIApplication.h \
    UIViewController.h \
    UIScreen.h \
    UIDevice.h \
    UIEvent.h \
    UIColor.h \
    UIFont.h \
    UIImage.h \
    UIImageView.h \
    UILabel.h \
    UIButton.h \
    UIGestureRecognizer.h \
    UITableView.h \
    UITableViewCell.h \
    UIStackView.h \
    UIAccessibility.h

RESOURCES=Info.plist

CFLAGS+= -I${.CURDIR}/.. -I${BUILDROOT}/usr/include \
	 -I../Foundation/Headers -I${.CURDIR} -I${MAKEOBJDIR} \
	 -I${SRCTOP}/Frameworks/CoreGraphics \
	 -D_BuildingFramework -pthread

LDFLAGS+= ${FMWK_LDFLAGS} \
	  -Wl,-R/System/Library/Frameworks/Foundation.framework/Versions/A \
	  -Wl,-R/System/Library/Frameworks/CoreFoundation.framework/Versions/A \
	  -Wl,-R/System/Library/Frameworks/CoreGraphics.framework/Versions/A \
	  -L${OBJTOP}/Frameworks/Foundation/Foundation.framework \
	  -L${OBJTOP}/Frameworks/CoreFoundation/CoreFoundation.framework \
	  -L${OBJTOP}/Frameworks/CoreGraphics/CoreGraphics.framework \
	  -pthread -lFoundation -lCoreFoundation -lCoreGraphics \
	  -lobjc -lm

make-obj-dirs: .PHONY
	echo ${FRAMEWORK} ${TARGET} in ${MAKEOBJDIR}

fmwkclean:
	rm -rf ${.CURDIR}/Headers

_install-hook:
	cp -fv lib${FRAMEWORK}.so ${FRAMEWORK}.framework/Versions/Current/
	rm -rf ${BUILDROOT}/System/Library/Frameworks/${FRAMEWORK}.framework
	cp -av ${FRAMEWORK}.framework ${BUILDROOT}/System/Library/Frameworks/

clean-deps:
	rm -f .depend.*

.include <rvn.framework.mk>

clean: fmwkclean
post-resources: _install-hook
clean: clean-deps
CFLAGS+= ${FMWK_CFLAGS}
