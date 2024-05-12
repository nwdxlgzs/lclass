/*
** $Id: lclass.c $
** Lua的面向对象支持
*/

#define lclass_c
#define LUA_CORE

#include "lclass.h"
#include "lprefix.h"
#include "lua.h"
#include "lapi.h"
#include "lauxlib.h"

static lclass_obj *fixcovert_lclass(lua_State *L, int idx) {
    luaL_checktype(L, 1, LUA_TUSERDATA);
    return lua_touserdata(L, idx);
}

static int luaOC_meta__gc(lua_State *L) {
    lclass_obj *obj = fixcovert_lclass(L, 1);
    if (obj == NULL)return luaL_error(L, "not a class or object");
    lua_settop(L, 1);
    lua_getmetatable(L, 1);//2
    lua_rawgeti(L, -1, LCLASS_ISOBJECT);
    int isobj = lua_toboolean(L, -1);
    lua_pop(L, 1);
    lua_rawgeti(L, -1, isobj ? LCLASS_DEL : LCLASS_CLASSDEL);
    if (lua_isfunction(L, -1)) {
        lua_pushvalue(L, 1);
        lua_call(L, 1, 0);
    }
    return 0;
}

static int oc_MFset(lua_State *L) {
    lclass_obj *obj = fixcovert_lclass(L, 1);
    if (obj == NULL)return luaL_error(L, "not a class or object");
    lua_settop(L, 3);
    lua_getmetatable(L, 1);//4
    lua_rawgeti(L, -1, LCLASS_ISOBJECT);
    int isobj = lua_toboolean(L, -1);
    lua_pop(L, 1);
    lua_rawgeti(L, -1, LCLASS_DEFS);
    if (!lua_istable(L, -1)) {
        return luaL_error(L, "not found define table");
    }
    lua_pushvalue(L, 2);
    lua_rawget(L, -2);
    if (lua_isnil(L, -1)) {
        lua_pop(L, 2);
        lua_pushcfunction(L, oc_MFset);
        lua_rawgeti(L, -2, LCLASS_SUPER);
        lua_pushvalue(L, 2);
        lua_pushvalue(L, 3);
        lua_call(L, 3, 0);
        return 0;
    } else {
        int idx = lua_tointeger(L, -1);
        if (!(idx & LCLASS_static) && !isobj) {
            return luaL_error(L, "not found in class(need object)");
        }
        lua_pop(L, 2);
        LClass_Index lidx;
        if (idx & LCLASS_static && idx & LCLASS_method) {
            lidx = LCLASS_STATIC_METHODS;
        } else if (idx & LCLASS_static && idx & LCLASS_field) {
            lidx = LCLASS_STATIC_FIELDS;
        } else if (!(idx & LCLASS_static) && idx & LCLASS_method) {
            lidx = LCLASS_METHODS;
        } else if (!(idx & LCLASS_static) && idx & LCLASS_field) {
            lidx = LCLASS_FIELDS;
        } else {
            return luaL_error(L, "Bad define table, idx %d", idx);
        }
        if (((idx & LCLASS_public) || (idx & LCLASS_private)) &&
            ((idx & LCLASS_accessField) != LCLASS_accessField)) {
            lua_rawgeti(L, -1, lidx);
            if (!lua_istable(L, -1)) {
                return luaL_error(L, "not found define indexed table");
            }
            if (idx & LCLASS_public) {
                lua_rawgeti(L, -1, LCLASS_public);
                if (!lua_istable(L, -1)) {
                    return luaL_error(L, "not found public table");
                }
            } else {
                lua_rawgeti(L, -1, LCLASS_private);
                if (!lua_istable(L, -1)) {
                    return luaL_error(L, "not found private table");
                }
            }
            lua_pushvalue(L, 2);
            lua_pushvalue(L, 3);
            lua_rawset(L, -3);
            return 0;
        } else {
            return luaL_error(L, "Bad define table, idx %d", idx);
        }
    }
}

static int luaOC_meta__newindex(lua_State *L) {
    lclass_obj *obj = fixcovert_lclass(L, 1);
    if (obj == NULL)return luaL_error(L, "not a class or object");
    lua_pushcfunction(L, oc_MFset);
    lua_pushvalue(L, 1);
    lua_pushvalue(L, 2);
    lua_pushvalue(L, 3);
    lua_call(L, 3, 0);
    return 0;
}

static int oc_MFget(lua_State *L) {
    lclass_obj *obj = fixcovert_lclass(L, 1);
    if (obj == NULL)return luaL_error(L, "not a class or object");
    lua_settop(L, 2);
    lua_getmetatable(L, 1);//3
    lua_rawgeti(L, -1, LCLASS_ISOBJECT);
    int isobj = lua_toboolean(L, -1);
    lua_pop(L, 1);
    lua_rawgeti(L, -1, LCLASS_DEFS);
    if (!lua_istable(L, -1)) {
        return luaL_error(L, "not found define table");
    }
    lua_pushvalue(L, 2);
    lua_rawget(L, -2);
    if (lua_isnil(L, -1)) {
        lua_pop(L, 2);
        lua_pushcfunction(L, oc_MFget);
        lua_rawgeti(L, -2, LCLASS_SUPER);
        lua_pushvalue(L, 2);
        lua_call(L, 2, 1);
        return 1;
    } else {
        int idx = lua_tointeger(L, -1);
        if (!(idx & LCLASS_static) && !isobj) {
            return luaL_error(L, "not found in class(need object)");
        }
        lua_pop(L, 2);
        LClass_Index lidx;
        if (idx & LCLASS_static && idx & LCLASS_method) {
            lidx = LCLASS_STATIC_METHODS;
        } else if (idx & LCLASS_static && idx & LCLASS_field) {
            lidx = LCLASS_STATIC_FIELDS;
        } else if (!(idx & LCLASS_static) && idx & LCLASS_method) {
            lidx = LCLASS_METHODS;
        } else if (!(idx & LCLASS_static) && idx & LCLASS_field) {
            lidx = LCLASS_FIELDS;
        } else {
            return luaL_error(L, "Bad define table, idx %d", idx);
        }
        if (((idx & LCLASS_public) || (idx & LCLASS_private)) &&
            ((idx & LCLASS_accessField) != LCLASS_accessField)) {
            lua_rawgeti(L, -1, lidx);
            if (!lua_istable(L, -1)) {
                return luaL_error(L, "not found define indexed table");
            }
            if (idx & LCLASS_public) {
                lua_rawgeti(L, -1, LCLASS_public);
                if (!lua_istable(L, -1)) {
                    return luaL_error(L, "not found public table");
                }
            } else {
                lua_rawgeti(L, -1, LCLASS_private);
                if (!lua_istable(L, -1)) {
                    return luaL_error(L, "not found private table");
                }
            }
            lua_pushvalue(L, 2);
            lua_rawget(L, -2);
            return 1;
        } else {
            return luaL_error(L, "Bad define table, idx %d", idx);
        }
    }
}

static int luaOC_meta__index(lua_State *L) {
    lclass_obj *obj = fixcovert_lclass(L, 1);
    if (obj == NULL)return luaL_error(L, "not a class or object");
    lua_pushcfunction(L, oc_MFget);
    lua_pushvalue(L, 1);
    lua_pushvalue(L, 2);
    lua_call(L, 2, 1);
    return 1;
}

#define initMF(L, K){ \
    lua_newtable(L);\
    lua_newtable(L);\
    lua_rawseti(L, -2, LCLASS_private);\
    lua_newtable(L);\
    lua_rawseti(L, -2, LCLASS_public);\
    lua_rawseti(L, -2, K);\
}

static int retobjecttype(lua_State *L) {
    lua_pushliteral(L, "luaobject");
    return 1;
}

/*
 * 参数：类
 */
int luaOC_newPreObject(lua_State *L) {
    lclass_obj *class = fixcovert_lclass(L, 1);
    if (class == NULL)return luaL_error(L, "not a class or object");
    lua_settop(L, 1);
    lua_getmetatable(L, 1);//2
    lclass_obj *obj = lua_newuserdata(L, sizeof(lclass_obj));
    lua_newtable(L);
    obj->meta = hvalue(s2v(L->top.p - 1));
    obj->lockdefine = 0;
    lua_pushvalue(L, 1), lua_rawseti(L, -2, LCLASS_OBJCLASS);
    lua_pushvalue(L, -2), lua_rawseti(L, -2, LCLASS_USERDATA);
    lua_pushboolean(L, 1), lua_rawseti(L, -2, LCLASS_ISOBJECT);
    lua_rawgeti(L, 2, LCLASS_METHODS), lua_rawseti(L, -2, LCLASS_METHODS);
    lua_rawgeti(L, 2, LCLASS_STATIC_METHODS), lua_rawseti(L, -2, LCLASS_STATIC_METHODS);
    initMF(L, LCLASS_FIELDS);
    lua_rawgeti(L, 2, LCLASS_STATIC_FIELDS), lua_rawseti(L, -2, LCLASS_STATIC_FIELDS);
    lua_rawgeti(L, 2, LCLASS_DEFS), lua_rawseti(L, -2, LCLASS_DEFS);
    lua_rawgeti(L, 2, LCLASS_DEL), lua_rawseti(L, -2, LCLASS_DEL);
    lua_pushcfunction(L, luaOC_meta__gc), lua_setfield(L, -2, "__gc");
    lua_pushcfunction(L, luaOC_meta__index), lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, luaOC_meta__newindex), lua_setfield(L, -2, "__newindex");
    lua_getfield(L, 2, "__name"), lua_setfield(L, -2, "__name");
    lua_pushcfunction(L, retobjecttype), lua_setfield(L, -2, "__type");
    lua_setmetatable(L, -2);
    return 1;
}

static int luaOC_meta__call(lua_State *L) {
    int rawtop = lua_gettop(L);
    int argc = rawtop - 1;
    lclass_obj *obj = fixcovert_lclass(L, 1);
    if (obj == NULL)return luaL_error(L, "not a class or object");
    lua_getmetatable(L, 1);//rawtop+1
    lua_rawgeti(L, -1, LCLASS_ISOBJECT);
    if (lua_toboolean(L, -1)) {
        return luaL_error(L, "object can't use constructor");
    }
    lua_pop(L, 1);
    lua_rawgeti(L, -1, LCLASS_SUPER);
    if (lua_isnil(L, -1)) {//直接构建
        lua_pop(L, 1);
        lua_pushcfunction(L, luaOC_newPreObject);
        lua_pushvalue(L, 1);
        lua_call(L, 1, 1);
        lua_rawgeti(L, rawtop + 1, LCLASS_INIT);
        if (!lua_isfunction(L, -1)) {
            lua_pop(L, 1);
            return 1;
        } else {
            lua_pushvalue(L, -2);//self
            for (int a = 0; a < argc; a++) {
                lua_pushvalue(L, a + 2);
            }
            lua_call(L, argc + 1, 0);
            return 1;
        }
    } else {
        for (int a = 0; a < argc; a++) {
            lua_pushvalue(L, a + 2);
        }
        lua_call(L, argc, 1);
        int supertop = lua_gettop(L);
        lua_pushcfunction(L, luaOC_newPreObject);
        lua_pushvalue(L, 1);
        lua_call(L, 1, 1);
        lua_rawgeti(L, rawtop + 1, LCLASS_INIT);
        if (!lua_isfunction(L, -1)) {
            lua_pop(L, 1);
            lua_getmetatable(L, -1);
            lua_pushvalue(L, supertop);
            lua_rawseti(L, -2, LCLASS_SUPER);
            lua_pop(L, 1);
            lua_getmetatable(L, supertop);
            lua_pushvalue(L, -2);
            lua_rawseti(L, -2, LCLASS_CHILD);
            lua_pop(L, 1);
            return 1;
        } else {
            lua_getmetatable(L, -2);
            lua_pushvalue(L, supertop);
            lua_rawseti(L, -2, LCLASS_SUPER);
            lua_pop(L, 1);
            lua_getmetatable(L, supertop);
            lua_pushvalue(L, -2);
            lua_rawseti(L, -2, LCLASS_CHILD);
            lua_pop(L, 1);
            lua_pushvalue(L, -2);//obj
            for (int a = 0; a < argc; a++) {
                lua_pushvalue(L, a + 2);
            }
            lua_call(L, argc + 1, 0);
            return 1;
        }
    }
}

static int retclasstype(lua_State *L) {
    lua_pushliteral(L, "luaclass");
    return 1;
}

/*
 * 参数：类名
 */
int luaOC_newClass(lua_State *L) {
    luaL_checktype(L, 1, LUA_TSTRING);//classname
    lua_settop(L, 1);
    lclass_obj *class = lua_newuserdata(L, sizeof(lclass_obj));
    lua_newtable(L);
    class->meta = hvalue(s2v(L->top.p - 1));
    class->lockdefine = 0;
    lua_pushvalue(L, -2), lua_rawseti(L, -2, LCLASS_USERDATA);
    lua_pushboolean(L, 0), lua_rawseti(L, -2, LCLASS_ISOBJECT);
    initMF(L, LCLASS_METHODS);
    initMF(L, LCLASS_STATIC_METHODS);
    initMF(L, LCLASS_FIELDS);
    initMF(L, LCLASS_STATIC_FIELDS);
    lua_newtable(L);
    lua_newtable(L);
    lua_pushliteral(L, "kv");
    lua_setfield(L, -2, "__mode");
    lua_setmetatable(L, -2);
    lua_rawseti(L, -2, LCLASS_DEFS);
    lua_pushcfunction(L, luaOC_meta__gc), lua_setfield(L, -2, "__gc");
    lua_pushcfunction(L, luaOC_meta__index), lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, luaOC_meta__newindex), lua_setfield(L, -2, "__newindex");
    lua_pushcfunction(L, luaOC_meta__call), lua_setfield(L, -2, "__call");
    lua_pushvalue(L, 1), lua_setfield(L, -2, "__name");
    lua_pushcfunction(L, retclasstype), lua_setfield(L, -2, "__type");
    lua_setmetatable(L, -2);
    return 1;
}

/*
 * 参数：类 构建函数
 */
int luaOC_setConstructor(lua_State *L) {
    lclass_obj *obj = fixcovert_lclass(L, 1);
    if (obj == NULL)return luaL_error(L, "not a class or object");
    if (obj->lockdefine)return luaL_error(L, "locked class/object can't set constructor");
    luaL_checktype(L, 2, LUA_TFUNCTION);
    lua_getmetatable(L, 1);
    lua_rawgeti(L, -1, LCLASS_ISOBJECT);
    if (lua_toboolean(L, -1)) {
        lua_getfield(L, -2, "__name");
        return luaL_error(L, "constructor never in object<%s>", luaL_optstring(L, -1, "unknown"));
    }
    lua_pop(L, 1);
    lua_rawgeti(L, -1, LCLASS_INIT);
    if (lua_isnil(L, -1)) {
        lua_pushvalue(L, 2), lua_rawseti(L, -3, LCLASS_INIT);
    } else {
        lua_getfield(L, -2, "__name");
        return luaL_error(L, "This class<%s> %s already exists constructor(__init__)",
                          luaL_optstring(L, -1, "unknown"));
    }
    return 0;
}

/*
 * 参数：类/对象 析构函数
 */
int luaOC_setDeconstructor(lua_State *L) {
    lclass_obj *obj = fixcovert_lclass(L, 1);
    if (obj == NULL)return luaL_error(L, "not a class or object");
    if (obj->lockdefine)return luaL_error(L, "locked class/object can't set deconstructor");
    luaL_checktype(L, 2, LUA_TFUNCTION);
    lua_settop(L, 2);
    lua_getmetatable(L, 1);
    lua_rawgeti(L, -1, LCLASS_ISOBJECT);
    int isobj = lua_toboolean(L, -1);
    lua_pop(L, 1);
    lua_rawgeti(L, -1, isobj ? LCLASS_DEL : LCLASS_CLASSDEL);
    if (lua_isnil(L, -1)) {
        lua_pushvalue(L, 2), lua_rawseti(L, -3, isobj ? LCLASS_DEL : LCLASS_CLASSDEL);
    } else {
        lua_getfield(L, -2, "__name");
        return luaL_error(L, "This %s<%s> %s already exists deconstructor(%s)",
                          isobj ? "object" : "class",
                          luaL_optstring(L, -1, "unknown"),
                          isobj ? "__del__" : "__finalize__");
    }
    return 0;
}

/*
 * 参数：类/对象 析构函数
 */
int luaOC_setObjectDeconstructor(lua_State *L) {
    lclass_obj *obj = fixcovert_lclass(L, 1);
    if (obj == NULL)return luaL_error(L, "not a class or object");
    if (obj->lockdefine)return luaL_error(L, "locked object can't set objectdeconstructor");
    luaL_checktype(L, 2, LUA_TFUNCTION);
    lua_settop(L, 2);
    lua_getmetatable(L, 1);
    lua_rawgeti(L, -1, LCLASS_DEL);
    if (lua_isnil(L, -1)) {
        lua_pushvalue(L, 2), lua_rawseti(L, -3, LCLASS_DEL);
    } else {
        lua_rawgeti(L, -2, LCLASS_ISOBJECT);
        int isobj = lua_toboolean(L, -1);
        lua_getfield(L, -3, "__name");
        return luaL_error(L, "This %s<%s> %s already exists deconstructor(%s)",
                          isobj ? "object" : "class",
                          luaL_optstring(L, -1, "unknown"),
                          isobj ? "__del__" : "__finalize__");
    }
    return 0;
}

/*
 * 参数：类/对象 访问修饰键 键 值
 */
int luaOC_setStaticField(lua_State *L) {
    lclass_obj *obj = fixcovert_lclass(L, 1);
    if (obj == NULL)return luaL_error(L, "not a class or object");
    if (obj->lockdefine)return luaL_error(L, "locked class/object can't set static field");
    LCLASS_AccessKey accessK = luaL_checkinteger(L, 2);
    lua_settop(L, 4);
    lua_getmetatable(L, 1);//5
    lua_rawgeti(L, -1, LCLASS_DEFS);
    if (!lua_istable(L, -1)) {
        return luaL_error(L, "defines table is not found");
    }
    lua_pushvalue(L, 3);
    lua_rawget(L, -2);
    if (!lua_isnil(L, -1)) {
        return luaL_error(L, "%s already exists", lua_tostring(L, 3));
    }
    lua_pop(L, 2);
    lua_rawgeti(L, -1, LCLASS_STATIC_FIELDS);
    if (!lua_istable(L, -1)) {
        return luaL_error(L, "bad static fields pool");
    }
    lua_rawgeti(L, -1, accessK);
    if (!lua_istable(L, -1)) {
        return luaL_error(L, "bad access key:%d", accessK);
    }
    lua_pushvalue(L, 3);
    lua_pushvalue(L, 4);
    lua_rawset(L, -3);
    lua_settop(L, 5);
    lua_rawgeti(L, -1, LCLASS_DEFS);
    lua_pushvalue(L, 3);
    lua_pushinteger(L, LCLASS_static | LCLASS_field | accessK);
    lua_rawset(L, -3);
    return 0;
}

/*
 * 参数：类/对象 访问修饰键 键 值
 */
int luaOC_setField(lua_State *L) {
    lclass_obj *obj = fixcovert_lclass(L, 1);
    if (obj == NULL)return luaL_error(L, "not a class or object");
    if (obj->lockdefine)return luaL_error(L, "locked class/object can't set field");
    LCLASS_AccessKey accessK = luaL_checkinteger(L, 2);
    lua_settop(L, 4);
    lua_getmetatable(L, 1);//5
    lua_rawgeti(L, -1, LCLASS_DEFS);
    if (!lua_istable(L, -1)) {
        return luaL_error(L, "defines table is not found");
    }
    lua_pushvalue(L, 3);
    lua_rawget(L, -2);
    if (!lua_isnil(L, -1)) {
        return luaL_error(L, "%s already exists", lua_tostring(L, 3));
    }
    lua_pop(L, 2);
    lua_rawgeti(L, -1, LCLASS_FIELDS);
    if (!lua_istable(L, -1)) {
        return luaL_error(L, "bad fields pool");
    }
    lua_rawgeti(L, -1, accessK);
    if (!lua_istable(L, -1)) {
        return luaL_error(L, "bad access key:%d", accessK);
    }
    lua_pushvalue(L, 3);
    lua_pushvalue(L, 4);
    lua_rawset(L, -3);
    lua_settop(L, 5);
    lua_rawgeti(L, -1, LCLASS_DEFS);
    lua_pushvalue(L, 3);
    lua_pushinteger(L, LCLASS_field | accessK);
    lua_rawset(L, -3);
    return 0;
}

/*
 * 参数：类/对象 访问修饰键 键 函数
 */
int luaOC_setStaticMethod(lua_State *L) {
    lclass_obj *obj = fixcovert_lclass(L, 1);
    if (obj == NULL)return luaL_error(L, "not a class or object");
    if (obj->lockdefine)return luaL_error(L, "locked class/object can't set static method");
    LCLASS_AccessKey accessK = luaL_checkinteger(L, 2);
    luaL_checktype(L, 4, LUA_TFUNCTION);
    lua_settop(L, 4);
    lua_getmetatable(L, 1);//5
    lua_rawgeti(L, -1, LCLASS_DEFS);
    if (!lua_istable(L, -1)) {
        return luaL_error(L, "defines table is not found");
    }
    lua_pushvalue(L, 3);
    lua_rawget(L, -2);
    if (!lua_isnil(L, -1)) {
        return luaL_error(L, "%s already exists", lua_tostring(L, 3));
    }
    lua_pop(L, 2);
    lua_rawgeti(L, -1, LCLASS_STATIC_METHODS);
    if (!lua_istable(L, -1)) {
        return luaL_error(L, "bad static methods pool");
    }
    lua_rawgeti(L, -1, accessK);
    if (!lua_istable(L, -1)) {
        return luaL_error(L, "bad access key:%d", accessK);
    }
    lua_pushvalue(L, 3);
    lua_pushvalue(L, 4);
    lua_rawset(L, -3);
    lua_settop(L, 5);
    lua_rawgeti(L, -1, LCLASS_DEFS);
    lua_pushvalue(L, 3);
    lua_pushinteger(L, LCLASS_static | LCLASS_method | accessK);
    lua_rawset(L, -3);
    return 0;
}

/*
 * 参数：类/对象 访问修饰键 键 函数
 */
int luaOC_setMethod(lua_State *L) {
    lclass_obj *obj = fixcovert_lclass(L, 1);
    if (obj == NULL)return luaL_error(L, "not a class or object");
    if (obj->lockdefine)return luaL_error(L, "locked class/object can't set method");
    LCLASS_AccessKey accessK = luaL_checkinteger(L, 2);
    luaL_checktype(L, 4, LUA_TFUNCTION);
    lua_settop(L, 4);
    lua_getmetatable(L, 1);//5
    lua_rawgeti(L, -1, LCLASS_DEFS);
    if (!lua_istable(L, -1)) {
        return luaL_error(L, "defines table is not found");
    }
    lua_pushvalue(L, 3);
    lua_rawget(L, -2);
    if (!lua_isnil(L, -1)) {
        return luaL_error(L, "%s already exists", lua_tostring(L, 3));
    }
    lua_pop(L, 2);
    lua_rawgeti(L, -1, LCLASS_METHODS);
    if (!lua_istable(L, -1)) {
        return luaL_error(L, "bad methods pool");
    }
    lua_rawgeti(L, -1, accessK);
    if (!lua_istable(L, -1)) {
        return luaL_error(L, "bad access key:%d", accessK);
    }
    lua_pushvalue(L, 3);
    lua_pushvalue(L, 4);
    lua_rawset(L, -3);
    lua_settop(L, 5);
    lua_rawgeti(L, -1, LCLASS_DEFS);
    lua_pushvalue(L, 3);
    lua_pushinteger(L, LCLASS_method | accessK);
    lua_rawset(L, -3);
    return 0;
}

/*
 * 参数：类/对象 super类/super对象
 */
int luaOC_setSuper(lua_State *L) {
    lclass_obj *obj = fixcovert_lclass(L, 1);
    if (obj == NULL)return luaL_error(L, "not a class or object");
    if (obj->lockdefine)return luaL_error(L, "locked class/object can't set super");
    obj = fixcovert_lclass(L, 2);
    if (obj == NULL)return luaL_error(L, "not a class or object");
    lua_settop(L, 2);
    lua_getmetatable(L, 1);//3
    lua_rawgeti(L, 3, LCLASS_SUPER);
    if (!lua_isnil(L, -1)) {
        return luaL_error(L, "super already exist");
    }
    lua_pop(L, 1);
    lua_rawgeti(L, -1, LCLASS_ISOBJECT);
    int setoc_isobj = lua_toboolean(L, -1);
    lua_pop(L, 1);
    lua_getmetatable(L, 2);//4
    lua_rawgeti(L, -1, LCLASS_ISOBJECT);
    int superoc_isobj = lua_toboolean(L, -1);
    lua_pop(L, 1);
    if (setoc_isobj) {
        lua_pushvalue(L, 2);
        lua_rawseti(L, 3, LCLASS_SUPER);
    } else {
        if (superoc_isobj) {//出错
            lua_settop(L, 2);
            lua_getmetatable(L, 1);//3
            lua_getmetatable(L, 2);//4
            lua_getfield(L, 3, "__name");
            lua_getfield(L, 4, "__name");
            return luaL_error(L, "class<%s> never extends object<%s>",
                              luaL_optstring(L, 5, "unknown"), luaL_optstring(L, 6, "unknown"));
        } else {//类定义的继承
            lua_pushvalue(L, 2);
            lua_rawseti(L, 3, LCLASS_SUPER);
        }
    }
    return 0;
}


/*
 * 参数：类/对象
 */
int luaOC_lockdefine(lua_State *L) {
    lclass_obj *obj = fixcovert_lclass(L, 1);
    if (obj == NULL)return luaL_error(L, "not a class or object");
    obj->lockdefine = 1;
    return 0;
}

/*
 * 参数：类/对象
 */
int luaOC_getSuper(lua_State *L) {
    lclass_obj *obj = fixcovert_lclass(L, 1);
    if (obj == NULL)return luaL_error(L, "not a class or object");
    lua_settop(L, 1);
    lua_getmetatable(L, 1);//2
    lua_rawgeti(L, -1, LCLASS_SUPER);
    return 1;
}

/*
 * 参数：类/对象
 */
int luaOC_getMethods(lua_State *L) {
    lclass_obj *obj = fixcovert_lclass(L, 1);
    if (obj == NULL)return luaL_error(L, "not a class or object");
    lua_settop(L, 1);
    lua_newtable(L);//2
    lua_getmetatable(L, 1);//3
    LClass_Index idxs[] = {LCLASS_METHODS, LCLASS_STATIC_METHODS};
    for (int i = 0; i < sizeof(idxs) / sizeof(idxs[0]); i++) {
        lua_rawgeti(L, -1, idxs[i]);
        if (lua_istable(L, -1)) {
            lua_rawgeti(L, -1, LCLASS_public);
            if (lua_istable(L, -1)) {
                lua_pushnil(L);
                while (lua_next(L, -2) != 0) {
                    lua_pushvalue(L, -2);
                    lua_pushvalue(L, -2);
                    lua_rawset(L, 2);
                    lua_pop(L, 1);
                }
                lua_pop(L, 2);
            } else
                lua_pop(L, 1);
            lua_pop(L, 1);
        } else
            lua_pop(L, 1);
    }
    lua_pushvalue(L, 2);
    return 1;
}

/*
 * 参数：类/对象
 */
int luaOC_getFields(lua_State *L) {
    lclass_obj *obj = fixcovert_lclass(L, 1);
    if (obj == NULL)return luaL_error(L, "not a class or object");
    lua_settop(L, 1);
    lua_newtable(L);//2
    lua_getmetatable(L, 1);//3
    LClass_Index idxs[] = {LCLASS_FIELDS, LCLASS_STATIC_FIELDS};
    for (int i = 0; i < sizeof(idxs) / sizeof(idxs[0]); i++) {
        lua_rawgeti(L, -1, idxs[i]);
        if (lua_istable(L, -1)) {
            lua_rawgeti(L, -1, LCLASS_public);
            if (lua_istable(L, -1)) {
                lua_pushnil(L);
                while (lua_next(L, -2) != 0) {
                    lua_pushvalue(L, -2);
                    lua_pushvalue(L, -2);
                    lua_rawset(L, 2);
                    lua_pop(L, 1);
                }
                lua_pop(L, 2);
            } else
                lua_pop(L, 1);
            lua_pop(L, 1);
        } else
            lua_pop(L, 1);
    }
    lua_pushvalue(L, 2);
    return 1;
}

/*
 * 参数：类/对象
 */
int luaOC_getDeclaredMethods(lua_State *L) {
    lclass_obj *obj = fixcovert_lclass(L, 1);
    if (obj == NULL)return luaL_error(L, "not a class or object");
    lua_settop(L, 1);
    lua_newtable(L);//2
    lua_getmetatable(L, 1);//3
    LClass_Index idxs[] = {LCLASS_METHODS, LCLASS_STATIC_METHODS};
    LCLASS_AccessKey acks[] = {LCLASS_public, LCLASS_private};
    int ctop = lua_gettop(L);
    for (int i = 0; i < sizeof(idxs) / sizeof(idxs[0]); i++) {
        lua_settop(L, ctop);
        lua_rawgeti(L, -1, idxs[i]);
        if (lua_istable(L, -1)) {
            int ttop = lua_gettop(L);
            for (int j = 0; j < sizeof(acks) / sizeof(acks[0]); j++) {
                lua_settop(L, ttop);
                lua_rawgeti(L, -1, acks[j]);
                if (lua_istable(L, -1)) {
                    lua_pushnil(L);
                    while (lua_next(L, -2) != 0) {
                        lua_pushvalue(L, -2);
                        lua_pushvalue(L, -2);
                        lua_rawset(L, 2);
                        lua_pop(L, 1);
                    }
                    lua_pop(L, 2);
                } else
                    lua_pop(L, 1);
            }
            lua_pop(L, 1);
        } else
            lua_pop(L, 1);
    }
    lua_pushvalue(L, 2);
    return 1;
}

/*
 * 参数：类/对象
 */
int luaOC_getDeclaredFields(lua_State *L) {
    lclass_obj *obj = fixcovert_lclass(L, 1);
    if (obj == NULL)return luaL_error(L, "not a class or object");
    lua_settop(L, 1);
    lua_newtable(L);//2
    lua_getmetatable(L, 1);//3
    LClass_Index idxs[] = {LCLASS_FIELDS, LCLASS_STATIC_FIELDS};
    LCLASS_AccessKey acks[] = {LCLASS_public, LCLASS_private};
    int ctop = lua_gettop(L);
    for (int i = 0; i < sizeof(idxs) / sizeof(idxs[0]); i++) {
        lua_settop(L, ctop);
        lua_rawgeti(L, -1, idxs[i]);
        if (lua_istable(L, -1)) {
            int ttop = lua_gettop(L);
            for (int j = 0; j < sizeof(acks) / sizeof(acks[0]); j++) {
                lua_settop(L, ttop);
                lua_rawgeti(L, -1, acks[j]);
                if (lua_istable(L, -1)) {
                    lua_pushnil(L);
                    while (lua_next(L, -2) != 0) {
                        lua_pushvalue(L, -2);
                        lua_pushvalue(L, -2);
                        lua_rawset(L, 2);
                        lua_pop(L, 1);
                    }
                    lua_pop(L, 2);
                } else
                    lua_pop(L, 1);
            }
            lua_pop(L, 1);
        } else
            lua_pop(L, 1);
    }
    lua_pushvalue(L, 2);
    return 1;
}

/*
 * 参数：类/对象
 */
int luaOC_getChild(lua_State *L) {
    lclass_obj *obj = fixcovert_lclass(L, 1);
    if (obj == NULL)return luaL_error(L, "not a class or object");
    lua_settop(L, 1);
    lua_getmetatable(L, 1);//2
    lua_rawgeti(L, -1, LCLASS_CHILD);
    return 1;
}

/*
 * 参数：类/对象 类/对象
 */
int luaOC_cast(lua_State *L) {
    lclass_obj *obj = fixcovert_lclass(L, 1);
    if (obj == NULL)return luaL_error(L, "not a class or object");
    obj = fixcovert_lclass(L, 2);//此时obj为目标类的USERDATA，比较它
    if (obj == NULL)return luaL_error(L, "not a class or object");
    lua_settop(L, 2);
    lua_getmetatable(L, 1);//3
    lua_getmetatable(L, 2);//4
    lua_rawgeti(L, 3, LCLASS_ISOBJECT);
    int waitcast_isobj = lua_toboolean(L, -1);
    lua_pop(L, 1);
    lua_rawgeti(L, 4, LCLASS_ISOBJECT);
    int castto_isobj = lua_toboolean(L, -1);
    lua_pop(L, 1);
    if (waitcast_isobj && !castto_isobj) {
        //先看自己是不是就是
        lua_settop(L, 3);//只保留当前需要cast的对象元表
        lua_rawgeti(L, -1, LCLASS_OBJCLASS);
        if (lua_touserdata(L, -1) == obj) {//找到了
            lua_settop(L, 1);
            return 1;
        }
        lua_settop(L, 3);//只保留当前需要cast的对象元表
        lua_pushvalue(L, -1);//4
        for (;;) {//super查找方向
            lua_settop(L, 4);
            lua_rawgeti(L, 4, LCLASS_SUPER);//5
            if (lua_isnil(L, -1)) {//没super了
                break;
            } else {
                lua_getmetatable(L, -1);//6
                lua_rawgeti(L, -1, LCLASS_OBJCLASS);
                if (lua_touserdata(L, -1) == obj) {//找到了
                    lua_pop(L, 2);
                    return 1;
                }
                lua_pop(L, 1);
                lua_replace(L, 4);
            }
        }
        lua_settop(L, 3);//只保留当前需要cast的对象元表
        lua_pushvalue(L, -1);//4
        for (;;) {//child查找方向
            lua_settop(L, 4);
            lua_rawgeti(L, 4, LCLASS_CHILD);//5
            if (lua_isnil(L, -1)) {//没child了
                break;
            } else {
                lua_getmetatable(L, -1);//6
                lua_rawgeti(L, -1, LCLASS_OBJCLASS);
                if (lua_touserdata(L, -1) == obj) {//找到了
                    lua_pop(L, 2);
                    return 1;
                }
                lua_pop(L, 1);
                lua_replace(L, 4);
            }
        }
        lua_settop(L, 2);
        lua_getmetatable(L, 1);//3
        lua_getmetatable(L, 2);//4
        lua_getfield(L, 3, "__name");
        lua_getfield(L, 4, "__name");
        return luaL_error(L, "bad cast:object<%s> not extends this class<%s>",
                          luaL_optstring(L, 5, "unknown"), luaL_optstring(L, 6, "unknown"));
    } else {
        return luaL_error(L, "input object cast to class type object");
    }
}

/*
 * 参数：类/对象 类/对象
 */
int luaOC_instanceof(lua_State *L) {
    lclass_obj *obj = fixcovert_lclass(L, 1);
    if (obj == NULL)return luaL_error(L, "not a class or object");
    obj = fixcovert_lclass(L, 2);//此时obj为目标类的USERDATA，比较它
    if (obj == NULL)return luaL_error(L, "not a class or object");
    lua_settop(L, 2);
    lua_getmetatable(L, 1);//3
    lua_getmetatable(L, 2);//4
    lua_rawgeti(L, 3, LCLASS_ISOBJECT);
    int waitcast_isobj = lua_toboolean(L, -1);
    lua_pop(L, 1);
    lua_rawgeti(L, 4, LCLASS_ISOBJECT);
    int castto_isobj = lua_toboolean(L, -1);
    lua_pop(L, 1);
    if (waitcast_isobj && !castto_isobj) {
        //先看自己是不是就是
        lua_settop(L, 3);//只保留当前需要cast的对象元表
        lua_rawgeti(L, -1, LCLASS_OBJCLASS);
        if (lua_touserdata(L, -1) == obj) {//找到了
            lua_pushboolean(L, 1);
            return 1;
        }
        lua_settop(L, 3);//只保留当前需要cast的对象元表
        lua_pushvalue(L, -1);//4
        for (;;) {//super查找方向
            lua_settop(L, 4);
            lua_rawgeti(L, 4, LCLASS_SUPER);//5
            if (lua_isnil(L, -1)) {//没super了
                break;
            } else {
                lua_getmetatable(L, -1);//6
                lua_rawgeti(L, -1, LCLASS_OBJCLASS);
                if (lua_touserdata(L, -1) == obj) {//找到了
                    lua_pushboolean(L, 1);
                    return 1;
                }
                lua_pop(L, 1);
                lua_replace(L, 4);
            }
        }
        lua_settop(L, 3);//只保留当前需要cast的对象元表
        lua_pushvalue(L, -1);//4
        for (;;) {//child查找方向
            lua_settop(L, 4);
            lua_rawgeti(L, 4, LCLASS_CHILD);//5
            if (lua_isnil(L, -1)) {//没child了
                break;
            } else {
                lua_getmetatable(L, -1);//6
                lua_rawgeti(L, -1, LCLASS_OBJCLASS);
                if (lua_touserdata(L, -1) == obj) {//找到了
                    lua_pushboolean(L, 1);
                    return 1;
                }
                lua_pop(L, 1);
                lua_replace(L, 4);
            }
        }
        lua_pushboolean(L, 0);
        return 1;
    } else {
        return luaL_error(L, "input object cast to class type object");
    }
}


static const luaL_Reg classlib[] = {
        {"newClass",               luaOC_newClass},
        {"setConstructor",         luaOC_setConstructor},
        {"setDeconstructor",       luaOC_setDeconstructor},
        {"setObjectDeconstructor", luaOC_setObjectDeconstructor},
        {"setStaticField",         luaOC_setStaticField},
        {"setField",               luaOC_setField},
        {"setStaticMethod",        luaOC_setStaticMethod},
        {"setMethod",              luaOC_setMethod},
        {"setSuper",               luaOC_setSuper},
        {"getSuper",               luaOC_getSuper},
        {"getChild",               luaOC_getChild},
        {"getMethods",             luaOC_getMethods},
        {"getFields",              luaOC_getFields},
        {"getDeclaredMethods",     luaOC_getDeclaredMethods},
        {"getDeclaredFields",      luaOC_getDeclaredFields},
        {"lockdefine",             luaOC_lockdefine},
        {"cast",                   luaOC_cast},
        {"instanceof",             luaOC_instanceof},
        {"public",  NULL},
        {"private", NULL},
        {NULL,      NULL}
};

LUAMOD_API int luaopen_lclass(lua_State *L) {
    luaL_newlib(L, classlib);
    lua_pushinteger(L, LCLASS_private);
    lua_setfield(L, -2, "private");
    lua_pushinteger(L, LCLASS_public);
    lua_setfield(L, -2, "public");
    return 1;
}

