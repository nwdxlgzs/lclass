# lclass
Advanced Lua object-oriented system framework

# version support
> lua54

# lib-content

|     function/field     |                          parameters                          |                                info                                |
| :--------------------: | :----------------------------------------------------------: | :----------------------------------------------------------------: |
|        newClass        |                        类名（string）                        |                             定义一个类                             |
|   newClassWithSuper    |                类名（string）,类（luaclass）                 |                 定义一个类顺便调用setSuper设置父类                 |
|     setConstructor     |             类（luaclass）,构建函数（function）              |                 定义这个类的构建函数，只有一次机会                 |
|    setDeconstructor    |      类/对象（luaclass/luaobject）,析构函数（function）      | 如果是类定义类的析构函数，如果是对象定义对象析构函数，只有一次机会 |
| setObjectDeconstructor |            对象（luaobject）,析构函数（function）            |                   定义对象析构函数，只有一次机会                   |
|     setStaticField     |  类/对象（luaclass/luaobject）,可见性修饰（number），键，值  |           定义静态字段，定义之后会记录键值，只有一次机会           |
|        setField        |  类/对象（luaclass/luaobject）,可见性修饰（number），键，值  |           定义实例字段，定义之后会记录键值，只有一次机会           |
|    setStaticMethod     | 类/对象（luaclass/luaobject）,可见性修饰（number），键，函数 |           定义静态方法，定义之后会记录键值，只有一次机会           |
|       setMethod        | 类/对象（luaclass/luaobject）,可见性修饰（number），键，函数 |           定义实例方法，定义之后会记录键值，只有一次机会           |
|        setSuper        | 类/对象（luaclass/luaobject）, 类/对象（luaclass/luaobject） |           定义父类，只有一次机会，类不可以定义对象为父类           |
|        getSuper        |                类/对象（luaclass/luaobject）                 |                      返回父类（无继承为nil）                       |
|        getChild        |                      对象（luaobject）                       |             返回当前对象继承子类对象（叶子对象时nil）              |
|       getMethods       |                类/对象（luaclass/luaobject）                 |                  获取当前类或对象定义的public方法                  |
|       getFields        |                类/对象（luaclass/luaobject）                 |                  获取当前类或对象定义的public字段                  |
|   getDeclaredMethods   |                类/对象（luaclass/luaobject）                 |                   获取当前类或对象定义的所有方法                   |
|   getDeclaredFields    |                类/对象（luaclass/luaobject）                 |                   获取当前类或对象定义的所有字段                   |
|       lockdefine       |                类/对象（luaclass/luaobject）                 |              锁定定义，用于完成构建类或对象后锁定定义              |
|          cast          |               对象（luaobject）,类（luaclass）               |             将当前对象转为目标类的对象（失败抛出错误）             |
|       instanceof       |               对象（luaobject）,类（luaclass）               |           判断当前对象是否（或其父/子类）为目标类的对象            |
|        getClass        |                类/对象（luaclass/luaobject）                 |                        拿到当前对象或类的类                        |
|         public         |                              -                               |         可见性修饰，不影响使用，影响后续基于此值设计的函数         |
|        private         |                              -                               |         可见性修饰，不影响使用，影响后续基于此值设计的函数         |

## 开启私有实现

编译参数携带`-DLCLASS_PRIVATE_IMPL`或代码开启`#define LCLASS_PRIVATE_IMPL 1`即可使用私有化检查

## 使用方法

```lua
local 生物 = lclass.newClass("生物")
lclass.setStaticField(生物,lclass.public,"生物实例总数",0)
lclass.setField(生物,lclass.public,"名字",nil)
lclass.setConstructor(生物,function(self,名字)
  local super = lclass.getSuper(self)
  self.名字=名字
  self.生物实例总数=self.生物实例总数+1
end)
lclass.lockdefine(生物)
local 动物 = lclass.newClassWithSuper("动物",生物)
lclass.setField(动物,lclass.public,"叫声",nil)
lclass.setConstructor(动物,function(self,名字)
  local super = lclass.getSuper(self)
  print("构建"..tostring(名字).."中")
end)
lclass.setMethod(动物,lclass.public,"bark",function(self)
  local super = lclass.getSuper(self)
  local _=self.叫声
  if _==nil then _="还没设置叫声" end
  print(tostring(self.名字).."："..tostring(_))
end)
lclass.lockdefine(动物)
local 猫 = lclass.newClass("猫")
lclass.setSuper(猫,动物)
lclass.setConstructor(猫,function(self,名字)
  local super = lclass.getSuper(self)
  self.叫声="喵喵喵"
end)
getmetatable(猫).__mul=function(self,other)
  if lclass.instanceof(other,猫) then
    return 猫(tostring(self.名字).."*"..tostring(other.名字))
  else
    return nil
  end
end
getmetatable(猫).__tostring=function(self)
  return tostring(self.名字).."猫"
end
lclass.lockdefine(猫)
狗 = lclass.newClassWithSuper("狗",动物)
lclass.setConstructor(狗,function(self,名字)
  local super = lclass.getSuper(self)
  self.叫声="汪汪汪"
end)
getmetatable(狗).__tostring=function(self)
  return tostring(self.名字).."狗"
end
lclass.lockdefine(狗)
dog1=狗("柯基")
dog1:bark()
dog2=狗("德牧")
dog2:bark()
cat1=猫("加菲")
cat1:bark()
cat2=猫("暹罗")
cat2:bark()
print(生物.生物实例总数)
cat3=cat1*cat2
print(cat3)
print(生物.生物实例总数)
cat4=(cat1*dog1)
if cat4==nil then cat4="杂交失败" end
print(cat4)
print(生物.生物实例总数)
print(dog1)
a=lclass.cast(dog1,猫)--报错
```

## 下面是我使用此库魔改Lua实现的面向对象（支持private）

```lua
--律动lua-1.0.20以后支持本语法
local class 生物{
  @static public 生物实例总数=0;
  public 名字;
  public __init__(名字){
    self.名字=名字;
    self.生物实例总数=self.生物实例总数+1
  }
}
local class 动物 extends 生物{
  public 叫声;
  public __init__(名字){
    print(@f"构建{名字}中")
  }
  public bark(){
    print(@f"{self.名字}：{self.叫声??\"还没设置叫声\"}")
  }
}
local class 猫 extends 动物{
  public __init__(名字){
    self.叫声="喵喵喵"
  }
  @meta public __mul(self,other){
    if other instanceof 猫 then
      return 猫(@f"{self.名字}*{other.名字}")
     else
      return nil
    end
  }
  @meta public __tostring(self){
    return @f"{self.名字}猫"
  }
}
class 狗 extends 动物{
  public __init__(名字){
    super.叫声="汪汪汪"
  }
  @meta public __tostring(self){
    return @f"{self.名字}狗"
  }
}
dog1=狗("柯基")
dog1:bark()
dog2=狗("德牧")
dog2:bark()
cat1=猫("加菲")
cat1:bark()
cat2=猫("暹罗")
cat2:bark()
print(生物.生物实例总数)
cat3=cat1*cat2
print(cat3)
print(生物.生物实例总数)
cat4=(cat1*dog1)??"杂交失败"
print(cat4)
print(生物.生物实例总数)
print(dog1)
a=cast<猫> dog1--报错
```

## 解释器指令细节
```c
vmcase(OP_NEWCLASS)
{/* R(A)=class R(A) B? extends X */
    StkId ra = RA(i);
    int hassuper = GETARG_B(i);
    setobjs2s(L, ra + 2, ra + 1);
    setobjs2s(L, ra + 1, ra);
    setfvalue(s2v(ra), hassuper ? luaOC_newClassWithSuper : luaOC_newClass);
    if (hassuper) {
        L->top.p = ra + 1 + 2;
    } else {
        L->top.p = ra + 1 + 1;
    }
    savepc(L);
    luaD_precall(L, ra, 1);
    updatetrap(ci);
    vmbreak;
}
vmcase(OP_SETCLASSFIELD)
{/* R(A) B?static C?public R(A):field[R(A+2)]=R(A+3) */
    StkId ra = RA(i);
    int is_static = GETARG_B(i);
    int is_public = GETARG_C(i);
    setivalue(s2v(ra + 1), is_public ? LCLASS_public : LCLASS_private);
    setfvalue(s2v(ra - 1), is_static ? luaOC_setStaticField : luaOC_setField);//编译时预留了空位
    L->top.p = ra + 4;
    savepc(L);
    luaD_precall(L, ra - 1, 0);
    updatetrap(ci);
    vmbreak;
}
vmcase(OP_SETCLASSMETHOD)
{/* R(A) B?static C?public R(A):method[R(A+2)]=R(A+3) */
    StkId ra = RA(i);
    int is_static = GETARG_B(i);
    int is_public = GETARG_C(i);
    setivalue(s2v(ra + 1), is_public ? LCLASS_public : LCLASS_private);
    setfvalue(s2v(ra - 1),
              is_static ? luaOC_setStaticMethod : luaOC_setMethod);//编译时预留了空位
    L->top.p = ra + 4;
    savepc(L);
    luaD_precall(L, ra - 1, 0);
    updatetrap(ci);
    vmbreak;
}
vmcase(OP_METHODINITSUPER)
{/* R(A) = super R(B) && R(A+1)=light userdata R(B) */
    StkId ra = RA(i);
    StkId rb = RB(i);
    lclass_obj *obj = NULL;
    if (ttype(s2v(rb)) == LUA_TUSERDATA) {
        Udata *U = uvalue(s2v(rb));
        lclass_obj *aobj = (lclass_obj *) getudatamem(U);
        if (aobj->meta == U->metatable && sizeof(lclass_obj) == U->len)
            obj = aobj;
    }
    setobjs2s(L, ra + 1, rb);
    setfvalue(s2v(ra), luaOC_getSuper);
    L->top.p = ra + 1 + 1;
    savepc(L);
    luaD_precall(L, ra, 1);
    updatetrap(ci);
    setpvalue(s2v(ra + 1), obj);
    vmbreak;
}
vmcase(OP_CLASSCONDECOFINA)
{
    //0：__init__ 1：__del__ 2：__finalize__
    StkId ra = RA(i);
    int b = GETARG_B(i);
    lua_CFunction f = NULL;
    switch (b) {
        case 0: {
            f = luaOC_setConstructor;
            break;
        }
        case 1: {
            f = luaOC_setObjectDeconstructor;
            break;
        }
        case 2: {
            f = luaOC_setDeconstructor;
            break;
        }
    }
    if (f) {
        setfvalue(s2v(ra - 1), f);
        setobjs2s(L, ra + 1, ra + 3);
        L->top.p = ra + 2;
        savepc(L);
        luaD_precall(L, ra-1, 0);
        updatetrap(ci);
    }
    vmbreak;
}
vmcase(OP_GETMETATABLE)
{/* R(A)=getmetatable(R(A)) */
    StkId ra = RA(i);
    setobjs2s(L, ra + 1, ra);
    setfvalue(s2v(ra), luaV_getmetatable);
    L->top.p = ra + 1 + 1;
    savepc(L);
    luaD_precall(L, ra, 1);
    updatetrap(ci);
    vmbreak;
}
vmcase(OP_LOCKCLASSDEF)
{/* R(A):lock */
    StkId ra = RA(i);
    if (ttype(s2v(ra)) == LUA_TUSERDATA) {
        Udata *U = uvalue(s2v(ra));
        lclass_obj *obj = (lclass_obj *) getudatamem(U);
        if (obj->meta == U->metatable && sizeof(lclass_obj) == U->len)
            obj->lockdefine = 1;
    }
    vmbreak;
}
vmcase(OP_OBJECTCAST)
{/* R(A):cast<R(B)> R(A) */
    StkId ra = RA(i);
    setobjs2s(L, ra + 2, RB(i));
    setobjs2s(L, ra + 1, ra);
    setfvalue(s2v(ra), luaOC_cast);
    L->top.p = ra + 1 + 2;
    savepc(L);
    luaD_precall(L, ra, 1);
    updatetrap(ci);
    vmbreak;
}
vmcase(OP_INSTANCEOF)
{
    StkId ra = RA(i);
    setobjs2s(L, ra + 2, RC(i));
    setobjs2s(L, ra + 1, RB(i));
    setfvalue(s2v(ra), luaOC_instanceof);
    L->top.p = ra + 1 + 2;
    savepc(L);
    luaD_precall(L, ra, 1);
    updatetrap(ci);
    vmbreak;
}
```
## 编译器细节（仅限class定义）
```c
static void classstat(LexState *ls, int onlocal) {
    FuncState *fs = ls->fs;
    int line = ls->linenumber;
    luaX_next(ls);//skip 'class'
    int annotation_ENV = luaS_cstrequal(ls->t.annotation, "ENV");
    TString *classnameS = str_checkname(ls);
    int localvidx = -1;
    if (onlocal) {
        localvidx = new_localvar(ls, classnameS);
        adjustlocalvars(ls, 1);
    }
    int classvidx = new_localvarliteral(ls, "(class state)")
    adjustlocalvars(ls, 1);
    expdesc classname = {0};
    codestring(ls, 0, &classname, classnameS);
    expdesc superclass = {0};
    init_exp(&superclass, VNIL, 0);
    if (testnext(ls, TK_EXTENDS)) {//继承
        singlevar(ls, &superclass);
    }
    luaK_exp2nextreg(fs, &classname);
    if (superclass.k != VNIL) {
        luaK_exp2nextreg(fs, &superclass);
        luaK_codeABC(fs, OP_NEWCLASS, classname.u.info, 1, 0);
    } else {
        luaK_codeABC(fs, OP_NEWCLASS, classname.u.info, 0, 0);
    }
    int classnameinfo = classname.u.info;
    expdesc cla = {0};
    init_var(fs, &cla, classvidx);
    if (classnameinfo != classvidx) {
        luaK_storevar(fs, &cla, &classname);
    }
    init_var(fs, &cla, classvidx);
    if (onlocal) {
        if (classnameinfo != localvidx) {
            expdesc lo = {0};
            init_var(fs, &lo, localvidx);
            luaK_setoneret(fs, &cla);
            luaK_storevar(fs, &lo, &cla);
        }
    } else {
        expdesc gl = {0};
        codestring(ls, 0, &gl, classnameS);
        singlevar_name(ls, &gl, classnameS, annotation_ENV);
        luaK_setoneret(fs, &cla);
        luaK_storevar(fs, &gl, &cla);
    }
    checknext(ls, '{');
    int freereg = fs->freereg;
    do {
        fs->freereg = freereg;
        if (ls->t.token == '}')break;
        luaK_reserveregs(fs, 1);//留足空位
        line = ls->linenumber;
        int is_static = luaS_cstrequal(ls->t.annotation, "static");
        int is_meta = luaS_cstrequal(ls->t.annotation, "meta");
        int is_public = testnext(ls, TK_PUBLIC);
        int is_private = 0;
        if (!is_public) {
            is_private = testnext(ls, TK_PRIVATE);
            if (!is_private) {
                luaX_syntaxerror(ls,
                                 luaO_pushfstring(ls->L, "class field/method define expect %s/%s",
                                                  luaX_token2str(ls, TK_PUBLIC),
                                                  luaX_token2str(ls, TK_PRIVATE)));
            }
        }
        TString *MF_name = str_checkname(ls);
        expdesc MFkey = {0};
        codestring(ls, 0, &MFkey, MF_name);
        int is__init__ = luaS_cstrequal(MF_name, "__init__");
        int is__del__ = luaS_cstrequal(MF_name, "__del__");
        int is__finalize__ = luaS_cstrequal(MF_name, "__finalize__");
        if (ls->t.token == ';' || ls->t.token == '=') {//字段定义
            expdesc fieldval = {0};
            if (ls->t.token == ';') {//字段定义，a; => a=nil;
                init_exp(&fieldval, VNIL, 0);
            } else {
                checknext(ls, '=');
                expr(ls, &fieldval);
            }
            luaK_reserveregs(fs, 1);//存放luaOC_set[Static]Field
            expdesc clazz = {0};
            init_exp(&clazz, VLOCAL, fs->freereg);
            luaK_reserveregs(fs, 1);//clazz
            init_var(fs, &cla, classvidx);
            luaK_storevar(fs, &clazz, &cla);
            luaK_reserveregs(fs, 1);//public/private
            luaK_exp2nextreg(fs, &MFkey);
            luaK_exp2nextreg(fs, &fieldval);
            luaK_codeABC(fs, OP_SETCLASSFIELD, clazz.u.info, is_static, is_public);
            checknext(ls, ';');
        } else if (testnext(ls, '(')) {//方法定义
            expdesc methodval = {0};
            if (is_meta) {
                //禁止：__call,__index,__newindex,__name,__type,__gc
                for (int i = 0;
                     i < sizeof(MetaCustom_banlist) / sizeof(MetaCustom_banlist[0]); i++) {
                    if (luaS_cstrequal(MF_name, MetaCustom_banlist[i]))
                        luaX_syntaxerror(ls, luaO_pushfstring(ls->L,
                                                              "class not allow define %s meta method",
                                                              MetaCustom_banlist[i]));
                }
                expdesc clazz = {0};
                init_exp(&clazz, VLOCAL, fs->freereg);
                luaK_reserveregs(fs, 1);//clazz
                init_var(fs, &cla, classvidx);
                luaK_storevar(fs, &clazz, &cla);
                luaK_reserveregs(fs, 1);//clazz
                luaK_exp2nextreg(fs, &clazz);
                luaK_codeABC(fs, OP_GETMETATABLE, clazz.u.info, 0, 0);
                expdesc key = {0};
                codestring(ls, 0, &key, MF_name);
                luaK_indexed(fs, &clazz, &key);
                FuncState new_fs = {0};
                BlockCnt bl;
                new_fs.f = addprototype(ls);
                new_fs.f->linedefined = line;
                new_fs.independent = 0;
                open_func(ls, &new_fs, &bl);
                TokenNodeArgs args = {0};
                parlist(ls, &args);
                checknext(ls, ')');
                checknext(ls, '{');
                fixDefArgs(ls, &args);
                statlist(ls);
                new_fs.f->lastlinedefined = ls->linenumber;
                codeclosure(ls, &methodval);
                close_func(ls);
                check_match(ls, '}', '{', line);
                testnext(ls, ';');
                luaK_storevar(fs, &clazz, &methodval);
            } else {
                luaK_reserveregs(fs, 1);//存放luaOC_set[Static]Method/set[De][Object]Constructor
                expdesc clazz = {0};
                init_exp(&clazz, VLOCAL, fs->freereg);
                luaK_reserveregs(fs, 1);//clazz
                init_var(fs, &cla, classvidx);
                luaK_storevar(fs, &clazz, &cla);
                luaK_reserveregs(fs, 1);//public/private
                luaK_exp2nextreg(fs, &MFkey);
                {
                    FuncState new_fs = {0};
                    BlockCnt bl;
                    new_fs.f = addprototype(ls);
                    new_fs.f->linedefined = line;
                    new_fs.independent = 0;
                    open_func(ls, &new_fs, &bl);
                    int selfvidx = new_localvarliteral(ls, "self");
                    adjustlocalvars(ls, 1);
                    TokenNodeArgs args = {0};
                    parlist(ls, &args);
                    checknext(ls, ')');
                    checknext(ls, '{');
                    fixDefArgs(ls, &args);
                    int supervidx = new_localvarliteral(ls, "super");
                    adjustlocalvars(ls, 1);
                    luaK_reserveregs(&new_fs, 1);
                    {//这里存放lightuserdata保证不可能是Lua自己不小心达到private判断条件
                        new_localvarliteral(ls, "(private self ptr)");
                        adjustlocalvars(ls, 1);
                        luaK_reserveregs(&new_fs, 1);
                    }
                    luaK_codeABC(&new_fs, OP_METHODINITSUPER, supervidx, selfvidx, 0);
                    statlist(ls);
                    new_fs.f->lastlinedefined = ls->linenumber;
                    codeclosure(ls, &methodval);
                    close_func(ls);
                }
                if (is__init__) {
                    luaK_codeABC(fs, OP_CLASSCONDECOFINA, clazz.u.info, 0, 0);
                } else if (is__del__) {
                    luaK_codeABC(fs, OP_CLASSCONDECOFINA, clazz.u.info, 1, 0);
                } else if (is__finalize__) {
                    luaK_codeABC(fs, OP_CLASSCONDECOFINA, clazz.u.info, 2, 0);
                } else {
                    luaK_codeABC(fs, OP_SETCLASSMETHOD, clazz.u.info, is_static, is_public);
                }
                check_match(ls, '}', '{', line);
                testnext(ls, ';');
            }
        } else
            luaX_syntaxerror(ls, "not a field/method define");
    } while (1);
    check_match(ls, '}', '{', line);
    if (onlocal) {
        luaK_codeABC(fs, OP_LOCKCLASSDEF, localvidx, 0, 0);
    } else {
        luaK_codeABC(fs, OP_LOCKCLASSDEF, classvidx, 0, 0);
    }
    removevars(fs, classvidx);
}
```
