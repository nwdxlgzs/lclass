/*
** $Id: lclass.h $
** Lua的面向对象支持
*/

#ifndef lclass_h
#define lclass_h


#include "lobject.h"

static const char *MetaCustom_banlist[] = {
        "__call", "__index", "__newindex", "__name", "__type", "__gc",
};
typedef enum LClass_Index {
    LCLASS_USERDATA = 1,//这样全程LClass_Index都在array，为了免Table的API
    LCLASS_ISOBJECT,
    LCLASS_METHODS,
    LCLASS_STATIC_METHODS,
    LCLASS_FIELDS,
    LCLASS_STATIC_FIELDS,
    LCLASS_SUPER,
    LCLASS_CHILD,
    LCLASS_OBJCLASS,
    LCLASS_INIT,
    LCLASS_DEL,
    LCLASS_CLASSDEL,
    LCLASS_DEFS,
} LClass_Index;
typedef enum LCLASS_AccessKey {
    LCLASS_public = 1,//这样全程LCLASS_AccessKey都在array，为了免Table的API
    LCLASS_private,
} LCLASS_AccessKey;
#define LCLASS_accessField (LCLASS_public|LCLASS_private)
#define LCLASS_static 4
#define LCLASS_method 8
#define LCLASS_field 16
typedef struct lclass_obj {
    Table *meta;//方便直接拿到元表信息
    lu_byte lockdefine;//锁定后不可继续添加定义
} lclass_obj;
#define LUA_LCLASSLIBNAME "lclass"

LUAMOD_API int luaopen_lclass(lua_State *L);

LUAI_FUNC int luaOC_newPreObject(lua_State *L);

LUAI_FUNC int luaOC_newClassWithSuper(lua_State *L);

LUAI_FUNC int luaOC_newClass(lua_State *L);

LUAI_FUNC int luaOC_setConstructor(lua_State *L);

LUAI_FUNC int luaOC_setDeconstructor(lua_State *L);

LUAI_FUNC int luaOC_setObjectDeconstructor(lua_State *L);

LUAI_FUNC int luaOC_setStaticField(lua_State *L);

LUAI_FUNC int luaOC_setField(lua_State *L);

LUAI_FUNC int luaOC_setStaticMethod(lua_State *L);

LUAI_FUNC int luaOC_setMethod(lua_State *L);

LUAI_FUNC int luaOC_setSuper(lua_State *L);

LUAI_FUNC int luaOC_getSuper(lua_State *L);

LUAI_FUNC int luaOC_getDeclaredFields(lua_State *L);

LUAI_FUNC int luaOC_getDeclaredMethods(lua_State *L);

LUAI_FUNC int luaOC_getFields(lua_State *L);

LUAI_FUNC int luaOC_getMethods(lua_State *L);

LUAI_FUNC int luaOC_getChild(lua_State *L);

LUAI_FUNC int luaOC_lockdefine(lua_State *L);

LUAI_FUNC int luaOC_cast(lua_State *L);

LUAI_FUNC int luaOC_instanceof(lua_State *L);

LUAI_FUNC int luaOC_getClass(lua_State *L);

#endif
