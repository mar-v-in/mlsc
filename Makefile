include $(TOPDIR)/rules.mk

PKG_NAME := mlsc
PKG_RELEASE := 1
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/mlsc
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Mozilla Location Services client
  URL:=https://location.services.mozilla.com/
  DEPENDS:=+libiwinfo +libpolarssl
endef

TARGET_CFLAGS += \
        -I$(STAGING_DIR)/usr/include \
       	-D_GNU_SOURCE

MAKE_FLAGS += \
       	FPIC="$(FPIC)" \
       	CFLAGS="$(TARGET_CFLAGS)" \
        LDFLAGS="$(TARGET_LDFLAGS)" \
        BACKENDS="iwinfo"

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Package/mlsc/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/mlsc $(1)/usr/bin/mlsc
endef


$(eval $(call BuildPackage,mlsc))
