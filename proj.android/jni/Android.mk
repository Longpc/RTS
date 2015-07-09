LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

$(call import-add-path,$(LOCAL_PATH)/../../cocos2d)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/external)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/cocos)

LOCAL_MODULE := cocos2dcpp_shared

LOCAL_MODULE_FILENAME := libcocos2dcpp

LOCAL_SRC_FILES := hellocpp/main.cpp \
                   ../../Classes/AppDelegate.cpp \
                   ../../Classes/HelloWorldScene.cpp \
				   ../../Classes/base/LayerBase.cpp \
				   ../../Classes/ModeSelectScene/ModeSelectScene.cpp \
				   ../../Classes/Multi/BatleResultScene/BatleResultScene.cpp \
				   ../../Classes/Multi/BatleScene/BattleScene.cpp \
				   ../../Classes/Multi/SkillSelectScene/SkillDetailDialog.cpp \
				   ../../Classes/Multi/SkillSelectScene/SkillSelectScene.cpp \
				   ../../Classes/Multi/TeamSelectScene/MultiTeamSelectScene.cpp \
				   ../../Classes/Multi/UnitSelectScene/UnitDetailDialog.cpp \
				   ../../Classes/Multi/UnitSelectScene/UnitSelectScene.cpp \
				   ../../Classes/Multi/MemberFullDialogLayer.cpp \
				   ../../Classes/base/MyBodyParser.cpp \
				   ../../Classes/base/font/LabelShow.cpp \
				   ../../Classes/base/dialogBase/DialogBase.cpp \
				   ../../Classes/Multi/BatleScene/OptionDialog.cpp \
				   ../../Classes/Multi/BatleScene/Character.cpp \
				   ../../Classes/base/database/DataUtils.cpp \
				   ../../Classes/base/database/sqlite3.c \
				   ../../Classes/base/database/SqlUtils.cpp \
				   ../../Classes/dataController/SkillData/SkillData.cpp \
				   ../../Classes/dataController/UnitData/UnitData.cpp \
				   ../../Classes/Multi/BatleScene/Effect.cpp \
				   ../../Classes/base/baseButton/ClippingButtonBase.cpp \
				   ../../Classes/Server/Server.cpp \
				   ../../Classes/UserSelectScene/UserSelectScene.cpp \
				   ../../Classes/Model/data/UserModel.cpp \
				   ../../Classes/Model/data/UserUnitModel.cpp \
				   ../../Classes/Model/data/UserSkillModel.cpp \
				   ../../Classes/Server/API/HttpClientbase.cpp \
				   ../../Classes/Server/API/ListUserPHPAPI.cpp \
				   ../../Classes/Server/API/UserLoginPHPAPI.cpp \
				   ../../Classes/Server/API/StartPHPAPI.cpp \
				   ../../Classes/Server/API/BattleAPI.cpp \
				   ../../Classes/Model/data/RoomUserModel.cpp \
				   ../../Classes/Model/data/BattleModel.cpp \
				   ../../Classes/Multi/BatleScene/Tower.cpp \
				   ../../Classes/Multi/BatleScene/Cannon.cpp \
				   ../../Classes/ModeSelectScene/LoadingSprite.cpp \
				   ../../Classes/Multi/BatleScene/Map.cpp \
				   ../../Classes/Multi/BatleScene/SummonPet.cpp

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../Classes

# _COCOS_HEADER_ANDROID_BEGIN
# _COCOS_HEADER_ANDROID_END


LOCAL_STATIC_LIBRARIES := cocos2dx_static

# _COCOS_LIB_ANDROID_BEGIN
# _COCOS_LIB_ANDROID_END

include $(BUILD_SHARED_LIBRARY)

$(call import-module,.)

# _COCOS_LIB_IMPORT_ANDROID_BEGIN
# _COCOS_LIB_IMPORT_ANDROID_END
